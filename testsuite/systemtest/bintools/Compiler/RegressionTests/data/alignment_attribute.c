#include <lwpr.h>

volatile int a __attribute__((aligned(8))) = 13;
volatile int b[2] __attribute__((aligned(64)));

int main() {

    int addr = (int)&a;
    int addr2 = (int)&b;
    lwpr_print_int(addr);_TCE_STDOUT('\n');
    lwpr_print_int(addr2);_TCE_STDOUT('\n');
    if (addr%8 != 0)
        lwpr_print_str("Variable a is misaligned!\n");
    else
        lwpr_print_str("Variable a is correctly aligned!\n");
    if (addr2%64 != 0)
        lwpr_print_str("Variable b is misaligned!\n");
    else
        lwpr_print_str("Variable b is correctly aligned!\n");

    return 0;
}
