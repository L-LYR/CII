#include "arena.h"

#include <stdlib.h>  // malloc() & free()
#include <string.h>  // memset()

#include "align.h"
#include "assert.h"
#include "except.h"
#include "mem.h"

// extra size for new chunk
#define NEW_CHUNK_EXTRA_SIZE 10240

// threshold for free chunk list
#define THRESHOLD 10

struct arena_t {
    struct arena_t* prev;  // previous chunk
    char* avail;           // begin of chunk
    char* limit;           // end of the chunk
};

// Make sure that arena->avail is set to a properly
// aligned address for the first allocation in this
// new chunk.
union header {
    struct arena* b;
    union align a;
};

// free chunk list
static struct arena_t* free_chunk_list = NULL;
static int nfree = 0;

const struct except_t arena_new_failed = {
    "Arena Creation Failed"};

const struct except_t arena_failed = {
    "Arena Allocation Failed"};

struct arena_t* Arena_new(void) {
    struct arena_t* arena;

    arena = malloc(sizeof(*arena));

    if (arena == NULL) RAISE(arena_new_failed);

    arena->prev = NULL;
    arena->avail = arena->limit = NULL;

    return arena;
}

void Arena_dispose(struct arena_t** ap) {
    assert(ap && *ap);
    Arena_free(*ap);
    free(*ap);
    *ap = NULL;
}

void* Arena_alloc(struct arena_t* arena, long nbytes,
                  const char* file, int line) {
    assert(arena != NULL);
    assert(nbytes > 0);

    nbytes = ROUND_UP_TO_ALIGN_BOUND(nbytes);

    // If there is enough space in some other chunk further down the list.
    while (arena->prev != NULL) {
        if (nbytes > (arena->limit - arena->avail))
            arena = arena->prev;
        else
            break;
    }

    // original version

    while (nbytes > (arena->limit - arena->avail)) {
        // get a new chunk
        struct arena_t* new_arena;
        char* limit;

        new_arena = free_chunk_list;
        if (new_arena != NULL) {
            free_chunk_list = free_chunk_list->prev;
            nfree--;
            limit = new_arena->limit;
        } else {
            long m = sizeof(union header) + nbytes + NEW_CHUNK_EXTRA_SIZE;
            new_arena = malloc(m);
            if (new_arena == NULL) {
                if (file == NULL)
                    RAISE(arena_failed);
                else
                    Except_raise(&arena_failed, file, line);
            }
            limit = (char*)new_arena + m;
        }

        *new_arena = *arena;
        arena->avail = (char*)((union header*)new_arena + 1);
        arena->limit = limit;
        arena->prev = new_arena;
    }

    arena->avail += nbytes;
    return (arena->avail - nbytes);
}

void* Arena_calloc(struct arena_t* arena, long count, long nbytes, const char* file, int line) {
    void* ptr;

    assert(count > 0);
    assert(nbytes > 0);

    ptr = Arena_alloc(arena, count * nbytes, file, line);
    memset(ptr, 0, count * nbytes);
    return ptr;
}

void Arena_free(struct arena_t* arena) {
    assert(arena != NULL);
    while (arena->prev != NULL) {
        struct arena_t tmp;

        tmp = *(arena->prev);
        if (nfree > THRESHOLD) {
            arena->prev->prev = free_chunk_list;
            free_chunk_list = arena->prev;
            nfree++;
            free_chunk_list->limit = arena->limit;
        } else
            free(arena->prev);
        *arena = tmp;
    }
}