#include "stack.h"

#include <stdbool.h>
#include <stddef.h>

#include "assert.h"
#include "mem.h"

struct elem {
    void *x;
    struct elem *link;
};

struct stack_t {
    int count;
    struct elem *head;
};

struct stack_t *Stack_new(void) {
    struct stack_t *stk;

    NEW(stk);
    stk->count = 0;
    stk->head = NULL;
    return stk;
}

bool Stack_empty(const struct stack_t *stk) {
    assert(stk);
    return stk->count == 0;
}

void Stack_push(struct stack_t *stk, void *x) {
    struct elem *t;

    assert(stk);

    NEW(t);
    t->x = x;
    t->link = stk->head;
    stk->head = t;
    stk->count++;
}

void *Stack_pop(struct stack_t *stk) {
    void *x;
    struct elem *t;

    assert(stk);
    assert(stk->count > 0);

    t = stk->head;
    stk->head = t->link;
    stk->count--;
    x = t->x;
    FREE(t);

    return x;
}

void Stack_free(struct stack_t **stkp) {
    struct elem *t, *u;

    assert(stkp && *stkp);

    for (t = (*stkp)->head; t != NULL; t = u) {
        u = t->link;
        FREE(t);
    }

    FREE(*stkp);
}