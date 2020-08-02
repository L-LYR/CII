#ifndef ARENA_INCLUDE
#define ARENA_INCLUDE

/*
    With the arena-based allocator, there’s no obligation to call free for
    every call to malloc; there’s only a single call that deallocates all the
    memory allocated in an arena since the last deallocation. Both allocation
    and deallocation are more efficient, and there are no storage leaks. But
    the most important benefit of this scheme is that it simplifies code.
    Applicative algorithms allocate new data structures instead of changing
    existing ones. The arena-based allocator encourages simple applicative
    algorithms in place of algorithms that might be more space-efficient but
    are always more complex because they must remember when to call free.

    There are two disadvantages of the arena-based scheme: It can use more 
    memory, and it can create dangling pointers.
*/

#include "except.h"

struct arena_t {
    struct arena_t *prev;  // previous chunk
    char *avail;           // begin of chunk
    char *limit;           // end of the chunk
};

extern const struct except_t arena_new_failed;
extern const struct except_t arena_failed;

/*
    Arena_new:
        1. Create a new arena and return an opaque pointer to the newly created arena.
        2. If it can't allocate the arena, it raises the exception arena_new_failed.
*/
extern struct arena_t *Arena_new(void);

/*
    Arena_dispose:
        1. Free the memory associated with the arena *ap.
        2. Dispose of the arena itself and clear *ap.
*/
extern void Arena_dispose(struct arena_t **ap);

/*
    Arena_alloc:
        1. Allocate a block of at least nbytes in arena and 
        return a pointer to the first byte.
        2. The block is aligned on an addressing boundary that is suitable 
        for the data with the strictest alignment requirement. 
*/
extern void *Arena_alloc(struct arena_t *arena, long nbytes, const char *file, int line);

/*
    Arena_calloc:
        1. Allocate a block large enough to hold an array of count elements, 
        each of size nbytes, in arena, and returns a pointer to the first byte.
        2. The block is aligned as for Arena_alloc, and is initialized to zeros.
*/
extern void *Arena_calloc(struct arena_t *arena, long count, long nbytes, const char *file, int line);

/*
    Arena_free:
        1. Deallocate all the storage in arena, which amounts to 
        deallocating everything that has been allocated in arena 
        since arena was created or since the last call to 
        Arena_free for that arena.
*/
extern void Arena_free(struct arena_t *arena);

#endif