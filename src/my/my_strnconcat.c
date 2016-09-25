#include "my.h"

/**
 * Same as my_strconcat except:
 * Copies all of a and then n chars or length of b
 */
char *my_strnconcat(char *a, char *b, int n)
{
    if (a == NULL && b == NULL)
        return NULL;

    if (a == NULL) {
        int len = my_strlen(b) < n ? my_strlen(b) : n;
        char *new = malloc(len + 1);
        my_strncpy(new, b, len);
        new[len] = '\0';
        return new;
    }

    if (b == NULL)
        return my_strdup(a);

    int lena = my_strlen(a);
    int lenb = my_strlen(b) < n ? my_strlen(b) : n;

    char *new = malloc(lena + lenb + 1);
    my_strcpy(new, a);
    my_strncpy(new + lena, b, lenb);
    new[lena + lenb] = '\0';
    return new;
}
