// 'double' can detect the neighboring duplicate words in the input texts.

#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// capsulation of strcmp
// op can only be == and !=.
#define STRCMP(lhs, op, rhs) (strcmp(lhs, rhs) op 0)

int line_num;  // number of current line

// int get_word(FILE *, char *, int);
// void find_double_word(char *, FILE *);

// I prefer to show the name of parameters
// to show more information about the function.
bool get_word(FILE* fp, char* buf, int size);
void find_double_word(char* word, FILE* fp);

int main(int argc, char* argv[]) {
    int i;

    // for no-input-file case
    if (argc == 1) find_double_word(NULL, stdin);

    for (i = 1; i < argc; i++) {
        FILE* fp = fopen(argv[i], "r");
        if (fp == NULL) {
            fprintf(stderr, "%s, can't open '%s' (%s)\n", argv[0], argv[i],
                    strerror(errno));
            // After call the library function,
            // remember to check return value and 'errno'.
            return EXIT_FAILURE;
        } else {
            find_double_word(argv[i], fp);
            fclose(fp);
        }
    }

    return EXIT_SUCCESS;
}

/* get_word: Get a word from fp.                                  */
/* If success, return true and copy the word into buf[0..size-1]; */
/* if not, return false and leave buf empty.                      */
bool get_word(FILE* fp, char* buf, int size) {
    int c, i;

    // scan forward to a nonspace character or EOF
    c = getc(fp);
    for (; c != EOF && isspace(c); c = getc(fp))
        if (c == '\n') line_num++;

    // copy the word into buf[0..size-1]
    for (i = 0; c != EOF && !isspace(c); c = getc(fp))
        if (i < size - 1) buf[i++] = tolower(c);
    if (i < size) buf[i] = '\0';

    // c may be '\n'
    if (c != EOF) ungetc(c, fp);

    // add parentheses to emphasize the logical expression
    return (buf[0] != '\0');
}

/* find_double_word: find and show the neighboring duplicate words. */
/* output format:        <filename>:<line number>: <word>           */
void find_double_word(char* name, FILE* fp) {
    char prev[128], word[128];
    bool repeated;

    prev[0] = '\0';
    line_num = 1;
    repeated = false;

    // while (get_word(fp, word, sizeof word))

    // I think 'sizeof(word)' is more specific.
    // For example, 'sizeof (int)*p' is confusing.
    // Actually, it is 'sizeof(int) * p', not 'sizeof((int)*p)'.
    while (get_word(fp, word, sizeof(word))) {
        if (isalpha(word[0]) && STRCMP(prev, ==, word)) {
            // word is a duplicate
            if (!repeated) {
                if (name) printf("%s:", name);
                printf("%d: %s\n", line_num, word);
                repeated = true;
            }
        } else
            repeated = false;
        strcpy(prev, word);
    }
}