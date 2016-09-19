#include "my.h"

/**
 * Prints a number using the length of the string as the base and the contents
 * as the alphabet. For example, if you called my_num_base(9, "RTFM"), then
 * R = 0, T = 1, F = 2, M = 3. 9 in base 4 is 21 and so the result is printed
 * out as "FT".
 *
 * If char* is NULL or empty, print an error message and return.
 * If given unary, repeat alphabet letter the specified number of times.
 * For negatives print a '-' and then the number.
 */
void my_num_base(int num_int, char *chars)
{
    if (!chars || !*chars) {
        my_str("Error: invalid alphabet");
        return;
    }

    long num = num_int; // for min int

    int base = my_strlen(chars);

    // negative
    if (num < 0) {
        my_char('-');
        num = -num;
    }

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
