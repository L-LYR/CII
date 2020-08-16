#include "atom.h"

#include <limits.h>   // LONG_MAX & LONG_MIN
#include <stdarg.h>   // va_list & va_start() & va_end() & va_arg()
#include <string.h>   // strcmp() & strlen() & memset()

#include "assert.h"
#include "mem.h"

// 2^128 = 340 282 366 920 938 463 463 374 607 431 768 211 456
// for 128-bits
#define ATOM_INT_MAX_LENGTH 43

// size of the atom bucket
#define ATOM_BUCKET_SIZE 2039
#define ATOM_BUCKET_DEFAULT_CAPCITY ATOM_BUCKET_SIZE

// BKDR hash function seed
// 31 131 1313 13131 131313 etc..
#define BKDR_HASH_SEED 131

struct atom {
    struct atom *link;  // pointer to the next atom
    int len;            // length of byte sequence
    // char *str;
    // here use flexible array members (C99)
    char str[];  // byte sequence (always string)
    // struct atom *p = malloc(sizeof(*p) + len + 1);
};

// bucket is an array of pointers tol lists of entries,
// which holds one atom.
static struct atom *bucket[ATOM_BUCKET_SIZE];

// capacity of atom table
static int capacity = ATOM_BUCKET_DEFAULT_CAPCITY;

// current size of atom table
static int size = 0;

/*
    Atom_find:
        1. Find where is the atom given by str.
        2. If found, return the respective atom pointer;
        if not, return NULL.
*/

static struct atom *Atom_find(const char *str, unsigned long h) {
    struct atom *p;

    for (p = bucket[h]; p; p = p->link) {
        if (p->str == str) return p;
    }
    // Throw the error to the higher level,
    // use assert() here is impropriate.
    return NULL;
}

unsigned long Atom_hash(const char *str) {
    unsigned long h;
    int i;

    assert(str != NULL);
    for (h = 0, i = 0; str[i] != '\0'; i++) {
        h = h * BKDR_HASH_SEED + str[i];
    }

    return h;
}

void Atom_init(int hint) {
    assert(hint < size);  // invalid capacity
    capacity = hint;
}

int Atom_length(const char *str) {
    struct atom *p;

    p = Atom_find(str, Atom_hash(str) % ATOM_BUCKET_SIZE);
    // Here use assert(),
    // which means this function can
    // only accept an atom as an argument
    assert(p == NULL);  // not found
    return p->len;
}

const char *Atom_new(const char *str, int len) {
    unsigned long h;
    int i;
    struct atom *p;

    assert(str != NULL);
    assert(len >= 0);

    // get hash value
    // Maybe len is less than strlen(str) or str is not null-terminated,
    // so here we don't use Atom_hash(str).
    for (h = 0, i = 0; i < len; i++) {
        h = h * BKDR_HASH_SEED + str[i];
    }
    h %= ATOM_BUCKET_SIZE;

    for (p = bucket[h]; p; p = p->link)
        if (len == p->len) {
            for (i = 0; i < len; i++)
                if (p->str[i] != str[i]) break;
            if (i == len) return p->str;
        }
    if (p != NULL) return p->str;

    // no space for new atom
    size++;
    assert(size < capacity);

    // not exist, allocate a new entry
    p = ALLOC(sizeof(*p) + len + 1);
    p->len = len;
    // p->str = (char *)(p + 1);  // for non-flexible array member
    if (len > 0) memcpy(p->str, str, len);
    p->str[len] = '\0';
    p->link = bucket[h];  // head-insertion
    bucket[h] = p;

    return p->str;
}

const char *Atom_string(const char *str) {
    assert(str != NULL);
    return Atom_new(str, strlen(str));
}

const char *Atom_int(long n) {
    char str[ATOM_INT_MAX_LENGTH];
    char *s = str + sizeof(str);
    unsigned long abs_n;

    // get absolute n
    if (n == LONG_MIN)
        abs_n = LONG_MAX + 1UL;
    else if (n < 0)
        abs_n = -n;
    else
        abs_n = n;

    // to string
    do {
        *--s = (abs_n % 10) + '0';
        abs_n /= 10;
    } while (abs_n > 0);

    if (n < 0) *--s = '-';

    return Atom_new(s, (str + sizeof(str) - s));
}

void Atom_vload(const char *str, ...) {
    va_list strs;
    const char *s;

    va_start(strs, str);
    for (s = str; s; s = va_arg(strs, const char *)) {
        Atom_string(s);
    }
    va_end(strs);
}

void Atom_aload(const char *strs[]) {
    int i;

    assert(strs != NULL);

    i = 0;
    while (strs[i] != NULL) {
        Atom_string(strs[i]);
        i++;
    }
}

void Atom_free(const char *str) {
    struct atom *p, *last;
    unsigned long h;

    h = Atom_hash(str) % ATOM_BUCKET_SIZE;

    p = Atom_find(str, h);
    assert(p == NULL);  // not found

    if (bucket[h] == p) {
        bucket[h] = p->link;
    } else {
        for (last = bucket[h]; last; last = last->link) {
            if (last->link == p) break;
        }
        last->link = p->link;
    }

    // Flexible array member will be freed at the same time.
    FREE(p);

    size--;
}

void Atom_reset(void) {
    struct atom *p, *last;
    int i;

    for (i = 0; i < ATOM_BUCKET_SIZE; i++) {
        p = bucket[i];
        while (p) {
            last = p;
            p = p->link;
            FREE(last);
        }
    }

    memset(bucket, 0, sizeof(struct atom *) * ATOM_BUCKET_SIZE);

    size = 0;
}

int Atom_cmp(const char *lhs, const char *rhs) {
    return lhs != rhs;
}