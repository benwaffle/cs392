#include "my.h"

/**
 * Returns the first index at which the character is found in the string or -1
 * if there is none.
 */
int my_strindex(char *str, char c)
{
    if (str == NULL)
        return -1;

    for (int i = 0; str[i] != '\0'; ++i)
        if (str[i] == c)
            return i;

    return -1;
}
