/*
    Associative table is a set of key-value pairs.
    This table looks like a hash table actually.
*/

#ifndef TABLE_INCLUDE
#define TABLE_INCLUDE

struct table_t;

/*
    Table_create:
        1. table_t will be allocated and deallocated by Table_create().
        2. hint is an estimate of the number of entries that the new 
        table is expected to hold. But all tables can hold an arbitrary
        number of entries ragardless of the value of hint. hint > 0.
        3. Both cmp() and hash() are function pointers which manipulate
        client-specific keys. 
            1) cmp() must return an integer less than zero, equal to zero, or
            greater than zero, if, respectively, x is less than y, x equals y,
            or x is greater than y. strcmp() is an example. 
            2) hash() must return a hash number for key.
            3) Each table can have its own cmp() and hash().
            4) In default, it will use Atom_hash() and Atom_cmp().
*/
extern struct table_t *Table_create(int hint,
                                    int (*cmp)(const void *x, const void *y),
                                    unsigned long (*hash)(const void *key));

/*
    Table_free:
        1. Free the table itself and set it to NULL.
        2. Won't free the key-value pairs.
*/
extern void Table_free(struct table_t **table);

/*
    Basic operations for table.
*/

/*
    Table_put:
        1. If key exists, it will overwrite the previous value and return it.
        If not, return NULL.
*/
extern void *Table_put(struct table_t *table, const void *key, void *value);

/*
    Table_get:
        1. If key exists, return the value. If not, return NULL. But if the value
        is NULL, the meaning of return value is ambiguous.
*/
extern void *Table_get(struct table_t *table, const void *key);

/*
    Table_remove:
        1. If key exists, remove the key-value pair, and return the value. If not,
        return NULL.
*/
extern void *Table_remove(struct table_t *table, const void *key);

/*
    Table_map:
        1. Call the function apply for every key-value pair in an unspecified order.

*/
extern void Table_map(struct table_t *table,
                      void (*apply)(const void *key, void **value, void *cl),
                      void *cl);

/*
    Table_to_array:
        1. Build an array with 2n+1 elements for an n elements table 
        and returns a pointer to the first element.
        2. The index of key is 2i+1 which the index of respective value is 2i.
        3. table can’t be changed while Table_map is visiting its bindings.
        4. Other features is like List_to_array() 2&3&4.
*/
extern void **Table_to_array(struct table_t *table, void *end);

// no need to implementation
// extern int Table_length(struct table_t *table);

#endif