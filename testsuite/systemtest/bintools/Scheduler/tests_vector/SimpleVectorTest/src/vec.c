#include <stdio.h>

typedef int int4 __attribute__((ext_vector_type(4)));

typedef float float2 __attribute__((ext_vector_type(2)));

volatile int4 foo;
volatile int4 foo2;

volatile float2 foo4;

int main(void) {
    float2 foo3;
    foo3.x = 1.0f;
    foo3.y = 2.0f;
    foo4 = foo3;
    foo3 = foo4;
    foo3 += 48.0f;
    int4 xyzzy;
    int4 xyzzy2;
//    int2 xyzzy3;
//    foo2.x = 1;
//    foo2.y = 2;
    xyzzy.x = 1;
    xyzzy.y = 2;
    xyzzy.z = 3;
    xyzzy.w = 4;

    xyzzy2.x = 3;
    xyzzy2.y = 4;
    xyzzy2.z = 5;
    xyzzy2.w = 6;
//    xyzzy3.x = 5;
//    xyzzy3.y = 6;

    foo = xyzzy;
    foo2 = xyzzy2;
    int4 bar = foo;
    bar*=2;
    bar+= foo2;
//    bar+=foo3;
/*
    putchar(48 + bar.x);
    putchar(48 + bar.y);
    putchar(48 + bar.z);
    putchar(48 + bar.w);*/
    bar += 48;
    putchar(bar.x);
    putchar(bar.y);
    putchar(bar.z);
    putchar(bar.w);
    putchar('\n');
    putchar(foo3.x);
    putchar(foo3.y);
}
