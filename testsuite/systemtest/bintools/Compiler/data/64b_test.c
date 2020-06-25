#include <stdint.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

volatile long x = UINT_MAX;

int main() {
    iprintf("%u %u %u %u %u %lu\n",
            (unsigned)sizeof(long), (unsigned)sizeof(int),
            (unsigned)sizeof(uint64_t), (unsigned)sizeof(int64_t),
            (unsigned)sizeof(void*),
            x * 2);
    return EXIT_SUCCESS;
}
