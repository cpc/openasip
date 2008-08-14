/**
 * @file endianess_test.c
 *
 * Program used to test that the endianess swapping is done
 * correctly in the simulator. Requires a special operation
 * called "printint" which prints the written integer to the
 * stdout of the simulator console in hexadecimal format. 
 *
 * Special operation "stdout" is used to printout comments 
 * to the simulator console.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 */

#include "userdef.h"

static inline void printstring(char* string) {
    while (*string) {
        WRITETO(stdout.1, *string);
        string++;
    }
}

static inline void printint(volatile int* int_to_print) {
    int i = 0;
    int temp;
    for (; i < 4; ++i) {
        temp = *((char*)int_to_print + i) & 0xFF;
        printstring(" ");
        WRITETO(print_int_as_hex.1, temp);
    }
}

static inline void printout(char* expected_out, volatile int* result) {
    printstring("expected: ");
    printstring(expected_out);
    printstring(" got: ");
    printint(result);
    printstring("\n");    
}

volatile int test_int1 = 0x11223344;
volatile int test_int2 = 0x11111111;
volatile short test_short1 = 0x1122;

int main() {
    
    test_int2 = test_int1 + test_int2;
    
    printout("0x11223344", &test_int1);
    printout("0x22334455", &test_int2);
    printout("0x1122", (int*)&test_short1);
    return 0;
}
