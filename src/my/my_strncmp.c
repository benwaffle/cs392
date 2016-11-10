#include "my.h"

/**
 * Same as my_strcmp except:
 * Only compares n chars or until the end of a string
 */
int my_strncmp(char *a, char *b, int n)
{
    if (a == NULL && b == NULL)
        return 0;

    if (a == NULL)
        return -1;

    if (b == NULL)
        return 1;

    if (n < 0)
        return 0;

    while (n > 0 && *a == *b) {
        ++a;
        ++b;
        --n;
    }

    if (n == 0)
        return 0;

    return *a - *b;
}
