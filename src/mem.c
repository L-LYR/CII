/*
    Production Implementation
    In the production implementation, the routines encapsulate calls to the
    memory-management functions in the standard library in the safer package
    specified by the Mem interface
*/

#include "mem.h"

#include <stddef.h>
#include <stdlib.h>

#include "assert.h"
#include "except.h"

const struct except_t mem_failed = {"Allocation Failed"};

/*
    All the implementation is simple.
*/

void *Mem_alloc(long nbytes, const char *file, int line) {
    void *ptr;

    assert(nbytes > 0);

    ptr = malloc(nbytes);
    if (ptr == NULL) {
        if (file == NULL)
            RAISE(mem_failed);
        else
            Except_raise(&mem_failed, file, line);
    }

    return ptr;
}

void *Mem_calloc(long count, long nbytes, const char *file, int line) {
    void *ptr;

    assert(count > 0);
    assert(nbytes > 0);

    ptr = calloc(count, nbytes);

    if (ptr == NULL) {
        if (file == NULL)
            RAISE(mem_failed);
        else
            Except_raise(&mem_failed, file, line);
    }

    return ptr;
}

void Mem_free(void *ptr, const char *file, int line) {
    if (ptr != NULL) free(ptr);
}

void *Mem_resize(void *ptr, long nbytes, const char *file, int line) {
    assert(ptr != NULL);
    assert(nbytes > 0);

    ptr = realloc(ptr, nbytes);
    if (ptr == NULL) {
        if (file == NULL)
            RAISE(mem_failed);
        else
            Except_raise(&mem_failed, file, line);
    }

    return ptr;
}