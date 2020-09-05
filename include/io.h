#ifndef IO_INCLUDE
#define IO_INCLUDE

#include <stdio.h>

/*
    get_word:
        1. Consume the next word in the file opened on fp,
        store it as a null-terminated string in but[0,size-1],
        and return true. 
        2. When reaching the end of the file without consuming
        a word, return false.
        3. More generally, a word begins with a charactor in a 
        first set followed by zero or more charactors in a rest
        set. Functions first() and rest() test a charactor for 
        membership in first and rest. If the charactor is in the 
        set, both of them will return nonzero value.
        4. If a word is longer than size-2 charactors, the excess
        charactors are discarded.
        5. size > 1 
        6. fp buf first rest != NULL
*/
int get_word(FILE* fp, char* buf, int size,
             int (*first)(int c),
             int (*rest)(int c));

#endif