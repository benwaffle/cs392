#define _DEFAULT_SOURCE // cfmakeraw()
#include "my.h"
#include <curses.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <term.h>
#include <termios.h>
#include <unistd.h>
#include <unistd.h>

#ifndef CTRL
#define CTRL(c) (c&037)
#endif

bool running = 0;

void rawmode(bool enable) {
    static struct termios old;
    static bool init = false;
    if (!init) {
        init = true;
        tcgetattr(0, &old);
    }

    if (enable) {
        struct termios raw = old;
        raw.c_lflag &= ~(ICANON | IEXTEN | ECHO | ECHONL);
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


#define DEBUGX 0
#define DEBUGY tigetnum("lines")-1

void do_input()
{
    char buf[4096] = {0};
    int pos = 0;
    while (running) {
        putp(tigetstr("sc")); // save cursor
        putp(tparm(tigetstr("cup"), DEBUGY, DEBUGX));
        putp(tigetstr("dl1")); // delete line
        printf("[DEBUG]: pos=%d, len=%lu, buf=%s", pos, strlen(buf), buf);
        putp(tigetstr("rc")); // restore cursor

        char c[6] = {0};
        if (read(0, &c, sizeof c) <= 0) {
            perror("read");
            running = false;
        }

        int len = strlen(c);

        if (c[0] == '\n') {
            putchar('\n');
            process_command(buf);
            break;
        } else if (c[0] == CTRL('D') || c[0] == CTRL('C')) {
            running = false;
        } else if (c[0] == 0x7F || strcmp(c, tigetstr("kbs")) == 0) { // backspace
            if (pos > 0) {
                putp(tigetstr("cub1")); // move left
                putp(tigetstr("dch1")); // delete char
                memmove(buf + pos - 1, buf + pos, strlen(buf) - pos + 1);
                --pos;
            }
        } else if (strcmp(c, tigetstr("kdch1")) == 0) { // delete
            if (pos < strlen(buf)) {
                putp(tigetstr("dch1"));
                memmove(buf + pos, buf + pos + 1, strlen(buf) - pos + 1);
            }
        } else if (strcmp(c, tigetstr("kcub1")) == 0) { // left
            if (pos > 0) {
                putp(tigetstr("cub1")); // move left
                --pos;
            }
        } else if (strcmp(c, tigetstr("kcuf1")) == 0) { // right
            if (pos < strlen(buf)) {
                putp(tigetstr("cuf1")); // move right
                ++pos;
            }
        } else if (c[0] == CTRL('A')) { // ^A
            putp(tparm(tigetstr("cub"), pos));
            pos = 0;
        } else if (c[0] == CTRL('E')) { // ^E
            putp(tparm(tigetstr("cuf"), strlen(buf) - pos));
            pos = strlen(buf);
        } else if (c[0] == CTRL('L')) { // ^L
            putp(tigetstr("clear"));
            shell_prompt();
            printf("%s", buf);
        } else { // any other char
            putp(tigetstr("smir")); // enter insert mode
            //printf(c[0] >= ' ' ? "%c" : "0x%X", c[0]);
            printf("%s", c);
            putp(tigetstr("rmir")); // exit insert mode
            memmove(buf + pos + len, buf + pos, strlen(buf) - pos);
            strcpy(buf + pos, c);
            pos += len;
        }
    }
}

int main()
{
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

    running = true;
    while (running) {
        shell_prompt();
        do_input();
    }

    putp(tigetstr("rmkx")); // disable keypad mode

    rawmode(false);

    //reset_shell_mode();

    puts("Bye");
}
