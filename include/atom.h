/*
    An atom is a pointer to a unique, immutable sequence of zero or more
    arbitrary bytes. Most atoms are pointers to null-terminated strings,
    but a pointer to any sequence of bytes can be an atom. Two atoms are
    identical if they point to the same location.

    TODO: Exercise 3.10 Atom_add() has not been implemented.
*/

#ifndef ATOM_INCLUDE
#define ATOM_INCLUDE
#include <stdbool.h>

/*
    Atom_init:
        1. Change the atom table capacity.
        2. hint must be bigger than the current size of atom table.
*/
extern void Atom_init(int hint);

/*
    Atom_new:
        1. Accept a pointer to a sequence of bytes and the number of it.
        2. Return the atom which is a pointer to the copy of the sequence
        in the atom table.
*/
extern const char* Atom_new(const char* str, int len);

/*
    Atom_string:
        1. Accept a null-teminated string, adds a copy of that string to
        the atom table.
        2. Return the atom.
*/
extern const char* Atom_string(const char* str);

/*
    Atom_int:
        1. Input a long integer.
        2. Return the atom foe the string representation of the argument.
*/
extern const char* Atom_int(long n);

/*
    Atom_length:
        1. Return the length of its atom argument.
*/
extern int Atom_length(const char* str);

/*
    Atom_vload:
        1. Accept at least one string.
        2. The string input sequence should be end with a NULL.
        For example:
            Atom_vload("At least", "one", "string", NULL);
*/
extern void Atom_vload(const char* str, ...);

/*
    Atom_aload:
        1. Accept an array of pinters to strings.
*/
extern void Atom_aload(const char* strs[]);

/*
    Atom_free:
        1. Free the atom given by str.
*/
extern void Atom_free(const char* str);

/*
    Atom_reset:
        1. Clear the atom table.
*/
extern void Atom_reset(void);

/*
    Atom_hash:
        1. Input a null-terminated string.
        2. Return the respective hash value.
        3. BKDR hash mentioned in THe C Programming Language.
*/
extern unsigned long Atom_hash(const char* str);

/*
    Atom_cmp:
        1. Atom is unique.
        2. It is simple, lhs != rhs.

*/
extern bool Atom_cmp(const char* lhs, const char* rhs);

#endif