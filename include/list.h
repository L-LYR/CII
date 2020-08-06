/*
    doubly linked list with a tail pointer and a head pointer
*/

#ifndef LIST_INCLUDE
#define LIST_INCLUDE
#include <stdbool.h>

struct list_t;

/*
    List_create:
        1. Creation of a list.
        2. Initialization list is terminated with a NULL.
*/
extern struct list_t* List_create(void* x, ...);

/*
    Basic operations on a list
*/
extern void List_push_back(struct list_t* list, void* x);
extern void* List_pop_back(struct list_t* list);
extern void List_push_front(struct list_t* list, void* x);
extern void* List_pop_front(struct list_t* list);
extern void List_reverse(struct list_t* list);
extern bool List_remove(struct list_t* list, void* x);  // remove the first one
extern void List_free(struct list_t** list);

/*
    List_map:
        1. Walk down the list while calling the closure function apply for all nodes.
        For example,
            apply_free (void **ptr, void *cl) {
                FREE(*ptr);
                *ptr = NULL;
            }
*/
extern void List_map(struct list_t* list, void(apply)(void** x, void* cl), void* cl);

/*
    List_to_array:
        1. Allocate an n+1 element array to hold the pointers in an n element list.
        2. Copy the pointers into the array.
        3. Never return NULL.
        4. The end of the array which will be returned is up to clients.
*/
extern void** List_to_array(struct list_t* list, void* end);

// no need to implementation
// extern struct list_t* List_copy(struct list_t* list);
// extern int List_length(struct list_t* list);

#endif