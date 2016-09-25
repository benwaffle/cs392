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

#define assert(p) ((p) ? my_str("\x1B[32m▸\x1B[0m ") : assert_failed(#p, __FILE__, __LINE__))
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

    my_char('\n');

    char *x = "hello";
    PRINT(assert(my_strfind(NULL, 'a') == NULL));
    PRINT(assert(my_strfind(x, '\0') == NULL));
    PRINT(assert(my_strfind("", '\0') == NULL));
    PRINT(assert(my_strfind(x, 'h') == x));
    PRINT(assert(my_strfind(x, 'e') == x + 1));
    PRINT(assert(my_strfind(x, 'l') == x + 2));

    my_char('\n');

    PRINT(assert(my_strrfind(NULL, 'a') == NULL));
    PRINT(assert(my_strrfind(x, '\0') == NULL));
    PRINT(assert(my_strrfind("", '\0') == NULL));
    PRINT(assert(my_strrfind(x, 'h') == x));
    PRINT(assert(my_strrfind(x, 'e') == x + 1));
    PRINT(assert(my_strrfind(x, 'l') == x + 3));

    my_char('\n');

    PRINT(assert(my_strcmp("sup", "sup") == 0));
    PRINT(assert(my_strcmp("su", "sup") < 0));
    PRINT(assert(my_strcmp("sup", "su") > 0));
    PRINT(assert(my_strcmp("abc", "abd") < 0));
    PRINT(assert(my_strcmp("abd", "abc") > 0));
    PRINT(assert(my_strcmp(NULL, NULL) == 0));
    PRINT(assert(my_strcmp("", NULL) > 0));
    PRINT(assert(my_strcmp(NULL, "") < 0));
    PRINT(assert(my_strcmp("", "") == 0));

    my_char('\n');

    PRINT(assert(my_strncmp("sup", "sup", 3) == 0));
    PRINT(assert(my_strncmp("su", "sup", 3) < 0));
    PRINT(assert(my_strncmp("su", "sup", 2) == 0));
    PRINT(assert(my_strncmp("su", "sup", 1) == 0));
    PRINT(assert(my_strncmp("sup", "su", 2) == 0));
    PRINT(assert(my_strncmp("abc", "abd", 3) < 0));
    PRINT(assert(my_strncmp("abd", "abc", 3) > 0));
    PRINT(assert(my_strncmp(NULL, NULL, 0) == 0));
    PRINT(assert(my_strncmp(NULL, NULL, 5) == 0));
    PRINT(assert(my_strncmp("", NULL, 1) > 0));
    PRINT(assert(my_strncmp(NULL, "", 1) < 0));
    PRINT(assert(my_strncmp("", "", 1) == 0));
    PRINT(assert(my_strncmp("totally", "different", 0) == 0));

    my_char('\n');

    PRINT({
        char x[] = "hello";
        assert(my_strcpy(x, "bye") == x);
        assert(my_strcmp(x, "bye") == 0);
    });

    PRINT({
        char x[] = "hello";
        assert(my_strcpy(x, "") == x);
        assert(my_strcmp(x, "") == 0);
    });

    PRINT({
        char x[] = "hello";
        assert(my_strcpy(x, "") == x);
        assert(my_strcmp(x, "") == 0);
    });

    PRINT(assert(my_strcpy(NULL, "source") == NULL));

    PRINT({
        char x[] = "hello";
        assert(my_strcpy(x, NULL) == x);
        assert(my_strcmp(x, "hello") == 0); // TODO: should this be "" or "hello"
    });

    PRINT({
        char x[] = "hello";
        assert(my_strncpy(x, "bye", 4) == x);
        assert(my_strcmp(x, "bye") == 0);
    });

    PRINT({
        char x[] = "hello";
        assert(my_strncpy(x, "bye", 2) == x);
        assert(my_strcmp(x, "byllo") == 0);
    });

    my_char('\n');

    PRINT({
        char x[10+1] = "hello";
        assert(my_strcat(x, "world") == x);
        assert(my_strcmp(x, "helloworld") == 0);
    });

    PRINT({
        char x[5+1] = "hello";
        assert(my_strcat(x, NULL) == x);
        assert(my_strcmp(x, "hello") == 0);
    });

    PRINT({
        assert(my_strcat(NULL, "world") == NULL);
    });

    PRINT({
        char *x = "g eazy";
        char *copy = my_strdup(x);
        assert(copy != x);
        assert(my_strcmp(copy, x) == 0);
        assert(my_strcmp(copy, "g eazy") == 0);
        free(copy);
    });

    my_char('\n');

    PRINT({
        char *a = "hello";
        char *b = "world";
        char *new = my_strconcat(a, b);
        assert(my_strcmp(new, "helloworld") == 0);
        free(new);
    });

    PRINT({
        char *new = my_strconcat("hello", NULL);
        assert(my_strcmp(new, "hello") == 0);
        free(new);
    });

    PRINT({
        char *new = my_strconcat(NULL, "world");
        assert(my_strcmp(new, "world") == 0);
        free(new);
    });

    PRINT({
        char *a = "hello";
        char *b = "world";
        char *new = my_strnconcat(a, b, 3);
        printf("%s\n", new);
        assert(my_strcmp(new, "hellowor") == 0);
        free(new);
    });

    PRINT({
        char *new = my_strnconcat("hello", NULL, 10);
        assert(my_strcmp(new, "hello") == 0);
        free(new);
    });

    PRINT({
        char *new = my_strnconcat(NULL, "world", 3);
        assert(my_strcmp(new, "wor") == 0);
        free(new);
    });

    my_char('\n');

    PRINT(assert(my_atoi(NULL) == 0));
    PRINT(assert(my_atoi("") == 0));
    PRINT(assert(my_atoi("1234") == 1234));
    PRINT(assert(my_atoi("12034") == 12034));
    PRINT(assert(my_atoi("450") == 450));
    PRINT(assert(my_atoi("4-50") == 4));
    PRINT(assert(my_atoi("1ab2") == 1));
    PRINT(assert(my_atoi("1-2") == 1));
    PRINT(assert(my_atoi("-ab12") == -12));
    PRINT(assert(my_atoi("12-") == 12));
    PRINT(assert(my_atoi("5") == 5));
    PRINT(assert(my_atoi("-5") == -5));
    PRINT(assert(my_atoi("--5") == 5));
    PRINT(assert(my_atoi("a-b54sc7-d") == -54));
    PRINT(assert(my_atoi("abcd") == 0));
    PRINT(assert(my_atoi("2147483647") == 2147483647));
    PRINT(assert(my_atoi("-2147483648") == -2147483648));

    my_str("\n--------- done ---------\n");
}
