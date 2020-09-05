#include "io.h"

#include "assert.h"

int get_word(FILE* fp, char* buf, int size,
             int (*first)(int c),
             int (*rest)(int c)) {
    assert(fp != NULL);
    assert(buf != NULL);
    assert(size > 1);
    assert(first != NULL);
    assert(rest != NULL);

    int i, c;
    // get first
    c = getc(fp);
    i = 0;
    while (c != EOF) {
        if (first(c)) {
            if (i < size - 1) buf[i++] = c;
            c = getc(fp);
            break;
        }
        c = getc(fp);
    }
    // get rest
    while (c != EOF && rest(c)) {
        if (i < size - 1) buf[i++] = c;
        c = getc(fp);
    }
    // set null-terminated
    if (i < size)
        buf[i] = '\0';
    else
        buf[size - 1] = '\0';

    if (c != EOF) ungetc(c, fp);

    return (i > 0);
}
