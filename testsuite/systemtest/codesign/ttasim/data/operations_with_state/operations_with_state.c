/**
 * A test program using operations with state (clocked and non-clocked).
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 */
#include <stdio.h>
#include "userdef.h"

#undef stdout

int putchar(int ch) {
    WRITETO(stdout.1, ch);
    return (unsigned)ch;
}

void print_clock_value() {
    int clock_value = 0;

    WRITETO(clock.1, 1);
    READINT(clock.2, clock_value);
    printf("Clock value is %d.\n", clock_value);
}

void inc_and_print_acc() {
    int acc_value = 0;

    WRITETO(acc.1, 1);
    READINT(acc.2, acc_value);
    printf("Accumulator value is %d.\n", acc_value);
}

int main() {
    int clock_value = 0;
    int acc_value = 0;
    
    printf("Reseting the real time clock.\n");
    WRITETO(clock.1, 0);
    READINT(clock.2, clock_value);

    print_clock_value();
    print_clock_value();
    print_clock_value();
    print_clock_value();
    print_clock_value();
    print_clock_value();

    printf("Reseting accumulator.\n");
    WRITETO(acc.1, 0);
    READINT(acc.2, acc_value);

    inc_and_print_acc();
    inc_and_print_acc();
    inc_and_print_acc();
    inc_and_print_acc();
    inc_and_print_acc();
    inc_and_print_acc();
    
    return 0;
}
