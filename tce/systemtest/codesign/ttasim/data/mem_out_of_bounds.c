/**
 * A test that performs a memory access out of bounds of the
 * data memory address space.
 *
 * Compile with -O0 to not optimize the whole thing away ;-)
 */

#include <assert.h>

volatile int* large = (int*)0x00FFFFF0; 
int main() {
    *large = 123;
    return 0;
}
