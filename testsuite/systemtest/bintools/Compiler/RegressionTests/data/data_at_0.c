#include <assert.h>

volatile char a=1,b=2,c=3;

int main() {
    volatile int value = *((int*)(0));
    assert(value == 0);
    return value;
}
