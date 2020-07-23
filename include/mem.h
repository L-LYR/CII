#ifndef MEM_INCLUDE
#define MEM_INCLUDE

// definition in mem.c
extern const struct except_t mem_failed;

/*
    Mem_alloc:
        1. Accept the size of required memory space and where it is called.
        2. Return the address of allocated memory space.
        3. nbytes > 0
        4. This function will not return a NULL, so is Mem_calloc().
        5. nbytes is not size_t type, but long type. This can avoid errors when
        negative numbers are passed to unsigned arguments, which will be a
        checked runtime error.
*/
extern void *Mem_alloc(long nbytes, const char *file, int line);

/*
    Mem_calloc:
        1. Accept the size of an array, size of an element and where it is
        called.
        2. Allocated memory space will be set to 0.
        3. Return the address of allocated memory space.
        4. count > 0, nbytes > 0
        5. The two function will be capsulation in macro for being called more
        easily.
*/
extern void *Mem_calloc(long count, long nbytes, const char *file, int line);

/*
    Mem_resize:
        1. Changes the size of the block allocated by a previous call to
        Mem_alloc, Mem_calloc, or Mem_resize.
        2. nbytes > 0
        3. Mem_resize expands or contracts the block so that it holds at
        least nbytes of memory, suitably aligned, and returns a pointer
        to the resized block.
        4. If Mem_resize cannot allocate the new block, it raises  Mem_Failed,
        with file and line as the exception coordinates.
*/
extern void *Mem_resize(void *ptr, long nbytes, const char *file, int line);

/*
    Mem_free:
        1. Accept a pointer to allocated space and where it is called.
        2. If ptr is NULL, Mem_free will do nothing.
        3. Mem_free will be packaged by FREE(ptr).
        4. In that implementation, it is a checked runtime error to pass
        Mem_free a nonnull ptr that was not returned by a previous call
        to Mem_alloc, Mem_calloc, or Mem_resize, or a ptr that has already
        been passed to Mem_free or Mem_resize. The values of Mem_free()'s
        file and line arguments are used to report these checked runtime
        errors.
*/
extern void Mem_free(void *ptr, const char *file, int line);

#define ALLOC(nbytes) Mem_alloc((nbytes), __FILE__, __LINE__)
#define CALLOC(count, nbytes) Mem_calloc((count), (nbytes), __FILE__, __LINE__)

/*
    When using malloc(), we always write codes like below:
        struct T* p;
        p = (struct T*)malloc(sizeof(struct T));
    In pure C, cast is redundant:
        p = malloc(sizeof(struct T));
    A better version of this idiom is:
        p = malloc(sizeof(*p));
    This can be used for any pointer type. More specifically, it is a template.
    So we get a further capsulation for ALLOC and CALLOC.
*/
#define NEW(p) ((p) = ALLOC((long)sizeof(*(p))))
#define NEW_0(p) ((p) = CALLOC(1, (long)sizeof(*(p))))

#define RESIZE(ptr, nbytes) \
    ((ptr) = Mem_resize((ptr), (nbytes), __FILE__, __LINE__))

// After being freed, ptr will be set to NULL for avoiding dangling pointers.
#define FREE(ptr) ((void)(Mem_free((ptr), __FILE__, __LINE__), (ptr) = 0))

#endif