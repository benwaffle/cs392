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
 * I.e. "hello \t\t\n class,\nhow are you?" -> {"hello", "class,", "how",
 * "are","you?", NULL}
 */
char **my_str2vect(char *str)
{
    int len = 0;
    int insq = 0, indq = 0; // in {single,double} quote
    for (int i = 0; str[i] != '\0'; ++i) {
        if (str[i] == '"') {
            indq = !indq;
        } else if (str[i] == '\'') {
            insq = !insq;
        }

        if (insq || indq)
            continue;

        // count the last letter of each word
        if (!isws(str[i]) && isws(str[i + 1])) {
            ++len;
        }
    }

    char **vec = calloc(len + 1, sizeof *vec);
    int veci = 0;
    int i = 0;
    int tokenstart = -1;
    insq = 0, indq = 0;

    while (str[i] != '\0') {
        if (!isws(str[i])) {
            // quotes
            if (str[i] == '\'') {
                insq = !insq;
            } else if (str[i] == '"') {
                indq = !indq;
            }

            // str[i] is not whitespace, so start a new token
            if (tokenstart == -1) {
                tokenstart = i;
            }

            // if next is whitespace, save this token
            // don't make tokens until we exit quotes
            if (!insq && !indq && isws(str[i + 1])) {
                // if we start with a quote, skip it
                if (str[tokenstart] == '\'' || str[tokenstart] == '\"')
                    tokenstart++;

                int tokenlen = i + 1 - tokenstart;
                // if we end with a quote, skip it
                if (str[i] == '\'' || str[i] == '\"')
                    tokenlen--;

                vec[veci] = calloc(tokenlen + 1, 1);
                my_strncpy(vec[veci], str + tokenstart, tokenlen);
                veci++;
                tokenstart = -1;
            }
        }

        ++i;
    }

    return vec;
}
