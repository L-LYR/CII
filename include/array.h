/*
    dynamic array
*/

#ifndef ARRAY_INCLUDE
#define ARRAY_INCLUDE

struct array_t;  // definition in arrayrep.h

/*
    Array_new:
        1. Create a dynamic array with len elements. elem_size is the size of each element.
        2. This function will call Arrayrep_init() to initialize the array_t.
*/
extern struct array_t* Array_new(int len, int elem_size);

/*
    Array_free:
        1. Deallocate an array.        
*/
extern void Array_free(struct array_t** arr);

// no need to implement
// extern int Array_length(struct array_t*arr);
// extern int Array_size(struct array_t*arr);

/*
    Array_get:
        1. Entry of the element in array. 
        2. Get by index and return a certain pointer.
*/
extern void* Array_get(struct array_t* arr, int i);

/*
    Array_put:
        1. Set the i-th element in array.
        2. Return the elem, if sucessfully.
*/
extern void* Array_put(struct array_t* arr, int i, void* elem);

/*
    Array_resize:
        1. Resize the array by len.
        2. If len > arr.length, the new blocks will be set to 0.
*/
extern void Array_resize(struct array_t* arr, int len);

/*
    Array_copy:
        1. Copy the first len elements from arr;
        2. If len > arr.length, the new blocks will be set to 0.
*/
extern struct array_t* Array_copy(struct array_t* arr, int len);

#endif