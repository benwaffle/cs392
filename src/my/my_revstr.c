#include "my.h"

/**
 * Reverse the string, directly in the actual string, without creating a new
 * string. Return the length of the string.
 */
int my_revstr(char *str)
{
    if (str == NULL)
        return -1;

    int len = my_strlen(str);
    for (int i = 0; i < len / 2; ++i) {
        char tmp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = tmp;
    }
    return len;
}
