#include "my.h"

/**
 * Reverse the string, directly in the actual string, without creating a new
 * string. Return the length of the string.
 */
char *my_strfind(char *str, char c)
{
    int i = my_strindex(str, c);

    if (i == -1)
        return NULL;

    return str + i;
}
