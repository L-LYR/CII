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