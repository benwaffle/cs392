#include "my.h"
#include <errno.h>
#include <netdb.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <sys/socket.h>

char *read_stdin()
{
    char *buf = calloc(1, 1024 + 1);
    ssize_t s = read(0, buf, 1024);
    if (s == 0) { // EOF
        free(buf);
        return NULL;
    } else if (s < 0) {
        perror("read");
        free(buf);
        return NULL;
    }

    if (buf[s - 1] == '\n')
        buf[s - 1] = '\0';
    buf[s] = '\0';

    return buf;
}

char *read_socket(int fd)
{
    char *buf = calloc(1, 1024 + 1);
    ssize_t s = recv(fd, buf, 1024, 0);
    if (s == 0) {
        my_str("Server disconnected\n");
        free(buf);
        return NULL;
    } else if (s < 0) {
        perror("recv");
        free(buf);
        return NULL;
    }
    buf[s] = '\0';
    return buf;
}

int running;

void stop_client() { running = 0; }

int main(int argc, char *argv[])
{
    if (argc != 3) {
        my_str("Usage: ");
        my_str(argv[0]);
        my_str(" <host> <port>\n");
        return 1;
    }

    char *host = argv[1];
    int port = my_atoi(argv[2]);

    struct addrinfo hints = {0};
    hints.ai_flags = 0;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;

    struct addrinfo *results;
    int err = getaddrinfo(host, NULL, &hints, &results);
    if (err != 0) {
        my_str((char *)gai_strerror(err));
        my_char('\n');
        return 1;
    }

    struct sockaddr *addr = results[0].ai_addr;
    socklen_t addrlen = results[0].ai_addrlen;

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    ((struct sockaddr_in *)addr)->sin_port = htons(port);

    my_str("Username: ");
    char *username = read_stdin();

    if (connect(sock, addr, addrlen) < 0) {
        perror("connect");
        close(sock);
        return 1;
    }

    send(sock, username, my_strlen(username), 0);

    while (1) {
        char *msg = read_socket(sock);
        if (msg == NULL) {
            return 1;
        }
        char ok[] = "good username";
        if (my_strncmp(msg, ok, sizeof ok - 1) == 0) {
            free(msg);
            break;
        }
        free(msg);

        my_str("That username is already in use. Please try again.\n"
               "Username: ");
        username = read_stdin();

        send(sock, username, my_strlen(username), 0);
    }

    // clang-format off
    struct pollfd fds[2] = {
        {
            .fd = 0, // stdin
            .events = POLLIN,
            .revents = 0
        },
        {
            .fd = sock,
            .events = POLLIN,
            .revents = 0
        }
    };
    // clang-format on

    signal(SIGINT, stop_client);

    running = 1;
    while (running) {
        my_str("> ");

        int res = poll(fds, 2, -1);
        if (res < 0) {
            if (errno == EINTR)
                my_str("Stopping client\n");
            else
                perror("poll");
            break;
        }

        if (fds[0].revents != 0) {
            char *msg = read_stdin();
            if (msg == NULL)
                break;
            if (send(sock, msg, my_strlen(msg), 0) < 0) {
                perror("send");
                break;
            }
            free(msg);
        } else {
            char *msg = read_socket(sock);
            if (msg == NULL) // error
                break;
            my_char('\r'); // erase prompt
            my_str(msg);
            my_char('\n');
            free(msg);
        }
    }

    freeaddrinfo(results);
    close(sock);
}
