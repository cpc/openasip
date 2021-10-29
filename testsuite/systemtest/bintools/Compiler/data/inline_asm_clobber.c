#include <stdio.h>

volatile int a;
volatile int b;
volatile int c;
volatile int d;
volatile int result;

int
main() {

    a = 0xC000;
    b = 0x0A00;
    c = 0x00F0;
    d = 0x000E;

    result = 0;

    // Clobber all register but RF.8, RF.9, RF.10, RF.11
    asm volatile ("TEST":"=r"(result)
                  :"ir"(a), "ir"(b), "ir"(c), "ir"(d)
                  :"RF.0", "RF.1", "RF.2", "RF.3",
                   "RF.4", "RF.5", "RF.6", "RF.7",
                   "RF.12", "RF.13", "RF.14", "RF.15");
    iprintf("%X\n", result);

    return 0;
}
