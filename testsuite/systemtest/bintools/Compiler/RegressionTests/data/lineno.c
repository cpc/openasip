#include <stdio.h>

__attribute__((noinline))
int test_function(int* x, int* y)
{
   (*x)++;
   (*y)--;
   return *x;
}

volatile int z = 0;
__attribute__((noinline))
int main() {
    int x = 1 , y = 2;
    z = x + y;
    x = test_function(&z, &y);
    y = test_function(&z, &y);
    return z + y;
}
