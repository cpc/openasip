/**
 * @file functions.c
 * 
 * Functions to be used in function pointer test.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 */

#include "functions.h"
#include "stdio.h"

#ifdef __TCE_V1__
#include "tceops.h"
#else
#include "userdef.h"
#endif


int sum(int a, int b) {
    printstring("Sum of ");
    putchar(a+'0');
    printstring(" and ");
    putchar(b+'0');
    printstring(" is ");
    return a + b;
}

int sub(int a, int b) {
    printstring("Difference of ");
    putchar(a+48);
    printstring(" and ");
    putchar(b+48);
    printstring(" is ");
    return a - b;
}

void printstring(char* string) {
    while (*string) {        
#ifdef __TCE_V1__
        _TCE_STDOUT((int)(*string));
#else
        WRITETO(stdout.1, *string);
#endif
        string++;
    }
}

