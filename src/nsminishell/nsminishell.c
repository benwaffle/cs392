#define DEBUG
#include "list.h"
#include "my.h"
#include <assert.h>
#include <curses.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <term.h>
#include <termios.h>
#include <unistd.h>

#ifndef CTRL
#define CTRL(c) (c & 037)
#endif

#define BUFSIZE 4096

bool running = 0;
struct s_node *history = NULL, *hcur = NULL;

void rawmode(bool enable)
{
    static struct termios old;
    static bool init = false;
    if (!init) {
        init = true;
        tcgetattr(0, &old);
    }

    if (enable) {
        struct termios raw = old;

        raw.c_lflag &= ~(ICANON | IEXTEN); // disable canonical mode
        raw.c_lflag &= ~(ECHO | ECHONL); // no echo

        raw.c_iflag |= ICRNL; // CR -> NL
        raw.c_iflag &= ~INLCR; // no NL -> CR

        raw.c_oflag |= OPOST; // implementation-defined output processing
        raw.c_oflag |= ONLCR; // NL -> CR,NL
        raw.c_oflag &= ~(OCRNL | ONOCR | ONLRET); // output CR

        raw.c_cc[VMIN] = 1; // minimum number of chars for read()
        raw.c_cc[VTIME] = 0; // read() timeout
        tcsetattr(0, TCSANOW, &raw);
    } else {
        tcsetattr(0, TCSANOW, &old);
    }
}

void shell_prompt()
{
    char path[PATH_MAX];
    getcwd(path, sizeof path);

    putp(tparm(tigetstr("setaf"), COLOR_GREEN));
    printf("%s ", path);
    putp(tparm(tigetstr("setaf"), COLOR_BLUE));
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
                printf("%s: no such command\n", cmd[0]);
            else
                perror("execvp");
        }
        exit(0);
    } else {
        if (wait(NULL) < 0) {
            perror("wait");
        }
    }
}

void process_command(char *cmd)
{
    rawmode(false);

    char **parts = my_str2vect(cmd);

    // TODO fix quotes

    if (parts[0]) {
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
}

void debug(int line, const char *fmt, ...)
{
#ifdef DEBUG
    va_list args;
    va_start(args, fmt);

    putp(tigetstr("sc")); // save cursor
    putp(tparm(tigetstr("cup"), tigetnum("lines") - line,
               0)); // move cur to y, x
    putp(tigetstr("dl1")); // delete line
    putp(tigetstr("cuu1")); // up one line

    vprintf(fmt, args);
    va_end(args);

    putp(tigetstr("rc")); // restore cursor
#endif
}

/**
 * insert `len' chars from src into beginning of dst
 */
void strinsert(char *dst, char *src, int len)
{
    memmove(dst + len, dst, strlen(dst) + 1); // shift chars
    strncpy(dst, src, len); // copy from src without '\0'
}

void clr2eol(int pos)
{
    if (pos > 0)
        putp(tparm(tigetstr("cub"), pos)); // move left `pos' spaces
    putp(tigetstr("el")); // delete to eol
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
    bool pasted = false;

    while (running) {
        int hpos = 1;
        for (struct s_node *cur = history; cur != NULL; ++hpos, cur = cur->next)
            if (cur == hcur)
                break;
        debug(1, "[DEBUG]: history=%d/%d, clipboard=`%s', pos=%d, buf=`%s'",
              hpos, count_s_nodes(history), clipboard, pos, buf);

        char c[6] = {0};
        if (read(0, &c, sizeof c) <= 0) {
            perror("read");
            running = false;
        }

        int len = strlen(c);

        // newline
        if (c[0] == '\n') {
            putchar('\n');
            if (buf[0] == '\0')
                remove_last(&history);
            process_command(buf);
            break;
        }
        // ^D (EOF)
        else if (c[0] == CTRL('D')) {
            running = false;
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
            if (pos - spos <= 0) {
                continue;
            }

            // copy to clipboard
            if (pasted) { // already pasted, clear clipboard
                strncpy(clipboard, buf + spos, pos - spos);
                clipboard[pos - spos] = '\0';
            } else { // not yet pasted, append to clipboard
                strinsert(clipboard, buf + spos, pos - spos);
            }

            pasted = false;

            // remove from buffer
            int mvlen = strlen(buf + pos);
            memmove(buf + spos, buf + pos, mvlen);
            buf[spos + mvlen] = '\0';

            // remove from terminal
            putp(tparm(tigetstr("cub"), pos - spos)); // left
            putp(tparm(tigetstr("dch"), pos - spos)); // del char

            pos = spos;
        }
        // ^U (kill to beginning)
        else if (c[0] == CTRL('U')) {
            if (pos == 0) {
                continue;
            }

            // act like we've already pasted, so that ^W doesn't append
            pasted = true;

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
        // ^Y (paste)
        else if (c[0] == CTRL('Y')) {
            pasted = true;
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
    }
}

void load_history()
{
    char histpath[PATH_MAX] = {0};
    sprintf(histpath, "%s/.nsmshistory", getenv("HOME"));

    // crete history file if it doesn't exist
    if (access(histpath, R_OK | W_OK) < 0) {
        if (errno == ENOENT) { // doesn't exist
            int fd = creat(histpath, 0600);
            if (fd < 0) {
                perror("creating ~/.nsmshistory");
                exit(1);
            }
            close(fd);
        } else {
            perror("~/.nsmshistory");
            exit(1);
        }
    }

    // read the history file into a string
    FILE *histfile = fopen(histpath, "r+");
    assert(histfile != NULL);

    while (true) {
        char *line = calloc(1, BUFSIZE);
        if (fgets(line, BUFSIZE, histfile) == NULL) {
            free(line);
            break;
        }
        line[strlen(line) - 1] = '\0'; // get rid of '\n'
        append(new_node(line, NULL, NULL), &history);
    }

#ifdef DEBUG
    printf("History:\n");
    for (struct s_node *line = history; line != NULL; line = line->next)
        printf("\t%s\n", (char *)line->elem);
#endif

    hcur = node_at(history, count_s_nodes(history) - 1);
}

void save_history()
{
    char histpath[PATH_MAX] = {0};
    sprintf(histpath, "%s/.nsmshistory", getenv("HOME"));

    FILE *histfile = fopen(histpath, "w");
    assert(histfile != NULL);

    for (struct s_node *line = history; line != NULL; line = line->next)
        if (fprintf(histfile, "%s\n", (char *)line->elem) <= 0)
            printf("Error writing to history file: %s\n",
                   strerror(ferror(histfile)));

    fclose(histfile);
}

int main()
{
    signal(SIGINT, SIG_IGN);

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
    putp(tigetstr("smkx")); // keypad mode

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
