
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



int main() {
    varargs_test(3, -123, 321, 0);
}
