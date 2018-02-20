#define __global__ __attribute__((address_space(5)))
#define __param__ __attribute__((address_space(6)))
#define __private__ __attribute__((address_space(0)))
#include "tceops.h"

#define N 64

 __global__ char test[4]={1,2,3,4};

volatile __global__ int results[N];

volatile __param__ int done = 0;

int fib(int n)
{
  volatile int a1=0, a2=1, retval;
  if (n <= 1)
    return n;
  for (volatile int i=0; i<n-1; ++i)
  {
      retval = a1+a2;
      a1 = a2;
      a2 = retval;
  }
  return retval;
}

int
main(void)
{
  // Force memory accesses with volatile
  for(volatile int i=0; i<N; ++i)
  {
    volatile int val = fib(i);
    val = ((val & 0xFF000000) >> 24)
        | ((val & 0x00FF0000) >> 8)
        | ((val & 0x0000FF00) << 8)
        | ((val & 0x000000FF) << 24);
    results[i] = val;
  }
  done = 1;
  return 0;
}
