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

bool running = 0;

// read an arbitrary-length string from fd
char *read_str(int fd)
{
    ssize_t ret, size = 4, oldsize = 4, append_off = 0;
    char *msg = calloc(size, 1);
    if (msg == NULL) {
        perror("calloc");
        exit(1);
    }

    while ((ret = read(0, msg + append_off, oldsize)) == oldsize) {
        if (msg[size - 1] == '\n')
            break;
        oldsize = append_off = size;
        msg = realloc(msg, size *= 2);
        if (msg == NULL) {
            perror("realloc");
            exit(1);
        }
    }
    if (ret == 0) { // EOF (^D)
        return NULL;
    } else if (ret < 0) {
        perror("read");
        exit(1);
    }
    append_off += ret - 1;
    msg[append_off] = '\0';
    return msg;
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

#define DEBUGX 0
#define DEBUGY tigetnum("lines")-1

void do_input()
{
    char buf[1024] = {0};
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

        if (c[0] == CTRL('D')) { // ^D
            running = false;
        } else if (strcmp(c, tigetstr("kbs")) == 0 ||
                   c[0] == 0x7F) { // backspace
            if (pos > 0) {
                putp(tigetstr("cub1")); // move left
                putp(tigetstr("dch1")); // delete char
                memmove(buf + pos - 1, buf + pos, strlen(buf) - pos + 1);
                --pos;
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
        } else {
            putchar(c[0]);
            memmove(buf + pos + 1, buf + pos, strlen(buf) - pos);
            buf[pos++] = c[0];
        }
    }
}

//void sigint_handler(int sig) {}

int main()
{
    setvbuf(stdout, NULL, _IONBF, 0);

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

    // put terminal in raw mode
    struct termios old;
    tcgetattr(1, &old);
    struct termios raw = old;
    cfmakeraw(&raw);
    tcsetattr(1, TCSANOW, &raw);

    putp(tigetstr("smkx")); // keypad mode

    running = true;
    while (running) {
        shell_prompt();
        do_input();

#if 0
        char *msg = read_str(0);
        if (msg == NULL)
            break;
        char **parts = my_str2vect(msg);

        if (parts[0] != NULL) { // non empty
            if (my_strcmp(parts[0], "cd") == 0) {
                int r = chdir(parts[1] == NULL ? getenv("HOME") : parts[1]);
                if (r < 0) {
                    perror("cd");
                }
            } else if (my_strcmp(parts[0], "exit") == 0) {
                break;
            } else if (my_strcmp(parts[0], "help") == 0) {
                my_str("Minishell Commands:\n"
                       "\tcd <dir> - change directory\n"
                       "\texit - exit the shell\n"
                       "\thelp - show this help message\n");
            } else { // run command
                pid_t child;
                if ((child = fork()) < 0) {
                    perror("fork");
                    exit(1);
                } else if (child == 0) { // child
                    if (execvp(parts[0], parts) < 0) {
                        if (errno == ENOENT) {
                            my_str(parts[0]);
                            my_str(": no such command\n");
                        } else {
                            perror("execvp");
                        }
                        exit(1);
                    }
                    exit(0);
                } else {
                    if (wait(NULL) < 0) {
                        perror("wait");
                    }
                }
            }
        }

        for (char **p = parts; *p != NULL; ++p)
            free(*p);
        free(parts);
        free(msg);
#endif
    }

    putp(tigetstr("rmkx")); // disable keypad mode

    tcsetattr(1, TCSANOW, &old);

    puts("Bye");
}
