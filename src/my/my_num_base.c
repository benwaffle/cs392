#include "my.h"

void my_num_base(int num_int, char *chars)
{
    if (!chars || !*chars) {
        my_str("Error: invalid alphabet");
        return;
    }

    long num = num_int; // for min int

    int base = my_strlen(chars);

    // unary
    if (base == 1) {
        for (int i=0; i<num; ++i)
            my_char(chars[0]);
        return;
    }

    // zero
    if (num == 0) {
        my_char(chars[0]);
        return;
    }

    // negative
    if (num < 0) {
        my_char('-');
        num = -num;
    }

    char str[32]; // unary is handled above, so MAX_INT in base 2 is 31 bits wide
    int i = 0;
    while (num > 0) {
        str[i++] = chars[num % base];
        num /= base;
    }
    str[i] = '\0';

    my_revstr(str);
    my_str(str);
}
