#define _GNU_SOURCE
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// read an arbitrary-length string from fd
char *read_str(int fd)
{
    ssize_t ret, size = 4, oldsize = 4, append_off = 0;
    char *msg = calloc(size, 1);
    if (msg == NULL) {
        perror("calloc");
        exit(1);
    }

    while ((ret = read(fd, msg + append_off, oldsize)) == oldsize) {
        oldsize = append_off = size;
        msg = realloc(msg, size *= 2);
        if (msg == NULL) {
            perror("realloc");
            exit(1);
        }
    }
    if (ret <= 0) {
        perror("read");
        exit(1);
    }
    append_off += ret - 1;
    msg[append_off] = '\0';
    return msg;
}

void sigusr1(int sig)
{
    printf("pausing");
    pause();
    printf("resuming");
}

void nothing() {}

void worker(int fd)
{
    while (true) {
        char *file = read_str(fd);
        printf("reading %s\n", file);

        int f = open(file, O_RDONLY);
        if (f < 0) {
            printf("no such file\n");
            size_t len = 0;
            write(fd, &len, sizeof len);
        } else {
            struct stat st;
            stat(file, &st);
            size_t len = st.st_size;
            write(fd, &len, sizeof len);
            sendfile(fd, f, NULL, len);
            close(f);
        }
    }
}

int main()
{
    int socks[2];
    socketpair(AF_UNIX, SOCK_STREAM, 0, socks);
    pid_t child = fork();
    if (child < 0) {
        perror("fork");
        return 1;
    } else if (child == 0) { // child
        close(socks[0]);
        worker(socks[1]);
        return 0;
    } else {
        close(socks[1]);
    }

    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGUSR2); // only allow SIGUSR2

    sigaction(SIGUSR1, &(struct sigaction){.sa_handler = sigusr1,
                                           .sa_mask = mask,
                                           .sa_flags = SA_RESTART},
              NULL);

    signal(SIGUSR2, nothing);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    // bind: address already in use
    int opt = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt) < 0) {
        perror("setsockopt");
        return 1;
    }

    struct sockaddr_in addr = {.sin_family = AF_INET,
                               .sin_port = htons(8080),
                               .sin_addr.s_addr = INADDR_ANY};

    if (bind(sock, (const struct sockaddr *)&addr, sizeof addr) < 0) {
        perror("bind");
        return 1;
    }

    if (listen(sock, 5) < 0) {
        perror("listen");
        close(sock);
        return 1;
    }

    while (true) {
        int client = accept(sock, NULL, NULL);

        char *msg = read_str(client);
        if (strncmp(msg, "GET /", 5) == 0) {
            char *file = msg + 5;
            char *end = file;
            while (*end != ' ')
                ++end;
            *end = '\0';
            if (strlen(file) == 0)
                file = "index.html";

            char *fullpath;
            asprintf(&fullpath, "/srv/http/%s", file);
            write(socks[0], fullpath, strlen(fullpath) + 1);
            size_t len;
            read(socks[0], &len, sizeof len);
            printf("len = %lu\n", len);
            if (len == 0) {
                char msg[] = "HTTP/1.1 404 Not Found\r\n"
                             "Content-Type: text/html\r\n"
                             "\r\n"
                             "error 404\r\n";
                write(client, msg, sizeof msg);
            } else {
                char msg[] = "HTTP/1.1 200 OK\r\n"
                             "Content-Type: text/html\r\n"
                             "\r\n";
                write(client, msg, sizeof msg);
                char c[len];
                read(socks[0], &c, len);
                write(client, &c, len);
            }
            free(fullpath);
        } else {
            char msg[] = "HTTP/1.1 400 Bad Request\r\n"
                         "Content-Type: text/html\r\n"
                         "\r\n"
                         "bad request";
            write(client, msg, sizeof msg);
        }
        free(msg);

        close(client);
    }

    close(sock);
}
