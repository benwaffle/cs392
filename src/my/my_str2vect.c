#include <stdio.h>
#include "my.h"

// is whitespace
static int isws(char c)
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\0';
}

/**
 * Takes a string
 * Allocates a new vector (array of string ended by a NULL),
 * Splits apart the input string x at each space character
 * Returns the newly allocated array of strings
 * Any number of ' ','\t', and '\n's can separate words.
 * I.e. "hello \t\t\n class,\nhow are you?" -> {"hello", "class,", "how", "are","you?", NULL}
 */
char **my_str2vect(char *str)
{
    int len = 0;
    for (int i = 0; str[i] != '\0'; ++i) {
        // count the last letter of each word
        if (!isws(str[i]) && isws(str[i+1])) {
            ++len;
        }
    }

    char **vec = calloc(len + 1, sizeof *vec);
    int veci = 0;

    int i = 0;
    while (str[i] != '\0') {
        // skip whitespace
        if (isws(str[i])) {
            ++i;
            continue;
        }

        int slen = 0; // length of current string
        while (!isws(str[i + slen])) {
            ++slen;
        }
        vec[veci] = calloc(slen + 1, 1);
        my_strncpy(vec[veci], str + i, slen);
        i += slen;
        ++veci;
    }

    return vec;
}
