/*
    A set is an unordered collection of distinct members.
    Basic operations on a set are testing for membership,
    adding members, intersection, difference, and symmetric
    difference.
*/

#ifndef SET_INCLUDE
#define SET_INCLUDE

typedef int (*cmp_t)(const void *, const void *);
typedef unsigned long (*hash_t)(const void *);

struct member {
    struct member *link;
    const void *value;  // Attention!
    // value of a member is read-only
};

struct set_t {
    // features
    int capacity;
    int size;
    unsigned long time_stamp;
    // function pointers
    cmp_t cmp;
    hash_t hash;
    // flexible array members
    struct member **buckets;
};

/*
    Set_create:
        1. set_t will be allocated by Set_create().
        2. hint is an estimate of the number of entries that the new 
        set is expected to hold. But all sets can hold an arbitrary
        number of entries ragardless of the value of hint. hint > 0.
        3. Both cmp() and hash() are function pointers which manipulate
        client-specific keys.
            1) cmp() must return an integer less than zero, equal to zero, or
            greater than zero, if, respectively, x is less than y, x equals y,
            or x is greater than y. strcmp() is an example. 
            2) hash() must return a hash number for key.
            3) Each set can have its own cmp() and hash().
            4) In default, it will use Atom_hash() and Atom_cmp().
*/
extern struct set_t *Set_create(int hint, cmp_t cmp, hash_t hash);

/*
    Set_free:
        1. Free the set and set it to NULL.
*/
extern void Set_free(struct set_t **set);

// Basic Operation
/*
    Set_has:
        1. Return 1, if the set has this member. Otherwise, return 0.
*/
extern int Set_has(struct set_t *set, const void *member);

/*
    Set_put:
        1. Put the member into the set.
        2. If it has already been in the set, overwrite the previous one.
*/
extern void Set_put(struct set_t *set, const void *member);

/*
    Set_remove:
        1. Remove the member from the set and return it. If not in, return NULL.
*/
extern void *Set_remove(struct set_t *set, const void *member);

/*
    Set_map:
        1. Call the function apply for each member in set.
*/
extern void Set_map(struct set_t *set,
                    void (*apply)(const void *member, void *cl),
                    void *cl);

/*
    Set_to_array:
        1. Return a pointer to an N+1-element array that holds the
        N elements of set in an arbitrary order.
        2. Other features are like List_to_array() 2&3&4.
*/
extern void **Set_to_array(struct set_t *set, void *end);

// no need to implement
// extern int Set_length(struct set_t* set);

extern struct set_t *Set_union(struct set_t *s, struct set_t *t);
extern struct set_t *Set_inter(struct set_t *s, struct set_t *t);
extern struct set_t *Set_minus(struct set_t *s, struct set_t *t);
extern struct set_t *Set_diff(struct set_t *s, struct set_t *t);

#endif