#include "my.h"

int my_strlen(char *str)
{
    if (!str)
        return -1;

    int len = 0;
    for (; *str; ++str)
        ++len;

    return len;
}
