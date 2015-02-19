#include <stdint.h>

volatile uint32_t a[2];
volatile uint32_t b;

void f(void) {
    void *s = &&s0;
    while (1) {
        uint32_t d = a[0];
        uint32_t e = a[1];
	goto *s;
    s0:
	if (e < 100 && d > 10) {
	    if (e < 1) {
		s = &&s1;
	    } else {
		s = &&s2;
	    }
	}
	continue;
    s2:
        b = 0;
    s1:
	continue;
    }
}

int main() {
    f();
    return(0);
}
