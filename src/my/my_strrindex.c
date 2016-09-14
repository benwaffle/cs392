#include "my.h"

int my_strrindex(char *str, char c)
{
    if (!str)
        return -1;

    int len = my_strlen(str);

    for (int i = len-1; i >= 0; --i)
        if (str[i] == c)
            return i;

    return -1;
}
