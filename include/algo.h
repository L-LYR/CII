#ifndef ALGO_INCLUDE
#define ALGO_INCLUDE

/*
    swap:
        1. Swap two memory blocks.
        2. size > 0
*/
extern void swap(void* lhs, void* rhs, long size);
// macro of swap for more convenient use
#define SWAP(lhs, rhs, type) swap(&(lhs), &(rhs), sizeof(type))

#endif