#include "my.h"

/**
 *  Compares strings by ascii value
 * If a and b are identical, return 0.
 * if a < b, return negative number
 * if a > b, return positive number
 * Two NULL are equal.
 * NULL is always less than a normal string
 */
int my_strcmp(char *a, char *b)
{
    int an = my_strlen(a);
    int bn = my_strlen(b);
    int min = an > bn ? an : bn;

    return my_strncmp(a, b, min);
}
