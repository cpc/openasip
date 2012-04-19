/*
 * Test code for multiple address spaces support.
 */

#include <stdio.h>

#define ASIZE 4

volatile int space0[ASIZE] __attribute__((address_space(0)));
volatile int space0_1[ASIZE] __attribute__((address_space(0)));
volatile int space1[ASIZE] __attribute__((address_space(1)));
volatile int space1_1[ASIZE] __attribute__((address_space(1)));
volatile int space2[ASIZE] __attribute__((address_space(2)));
volatile int space2_1[ASIZE] __attribute__((address_space(2)));

int main() {
    int i = 0;
    for (; i < ASIZE; ++i) {
        space0[i] = i;
        space1[i] = space0[i] + 1;
        space2[i] = space0[i] + space1[i] + 2;
    }

    /* The start addresses of the tables should overlap as
       they are allocated in different address spaces. */
    iprintf("space0@%x space0_1@%x space1@%x space1_1@%x space2@%x space2_1@%x\n",
            (unsigned)space0, (unsigned)space0_1, (unsigned)space1, (unsigned)space1_1,
            (unsigned)space2, (unsigned)space2_1);
    for (i = 0; i < ASIZE; ++i) {
        iprintf("i=%d space0[i]=%d space1[i]=%d space2[i]=%d\n",
                i, space0[i], space1[i], space2[i]);
    }

    return 0;
}
