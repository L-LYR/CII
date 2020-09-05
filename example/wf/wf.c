#include <atom.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assert.h"
#include "atom.h"
#include "io.h"
#include "mem.h"
#include "table.h"

int first(int c) {
    return isalpha(c);
}

int rest(int c) {
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

    Atom_reset();
    return EXIT_SUCCESS;
}