#include "list.h"
#include "my.h"
#include <assert.h>
#include <curses.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
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

void clr2eol(int pos)
{
    if (pos > 0)
        putp(tparm(tigetstr("cub"), pos)); // move left `pos' spaces
    putp(tigetstr("el")); // delete to eol
}

void do_input()
{
    // the last node is the user's input buffer
    struct s_node *hlast = new_node(calloc(BUFSIZE, 1), NULL, NULL);
    append(hlast, &history);
    hcur = hlast;

    int pos = 0;
    char *buf = hlast->elem;

    while (running) {
        putp(tigetstr("sc")); // save cursor
        int hpos = 1;
        for (struct s_node *cur = history; cur != NULL; ++hpos, cur = cur->next)
            if (cur == hcur)
                break;
        putp(tparm(tigetstr("cup"), tigetnum("lines") - 1, 0));
        putp(tigetstr("dl1")); // delete line
        printf("[DEBUG]: history=%d/%d, pos=%d, buf=%s", hpos,
               count_s_nodes(history), pos, buf);
        putp(tigetstr("rc")); // restore cursor

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
            memmove(buf + pos + len, buf + pos, strlen(buf) - pos);
            strcpy(buf + pos, c);
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

#if 0
    printf("History:\n");
    for (struct s_node *line = history; line != NULL; line = line->next)
        printf("\t%s\n", (char*)line->elem);
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

    rawmode(true);
    putp(tigetstr("smkx")); // keypad mode

    load_history();

    running = true;
    while (running) {
        shell_prompt();
        do_input();
    }

    save_history();

    putp(tigetstr("rmkx")); // disable keypad mode
    rawmode(false);

    // reset_shell_mode();

    puts("Bye");
}
