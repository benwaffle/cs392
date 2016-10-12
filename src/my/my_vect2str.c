#include "my.h"

char *my_vect2str(char **x)
{
    int len = my_strlen(*x);
    for (char **y = x+1; *y != NULL; y++) {
        len += 1 + my_strlen(*y);
    }
    len++; // null terminator

    char *s = malloc(len);
    my_strcpy(s, *x);
    for (char **y = x+1; *y != NULL; ++y) {
        my_strcat(s, " ");
        my_strcat(s, *y);
    }
    return s;
}
