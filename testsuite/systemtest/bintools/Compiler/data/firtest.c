// Simple memset to test simplest possible software pipelining

#include <stdio.h>

#define N 3
volatile char dstBuf[N];
volatile char dstBuf2[N];

int
main(void) {
    volatile char* dstPtr = dstBuf;
    volatile char* dstPtr2 = dstBuf2;

    #pragma unroll
    for (int i = 0; i < N; i++) {
        *dstPtr++ = 2;
        *dstPtr2++ = 4;
    }

    int sum = 0;
    #pragma nounroll
    for (int i = 0; i < N; i++) {
        sum += dstBuf[i] * dstBuf2[i];
    }

    if (sum == N * 2 * 4) {
        putchar('o');
    } else {
        putchar('f');
    }
}
