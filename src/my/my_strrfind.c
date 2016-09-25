#include "my.h"

/**
 * Returns a pointer to the last char in the string which matches.
 * Return NULL if the letter is not found.
 */
char *my_strrfind(char *str, char c)
{
    int i = my_strrindex(str, c);

    if (i == -1)
        return NULL;

    return str + i;
}
