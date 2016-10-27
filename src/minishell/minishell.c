#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <limits.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include "my.h"

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
        if (msg[size-1] == '\n')
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

pid_t child = 0;

void sigint()
{

}

int main() {
    signal(SIGINT, sigint);

    while (true) {
        my_str("MINISHELL: ");

        char path[PATH_MAX];
        getcwd(path, sizeof path);
        my_str(path);

        my_str(" ➜ ");

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
                my_str("Bye!\n");
                exit(0);
            } else if (my_strcmp(parts[0], "help") == 0) {
                my_str(
                    "Minishell Commands:\n"
                    "\tcd <dir> - change directory\n"
                    "\texit - exit the shell\n"
                    "\thelp - show this help message\n"
                );
            } else { // run command
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
                    child = 1;
                    wait(NULL);
                }
            }
        }

        for (char **p = parts; *p != NULL; ++p)
            free(*p);
        free(parts);
        free(msg);
    }
}
