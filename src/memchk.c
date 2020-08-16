/*
    Checking Implementation
    The functions exported by the checking implementation of the Mem
    interface catch the kinds of access errors described at the
    beginning of this chapter and report them as checked runtime errors.

    TODO:
        1. How to detect and remove such blocks whose size is less than sizeof(align).
        2. Mem_leak().
    
    For the first one, I think it is impossible, because this lib is not the implementation 
    of malloc() and free(), but the capsulation of them, I can't free the certain blocks.
*/

#include <stdio.h>   // fprintf() & FILE
#include <stdlib.h>  // malloc() & free()
#include <string.h>  // memset()

#include "align.h"
#include "assert.h"
#include "except.h"
#include "mem.h"

// size of memory block hash table
#define HASH_TABLE_SIZE 2039

// default number of descriptors
#define DEFAULT_NUM_DESCRIPTOR 512

// space allocation increment
#define SPACE_ALLOC_INCREMENT 4096

struct descriptor {
    struct descriptor *free;  // linked in free-block list
    struct descriptor *link;  // linked in hash table
    const void *ptr;          // allocated block
    long size;                // size of block
    const char *file;         // location
    int line;
};

// invalid operations in Mem_resize and Mem_free
static enum {
    VALID_ARG,
    INVALID_PTR,
    FREE_FREED_MEM,
    RESIZE_UNALLOC_MEM,
} invalid_operaion = VALID_ARG;

// memory-allocation-failed exception
const struct except_t mem_failed = {"Allocation Failed"};

// allocated memory block descriptor pointer hash table
static struct descriptor *mem_block_hash_table[HASH_TABLE_SIZE];

// free_block_list is a circular linked list in which
// there are all unused allocated memory blocks.
static struct descriptor free_block_list = {&free_block_list};

// available descriptors
static struct descriptor *avail = NULL;

// number of left descriptors
static int nleft = 0;

// memory allocation log stream
static FILE *mem_log_stm = NULL;

/*
    is_aligned:
        1. Judge whether ptr is aligned by union align.
*/

static inline int is_aligned(const void *ptr) {
    return ((unsigned long)ptr % sizeof(union align)) == 0;
}

/*
    hash:
        1. hash function of descriptor.ptr.
        2. Judge a pointer as an unsigned long int.
*/
static inline unsigned long hash(const void *ptr) {
    return (unsigned long)ptr % HASH_TABLE_SIZE;
}

/*
    hash_table_find:
        1. Find the descriptor in mem_block_hash_table for ptr.
        2. If not found, return NULL.
*/
static struct descriptor *hash_table_find(const void *ptr) {
    struct descriptor *bp;

    bp = mem_block_hash_table[hash(ptr)];
    while (bp != NULL && bp->ptr != ptr) bp = bp->link;
    return bp;
}

/*
    dalloc:
        1. Allocator of descriptors
        2. In memory level, descriptors will be arranged in an array whose
        default size is 512.
        3. If allocation failed, return NULL.
*/
static struct descriptor *dalloc(void *ptr, long size, const char *file, int line) {
    if (nleft <= 0) {
        avail = malloc(DEFAULT_NUM_DESCRIPTOR * sizeof(*avail));
        if (avail == NULL) return NULL;
    }

    avail->ptr = ptr;
    avail->size = size;
    avail->file = file;
    avail->line = line;
    avail->free = avail->link = NULL;

    nleft--;
    return avail++;
}

// This kind of implementation use the First-Fit algorithm.
void *Mem_alloc(long nbytes, const char *file, int line) {
    struct descriptor *bp;
    void *ptr;

    assert(nbytes > 0);

    // round nbytes up to an alignment boundary
    nbytes = ROUND_UP_TO_ALIGN_BOUND(nbytes);

    for (bp = free_block_list.free; bp != NULL; bp = bp->free) {
        if (bp->size > nbytes) {
            // use the end of the block at bp->ptr
            bp->size -= nbytes;
            ptr = (char *)bp->ptr + bp->size;

            if ((bp = dalloc(ptr, nbytes, file, line)) != NULL) {
                unsigned long h;

                h = hash(ptr);
                bp->link = mem_block_hash_table[h];
                mem_block_hash_table[h] = bp;
                return ptr;
            } else {
                // memory allocation failed
                if (file == NULL)
                    RAISE(mem_failed);
                else
                    Except_raise(&mem_failed, file, line);
            }
        }
        if (bp == &free_block_list) {
            struct descriptor *new_ptr;

            // allocate a new block whose size is equal to nbytes+SPACE_ALLOC_INCREMENT
            if ((ptr = malloc(nbytes + SPACE_ALLOC_INCREMENT)) == NULL ||
                (new_ptr = dalloc(ptr, nbytes + SPACE_ALLOC_INCREMENT,
                                  __FILE__, __LINE__)) == NULL) {
                // memory allocation failed
                if (file == NULL)
                    RAISE(mem_failed);
                else
                    Except_raise(&mem_failed, file, line);
            }

            // initializing uninitialized memory to some distinctive bit pattern to help
            // diagnose bugs that are caused by accessing uninitialized memory.
            memset(ptr, 0xCC, nbytes + SPACE_ALLOC_INCREMENT);
            //
            new_ptr->free = free_block_list.free;
            free_block_list.free = new_ptr;
        }
    }

    assert(0);
    return NULL;
}

