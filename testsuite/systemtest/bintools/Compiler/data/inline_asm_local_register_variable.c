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
    register int a_reg asm ("RF.13");
    register int b_reg asm ("RF.14");
    register int c_reg asm ("RF.9");
    register int d_reg asm ("RF.7");

    a_reg = a;
    b_reg = b;
    c_reg = c;
    d_reg = d;
    asm ("TEST":"=r"(result):"ir"(a_reg), "ir"(b_reg), "ir"(c_reg), "ir"(d_reg));
    iprintf("%X\n", result);

    return 0;
}
