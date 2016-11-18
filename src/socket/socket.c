#define _GNU_SOURCE // inet_aton()
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <host> <port> <message>\n", argv[0]);
        return 1;
    }

    char *host = argv[1];
    int port = atoi(argv[2]);
    char *msg = argv[3];

    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    struct in_addr inp;
    inet_aton(host, &inp);

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr = inp;

    connect(sock, (struct sockaddr *)&addr, sizeof(addr));

    int len = strlen(msg);

    write(sock, msg, len);

    char buf[len + 1];
    memset(buf, 0, len + 1);
    ssize_t x = read(sock, &buf, len);
    if (x < 0) {
        perror("read");
        return 1;
    }
    printf("%s\n", buf);

    memset(buf, 0, len + 1);
    x = read(sock, &buf, sizeof(buf));
    if (x < 0) {
        perror("read");
        return 1;
    }
    printf("%s\n", buf);

    close(sock);
}
