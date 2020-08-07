/*
    The Except interface wraps the setjmp/longjmp facility in a
    set of macros and functions that collaborate to provide a
    structured exception facility. It isn't perfect, but it just
    can handle the Allocate_Failed.

    The macros in the Except interface are admittedly crude and
    somewhat brittle. Their unchecked runtime errors are
    particularly troublesome, and can be particularly difficult
    bugs to find. They suffice for most applications because
    exceptions should be used sparingly — only a handful in a
    large application. If exceptions proliferate, it’s usually
    a sign of more serious design errors.
*/

#ifndef EXCEPT_INCLUDE
#define EXCEPT_INCLUDE

#include <setjmp.h> // setjmp() & longjmp() & jmp_buf 

struct except_t {
    char* reason;
    // be initialized to a string that describes the exception
    // and printed when an unhandled exception occurs.
};
// Exceptions must be global or static variables
// so that their addresses identify them uniquely.

struct except_frame {
    struct except_frame* prev;  // point to predecessor
    jmp_buf env;
    const char* file;
    int line;
    const struct except_t* exception;
};

// states used in TRY-EXCEPT and TRY-FINALLY
enum {
    EXCEPT_ENTERED,
    EXCEPT_RAISED,
    EXCEPT_HANDLED,
    EXCEPT_FINALIZED,
};

// stack-top pointer
extern struct except_frame* except_stack;

// definition in assert.c
extern const struct except_t assert_failed;

/*
    Except_raise:
        1. Base of macro RAISE(e) & RE_RAISE.
        2. It is a checked runtime error to pass a null e to Except_raise.
*/
void Except_raise(const struct except_t* e, const char* file, int line);

#define RAISE(e) Except_raise(&(e), __FILE__, __LINE__)

#define RE_RAISE                                     \
    Except_raise(                                    \
        _except_frame.exception, _except_frame.file, \
        _except_frame.line)  // _except_frame is a object in TRY block

/*
    It is a unchecked runtime error to execute the C return
    statement inside a TRY-EXCEPT or TRY-FINALLY statement.
    If any of the statementsin a TRY-EXCEPT or TRY-FINALLY
    must do a return, they must do so with this macro instead
    of with the usual C return statement.

    Tricky: use comma expression here
*/
#define RETURN                   \
    switch (EXCEPT_STACK_POP, 0) \
    default:                     \
        return

/*
    TRY-EXCEPT grammer:

        TRY
            S
        EXCEPT(e1)
            S1
        EXCEPT(e2)
            S2
        ...
        EXCEPT(en)
            Sn
        ELSE                   // ELSE is optional
            S0
        END_TRY;

    TRY-FINALLY grammer:

        TRY
            S
        FINALLY
            S1
        END_TRY;
    
    S1 is always used to "clean up".

    TRY-FINALLY equal to TRY-EXCEPT with RE_RAISE:

        TRY
            S
        ELSE
            S1
            RE_RAISE;
        END_TRY;
        S1

    Expanded form:

    do {
        <create and push an except_frame>

        if (<first return from setjmp (0 by default)>) {
            [code block]
        } else if (<exception is e1>) {
            [Solution to e1]
            ...
        } else if (<exception is en>) {
            [Solution to en]
            ...
        } else {                // optional
            [Unexpected exception]
        }

        if (<an exception occurred and wasn't handled>)
            RE_RAISE;

    } while(0)
*/

#define EXCEPT_STACK_POP except_stack = except_stack->prev

#define TRY                                       \
    do {                                          \
        volatile int _except_flag;                \
        struct except_frame _except_frame;        \
        _except_frame.prev = except_stack;        \
        except_stack = &_except_frame;            \
        _except_flag = setjmp(_except_frame.env); \
        if (_except_flag == EXCEPT_ENTERED) {
#define EXCEPT(e)                                         \
    if (_except_flag == EXCEPT_ENTERED) EXCEPT_STACK_POP; \
    }                                                     \
    else if (_except_frame.exception == &(e)) {           \
        _except_flag = EXCEPT_HANDLED;
#define ELSE                                              \
    if (_except_flag == EXCEPT_ENTERED) EXCEPT_STACK_POP; \
    }                                                     \
    else {                                                \
        _except_flag = EXCEPT_HANDLED;
#define FINALLY                                           \
    if (_except_flag == EXCEPT_ENTERED) EXCEPT_STACK_POP; \
    }                                                     \
    {                                                     \
        if (_except_flag == EXCEPT_ENTERED) _except_flag = EXCEPT_FINALIZED;
#define END_TRY                                           \
    if (_except_flag == EXCEPT_ENTERED) EXCEPT_STACK_POP; \
    }                                                     \
    if (_except_flag == EXCEPT_RAISED) RE_RAISE;          \
    }                                                     \
    while (0)

#endif