// Tests values of uninitialized global and static values.

// The C standard mandates uninitialized globals and variables to be
// implicitly initialized to zero or NULL.

#include <stdio.h>

volatile int foo;

__attribute__((noinline))
int fn() {
    volatile static int bar;
    if (!bar) {
        bar = 321;
    }
    return bar + foo;
}

int
main() {
    if (!foo) foo = 123;

    if (fn() == 444) {
        puts("OK");
    } else {
        puts("FAIL");
    }
    return 0;
}
