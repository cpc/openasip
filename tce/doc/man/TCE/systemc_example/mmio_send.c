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
    char old_values_received = 0, values_received = 0;
    for (int i = 1; i <= 10; ++i) {
        int new_value = 1234 * i;
        *DATA_ADDR = new_value;
        /* Wait until the other TTA has processed the value. This is
           signalled by writing the count of values received so far
           to the BUSY_ADDR. */
        while ((values_received = *BUSY_ADDR) == old_values_received); 
        old_values_received = values_received;
    }
    return EXIT_SUCCESS;
}
