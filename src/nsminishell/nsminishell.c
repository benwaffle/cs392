#include "list.h"
#include "my.h"
#include <assert.h>
#include <curses.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <term.h>

#ifndef CTRL
#define CTRL(c) (c & 037)
#endif

#define BUFSIZE 4096

bool running = 0;
struct s_node *history = NULL, *hcur = NULL;
int retcode = 0;

void rawmode(bool enable);
void debug(int line, const char *fmt, ...);
void clr2eol(int pos);

void load_history();
void save_history();

// read an arbitrary-length string from fd
char *read_str(int fd)
{
    ssize_t ret, size = 4, oldsize = 4, append_off = 0;
    char *msg = calloc(size, 1);
    if (msg == NULL) {
        perror("calloc");
        exit(1);
    }

    while ((ret = read(fd, msg + append_off, oldsize)) == oldsize) {
        if (msg[size - 1] == '\n')
            break;
        oldsize = append_off = size;
        msg = realloc(msg, size *= 2);
        if (msg == NULL) {
            perror("realloc");
            exit(1);
        }
    }
    if (ret < 0) {
        perror("read");
        exit(1);
    }
    append_off += ret - 1;
    msg[append_off] = '\0';
    return msg;
}

void shell_prompt()
{
    // enable keypad mode
    // we set this each time we show the prompt in case programs like htop turn
    // off keypad mode
    putp(tigetstr("smkx"));

    char path[PATH_MAX];
    getcwd(path, sizeof path);

    putp(tparm(tigetstr("setaf"), COLOR_BLUE));
    printf("%s ", path);
    if (retcode == 0)
        putp(tparm(tigetstr("setaf"), COLOR_GREEN));
    else
        putp(tparm(tigetstr("setaf"), COLOR_RED));
    printf("➜ ");
    putp(tigetstr("sgr0"));
}

void run_cmd(char *const *cmd)
{
    // TODO: do we need reset_shell_mode
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
    } else if (pid == 0) { // child
        signal(SIGINT, SIG_DFL);
        if (execvp(cmd[0], cmd) < 0) {
            if (errno == ENOENT)
                fprintf(stderr, "%s: no such command\n", cmd[0]);
            else
                perror("execvp");
        }
        exit(0);
    } else {
        int status;
        if (wait(&status) < 0) {
            perror("wait");
        } else if (WIFEXITED(status)) {
            retcode = WEXITSTATUS(status);
        } else if (WIFSIGNALED(status)) {
            retcode = 0200 | WTERMSIG(status);
        }
    }
}

char *run_subshell(char **cmd)
{
    int pipefd[2]; // 0 - read, 1 - write
    pipe(pipefd);

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return NULL;
    } else if (pid == 0) { // child
        close(pipefd[0]);
        dup2(pipefd[1], 1);
        signal(SIGINT, SIG_DFL);
        if (execvp(cmd[0], cmd) < 0) {
            if (errno == ENOENT)
                fprintf(stderr, "%s: no such command\n", cmd[0]);
            else
                perror("execvp");
        }
        exit(0);
    } else { // parent
        close(pipefd[1]);
        char *output = read_str(pipefd[0]);
        wait(NULL);
        close(pipefd[0]);
        return output;
    }
}

