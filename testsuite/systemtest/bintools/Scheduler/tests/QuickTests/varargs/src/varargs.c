
#include "scheduler_tester_macros.h"

#include <stdarg.h>

int varargs_test(int, ... );

// print int values until 0 is read
int varargs_test(int i, ... ) {    
    va_list vl;
    va_start( vl, i );
    
    do {
        OUTPUT_VAR(i);
        i = va_arg(vl, int);
    } while(i);
    
    va_end( vl );
}

int varargs_test2(int i, ... ) __attribute__((noinline));

// calculate until 0 is read 
int varargs_test2(int i, ... ) {
    va_list vl;
    va_start( vl, i );
    int a=1,b=1,c=1,d=1,e=1,f=1,g=1;
    int j;
    do {
        j = va_arg(vl, int);

        switch (j%7) {
        case 0:
            a=i;
        case 1:
            b=a*2;
        case 2:
            c=b+b;
        case 3:
            d=c+a;
        case 4:
            e=d+b;
        case 5:
            f=e+1;
        case 6:
            g=f+f;
        }        
        i += a+b+c+d+e+f+g;
    } while(j);
    
    OUTPUT_VAR(i);

    va_end( vl );
}

int main() {
    varargs_test(1,2,0);
    varargs_test(3, -123, 321, 0);
    varargs_test2(3, 123, 321, 0, 12, 31, 4, 0);
}
