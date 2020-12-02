#include "array.h"

#include <stdlib.h>
#include <string.h>

#include "arrayrep.h"
#include "assert.h"
#include "mem.h"

void Arrayrep_init(struct array_t* array, int len, int elem_size, void* arr) {
    assert(array != NULL);
    assert(len == 0 && arr == NULL || len > 0 && arr != NULL);
    assert(elem_size > 0);

    array->length = len;
    array->elem_size = elem_size;
    array->array = arr;
}

struct array_t* Array_new(int len, int elem_size) {
    struct array_t* arr;
    NEW(arr);
    if (len > 0) {
        Arrayrep_init(arr, len, elem_size, CALLOC(len, elem_size));
    } else {
        Arrayrep_init(arr, len, elem_size, NULL);
    }
    return arr;
}

void Array_free(struct array_t** arrp) {
    assert(arrp != NULL && *arrp != NULL);
    FREE((*arrp)->array);
    FREE(arrp);
}

void* Array_get(struct array_t* arr, int i) {
    assert(arr != NULL);
    assert(i >= 0 && i < arr->length);
    return arr->array + i * arr->elem_size;
}

void* Array_put(struct array_t* arr, int i, void* elem) {
    assert(arr != NULL);
    assert(i >= 0 && i < arr->length);
    assert(elem != NULL);
    memcpy(arr->array + i * arr->elem_size, elem, arr->elem_size);
    return elem;
}

void Array_resize(struct array_t* arr, int len) {
    assert(arr != NULL);
    assert(len >= 0);
    if (len == 0) {
        FREE(arr->array);
    } else if (arr->length == 0) {
        arr->array = ALLOC(len * arr->elem_size);
    } else {
        RESIZE(arr->array, len * arr->elem_size);
    }
    arr->length = len;
}

struct array_t* Array_copy(struct array_t* arr, int len) {
    struct array_t* dup;

    assert(arr != NULL);
    assert(len >= 0);
    dup = Array_new(len, arr->elem_size);
    if (dup->length >= arr->length && arr->length > 0) {
        memcpy(dup->array, arr->array,
               arr->length * arr->elem_size);
    } else if (arr->length > dup->length && dup->length > 0) {
        memcpy(dup->array, arr->array,
               dup->length * arr->elem_size);
    }
    return dup;
}