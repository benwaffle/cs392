#include "my.h"

/**
 * Prints a NULL terminated C string to the console. Do nothing if given NULL
 */
void my_str(char *str)
{
    if (str == NULL)
        return;

    for (; *str; ++str)
        my_char(*str);
}
