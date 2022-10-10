#include <math.h>
#include <stdio.h>
#include <string.h>

int main() {
    volatile double a = 1.9;
    volatile double b = 2.2;
    volatile double c = exp(a);
    volatile int d = (int)c;

    printf("Testing printf\n");
    puts("Testing puts");

    printf("exp(1.9) is roughly %d", d);
    printf("\n");

    c = 100*sin(b);
    d = (int)c;

    printf("100*sin(2.2) is roughly %d", d);
    printf("\n");

    const char src[14] = "OpenASIP rules";
    const char dst[14];
    memcpy(&dst, &src, strlen(src)+1);
    printf("%s", dst);
    return 0;
}