char *process_command(char *_cmd, bool subshell)
{
    char *output = NULL;
    char *cmd = strdup(_cmd);
    for (int i = 0; cmd[i] != '\0'; ++i) {
        if (cmd[i] == '$' && cmd[i + 1] == '(') { // subshell
            int paren = 1;
            int j = i + 2;
            while (paren != 0 && cmd[j] != '\0') {
                if (cmd[j] == '(')
                    ++paren;
                else if (cmd[j] == ')')
                    --paren;
                ++j;
            }
            if (paren != 0) { // unbalanced parens
                printf("Error: unbalanced parentheses\n");
                return NULL;
            } else {
                int begin = i + 2, end = j - 1;
                char *subcmd = calloc(end - begin + 1, 1);
                strncpy(subcmd, cmd + begin, end - begin);

                char *output = process_command(subcmd, true);
                assert(output != NULL);

                free(subcmd);

                // new command = <left> '<subshell output>' <right>
                int len =
                    i + 1 + strlen(output) + 1 + strlen(cmd + end + 1) + 1;
                char *newcmd = calloc(len, 1);
                strncat(newcmd, cmd, i);
                strncat(newcmd, "'", 1);
                strcat(newcmd, output);
                strncat(newcmd, "'", 1);
                strcat(newcmd, cmd + end + 1);

                free(output);
                free(cmd);
                cmd = newcmd;
            }
        }
    }

    rawmode(false);

    char **parts = my_str2vect(cmd);

    if (subshell) {
        output = run_subshell(parts);
    } else if (parts[0]) {
        if (strcmp(parts[0], "cd") == 0) {
            int r = chdir(parts[1] == NULL ? getenv("HOME") : parts[1]);
            if (r < 0) {
                printf("cd: %s: %s\n", strerror(errno), parts[1]);
            }
        } else if (strcmp(parts[0], "exit") == 0) {
            running = false;
        } else if (strcmp(parts[0], "help") == 0) {
            printf("Not-So-Minishell Commands:\n"
                   "\tcd <dir> - change directory\n"
                   "\texit - exit the shell\n"
                   "\thelp - show this help message\n");
        } else {
            run_cmd(parts);
        }
    }

    for (char **p = parts; *p != NULL; ++p)
        free(*p);
    free(parts);

    rawmode(true);

    free(cmd);
    return output;
}

/**
 * insert `len' chars from src into beginning of dst
 */
void strinsert(char *dst, char *src, int len)
{
    memmove(dst + len, dst, strlen(dst) + 1); // shift chars
    strncpy(dst, src, len); // copy from src without '\0'
}

