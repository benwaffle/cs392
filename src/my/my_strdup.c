#include "my.h"

/**
 * Allocates new memory
 * Copies str into that new memory
 * returns pointer to new string
 */
char *my_strdup(char *str)
{
    int len = my_strlen(str);
    char *new = malloc(len + 1);
    my_strcpy(new, str);
    return new;
}
