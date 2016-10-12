#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "my.h"

int main(int argc, char *argv[])
{
    if (argc <= 1) {
        fprintf(stderr, "Usage: %s <message>\n", argv[0]);
        return 1;
    }

    // 0 - read, 1 - write
    int pipe1[2];
    int pipe2[2];

    if (pipe(pipe1) < 0) {
        perror("pipe");
        return 1;
    }
    if (pipe(pipe2) < 0) {
        perror("pipe");
        return 1;
    }

    pid_t child;
    pid_t gchild;

    if ((child = fork()) < 0) {
        perror("fork");
        return 1;
    } else if (child > 0) { // parent
        close(pipe1[0]);
        int wfd = pipe1[1];

        char *msg = my_vect2str(argv+1);
        printf("Parent: %s\n", msg);
        write(wfd, msg, my_strlen(msg));
        free(msg);

        close(wfd);
        if (wait(NULL) < 0) {
            perror("wait");
            return 1;
        }
    } else { // child
        if ((gchild = fork()) < 0) {
            perror("fork");
            return 1;
        } else if (gchild > 0) { // child
            close(pipe1[1]);
            close(pipe2[0]);
            int rfd = pipe1[0];
            int wfd = pipe2[1];

            char msg[100] = {0};
            ssize_t size = read(rfd, &msg, 100);
            printf("Child: %s\n", msg);
            write(wfd, msg, size);

            close(rfd);
            close(wfd);
            if (wait(NULL) < 0) {
                perror("wait");
                return 1;
            }
        } else { // grandchild
            close(pipe1[0]);
            close(pipe1[1]);
            close(pipe2[1]);
            int rfd = pipe2[0];

            char msg[100] = {0};
            read(rfd, &msg, 100);
            printf("Grandchild: %s\n", msg);
            close(rfd);
        }
    }
}
