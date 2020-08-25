#include "set.h"

#include <limits.h>  // INT_MAX
#include <string.h>  // memset()

#include "algo.h"
#include "assert.h"
#include "atom.h"
#include "mem.h"

// Size of set (the same with table)
// Table_create will choose the greatest value which is less than hint
static int primes[] = {
    509,
    509,    // 512   = 2 ^ 9
    1021,   // 1025  = 2 ^ 10
    2039,   // 2048  = 2 ^ 11
    4093,   // 4096  = 2 ^ 12
    8191,   // 8192  = 2 ^ 13
    16381,  // 16384 = 2 ^ 14
    32771,  // 32768 = 2 ^ 15
    65521,  // 65535 = 2 ^ 16
    INT_MAX,
};

struct set_t *Set_create(int hint,
                         int (*cmp)(const void *x, const void *y),
                         unsigned long (*hash)(const void *x)) {
    struct set_t *set;
    int i;

    assert(hint >= 0);

    // determine the size
    for (i = 1; primes[i] < hint; i++)
        ;
    hint = primes[i - 1];

    set = ALLOC(sizeof(*set) + hint * sizeof(set->buckets[0]));
    set->capacity = hint;
    if (cmp == NULL) {
        set->cmp = (int (*)(const void *, const void *))Atom_cmp;  // avoid warnings
    } else {
        set->cmp = cmp;
    }
    if (hash == NULL) {
        set->hash = (unsigned long (*)(const void *))Atom_hash;  // avoid warnings
    } else {
        set->hash = hash;
    }
    set->buckets = (struct member **)(set + 1);
    memset(set->buckets, 0, sizeof(set->buckets[0]) * hint);
    set->size = 0;
    set->time_stamp = 0;

    return set;
}

void Set_free(struct set_t **set) {
    assert(set != NULL);
    assert(*set != NULL);

    if ((*set)->size > 0) {
        int i;
        struct member *p, *q;
        for (i = 0; i < (*set)->capacity; i++) {
            for (p = (*set)->buckets[i]; p != NULL; p = q) {
                q = p->link;
                FREE(p);
            }
        }
    }
    FREE(*set);
}

int Set_has(struct set_t *set, const void *member) {
    unsigned long h;
    struct member *p;

    assert(set != NULL);
    assert(member != NULL);

    h = (*set->hash)(member) % set->size;
    for (p = set->buckets[h]; p != NULL; p = p->link) {
        if ((*set->cmp)(member, p->value) == 0) {
            break;
        }
    }

    return (p != NULL);
}

void Set_put(struct set_t *set, const void *member) {
    unsigned long h;
    struct member *p;

    assert(set != NULL);
    assert(member != NULL);

    h = (*set->hash)(member) % set->size;
    for (p = set->buckets[h]; p != NULL; p = p->link) {
        if ((*set->cmp)(member, p->value) == 0) {
            break;
        }
    }

    if (p == NULL) {
        // add member
        NEW(p);
        p->value = member;
        p->link = set->buckets[h];
        set->buckets[h] = p;
        set->size++;
    } else {
        // overwrite the previous one
        p->value = member;
    }

    set->time_stamp++;
}

void *Set_remove(struct set_t *set, const void *member) {
    unsigned long h;
    struct member **pp, *p;
    void *prev;

    assert(set != NULL);
    assert(member != NULL);

    prev = NULL;
    h = (*set->hash)(member) % set->size;
    for (pp = &set->buckets[h]; *pp != NULL; pp = &(*pp)->link) {
        if ((*set->cmp)(member, (*pp)->value) == 0) {
            p = *pp;
            prev = p->link;
            FREE(p);
            set->size--;
            break;
        }
    }
    set->time_stamp++;
    return prev;
}

void Set_map(struct set_t *set,
             void (*apply)(const void *member, void *cl),
             void *cl) {
    int i;
    unsigned stamp;
    struct member *p;
    assert(set != NULL);
    assert(apply != NULL);
    stamp = set->time_stamp;
    for (i = 0; i < set->capacity; ++i) {
        for (p = set->buckets[i]; p != NULL; p = p->link) {
            apply(p->value, cl);
            assert(stamp == set->time_stamp);  // set cannot be modified during Set_map
        }
    }
}

void **Set_to_array(struct set_t *set, void *end) {
    int i, j;
    void **arr;
    struct member *p;

    assert(set != NULL);

    arr = ALLOC((set->size + 1) * sizeof(*arr));
    for (i = 0; i < set->capacity; i++) {
        for (p = set->buckets[i]; p != NULL; p = p->link) {
            arr[j++] = (void *)p->value;
        }
    }
    arr[j] = end;
    return arr;
}

// Union operation

