#include "my.h"
#include <curses.h>
#include <errno.h>
#include <assert.h>
#include <stdlib.h>
#include <limits.h>
#include <signal.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <locale.h> 
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

void sigint_handler(int sig) {}

void run_cmd(const char *cmd)
{
    int socks[2];
    socketpair(AF_UNIX, SOCK_STREAM, 0, socks);

    if (fork() == 0) { // child
        close(socks[0]);
        dup2(socks[1], STDOUT_FILENO);
        dup2(socks[1], STDERR_FILENO);
        execlp(cmd, cmd, NULL);
    } else { // parent
        close(socks[1]);
        char c;
        while (read(socks[0], &c, 1) > 0) {
            printw("%c", c);
        }

        close(socks[0]);
    }
}

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
            printw("Minishell Commands:\n"
                   "\tcd <dir> - change directory\n"
                   "\texit - exit the shell\n"
                   "\thelp - show this help message\n");
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

        mvprintw(getmaxy(stdscr) - 1, 0, "[DEBUG] pos = %d, len = %d, buf = %s", pos, len, buf);
        for (int i=0; i<getmaxx(stdscr); ++i)
            addch(' ');
        move(y, x);

        int c = getch();
        if (c == ERR) {
            running = false;
        } else if (c == '\n') { // enter
            move(y+1, 0);
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
        } else if (c == 0x1) { // ^A
            move(y, x - pos);
            pos = 0;
        } else if (c == 0x5) { // ^E
            move(y, x + (len - pos));
            pos = len;
        } else if (c == 0xc) { // ^L
            clear();
            shell_prompt();
            printw("%s", buf);
            move(0, x);
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

int oldmain()
{
    signal(SIGINT, sigint_handler);
    sigset_t blocked;
    sigemptyset(&blocked);
    // we use sigaction for SA_RESTART
    sigaction(SIGINT,
              &(struct sigaction){
                  // can't use SIG_IGN, it turns this off
                  .sa_handler = sigint_handler,
                  .sa_mask = blocked,
                  .sa_flags = SA_RESTART // restart interrupted signals
              },
              NULL);

    while (true) {
#if 0
        my_str(KBLU);

        char path[PATH_MAX];
        getcwd(path, sizeof path);
        my_str(path);

        my_str(KGRN);
        my_str(" ➜ ");
        my_str(KRST);
#endif

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
    }

    my_str("Bye\n");
    return 0;
}
