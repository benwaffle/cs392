#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

int main()
{
    int enc = open("encrypted.txt", O_RDONLY);
    if (enc < 0) {
        perror("encrypted.txt");
        return 1;
    }

    int dec = open("solution.txt", O_WRONLY | O_CREAT,
                   S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

    int key = 5;

    char buf[3];
    int bytes;
    while ((bytes = read(enc, buf, 3)) > 0) {
        for (int i = 0; i < bytes; ++i) {
            if (buf[i] != ' ' && buf[i] != '!') {
                if ('A' <= buf[i] && buf[i] <= 'Z') // caps
                    buf[i] = ((buf[i] - 'A') - key + 26) % 26 + 'A';
                else
                    buf[i] = ((buf[i] - 'a') - key + 26) % 26 + 'a';
            }
        }

        write(dec, buf, bytes);

        key += 2;
    }

    close(enc);
    close(dec);
}
