/**
 * Tests issues with constants and reduced connectivity machines.
 *
 * Compile the .bc with -O0.
 */

#include <assert.h>

volatile int b = 0x00AAAAAA;

int main() {
    int a, res;
    a = 0x0000FFFF;
    res = a & b;
    printf("res == %d\n", res);
    return 0;
}
