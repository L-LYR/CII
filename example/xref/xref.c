#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "atom.h"
#include "io.h"
#include "mem.h"
#include "set.h"
#include "table.h"

//
static int line_num;

// For get_word()
// Identifier begins with a letter or a '_'.
int first(int c) {
    if (c == '\n') {
        line_num++;
    }
    return isalpha(c) || c == '_';
}
// The rest part is made up with digits, letters and '_'s.
int rest(int c) {
    return isalpha(c) || c == '_' || isdigit(c);
}

// For identifier table and file table
int cmp_str(const void* x, const void* y) {
    return strcmp(*(char**)x, *(char**)y);
}

// For qsort()
int sort_int(const void* x, const void* y) {
    if (**(int**)x < **(int**)y)
        return -1;
    else if (**(int**)x > **(int**)y)
        return 1;
    else
        return 0;
}

// For int set
int cmp_int(const void* x, const void* y) {
    return sort_int(&x, &y);
}

unsigned long hash_int(const void* x) {
    return (unsigned long)*(int*)x;
}

// Free the ref-count set of each identifier
void int_set_free(const void* member, void* cl) {
    free((void*)member);
}

// Free the secondary table for file
void set_table_free(const void* key, void** value, void* cl) {
    Set_map(*value, int_set_free, NULL);
    Set_free((struct set_t**)value);
}

// Free the first table for identifier
void table_table_free(const void* key, void** value, void* cl) {
    Table_map(*value, set_table_free, NULL);
    Table_free((struct table_t**)value);
}

void print(struct table_t* files) {
    int i, j;
    void **array, **lines;

    array = Table_to_array(files, NULL);
    qsort(array, files->size, 2 * sizeof(*array), cmp_str);
    for (i = 0; array[i] != NULL; i += 2) {
        if (*(char*)array[i] != '\0') {
            printf("\t%s:", (char*)array[i]);
        }
        lines = Set_to_array(array[i + 1], NULL);
        qsort(lines, ((struct set_t*)array[i + 1])->size,
              sizeof(*lines), sort_int);
        for (j = 0; lines[j] != NULL; j++) {
            if (j > 0 && *(int*)lines[j] - 1 == *(int*)lines[j - 1]) {
                if (lines[j + 1] == NULL || *(int*)lines[j] + 1 != *(int*)lines[j + 1]) {
                    printf("-");
                    printf("%d", *(int*)lines[j]);
                }
            } else {
                printf(" %d", *(int*)lines[j]);
            }
        }
        FREE(lines);
        printf("\n");
    }

    FREE(array);
}

void xref(const char* name, FILE* fp, struct table_t* identifiers) {
    char buf[128];
    struct set_t* lines;
    struct table_t* files;
    const char* id;
    int* p;

    if (name == NULL) name = "";
    name = Atom_string(name);

    while (get_word(fp, buf, sizeof(buf), first, rest)) {
        id = Atom_string(buf);
        files = Table_get(identifiers, id);
        if (files == NULL) {
            files = Table_create(0, NULL, NULL);
            Table_put(identifiers, id, files);
        }
        lines = Table_get(files, name);
        if (lines == NULL) {
            lines = Set_create(0, cmp_int, hash_int);
            Table_put(files, name, lines);
        }
        p = &line_num;
        if (!Set_has(lines, p)) {
            NEW(p);
            *p = line_num;
            Set_put(lines, p);
        }
    }
}

int main(int argc, char* argv[]) {
    int i;
    struct table_t* identifiers;
    void** array;
    FILE* fp;

    identifiers = Table_create(0, NULL, NULL);
    for (i = 1; i < argc; i++) {
        fp = fopen(argv[i], "r");
        if (fp == NULL) {
            fprintf(stderr, "%s: can't open '%s' (%s)n",
                    argv[0], argv[i], strerror(errno));
            return EXIT_FAILURE;
        } else {
            xref(argv[i], fp, identifiers);
            fclose(fp);
        }
    }
    if (argc == 1) xref(NULL, stdin, identifiers);
    /* Print */
    array = Table_to_array(identifiers, NULL);
    qsort(array, identifiers->size, 2 * sizeof(*array), cmp_str);
    for (i = 0; array[i] != NULL; i += 2) {
        printf("%s\n", (char*)array[i]);
        print(array[i + 1]);
    }
    FREE(array);

    /* Free all */
    Table_map(identifiers, table_table_free, NULL);
    Table_free(&identifiers);
    Atom_reset();

    return EXIT_SUCCESS;
}