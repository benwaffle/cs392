#include "my.h"

void my_str(char *str)
{
    if (!str)
        return;

    for (; *str; ++str)
        my_char(*str);
}
