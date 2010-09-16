#include <stdio.h>
#include <stdlib.h>

#include "mem_map.h"

/**
 * mmio_recv.c: 
 *
 * Polls an I/O register and prints out its value whenever it changes.
 */
int main() {
    int last_value = 0;
    char values_received = 0;
    do {
        int new_value;
        *BUSY_ADDR = values_received;
        // should place a barrier here to ensure the compiler doesn't
        // move the while loop above the write (both access constant
        // addresses thus are trivial to alias analyze)
        while ((new_value = *DATA_ADDR) == last_value);
        ++values_received;
        iprintf("mmio_recv got %d\n", new_value);
        last_value = new_value;
    } while(1);
    return EXIT_SUCCESS;
}
