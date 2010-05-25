#include <assert.h>

volatile char a=1,b=2,c=3, addr=0;

int main() {
    volatile int value = *((int*)(addr));
    assert(value == 0);
    return value;
}
