#include <lwpr.h>

__attribute__((always_inline))
int main() {
    volatile char stack_object[10];
    lwpr_print_hex((unsigned int)(&stack_object[9]));
    return 0;
}
