#include "my.h"

int my_strcmp(char *a, char *b)
{
    int an = my_strlen(a);
    int bn = my_strlen(b);
    int min = an > bn ? an : bn;

    return my_strncmp(a, b, min);
}
