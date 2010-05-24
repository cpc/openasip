#include <math.h>
#include <stdio.h>

volatile double d = 0.34f;
volatile float f = 0.54f;

int main() {
    d = 4.00f;
    f = 4.00f;
    d = sqrt(d);
    if (d == 2.00) {
	puts("OK");
    } else {
	puts("FAIL");
    }
    f = sqrtf(f);
    if (f == 2.00f) {
	puts("OK");
    } else {
	puts("FAIL");
    }
    return 0;
}