void *Mem_calloc(long count, long nbytes, const char *file, int line) {
    void *ptr;

    assert(count > 0);
    assert(nbytes > 0);

    ptr = Mem_alloc(count * nbytes, file, line);
    memset(ptr, '\0', count * nbytes);

    return ptr;
}

void Mem_log(FILE *stm) {
    mem_log_stm = stm;
}

static void logger(const void *ptr, const char *caller, const char *file, int line) {
    struct descriptor *bp;

    switch (invalid_operaion) {
        case INVALID_PTR:
            fprintf(mem_log_stm, "** invalid pointer argument\n");
            fprintf(mem_log_stm, "%s(%lx) called from %s:%d\n",
                    caller, (unsigned long)ptr, file, line);
            break;
        case FREE_FREED_MEM:
            bp = hash_table_find(ptr);
            fprintf(mem_log_stm, "** freeing free memory\n");
            fprintf(mem_log_stm, "%s(%lx) called from %s:%d\n",
                    caller, (unsigned long)ptr, file, line);
            fprintf(mem_log_stm, "This block is %ld bytes and was allocated from %s:%d",
                    bp->size, bp->file, bp->line);
            break;
        case RESIZE_UNALLOC_MEM:
            bp = hash_table_find(ptr);
            fprintf(mem_log_stm, "** resizing unallocated memory\n");
            fprintf(mem_log_stm, "%s(%lx) called from %s:%d\n",
                    caller, (unsigned long)ptr, file, line);
            break;
        default:
            break;
    }

    invalid_operaion = VALID_ARG;
}

void *Mem_resize(void *ptr, long nbytes, const char *file, int line) {
    struct descriptor *bp;
    void *new_ptr;

    assert(ptr != NULL);
    assert(nbytes > 0);

    // if (!is_aligned(ptr) || (bp = hash_table_find(ptr)) == NULL || bp->free != NULL)
    //     Except_raise(&assert_failed, file, line);

    if (!is_aligned(ptr)) {
        invalid_operaion = INVALID_PTR;
    } else if ((bp = hash_table_find(ptr)) == NULL || bp->free != NULL) {
        invalid_operaion = RESIZE_UNALLOC_MEM;
    }

    if (invalid_operaion != VALID_ARG) {
        if (mem_log_stm == NULL)
            Except_raise(&assert_failed, file, line);  // abort
        else {
            logger(ptr, __func__, file, line);
            return ptr;  // do nothing
        }
    }

    new_ptr = Mem_alloc(nbytes, file, line);
    memcpy(new_ptr, ptr, (nbytes < bp->size) ? nbytes : bp->size);
    Mem_free(ptr, file, line);

    return new_ptr;
}

/*
    delete: 
        1. Find the descriptor in mem_block_hash_table ptr and remove it.
        2. The removed one will be joined into unused list.
*/
static void hash_table_delete(const void *ptr) {
    struct descriptor *bp, *last;
    last = bp = mem_block_hash_table[hash(ptr)];

    if (bp->ptr == ptr) {
        mem_block_hash_table[hash(ptr)] = bp->link;
    } else {
        bp = bp->link;
        while (bp != NULL) {
            if (bp->ptr == ptr) break;
            last = bp;
            bp = bp->link;
        }
        last->link = bp->link;
    }
}

/*
    free_block_merge:
        1. Merge freed block neighbouring with p in free_block_list.
        2. Used in Mem_free().
        3. Return whether it is merged.
*/
static int free_block_merge(struct descriptor *p) {
    struct descriptor *bp, *last;

    bp = NULL;

    for (last = p->free; last->free == p; last = last->free)
        if ((last->size + (char *)last->ptr) == p->ptr)
            bp = last;

    if (bp != NULL) {
        bp->size += p->size;
        last->free = p->free;
        hash_table_delete(p->ptr);
        return 1;
    }

    return 0;
}

void Mem_free(void *ptr, const char *file, int line) {
    if (ptr != NULL) {
        struct descriptor *bp;

        // if (!is_aligned(ptr) || (bp = hash_table_find(ptr)) == NULL || bp->free != NULL)
        //     Except_raise(&assert_failed, file, line);

        // It's too strict to raise assert_failed here.

        if (!is_aligned(ptr) || (bp = hash_table_find(ptr)) == NULL) {
            invalid_operaion = INVALID_PTR;
        } else if (bp->free != NULL) {
            invalid_operaion = FREE_FREED_MEM;
        }

        if (invalid_operaion != VALID_ARG) {
            if (mem_log_stm == NULL)
                Except_raise(&assert_failed, file, line);
            else {
                logger(ptr, __func__, file, line);
                return;  // remember
            }
        }

        if (free_block_merge(bp) == 0) {      // If cannot be merged, insert in free_block_list.
            bp->free = free_block_list.free;  // head insertion
            free_block_list.free = bp;
        }
    }
}