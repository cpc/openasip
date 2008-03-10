/**
 * @file function_pointers.c
 * 
 * Example program that tests function pointers.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 */

#include "userdef.h"
#include "functions.h"

typedef struct {
    int (*first)(int, int);
    int (*second)(int, int);    
} funcs;

/* Function pointers are passed as arguments to another function to avoid
   compiler optimizing them away (like it was with -O2 when called directly
   from main()). */
void doit(int a, int b, funcs funcs_to_exec) {
    printint(funcs_to_exec.first(a, b));
    printstring("\n");
    printint(funcs_to_exec.second(a, b));
    printstring("\n");
}

int main(int argc,char **argv,char **envp) {
    int a, b;
    funcs my_funcs;
    my_funcs.first = sum;
    my_funcs.second = sub;
    a = 27;
    b = 6;
    doit(a, b, my_funcs);

    return 0;
}
