#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include "my.h"

char *my_vect2str(char **x)
{
    int len = my_strlen(*x);
    for (char **y = x+1; *y != NULL; y++) {
        len += 1 + my_strlen(*y);
    }

    char *s = malloc(len);
    my_strcpy(s, *x);
    for (char **y = x+1; *y != NULL; ++y) {
        my_strcat(s, " ");
        my_strcat(s, *y);
    }
    return s;
}

int main(int argc, char *argv[])
{
    if (argc <= 1) {
        fprintf(stderr, "Usage: %s <message>\n", argv[0]);
        return 1;
    }

    int pipefd1[2]; // 0 - read, 1 - write
    pipe(pipefd1);

    pid_t child;
    if ((child = fork()) < 0) {
        perror("fork");
        return 1;
    } else if (child > 0) { // parent
        close(pipefd1[0]);
        char *msg = my_vect2str(argv+1);
        printf("Parent: %s\n", msg);
        write(pipefd1[1], msg, my_strlen(msg));
        close(pipefd1[1]);
    } else { // child
        close(pipefd1[1]);

        int pipefd2[2]; // 0 - read, 1 - write
        pipe(pipefd2);
        pid_t gchild;

        if ((gchild = fork()) < 0) {
            perror("fork");
            return 1;
        } else if (gchild > 0) { // child
            close(pipefd2[0]); 
            char msg[100];
            ssize_t size = read(pipefd1[0], &msg, 100);
            msg[size] = '\0';
            printf("Child: %s\n", msg);
            write(pipefd2[1], msg, size);
            close(pipefd2[1]);
        } else { // grandchild
            close(pipefd2[1]); 
            char msg[100];
            ssize_t size = read(pipefd2[0], &msg, 100);
            msg[size] = '\0';
            printf("Grandchild: %s\n", msg);
            close(pipefd2[0]);
        }
    }
}
