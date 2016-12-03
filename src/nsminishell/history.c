#include "list.h"
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFSIZE 4096

extern struct s_node *history;

void load_history()
{
    char histpath[PATH_MAX] = {0};
    sprintf(histpath, "%s/.nsmshistory", getenv("HOME"));

    // crete history file if it doesn't exist
    if (access(histpath, R_OK | W_OK) < 0) {
        if (errno == ENOENT) { // doesn't exist
            int fd = creat(histpath, 0600);
            if (fd < 0) {
                perror("creating ~/.nsmshistory");
                exit(1);
            }
            close(fd);
        } else {
            perror("~/.nsmshistory");
            exit(1);
        }
    }

    // read the history file into a string
    FILE *histfile = fopen(histpath, "r+");
    assert(histfile != NULL);

    while (true) {
        char *line = calloc(1, BUFSIZE);
        if (fgets(line, BUFSIZE, histfile) == NULL) {
            free(line);
            break;
        }
        line[strlen(line) - 1] = '\0'; // get rid of '\n'
        append(new_node(line, NULL, NULL), &history);
    }

#if 0
    printf("History:\n");
    for (struct s_node *line = history; line != NULL; line = line->next)
        printf("\t%s\n", (char *)line->elem);
#endif
}

void save_history()
{
    char histpath[PATH_MAX] = {0};
    sprintf(histpath, "%s/.nsmshistory", getenv("HOME"));

    FILE *histfile = fopen(histpath, "w");
    assert(histfile != NULL);

    for (struct s_node *line = history; line != NULL; line = line->next)
        if (fprintf(histfile, "%s\n", (char *)line->elem) <= 0)
            printf("Error writing to history file: %s\n",
                   strerror(ferror(histfile)));

    fclose(histfile);
}
