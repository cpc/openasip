#include <lwpr.h>

volatile int a __attribute__((aligned(8))) = 13;

int main() {

    int addr = (int)&a;
    lwpr_print_int(addr);_TCE_STDOUT('\n');
    if (addr%8 != 0)
        lwpr_print_str("Variable a is misaligned!\n");
    else
        lwpr_print_str("Variable a is correctly aligned!\n");
    return 0;
}
