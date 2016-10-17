#include "my.h"
#include "list.h"
#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <signal.h>

int init_server(int port)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(1);
    }
    
    // bind: address already in use
    int opt = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt) < 0) {
        perror("setsockopt");
        exit(1);
    }

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr.s_addr = INADDR_ANY
    };

    if (bind(sock, (const struct sockaddr *)&addr, sizeof addr) < 0) {
        perror("bind");
        exit(1);
    }

    if (listen(sock, 5) < 0) {
        perror("listen");
        close(sock);
        exit(1);
    }

    return sock;
}

struct s_node *clients = NULL;
typedef struct {
    int fd;
    char *username;
} client;

void send_msg(const char *msg, int len)
{
    my_str("Sending `");
    my_str((void*)msg);
    my_str("'\n");

    for (struct s_node *c = clients; c != NULL; c = c->next)
        send(((client*)c->elem)->fd, msg, len, 0);
}

void handle_client(int fd)
{
    my_str("["); my_int(fd); my_str("] ");
    my_str("event\n");

    int i = 0;
    for (struct s_node *x = clients; x != NULL; x = x->next, ++i)
        if (fd == ((client*)x->elem)->fd)
            break;
    client *c = elem_at(clients, i);

    char buf[1024];
    ssize_t s = recv(fd, buf, sizeof buf, 0);
    if (s == 0) {
        my_str("["); my_int(fd); my_str("] ");
        my_str("socket closed\n");

        remove_node_at(&clients, i);
        close(fd);
        char *msg = my_strconcat(c->username, " left");
        send_msg(msg, my_strlen(msg));
        free(msg);
        free(c);

        return;
    }
    buf[s] = '\0';

    if (c->username == NULL) {
        my_str("["); my_int(fd); my_str("] ");
        my_str("username = ");
        my_str(buf);
        my_char('\n');

        c->username = my_strdup(buf);
        char *msg = my_strconcat(c->username, " joined");
        send_msg(msg, my_strlen(msg));
        free(msg);
        return;
    }

    if (buf[0] == '/') {
        if (my_strncmp(buf + 1, "nick ", 5) == 0) {
            free(c->username);
            c->username = my_strdup(buf + 6);
        } else if (my_strncmp(buf + 1, "exit", 4) == 0) {
            remove_node_at(&clients, i);
            close(fd);
            char *msg = my_strconcat(c->username, " left");
            send_msg(msg, my_strlen(msg));
            free(msg);
            free(c);
        } else if (my_strncmp(buf + 1, "me ", 3) == 0) {
            char *p1 = my_strconcat(c->username, " ");
            char *msg = my_strconcat(p1, buf + 4);
            send_msg(msg, my_strlen(msg));
            free(p1);
            free(msg);
       } else {
           char msg[] = "Error: invalid command";
           send(c->fd, msg, sizeof msg, 0);
       }
    } else {
        char *p1 = my_strconcat(c->username, ": ");
        char *msg = my_strconcat(p1, buf);
        send_msg(msg, my_strlen(msg));
        free(p1);
        free(msg);
    }
}

void handle_server(int sock)
{
    struct sockaddr_in addr;
    socklen_t len = sizeof addr;

    int newfd = accept(sock, (struct sockaddr *)&addr, &len);

    char ip[INET_ADDRSTRLEN];
    inet_ntop(addr.sin_family, &addr.sin_addr, ip, sizeof ip);

    my_str("["); my_int(newfd); my_str("] ");
    my_str("new connection from ");
    my_str(ip);
    my_char('\n');

    client *c = calloc(1, sizeof *c);
    c->fd = newfd;
    add_elem(c, &clients);
}

int running;

void stop_server() {
    /*
     * There is a race condition between checking this flag and calling poll()
     * The solution is to use ppoll(2) or signalfd(2)
     */
    running = 0;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        my_str("Usage: ");
        my_str(argv[0]);
        my_str(" <port>\n");
        return 1;
    }

    int port = my_atoi(argv[1]);
    int sock = init_server(port);

    signal(SIGINT, stop_server);

    running = 1;
    while (running) {
        nfds_t len = count_s_nodes(clients) + 1;
        struct pollfd fds[len];
        // server
        fds[0] = (struct pollfd){
            .fd = sock,
            .events = POLLIN,
            .revents = 0
        };

        int i = 0;
        for (struct s_node *cl = clients; cl != NULL; cl = cl->next, ++i) {
            fds[1 + i] = (struct pollfd){
                .fd = ((client*)cl->elem)->fd,
                .events = POLLIN,
                .revents = 0
            };
        }

        int res = poll(fds, len, -1);

        if (res < 0) {
            if (errno == EINTR) // SIGINT
                my_str("Stopping server\n");
            else
                perror("poll");
            break;
        }

        for (int i = 0; i < len; ++i) {
            if (fds[i].revents != 0) {
                if (i == 0)
                    handle_server(fds[0].fd);
                else
                    handle_client(fds[i].fd);
            }
        }
    }

    for (struct s_node *cl = clients; cl != NULL; cl = cl->next) {
        client *c = cl->elem;
        close(c->fd);
        free(c->username);
    }
    empty_list(&clients);
    close(sock);
}
