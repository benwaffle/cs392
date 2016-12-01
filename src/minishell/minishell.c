#include "my.h"
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

// colors
#define KRST "\x1B[0m"
#define KRED "\x1B[31m"
#define KGRN "\x1B[32m"
#define KYEL "\x1B[33m"
#define KBLU "\x1B[34m"
#define KMAG "\x1B[35m"
#define KCYN "\x1B[36m"
#define KWHT "\x1B[37m"

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

int main()
{
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
        my_str(KBLU);

        char path[PATH_MAX];
        getcwd(path, sizeof path);
        my_str(path);

        my_str(KGRN);
        my_str(" ➜ ");
        my_str(KRST);

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
}
