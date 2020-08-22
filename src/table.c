#include "table.h"

#include <limits.h>  // INT_MAX
#include <string.h>  // memset()

#include "assert.h"
#include "atom.h"
#include "mem.h"

// Size of table
// Table_create will choose the greatest value which is less than hint
static int primes[] = {
    509,
    509,    // 512   = 2 ^ 9
    1021,   // 1025  = 2 ^ 10
    2039,   // 2048  = 2 ^ 11
    4093,   // 4096  = 2 ^ 12
    8191,   // 8192  = 2 ^ 13
    16381,  // 16384 = 2 ^ 14
    32771,  // 32768 = 2 ^ 15
    65521,  // 65535 = 2 ^ 16
    INT_MAX,
};

struct table_t *Table_create(int hint,
                             int (*cmp)(const void *x, const void *y),
                             unsigned long (*hash)(const void *key)) {
    struct table_t *table;
    int i;

    assert(hint >= 0);

    for (i = 1; primes[i] < hint; i++)
        ;
    hint = primes[i - 1];

    table = ALLOC(sizeof(*table) + hint * sizeof(table->buckets[0]));
    table->capacity = hint;
    // table->cmp = ((cmp == NULL) ? (Atom_cmp) : (cmp));
    // table->hash = ((hash == NULL) ? (Atom_hash) : (hash));
    if (cmp == NULL)
        table->cmp = Atom_cmp;
    else
        table->cmp = cmp;
    if (hash == NULL)
        table->hash = Atom_hash;
    else
        table->hash = hash;
    table->buckets = (struct binding **)(table + 1);
    memset(table->buckets, 0, sizeof(table->buckets[0]) * hint);
    table->size = 0;
    table->time_stamp = 0;

    return table;
}

void Table_free(struct table_t **table) {
    assert(table != NULL);
    assert(*table != NULL);

    if ((*table)->size > 0) {
        int i;
        struct binding *p, *q;
        for (i = 0; i < (*table)->capacity; i++) {
            for (p = (*table)->buckets[i]; p != NULL; p = q) {
                q = p->link;
                FREE(p);
            }
        }
    }
    FREE(*table);
}

void *Table_put(struct table_t *table, const void *key, void *value) {
    unsigned long h;
    struct binding *p;
    void *prev;

    assert(table != NULL);
    assert(key != NULL);

    h = (*table->hash)(key) % table->capacity;
    for (p = table->buckets[h]; p != NULL; p = p->link) {
        if ((*table->cmp)(key, p->key) == 0) break;
    }
    if (p == NULL) {
        NEW(p);
        p->key = key;
        p->link = table->buckets[h];
        table->buckets[h] = p;
        table->size++;
        prev = NULL;
    } else
        prev = p->value;
    p->value = value;  // overwrite or initialize
    table->time_stamp++;
    return prev;
}

void *Table_get(struct table_t *table, const void *key) {
    unsigned long h;
    struct binding *p;

    assert(table != NULL);
    assert(key != NULL);

    h = (*table->hash)(key) % table->capacity;
    for (p = table->buckets[h]; p != NULL; p = p->link) {
        if ((*table->cmp)(key, p->key) == 0) break;
    }

    return (p == NULL) ? NULL : p->value;
}

void *Table_remove(struct table_t *table, const void *key) {
    unsigned long h;
    struct binding **pp, *p;
    void *prev;

    assert(table != NULL);
    assert(key != NULL);

    prev = NULL;
    h = (*table->hash)(key) % table->capacity;
    for (pp = &table->buckets[h]; *pp != NULL; pp = &(*pp)->link) {
        if ((*table->cmp)(key, (*pp)->key) == 0) {
            p = *pp;
            prev = p->link;
            FREE(p);
            table->size--;
            break;
        }
    }
    table->time_stamp++;
    return prev;
}

void Table_map(struct table_t *table,
               void (*apply)(const void *key, void **value, void *cl),
               void *cl) {
    int i;
    unsigned long stamp;
    struct binding *p;

    assert(table != NULL);
    assert(apply != NULL);
    stamp = table->time_stamp;
    for (i = 0; i < table->capacity; i++) {
        for (p = table->buckets[i]; p != NULL; p = p->link) {
            apply(p->key, &(p->value), cl);
            // table can’t be changed while Table_map is visiting its bindings.
            assert(stamp == table->time_stamp);
        }
    }
}

void **Table_to_array(struct table_t *table, void *end) {
    int i, j;
    void **arr;
    struct binding *p;

    assert(table != NULL);

    arr = ALLOC((2 * table->size + 1) * sizeof(*arr));
    for (i = 0, j = 0; i < table->capacity; i++) {
        for (p = table->buckets[i]; p != NULL; p = p->link) {
            arr[j++] = (void *)p->key;  // cast const
            arr[j++] = p->value;
        }
    }
    arr[j] = end;
    return arr;
}