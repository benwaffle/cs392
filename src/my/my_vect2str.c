#include "my.h"
#include <stdio.h>

char *my_vect2str(char **x)
{
    if (x == NULL)
        return NULL;

    if (*x == NULL)
        return my_strdup("");

    int len = 1;
    for (char **y = x; *y != NULL; y++) {
        len += 1 + my_strlen(*y);
    }
    len--; // last space

    char *s = calloc(len, 1);
    for (char **y = x; *y != NULL; ++y) {
        my_strcat(s, *y);
        s[my_strlen(s)] = ' ';
    }
    s[len - 1] = '\0';
    return s;
}
