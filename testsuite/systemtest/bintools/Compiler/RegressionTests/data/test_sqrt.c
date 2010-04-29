#include <math.h>
#include <stdio.h>

volatile double d = 0.34f;
volatile float f = 0.54f;

int main() {
    d = 4.00f;
    f = 4.00f;
    printf("square root of %f is %f\n", d, sqrt(d));
    printf("square root of %f is %f\n", f, sqrtf(f));
    return 0;
}
