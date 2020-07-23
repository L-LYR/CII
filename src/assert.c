#include "assert.h"

const struct except_t assert_failed = {"Assertion Failed"};

void(assert)(int e) {
    assert(e);
}  // Here use parentheses to avoid the macro expanding.