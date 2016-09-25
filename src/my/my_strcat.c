#include "my.h"

/**
 * Copies src onto the end of dst and returns dst
 * Does not allocate memory
 * Assumes there is enough memory allocated in dst to hold both strings
 * Overwrites src's original '\0' and places a new '\0' onto the end
 */
char *my_strcat(char *dst, char *src)
{
    if (dst == NULL)
        return NULL;

    char *cat = dst;

    while (*cat != '\0')
        cat++;

    my_strcpy(cat, src);

    return dst;
}
