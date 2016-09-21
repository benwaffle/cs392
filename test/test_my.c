#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include "my.h"

// test code that prints to stdout
#define PRTEST(code) { \
    my_str(#code); \
    my_str(": "); \
    code; \
    my_char('\n'); \
}

// just print the code
#define PRINT(code) { \
    code; \
    my_str(#code); \
    my_str(";\n"); \
}

#define assert(p) ((p) ? (void)0 : assert_failed(#p, __FILE__, __LINE__))
void assert_failed(char *p, char *file, int line) {
    my_str(file);
    my_char(':');
    my_int(line);
    my_str(": Assertion `");
    my_str(p);
    my_str("' failed\n");
    kill(getpid(), SIGABRT); // suicide
}

int main()
{
    my_str("------- test_my --------\n\n");

    PRTEST(my_char('A'));
    PRTEST(my_char('Z'));
    PRTEST(my_char('a'));
    PRTEST(my_char('z'));
    PRTEST(my_char(0));

    my_char('\n');
    PRTEST(my_str(NULL));
    PRTEST(my_str("h"));
    PRTEST(my_str("Hello world"));
    PRTEST(my_str("just a\0test"));

    my_char('\n');
    PRTEST(my_int(0));
    PRTEST(my_int(-1));
    PRTEST(my_int(1));
    PRTEST(my_int(-42));
    PRTEST(my_int(10));
    PRTEST(my_int(53));
    PRTEST(my_int(100));
    PRTEST(my_int(120304));
    PRTEST(my_int(1234567890));
    PRTEST(my_int(2147483647)); // max int
    PRTEST(my_int(-2147483647));
    PRTEST(my_int(-2147483648)); // min int

    my_char('\n');
    PRTEST(my_num_base(42, ""));
    PRTEST(my_num_base(42, NULL));
    PRTEST(my_num_base(9, "RTFM"));
    PRTEST(my_num_base(264837422, "abcdefghijklmnopqrstuvwxyz"));
    PRTEST(my_num_base(-264837422, "abcdefghijklmnopqrstuvwxyz"));
    PRTEST(my_num_base(256, "01"));
    PRTEST(my_num_base(0, "a"));
    PRTEST(my_num_base(1, "a"));
    PRTEST(my_num_base(-3, "a"));
    PRTEST(my_num_base(7, "z"));
    PRTEST(my_num_base(7, "0123456789"));
    PRTEST(my_num_base(2147483647, "01"));
    PRTEST(my_num_base(2147483647, "012"));
    PRTEST(my_num_base(-2147483647, "012"));
    PRTEST(my_num_base(-2147483648, "012"));

    for (int i = -8; i <= 8; ++i) {
        my_int(i);
        my_str(": ");
        my_num_base(i, "wtf");
        my_str("\n");
    }

    my_char('\n');
    PRTEST(my_alpha());

    my_char('\n');
    PRTEST(my_digits());

    my_char('\n');
    PRINT(assert(my_strindex("hello", 'h') == 0));
    PRINT(assert(my_strindex("hello", 'o') == 4));
    PRINT(assert(my_strindex("hello", 'l') == 2));
    PRINT(assert(my_strindex("hello", 'z') == -1));
    PRINT(assert(my_strindex(NULL, 'a') == -1));
    PRINT(assert(my_strindex("", 'a') == -1));

    my_char('\n');
    PRINT(assert(my_strrindex("hello", 'h') == 0));
    PRINT(assert(my_strrindex("hello", 'o') == 4));
    PRINT(assert(my_strrindex("hello", 'l') == 3));
    PRINT(assert(my_strrindex("hello", 'z') == -1));
    PRINT(assert(my_strrindex(NULL, 'a') == -1));
    PRINT(assert(my_strrindex("", 'a') == -1));

    my_char('\n');
    PRINT(assert(my_strlen(NULL) == -1));
    PRINT(assert(my_strlen("") == 0));
    PRINT(assert(my_strlen("a") == 1));
    PRINT(assert(my_strlen("ab") == 2));
    PRINT(assert(my_strlen("nicki minaj") == 11));

    my_char('\n');

    PRTEST(
        char x[] = "stevens";
        assert(my_revstr(x) == 7);
        my_str(x)
    );

    PRTEST(
        char x[] = "a";
        assert(my_revstr(x) == 1);
        my_str(x)
    );

    PRTEST(
        char x[] = "ab";
        assert(my_revstr(x) == 2);
        my_str(x)
    );

    PRINT(assert(my_revstr(NULL) == -1));

    PRTEST(
        char y[] = "";
        assert(my_revstr(y) == 0);
        my_str(y)
    );

    PRTEST(
        char y[] = "abcdefghijklmnopqrstuvwxyz";
        assert(my_revstr(y) == 26);
        my_str(y)
    );

    my_str("\n--------- done ---------\n");
}
