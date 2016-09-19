#include "my.h"

/**
 * Calculates the length of a string (not including the null terminator).
 * Length of NULL is -1.
 */
int my_strlen(char *str)
{
    if (!str)
        return -1;

    int len = 0;
    for (; *str; ++str)
        ++len;

    return len;
}
