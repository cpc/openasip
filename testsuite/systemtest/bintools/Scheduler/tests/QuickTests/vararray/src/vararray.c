
#include <stdio.h>
#include <alloca.h>

void foo(int n)  __attribute__((noinline));
void bar(int n, int* arr)  __attribute__((noinline));

volatile int j = 4;

int main(void) {
    foo(j);
}

void foo(int n) {
//    int arr[n];
    int *arr = alloca(n*sizeof(int));
    for (int j = 0; j < n; j++) {
        arr[j]  = j;
    }
    bar(n, arr);
}

void bar(int n, int* arr) {
    int sum = 0;
    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }
    putchar('0' + sum);
}
