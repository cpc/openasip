// Used for check data memory initialization image generation for
// little and big endian modes.

#include <lwpr.h>

volatile const int gInt = 0xCAFEBABE;
const unsigned gIntSize = sizeof(gInt);
volatile const short gShort = 0xBEEF;
const unsigned gShortSize = sizeof(gShort);

int
main() {

    unsigned char* ptr = (unsigned char*)&gInt;
    for (int i = 0; i < gIntSize; i++) {
        lwpr_print_hex(*ptr++);
        lwpr_print_str(" ");
    }
    lwpr_newline();

    ptr = (unsigned char*)&gShort;
    for (int i = 0; i < gShortSize; i++) {
        lwpr_print_hex(*ptr++);
        lwpr_print_str(" ");
    }
    lwpr_newline();

    return 0;
}
