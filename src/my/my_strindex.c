#include "my.h"

int my_strindex(char *str, char c)
{
    if (!str)
        return -1;

    for (int i = 0; str[i] != '\0'; ++i)
        if (str[i] == c)
            return i;

    return -1;
}
