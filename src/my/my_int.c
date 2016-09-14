#include "my.h"

void my_int(int i)
{
    // because i = -i doesn't change it here
    if (i == -2147483648) {
        my_str("-2147483648");
        return;
    }

    // because this will just be ""
    if (i == 0) {
        my_char('0');
        return;
    }

    if (i < 0) {
        my_char('-');
        i = -i;
    }

    int print = 0;
    for (int z = 1e9; z != 0; i %= z, z /= 10) {
        int div = i / z;
        if (!print && div > 0)
            print = 1;
        if (print)
            my_char(div + '0');
    }
}
