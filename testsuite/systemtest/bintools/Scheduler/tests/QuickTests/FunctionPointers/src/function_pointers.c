/**
 * @file function_pointers.c
 * 
 * Example program that tests function pointers.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 */

#include "functions.h"
#include "stdio.h"

typedef struct {
    int (*first)(int, int);
    int (*second)(int, int);    
} funcs;

/* Make sure it really uses the function pointers instead of inlining the 
   calls away */
void doit(int a, int b, funcs funcs_to_exec) __attribute__((__noinline__));

/* Function pointers are passed as arguments to another function to avoid
   compiler optimizing them away (like it was with -O2 when called directly
   from main()). */
void doit(int a, int b, funcs funcs_to_exec) {
    putchar(funcs_to_exec.first(a, b)+48);
    printstring("\n");
    putchar(funcs_to_exec.second(a, b)+48);
    printstring("\n");
}

int main(int argc,char **argv,char **envp) {
    int a, b;
    funcs my_funcs;
    my_funcs.first = sum;
    my_funcs.second = sub;
    a = 7;
    b = 2;
    doit(a, b, my_funcs);

    return 0;
}
