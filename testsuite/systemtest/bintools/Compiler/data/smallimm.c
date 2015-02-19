#include <lwpr.h>
#include <stdio.h>

volatile int a;
volatile unsigned b;

void f(void) {
    if (b < 100 && (a > -50 && a < 50)) {
        puts("Hello.");
    }
}

int main() {
    b = 2;
    a = 0;
    f();
    return 0;
}
