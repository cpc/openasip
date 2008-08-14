/**
 * @file functions.c
 * 
 * Functions to be used in function pointer test.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 */

#include "functions.h"


#ifdef __TCE_V1__
#include "tceops.h"
#else
#include "userdef.h"
#endif


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

void printint(int int_to_print) {
#ifdef __TCE_V1__
    _TCE_PRINT_INT(int_to_print);
#else
    WRITETO(print_int.1, int_to_print);
#endif

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

