#include <atom.h>
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assert.h"
#include "atom.h"
#include "mem.h"
#include "table.h"

/*
    get_word:
        1. Consume the next word in the file opened on fp,
        store it as a null-terminated string in but[0,size-1],
        and return true. 
        2. When reaching the end of the file without consuming
        a word, return false.
        3. More generally, a word begins with a charactor in a 
        first set followed by zero or more charactors in a rest
        set. Functions first() and rest() test a charactor for 
        membership in first and rest. If the charactor is in the 
        set, both of them will return nonzero value.
        4. If a word is longer than size-2 charactors, the excess
        charactors are discarded.
        5. size > 1 
        6. fp buf first rest != NULL
*/
bool get_word(FILE* fp, char* buf, int size,
              bool (*first)(int c),
              bool (*rest)(int c)) {
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

bool first(int c) {
    return isalpha(c);
}

bool rest(int c) {
    return isalpha(c) || c == '-';
}

int compare(const void* x, const void* y) {
    return strcmp(*(char**)x, *(char**)y);
}

void vfree(const void* key, void** cnt, void* cl) {
    FREE(*cnt);
}

void wf(char* name, FILE* fp) {
    struct table_t* table;
    char buf[128];
    table = Table_create(0, NULL, NULL);

    while (get_word(fp, buf, sizeof(buf), first, rest)) {
        const char* word;
        int i, *cnt;
        for (i = 0; buf[i] != '\0'; i++)
            buf[i] = tolower(buf[i]);
        word = Atom_string(buf);
        cnt = Table_get(table, word);
        if (cnt != NULL)
            (*cnt)++;
        else {
            NEW(cnt);
            *cnt = 1;
            Table_put(table, word, cnt);
        }
    }

    if (name != NULL) printf("%s:\n", name);

    int i;
    void** arr;

    arr = Table_to_array(table, NULL);
    // in the alphabetic order
    qsort(arr, table->size, 2 * sizeof(*arr), compare);

    for (i = 0; arr[i] != NULL; i += 2)
        printf("%d\t%s\n", *(int*)arr[i + 1], (char*)arr[i]);

    FREE(arr);
    Table_map(table, vfree, NULL);
    Table_free(&table);
}

int main(int argc, char* argv[]) {
    int i;
    FILE* fp;

    for (i = 1; i < argc; i++) {
        fp = fopen(argv[i], "r");
        if (fp == NULL) {
            fprintf(stderr, "%s:can't open '%s' (%s)\n",
                    argv[0], argv[i], strerror(errno));
            return EXIT_FAILURE;
        } else {
            wf(argv[i], fp);
            fclose(fp);
        }
    }
    if (argc == 1) wf(NULL, stdin);

    return EXIT_SUCCESS;
}