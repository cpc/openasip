/**
 * A test to exercise the "address clipping" of the TTA:
 * access an address that is larger than the data address bus. 
 * In this case the address should drop the upper bits in
 * case the LSU data port is as wide as the address bus.
 *
 * Compile with -O0 to not optimize the whole thing away ;-)
 */

#include <assert.h>

int main() {
    /* This should access address 0x0000FFFF on a machine with 
       16 bit address port in LSU. */
    int* clipped = (int*)0x0001FFF0; 

    /* Thus, this should point to the same address. */
    int* not_clipped = (int*)0x0000FFF0; 

    *clipped = 123;
    assert(*clipped == *not_clipped);
}
