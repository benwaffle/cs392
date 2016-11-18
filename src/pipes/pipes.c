#include "my.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define CHECK(x)                                                               \
    if ((x) < 0) {                                                             \
        perror(#x);                                                            \
        return 1;                                                              \
    }

int main(int argc, char *argv[])
{
    if (argc <= 1) {
        fprintf(stderr, "Usage: %s <message>\n", argv[0]);
        return 1;
    }

    // 0 - read, 1 - write
    int pipe1[2];
    int pipe2[2];

    CHECK(pipe(pipe1))
    CHECK(pipe(pipe2))

    pid_t child;
    pid_t gchild;

    if ((child = fork()) < 0) {
        perror("fork");
        return 1;
    } else if (child > 0) { // parent
        CHECK(close(pipe1[0]))
        int wfd = pipe1[1];

        char *msg = my_vect2str(argv + 1);
        printf("Parent: %s\n", msg);
        CHECK(write(wfd, msg, my_strlen(msg)))
        free(msg);

        CHECK(close(wfd))
        CHECK(wait(NULL))
    } else { // child
        if ((gchild = fork()) < 0) {
            perror("fork");
            return 1;
        } else if (gchild > 0) { // child
            CHECK(close(pipe1[1]))
            CHECK(close(pipe2[0]))
            int rfd = pipe1[0];
            int wfd = pipe2[1];

            char msg[100] = {0};
            ssize_t size;
            CHECK(size = read(rfd, &msg, 100))
            printf("Child: %s\n", msg);
            CHECK(write(wfd, msg, size))

            CHECK(close(rfd))
            CHECK(close(wfd))
            CHECK(wait(NULL))
        } else { // grandchild
            CHECK(close(pipe1[0]))
            CHECK(close(pipe1[1]))
            CHECK(close(pipe2[1]))
            int rfd = pipe2[0];

            char msg[100] = {0};
            CHECK(read(rfd, &msg, 100))
            printf("Grandchild: %s\n", msg);
            CHECK(close(rfd))
        }
    }
}
