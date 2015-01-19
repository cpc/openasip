#include <lwpr.h>
volatile int statex;

int main() {
    void *state = &&state0;
    while (1) {
        goto *state;
    state1:
        state = &&state2;
        statex = 1;
        lwpr_print_str("s1 ");
        continue;
    state0:
        state = &&state1;
        statex = 0;
        lwpr_print_str("s0 ");
        continue;
    }
    state2:
    lwpr_print_str("exiting");
    return 0;
}
