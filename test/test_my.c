#include <stdio.h>
#include "my.h"

#define TEST(code) { \
    my_str(#code); \
    my_str(": "); \
    code; \
    my_str("\n"); \
}

int main()
{
    my_str("------- test_my --------\n\n");

    TEST(my_char('A'));
    TEST(my_char('Z'));
    TEST(my_char('a'));
    TEST(my_char('z'));
    TEST(my_char(0));

    my_char('\n');
    TEST(my_str(NULL));
    TEST(my_str("h"));
    TEST(my_str("Hello world"));
    TEST(my_str("just a\0test"));

    my_char('\n');
    TEST(my_int(0));
    TEST(my_int(-1));
    TEST(my_int(1));
    TEST(my_int(-42));
    TEST(my_int(10));
    TEST(my_int(53));
    TEST(my_int(100));
    TEST(my_int(120304));
    TEST(my_int(1234567890));
    TEST(my_int(2147483647)); // max int
    TEST(my_int(-2147483647));
    TEST(my_int(-2147483648)); // min int

    my_char('\n');
    TEST(my_num_base(42, ""));
    TEST(my_num_base(42, NULL));
    TEST(my_num_base(9, "RTFM"));
    TEST(my_num_base(264837422, "abcdefghijklmnopqrstuvwxyz"));
    TEST(my_num_base(-264837422, "abcdefghijklmnopqrstuvwxyz"));
    TEST(my_num_base(256, "01"));
    TEST(my_num_base(0, "a"));
    TEST(my_num_base(1, "a"));
    TEST(my_num_base(7, "z"));
    TEST(my_num_base(7, "0123456789"));
    TEST(my_num_base(2147483647, "012"));
    TEST(my_num_base(-2147483647, "012"));
    TEST(my_num_base(-2147483648, "012"));
    for (int i = -8; i <= 8; ++i) {
        my_int(i);
        my_str(": ");
        my_num_base(i, "wtf");
        my_str("\n");
    }

    my_char('\n');
    TEST(my_alpha());

    my_char('\n');
    TEST(my_digits());

    my_char('\n');
    TEST(my_int(my_strindex("hello", 'h')));
    TEST(my_int(my_strindex("hello", 'o')));
    TEST(my_int(my_strindex("hello", 'l')));
    TEST(my_int(my_strindex("hello", 'z')));
    TEST(my_int(my_strindex(NULL, 'a')));
    TEST(my_int(my_strindex("", 'a')));

    my_char('\n');
    TEST(my_int(my_strrindex("hello", 'h')));
    TEST(my_int(my_strrindex("hello", 'o')));
    TEST(my_int(my_strrindex("hello", 'l')));
    TEST(my_int(my_strrindex("hello", 'z')));
    TEST(my_int(my_strrindex(NULL, 'a')));
    TEST(my_int(my_strrindex("", 'a')));

    my_char('\n');
    TEST(my_int(my_strlen(NULL)));
    TEST(my_int(my_strlen("")));
    TEST(my_int(my_strlen("a")));
    TEST(my_int(my_strlen("ab")));
    TEST(my_int(my_strlen("nicki minaj")));

    my_char('\n');
    TEST(
        char x[] = "stevens";
        my_int(my_revstr(x));
        my_str(x)
    );

    TEST(
        char x[] = "a";
        my_int(my_revstr(x));
        my_str(x)
    );

    TEST(
        char x[] = "ab";
        my_int(my_revstr(x));
        my_str(x)
    );

    TEST(
        my_int(my_revstr(NULL))
    );

    TEST(
        char y[] = "";
        my_int(my_revstr(y));
        my_str(y)
    );

    TEST(
        char y[] = "abcdefghijklmnopqrstuvwxyz";
        my_int(my_revstr(y));
        my_str(y)
    );

    my_str("\n--------- done ---------\n");
}
