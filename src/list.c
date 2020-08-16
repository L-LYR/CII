#include "list.h"

#include <stdarg.h>  // va_list & va_start() & va_end() & va_arg()

#include "algo.h"
#include "assert.h"
#include "mem.h"

static struct node_t* new_node(void* x) {
    struct node_t* p;
    NEW(p);
    p->data = x;
    p->prev = p->next = NULL;
    return p;
}

// Codes in comment are the implementation for
// the one-way circular list without a head pointer.

struct list_t* List_create(void* x, ...) {
    /*
    struct list_t* lst;
    struct node_t **p, *last;
    va_list ap;

    NEW(lst);
    lst->tail = NULL;
    lst->len = 0;

    va_start(ap, x);
    *p = &lst->tail;
    last = NULL;
    while (x != NULL) {
        NEW(*p);
        (*p)->data = x;
        lst->len++;

        x = va_arg(ap, void*);     // next arg
        if (x == NULL) last = *p;  // point to the last node

        p = &(*p)->next;
    }
    va_end(ap);

    // make the circle
    *p = lst->tail;
    // let lst->tail points to the last node
    lst->tail = last;

    return lst;
    */
    struct list_t* lst;
    struct node_t *last, *p;
    va_list ap;

    NEW(lst);
    lst->tail = lst->head = NULL;
    lst->len = 0;

    va_start(ap, x);
    while (x != NULL) {
        p = new_node(x);
        if (lst->head == NULL)
            lst->head = p;
        else {
            last->next = p;
            p->prev = last;
        }

        last = p;
        lst->len++;
        x = va_arg(ap, void*);
    }
    va_end(ap);

    return lst;
}

void List_push_back(struct list_t* list, void* x) {
    /*
    struct node_t* p;

    NEW(p);
    p->data = x;

    if (list->len == 0)
        p->next = p;
    else
        p->next = list->tail->next;
    list->tail = p;

    list->len++;
    */
    if (list->len == 0) {
        list->head = list->tail = new_node(x);
    } else {
        list->tail->next = new_node(x);
        list->tail->next->prev = list->tail;
    }
    list->len++;
}

void* List_pop_back(struct list_t* list) {
    if (list->len == 0) {
        return NULL;
    } else {
        struct node_t* p;
        void* x;

        p = list->tail;
        list->tail = p->prev;
        x = p->data;
        FREE(p);
        list->len--;
        if (list->len == 0) list->head = NULL;
        return x;
    }
}

void List_push_front(struct list_t* list, void* x) {
    /*
    struct node_t* p;

    NEW(p);
    p->data = x;

    if (list->len == 0)
        p->next = p;
    else
        p->next = list->tail->next;
    list->tail->next = p;

    list->len++;
    */
    if (list->len == 0) {
        list->head = list->tail = new_node(x);
    } else {
        list->head->prev = new_node(x);
        list->head->prev->next = list->head;
    }
    list->len++;
}

void* List_pop_front(struct list_t* list) {
    /*
    if (list->len == 0)
        return NULL;
    else {
        void* x;
        struct node_t* p;

        p = list->tail;
        list->tail = p->next;
        x = p->data;
        FREE(p);
        list->len--;
        return x;
    }
    */
    if (list->len == 0) {
        return NULL;
    } else {
        struct node_t* p;
        void* x;

        p = list->head;
        list->head = p->next;
        x = p->data;
        FREE(p);
        list->len--;
        if (list->len == 0) list->tail = NULL;
        return x;
    }
}

void List_reverse(struct list_t* list) {
    if (list->len == 0)
        return;
    else {
        struct node_t* p;
        for (p = list->head; p != NULL; p = p->prev) {
            SWAP(p->next, p->prev, struct node_t*);
        }
        SWAP(list->head, list->tail, struct node_t*);
    }
}

extern int List_remove(struct list_t* list, void* x) {
    struct node_t* p;
    for (p = list->head; p != NULL; p = p->next) {
        if (p->data == x) {
            p->prev->next = p->next;
            p->next->prev = p->prev;
            list->len--;
            FREE(p);
            return 1;
        }
    }
    return 0;
}

void List_free(struct list_t** list) {
    assert(list != NULL);
    assert(*list != NULL);

    struct node_t *p, *del;
    p = (*list)->head;
    while (p != NULL) {
        del = p;
        p = p->next;
        FREE(del);
    }
    FREE(*list);
}

void List_map(struct list_t* list, void (*apply)(void** x, void* cl), void* cl) {
    assert(apply != NULL);

    struct node_t* p;
    for (p = list->head; p != NULL; p = p->next) {
        apply(&p->data, cl);
    }
}

void** List_to_array(struct list_t* list, void* end) {
    int i, n;
    void** arr;
    struct node_t* p;

    n = list->len;
    arr = ALLOC((n + 1) * sizeof(*arr));

    for (i = 0, p = list->head; i < n; i++) {
        arr[i] = p->data;
        p = p->next;
    }
    arr[i] = end;

    return arr;
}