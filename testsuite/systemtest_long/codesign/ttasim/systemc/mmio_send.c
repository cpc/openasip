#include <stdio.h>
#include <stdlib.h>

#include "mem_map.h"

/**
 * mmio_send.c: 
 *
 * Write data to a memory mapped shared register. Another TTA (see 
 * mmio_recv.c) is polling this register and printing it to a console 
 * whenever its value changes.
 */
int main() {
    /* These should get printed to the console through the another TTA. */
    for (int i = 1; i <= 10; ++i) {
        int new_value = 1234 * i;
        while (*BUSY_ADDR); /* Wait until not busy anymore. */
        // should place a barrier here to ensure the compiler doesn't
        // move the write above the loop or the iprintf call
        *DATA_ADDR = new_value;
    }
    return EXIT_SUCCESS;
}
