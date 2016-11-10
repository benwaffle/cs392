#include "my.h"

/**
 * Same as my_strcpy except:
 * Only copies n chars or until the end of src
 */
char *my_strncpy(char *dst, char *src, int n)
{
    if (src == NULL || dst == NULL)
        return dst;

    int i;

    for (i = 0; src[i] != '\0' && n > 0; ++i, --n)
        dst[i] = src[i];

    dst[i] = '\0';

    return dst;
}
