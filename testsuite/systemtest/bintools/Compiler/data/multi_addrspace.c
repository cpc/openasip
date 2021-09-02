/*
 * Test code for multiple address spaces support.
 */

#include <stdio.h>
#include <stdint.h>

#define ASIZE 4

#define memory_0 __attribute__((address_space(0)))
#define memory_1 __attribute__((address_space(1)))
#define memory_2 __attribute__((address_space(2)))

/* Clang 13 orders the global data according to their lexical
   usage order, it seems, so we cannot guarantee them to be in
   this order. */
memory_0 volatile int space0[ASIZE];
memory_0 volatile int space0_1[ASIZE];
memory_1 volatile int space1[ASIZE];
memory_1 volatile int space1_1[ASIZE];
memory_2 volatile int space2[ASIZE];
memory_2 volatile int space2_1[ASIZE];

#define MAX(x, y) ((((uint32_t)x) > ((uint32_t)y)) ? ((uint32_t)x) : ((uint32_t)y))

int main() {
    int i = 0;
    for (; i < ASIZE; ++i) {
        space0[i] = i;
        space1[i] = space0[i] + 1;
        space2[i] = space0[i] + space1[i] + 2;
    }

    uint32_t max_start_addr = MAX(space1_1, space2_1);
    max_start_addr = MAX(max_start_addr, space0);
    max_start_addr = MAX(max_start_addr, space1);
    max_start_addr = MAX(max_start_addr, space2);
    max_start_addr = MAX(max_start_addr, space0_1);

    /* All of the arrays should start at zero thus receive smaller
       or equal start address as the (likely) largest one. */
    iprintf("%lu\n", max_start_addr);
#if 0
    /* This causes out of reg failure since LLVM generates a huge
       selection clause out of it, which likely doesn't play well
       with the predicate registers. */
    if (max_start_addr <= MAX(space0_1, space0))
        iprintf("OK\n");
    else
        iprintf("NOK\n");
#endif
    int tmp;
    _TCEAS_LDW("data", space0, tmp);
    iprintf("i=%d space0 ", tmp);
    _TCEAS_LDW("#1", space1, tmp);
    iprintf("i=%d space1 ", tmp);
    _TCEAS_LDW("data2", space2, tmp);
    iprintf("i=%d space2 ", tmp);

    return 0;
}
