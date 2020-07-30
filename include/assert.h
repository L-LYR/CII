/*
    This implementation is similar to the C standard library assert.h.
*/

#undef assert
#ifdef NDEBUG
#define assert(e) ((void)0);
#else
#include "except.h"
extern void assert(int e);
#define assert(e) ((void)((e) || (RAISE(assert_failed), 0)))
#endif

/*
It’s often possible to detect certain invalid pointers. For example, a
nonnull pointer is invalid if it specifies an address outside the client’s
address space, and pointers are often subject to alignmentrestrictions; 
for example, on some systems a pointer to a doublemust be a multiple of eight.


#define isBadPtr(p) ( ( ((unsigned int)(p)) % ((unsigned int)(p+1) - (unsigned int)(p)) ) != 0 )

    assert(!isBadPtr(ptr));

I don't know whether it is good to use, because I've not seen it anywhere before.
*/