void do_input()
{
    static char clipboard[BUFSIZE] = {0};

    // the last node is the user's input buffer
    struct s_node *hlast = new_node(calloc(BUFSIZE, 1), NULL, NULL);
    append(hlast, &history);
    hcur = hlast;

    int pos = 0;
    char *buf = hlast->elem;
    bool copying = false; // last command was copy

    while (running) {
        int hpos = 1;
        for (struct s_node *cur = history; cur != NULL; ++hpos, cur = cur->next)
            if (cur == hcur)
                break;
        debug(0, "[DEBUG]: history=%d/%d, copying=%d, clipboard=`%s', pos=%d, "
                 "buf=`%s'",
              hpos, count_s_nodes(history), copying, clipboard, pos, buf);

        char c[6] = {0};
        if (read(0, &c, sizeof c) <= 0) {
            perror("read");
            running = false;
        }

        int len = strlen(c);

        // newline
        if (c[0] == '\n') {
            putchar('\n');
            // if user is executing a command in history
            if (buf != hlast->elem) {
                free(hlast->elem);
                remove_last(&history);
                // duplicate the cmd from histroy
                append(new_node(strdup(buf), NULL, NULL), &history);
            } else if (buf[0] == '\0') {
                remove_last(&history);
            }
            process_command(buf, false);
            break;
        }
        // ^D (EOF)
        else if (c[0] == CTRL('D')) {
            running = false;
            remove_last(&history);
        }

        /**
         * Line editing
         */

        // backspace
        else if (c[0] == 0x7F || strcmp(c, tigetstr("kbs")) == 0) {
            if (pos > 0) {
                putp(tigetstr("cub1")); // move left
                putp(tigetstr("dch1")); // delete char
                memmove(buf + pos - 1, buf + pos, strlen(buf) - pos + 1);
                --pos;
            }
        }
        // delete
        else if (strcmp(c, tigetstr("kdch1")) == 0) {
            if (pos < strlen(buf)) {
                putp(tigetstr("dch1")); // delete char
                memmove(buf + pos, buf + pos + 1, strlen(buf) - pos + 1);
            }
        }
        // left
        else if (strcmp(c, tigetstr("kcub1")) == 0) {
            if (pos > 0) {
                putp(tigetstr("cub1")); // move left
                --pos;
            }
        }
        // right
        else if (strcmp(c, tigetstr("kcuf1")) == 0) {
            if (pos < strlen(buf)) {
                putp(tigetstr("cuf1")); // move right
                ++pos;
            }
        }
        // ^A (beginning)
        else if (c[0] == CTRL('A')) {
            if (pos > 0) {
                putp(tparm(tigetstr("cub"), pos)); // backward `pos' times
                pos = 0;
            }
        }
        // ^E (end)
        else if (c[0] == CTRL('E')) {
            if (pos < strlen(buf)) {
                putp(tparm(tigetstr("cuf"), strlen(buf) - pos));
                pos = strlen(buf);
            }
        }
        // ^C
        else if (c[0] == CTRL('C')) {
            putchar('\n');
            remove_last(&history);
            break;
        }

        /**
         * Clipboard
         */

        // ^W (kill word)
        else if (c[0] == CTRL('W')) {
            // scan for text to copy
            int spos = pos - 1;
            while (spos >= 0 && buf[spos] == ' ') // go back skip spaces
                --spos;
            while (spos >= 0 && buf[spos] != ' ') // go back skip chars
                --spos;
            ++spos;

            // don't copy empty text
            if (pos - spos > 0) {
                // copy to clipboard
                if (!copying) { // did something else (not copying), clear
                                // clipboard
                    strncpy(clipboard, buf + spos, pos - spos);
                    clipboard[pos - spos] = '\0';
                } else { // still copying, append to clipboard
                    strinsert(clipboard, buf + spos, pos - spos);
                }

                // remove from buffer
                int mvlen = strlen(buf + pos);
                memmove(buf + spos, buf + pos, mvlen);
                buf[spos + mvlen] = '\0';

                // remove from terminal
                putp(tparm(tigetstr("cub"), pos - spos)); // left
                putp(tparm(tigetstr("dch"), pos - spos)); // del char

                pos = spos;
            }
        }
        // ^U (kill to beginning)
        else if (c[0] == CTRL('U')) {
            if (pos > 0) {
                // copy to clipboard
                strncpy(clipboard, buf, pos);
                clipboard[pos] = '\0';

                // remove from buffer
                int mvlen = strlen(buf + pos);
                memmove(buf, buf + pos, mvlen);
                buf[mvlen] = '\0';

                // remove from terminal
                putp(tparm(tigetstr("cub"), pos)); // left
                putp(tparm(tigetstr("dch"), pos)); // del char

                pos = 0;
            }
        }
        // ^Y (paste)
        else if (c[0] == CTRL('Y')) {
            putp(tigetstr("smir")); // enter insert mode
            printf("%s", clipboard);
            putp(tigetstr("rmir")); // exit insert mode
            strinsert(buf + pos, clipboard, strlen(clipboard));
            pos += strlen(clipboard);
        }

        /**
         * History
         */

        // up
        else if (strcmp(c, tigetstr("kcuu1")) == 0) {
            if (hcur->prev != NULL) {
                hcur = hcur->prev;
                buf = hcur->elem;
            }
            clr2eol(pos);
            printf("%s", buf);
            pos = strlen(buf);
        }
        // down
        else if (strcmp(c, tigetstr("kcud1")) == 0) {
            if (hcur->next != NULL) {
                hcur = hcur->next;
                buf = hcur->elem;
            }
            clr2eol(pos);
            printf("%s", buf);
            pos = strlen(buf);
        }

        // ^L
        else if (c[0] == CTRL('L')) {
            putp(tigetstr("clear"));
            shell_prompt();
            printf("%s", buf);
        }

        // regular chars
        else if (c[0] >= ' ') {
            putp(tigetstr("smir")); // enter insert mode
            printf("%s", c);
            putp(tigetstr("rmir")); // exit insert mode
            strinsert(buf + pos, c, len);
            pos += len;
        }

        if (c[0] == CTRL('W') || c[0] == CTRL('U')) {
            copying = true;
        } else {
            copying = false;
        }
    }
}

void ignore(int sig) {}

int main()
{
    signal(SIGINT, SIG_IGN);

    sigset_t mask;
    sigemptyset(&mask);
    // restart the subshell read() on SIGCHLD
    // clang-format off
    sigaction(SIGCHLD,
              &(struct sigaction){
                  .sa_handler = ignore,
                  .sa_mask = mask,
                  .sa_flags = SA_RESTART
              },
              NULL);
    // clang-format on

    // disable buffering
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    int err;
    if (setupterm(NULL, 1, &err) != OK) {
        if (err == 1)
            puts("setupterm: terminal is hardcopy, cannot be used");
        else if (err == 0)
            puts("setupterm: terminal not found or is generic type");
        else if (err == -1)
            puts("setupterm: cannot find terminfo database");
        return 1;
    }

    load_history();

    rawmode(true);

    running = true;
    while (running) {
        shell_prompt();
        do_input();
    }

    putp(tigetstr("rmkx")); // disable keypad mode
    rawmode(false);

    save_history();

    // reset_shell_mode();

    puts("Bye");
}
