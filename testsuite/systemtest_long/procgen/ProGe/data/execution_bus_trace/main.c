#include "tceops.h"

int main() {

    _TCE_LOCKTEST(1);

    volatile int a[10];
    volatile int b[10];
    int c = 10;
    int i;
    
    for (i = 0; i < 10; i++) {
        a[i] = i;
    }
    for (i = 0; i < 10; i++) {
        b[i] = a[10-1-i] + c;
    }
    return 0;
}
