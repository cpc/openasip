/**
 * @file datamem_init_test.c
 *
 * Example program for testing that initial values are written to 
 * data memory before starting simulation.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pjaaskel@cs.tut.fi)
 */

#include "userdef.h"

extern int foo;

static void printstring(char* string) {
    int printcount = 0;
    while (*string) {
        WRITETO(stdout.1, *string);
        string++;
        printcount++;
    }
}

static inline void printashex(int int_to_print) {
    WRITETO(print_int_as_hex.1, int_to_print);
}

int b = 0xffee1122;
int c; /* uninitialized, should be initialized to zero by ttasim */

int main() {
    int a = 0x11dd8822;

    printstring("intially, \t\ta=");
    printashex(a);

    printstring(", b=");
    printashex(b);

    printstring(", c=");
    printashex(c);

    printstring("\n");

    a = 0xdd881111;
    b = 0x11dd8811;
    c = 0x01010101;

    printstring("after assignment, \ta=");
    printashex(a);

    printstring(", b=");
    printashex(b);

    printstring(", c=");
    printashex(c);

    printstring("\n");

    return 0;
}
