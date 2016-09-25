#include "my.h"

/**
 * Copies the characters from src into dst overwriting what's already in there.
 * Always copies a '\0'
 * Assumes dst has enough memory allocated for the new string if it is longer.
 */
char *my_strcpy(char *dst, char *src)
{
    if (dst == NULL)
        return NULL;

    if (src == NULL)
        return dst;

    int i;

    for (i = 0; src[i] != '\0'; ++i)
        dst[i] = src[i];

    dst[i] = '\0';

    return dst;
}
