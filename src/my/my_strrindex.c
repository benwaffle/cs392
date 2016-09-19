#include "my.h"

/**
 * Returns the last index at which the character is found in the string or -1
 * if there is none.
 */
int my_strrindex(char *str, char c)
{
    if (str == NULL)
        return -1;

    int len = my_strlen(str);

    for (int i = len-1; i >= 0; --i)
        if (str[i] == c)
            return i;

    return -1;
}