/*
    Set_copy:
        1. Copy the set t.
        2. The capacity of the duplicate will be no less than the original one.
*/
static struct set_t *Set_copy(struct set_t *t) {
    assert(t != NULL);
    struct set_t *dup;
    struct member *p, *q;
    const void *member;
    unsigned long h;
    int i;
    // dup and t may have different capacity
    // dup.capacity >= t.capacity
    dup = Set_new(t->capacity, t->cmp, t->hash);
    for (i = 0; i < t->size; ++i) {
        for (p = t->buckets[i]; p != NULL; p = p->link) {
            member = p->value;
            h = (*dup->hash)(member) % dup->capacity;
            NEW(q);
            q->value = member;
            q->link = dup->buckets[h];
            dup->buckets[h] = q;
            dup->size++;
        }
    }
    return dup;
}

struct set_t *Set_union(struct set_t *s, struct set_t *t) {
    if (s == NULL) {
        assert(t != NULL);
        return Set_copy(t);
    } else if (t == NULL) {
        return Set_copy(s);
    } else {
        assert(s->cmp == t->cmp);
        assert(s->hash == t->hash);

        struct set_t *res;
        struct member *p;
        int i;

        res = Set_copy(s);
        for (i = 0; i < t->size; ++i) {
            for (p = t->buckets[i]; p != NULL; p = p->link) {
                Set_put(res, p->value);
            }
        }
        return res;
    }
}

static int min(int lhs, int rhs) { return (lhs < rhs) ? lhs : rhs; }

struct set_t *Set_inter(struct set_t *s, struct set_t *t) {
    if (s == NULL) {
        assert(t != NULL);
        return Set_create(t->capacity, t->cmp, t->hash);
    } else if (t == NULL) {
        return Set_create(s->capacity, s->cmp, s->hash);
    } else if (s->size < t->size) {
        return Set_Inter(t, s);  // to traverse the smaller set
    } else {
        assert(s->cmp == t->cmp);
        assert(s->hash == t->hash);

        struct set_t *res;
        struct member *p, *q;
        const void *member;
        int i;
        unsigned long h;

        res = Set_create(min(t->capacity, s->capacity), s->cmp, s->hash);
        for (i = 0; i < t->capacity; i++) {
            for (p = t->buckets[i]; p != NULL; p = p->link) {
                if (Set_has(s, p->value)) {
                    member = p->value;
                    h = (*res->hash)(member) % res->capacity;
                    NEW(q);
                    q->value = member;
                    q->link = res->buckets[h];
                    res->buckets[h] = q;
                    res->size++;
                }
            }
        }
        return res;
    }
}

struct set_t *Set_minus(struct set_t *s, struct set_t *t) {
    // return s - t;
    if (s == NULL) {  // if s = {}, then s - t = {}
        assert(t != NULL);
        return Set_new(t->capacity, t->cmp, t->hash);
    } else if (t == NULL) {  // if t = {}, then s - t = s
        return Set_copy(s);
    } else {
        assert(s->cmp == t->cmp);
        assert(s->hash == t->hash);

        struct set_t *res;
        struct member *p, *q;
        const void *member;
        int i;
        unsigned long h;

        res = Set_create(min(t->capacity, s->capacity), t->cmp, t->hash);
        for (i = 0; i < s->capacity; i++) {
            for (p = s->buckets[i]; p != NULL; p = p->link) {
                if (!Set_has(t, p->value)) {
                    member = p->value;
                    h = (*res->hash)(member) % res->capacity;
                    NEW(q);
                    q->value = member;
                    q->link = res->buckets[h];
                    res->buckets[h] = q;
                    res->size++;
                }
            }
        }
        return res;
    }
}

struct set_t *Set_diff(struct set_t *s, struct set_t *t) {
    // return s ^ t
    if (s == NULL) {  // if s == {}, s ^ t = t
        assert(t != NULL);
        return Set_copy(t);
    } else if (t == NULL) {  // if t == {}, s ^ t = s
        return Set_copy(s);
    } else {
        assert(s->cmp == t->cmp);
        assert(s->hash == t->hash);

        struct set_t *res;
        struct member *p, *q;
        const void *member;
        int i;
        unsigned long h;

        res = Set_create(min(t->capacity, s->capacity), t->cmp, t->hash);

        for (i = 0; i < s->capacity; i++) {
            for (p = s->buckets[i]; p != NULL; p = p->link) {
                if (!Set_has(t, p->value)) {
                    member = p->value;
                    h = (*res->hash)(member) % res->capacity;
                    NEW(q);
                    q->value = member;
                    q->link = res->buckets[h];
                    res->buckets[h] = q;
                    res->size++;
                }
            }
        }

        for (i = 0; i < t->capacity; i++) {
            for (p = t->buckets[i]; p != NULL; p = p->link) {
                if (!Set_has(s, p->value)) {
                    member = p->value;
                    h = (*res->hash)(member) % res->capacity;
                    NEW(q);
                    q->value = member;
                    q->link = res->buckets[h];
                    res->buckets[h] = q;
                    res->size++;
                }
            }
        }

        return res;
    }
}