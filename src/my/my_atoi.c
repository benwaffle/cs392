#include "my.h"

/**
 * Returns the int represented by the ascii string
 * Handles negatives.
 * Ignores preceding characters and trailing numbers and chars
 * Examples:
 * "5" => 5
 * "-5" => -5
 * "--5" => 5
 * "a-b54sc7-d" => -54
 * "abcd" => 0
 */
int my_atoi(char *str)
{
    if (str == NULL)
        return 0;

    int neg = 1;
    int res = 0;

    // consume non-numbers
    while (!('0' <= *str && *str <= '9') && *str != '\0') {
        if (*str == '-')
            neg *= -1;

        str++;
    }

    while (*str != '\0') {
        if ('0' <= *str && *str <= '9') {
            res *= 10;
            res += *str - '0';
        } else {
            break;
        }

        str++;
    }

    return neg * res;
}
