#include "my.h"
#include <assert.h>
#include <curses.h>
#include <errno.h>
#include <limits.h>
#include <locale.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

// colors
#define KRST 1
#define KRED 2
#define KGRN 3
#define KYEL 4
#define KBLU 5
#define KMAG 6
#define KCYN 7
#define KWHT 8

#define CTRL(c) ((c)-64)

bool running = true;

void make_colors()
{
    init_pair(KRST, -1, -1);
    init_pair(KRED, COLOR_RED, -1);
    init_pair(KGRN, COLOR_GREEN, -1);
    init_pair(KYEL, COLOR_YELLOW, -1);
    init_pair(KBLU, COLOR_BLUE, -1);
    init_pair(KMAG, COLOR_MAGENTA, -1);
    init_pair(KCYN, COLOR_CYAN, -1);
    init_pair(KWHT, COLOR_WHITE, -1);
}

typedef void (*on_output)(const char *, void *);

void run_cmd(char *const *cmd, on_output fn, void *data)
{
    int socks[2];
    socketpair(AF_UNIX, SOCK_STREAM, 0, socks);

    if (fork() == 0) { // child
        close(socks[0]);

        dup2(socks[1], STDIN_FILENO);
        dup2(socks[1], STDOUT_FILENO);
        dup2(socks[1], STDERR_FILENO);
        if (execvp(cmd[0], cmd) < 0) {
            if (errno == ENOENT)
                printf("%s: no such command\n", cmd[0]);
            else
                perror("execvp");
        }
        exit(0);
    } else { // parent
        close(socks[1]);

        char str[1024] = {0};
        int r;
        while ((r = read(socks[0], str, sizeof(str) - 1)) > 0) {
            fn(str, data);
        }

        if (wait(NULL) < 0) {
            printw("wait: %s\n", strerror(errno));
        }

        close(socks[0]);
    }
}

void print_str(const char *str, void *data) { printw("%s", str); }

void process_command(char *cmd)
{
    char **parts = my_str2vect(cmd);

    // TODO fix quotes

    if (parts[0]) {
        if (strcmp(parts[0], "cd") == 0) {
            int r = chdir(parts[1] == NULL ? getenv("HOME") : parts[1]);
            if (r < 0) {
                printw("cd: %s: %s\n", strerror(errno), parts[1]);
            }
        } else if (strcmp(parts[0], "exit") == 0) {
            running = false;
        } else if (strcmp(parts[0], "help") == 0) {
            printw("Not-So-Minishell Commands:\n"
                   "\tcd <dir> - change directory\n"
                   "\texit - exit the shell\n"
                   "\thelp - show this help message\n");
        } else {
            run_cmd(parts, print_str, NULL);
        }
    }

    for (char **p = parts; *p != NULL; ++p)
        free(*p);
    free(parts);
}

void shell_prompt()
{
    char path[PATH_MAX];
    getcwd(path, sizeof path);

    color_set(KGRN, NULL);
    printw("%s ", path);
    color_set(KBLU, NULL);
    printw("➜ ", path);
    color_set(KRST, NULL);
}

void do_input()
{
    char buf[1024] = {0};
    int len = 0;
    int pos = 0;
    while (running) {
        int y, x;
        getyx(stdscr, y, x);

        mvprintw(getmaxy(stdscr) - 1, 0, "[DEBUG] pos = %d, len = %d, buf = %s",
                 pos, len, buf);
        for (int i = 0; i < getmaxx(stdscr); ++i)
            addch(' ');
        move(y, x);

        int c = getch();
        if (c == ERR) {
            running = false;
        } else if (c == '\n') { // enter
            move(y + 1, 0);
            process_command(buf);
            break;
        } else if (c == KEY_BACKSPACE || c == 0x7F) { // backspace
            if (pos > 0) {
                mvdelch(y, x - 1);
                memmove(buf + pos - 1, buf + pos, len - pos + 1);
                pos--;
                len--;
            }
        } else if (c == KEY_LEFT) { // left
            if (pos > 0) {
                move(y, x - 1);
                pos--;
            }
        } else if (c == KEY_RIGHT) { // right
            if (pos < len) {
                move(y, x + 1);
                pos++;
            }
        } else if (c == CTRL('A')) { // ^A
            move(y, x - pos);
            pos = 0;
        } else if (c == CTRL('E')) { // ^E
            move(y, x + (len - pos));
            pos = len;
        } else if (c == CTRL('L')) { // ^L
            clear();
            shell_prompt();
            printw("%s", buf);
            move(0, x);
        } else if (c == CTRL('D')) { // ^D
            running = false;
        } else if (c > 0x1F) { // insert char, don't print control chars
            // TODO fix text wrapping
            insch(c);
            move(y, x + 1);
            memmove(buf + pos + 1, buf + pos, len - pos);
            len++;
            buf[pos++] = (char)c;
        }
    }
}

int main()
{
    setlocale(LC_ALL, "");
    initscr();
    raw();
    noecho();
    keypad(stdscr, TRUE);

    start_color();
    use_default_colors();
    make_colors();

    while (running) {
        shell_prompt();
        do_input();
    }

    endwin();
}
