#include "algo.h"

#include "assert.h"

static void swap_char(char *l, char *r) {
    char t;
    t = *l;
    *l = *r;
    *r = t;
}

void swap(void *lhs, void *rhs, long size) {
    assert(size > 0);

    char *l, *r;
    for (l = (char *)lhs, r = (char *)rhs; size > 0; size--)
        swap_char(l++, r++);
}