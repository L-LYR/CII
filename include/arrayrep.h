/*
    lower level interface of dynamic array.
*/

#ifndef ARRAYREP_INCLUDE
#define ARRAYREP_INCLUDE

struct array_t {
    int length;
    int elem_size;
    char* array;
};

/*
    Array_Rep_init:
        1. Only valid way to initialize the fileds of the descriptors.
        2. len for array_t.length, elem_size for array_t.elem_size, 
        arr for array_t.array
        3. If length == 0, array must be NULL. 
        4. If length != 0, array mustn't be NULL.
*/
extern void Arrayrep_init(struct array_t* array, int len, int elem_size, void* arr);

#endif