#ifndef ALIGN_INCLUDE
#define ALIGN_INCLUDE

// used in memchk.c arena.c

// for boundary alignment
union align {
    int i;
    long l;
    long long ll;
    long *lp;
    void *p;
    void (*fp)(void);
    float f;
    double d;
    long double ld;
};

#define ALIGN_BOUND sizeof(union align)
#define ROUND_UP(nbytes, bound) (((nbytes) + (bound)-1) / (bound)) * (bound)
#define ROUND_UP_TO_ALIGN_BOUND(nbytes) ROUND_UP(nbytes, ALIGN_BOUND)

#endif