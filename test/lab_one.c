#include "my.h"

int main()
{
    for (int i=1; i<=100; ++i) {
        if (i%3 == 0 && i%5 == 0)
            my_str("FB");
        else if (i%3 == 0)
            my_char('F');
        else if (i%5 == 0)
            my_char('B');
        else
            my_int(i);
    }
}
