 #include <stdio.h>

volatile int eq = 0;
volatile int gt = 0;

volatile int nef = 0;
volatile int gtf = 0;

__attribute__((noinline)) bool is_eq(int x, int y) {
    return x == y;
}

__attribute__((noinline)) bool is_gt(int x, int y) {
    return x > y;
}

__attribute__((noinline)) bool is_nef(float x, float y) {
    return x != y;
}

__attribute__((noinline)) bool is_gtf(float x, float y) {
    return x > y;
}

int main() {
    eq = is_eq(1, 1);
    gt = is_gt(5, 6);

    nef = is_nef(1.0, 1.0);
    gtf = is_gtf(6.0, 5.0);

    return 0;
}
