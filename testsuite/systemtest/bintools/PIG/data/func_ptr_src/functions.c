/**
 * @file functions.c
 * 
 * Functions to be used in function pointer test.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 */

#include "functions.h"
#include "tceops.h"

char buf[32] = {0};

int sum(int a, int b) {
    printstring("Sum of ");
    printint(a);
    printstring(" and ");
    printint(b);
    printstring(" is ");
    return a + b;
}

int sub(int a, int b) {
    printstring("Difference of ");
    printint(a);
    printstring(" and ");
    printint(b);
    printstring(" is ");
    return a - b;
}

/* derived from Robert Jan Schaper's itoa function on Google Groups
 * http://groups.google.com/group/comp.lang.c++.moderated/browse_thread/thread/8a169f6b25a6594f/00c4c8fcb24dd1cc?q=itoa+gcc+base+kanze&_done=%2Fgroups%3Fhl%3Den%26lr%3D%26q%3Ditoa+gcc+base+kanze%26qt_s%3DSearch+Groups%26&_doneTitle=Back+to+Search&&d#00c4c8fcb24dd1cc
 */
char* int2charBuf(int number) {
    int radix = 10;
    int i = 30;

    /* make sure 0x00 is at last pos */
    buf[31] = 0x00;

    for (; number && i; --i, number /= radix) {
        buf[i] = "0123456789"[number % radix];
    }
    /* +1 due to predecrement in loop */
    return &buf[i+1];
}

void printint(int int_to_print) __attribute__((noinline));
void printstring(char* string) __attribute__((noinline));

void printint(int int_to_print)  {
    int i;
    char* startPoint = int2charBuf(int_to_print);
    printstring(startPoint);
}

void printstring(char* string) {
    while (*string) {        
        _TCE_STDOUT((int)(*string));
        string++;
    }
}

