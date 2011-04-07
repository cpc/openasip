#include "tceops.h"

#define JTAG_DATA 0x00003000
#define JTAG_CTRL 0x00003004
#define JTAG_WR_SPACE_MASK 0xFFFF0000

#ifdef _AVALON_SFU

void avalon_puts(char* str) {

    while (*str != '\0') {
        unsigned int reg = 0;
        while ((reg & JTAG_WR_SPACE_MASK) == 0) {
            _TCE_AVALON_LDW(JTAG_CTRL, reg);
        }
        _TCE_AVALON_STW(JTAG_DATA, *str);
        str++;
    }
}

#else

void avalon_puts(char* str) {

    volatile int* data_reg = (int*) JTAG_DATA;
    volatile int* ctrl_reg = (int*) JTAG_CTRL;
    
    while (*str != '\0') {
        unsigned int reg = 0;
        while ((reg & JTAG_WR_SPACE_MASK) == 0) {
            reg = (unsigned int) *ctrl_reg;
        }
        *data_reg = (int) *str;
        str++;
    }
}

#endif

int main() {
    
    char* msg = "Hello world!\n";
    
    unsigned char i = 0;
    while (1) {
        avalon_puts(msg);
        _TCE_LEDS(i);
        i++;
    }
    return 0;
}
