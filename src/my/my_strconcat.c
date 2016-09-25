#include "my.h"

/**
 *  Allocates new memory
 * Copies concatenated strings in that new memory
 * Returns pointer to it
 * If both NULL return NULL
 */
char *my_strconcat(char *a, char *b)
{
    if (a == NULL && b == NULL)
        return NULL;

    if (a == NULL)
        return my_strdup(b);

    if (b == NULL)
        return my_strdup(a);

    int lena = my_strlen(a);
    int lenb = my_strlen(b);
    char *new = malloc(lena + lenb + 1);
    my_strcpy(new, a);
    my_strcpy(new + lena, b);
    return new;
}
