#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// This number has the 8th, 16th and 32nd bits set to '1', so when it
// is loaded usign signed pointers, the end result should be negative,
// except when using full width.
volatile long unsigned a = 2147516544;

int main() {
    // Test that load + sign extension to 64b works
    volatile long signed signedByteLoad = *(signed char*)&a + 123;
    volatile long signed signedShortLoad = *(short signed*)&a + 123;
    volatile long signed signedWordLoad = *(int*)&a + 123;
    volatile long signed signedLongLoad = *(long signed*)&a + 123;

    // Test that load + zero extension to 64b works
    volatile long signed byteLoad = *(unsigned char*)&a;
    volatile long signed shortLoad = *(short unsigned*)&a;
    volatile long signed wordLoad = *(unsigned int*)&a;
    volatile long signed longLoad = *(long unsigned*)&a;

    iprintf("%ld %ld %ld %ld %ld %ld %ld %ld \n",
            signedByteLoad, signedShortLoad, signedWordLoad, signedLongLoad,
            byteLoad, shortLoad, wordLoad, longLoad);
    return EXIT_SUCCESS;
}
