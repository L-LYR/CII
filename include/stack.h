#ifndef STACK_INCLUDED
#define STACK_INCLUDED
#include <stdbool.h> // bool type

struct elem {
    void *x;
    struct elem *link;
};

struct stack_t {
    int count;
    struct elem *head;
};

extern struct stack_t *Stack_new(void);
extern bool Stack_empty(const struct stack_t *stk);
extern void Stack_push(struct stack_t *stk, void *x);
extern void *Stack_pop(struct stack_t *stk);
extern void Stack_free(struct stack_t **stk);

#endif