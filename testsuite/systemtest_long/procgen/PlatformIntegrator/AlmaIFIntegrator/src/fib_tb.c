#define __global__ __attribute__((address_space(5)))
#define __param__ __attribute__((address_space(6)))
#define __private__ __attribute__((address_space(0)))
#include "tceops.h"

#define N 64

volatile __param__ int done = 0;

__param__ char test[4]={1,2,3,4};

volatile __param__ int results[N];


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
    volatile char* val_c = (volatile char*)(&val);
    volatile short* val_h = (volatile short*)(&val);
    volatile char temp8;
    volatile short temp16;
    // Do some byte juggling to test 16- and 8-bit accesses
    temp8 = val_c[2];
    val_c[2] = val_c[0];
    val_c[0] = temp8;
    temp8 = val_c[3];
    val_c[3] = val_c[1];
    val_c[1] = temp8;
    temp16 = val_h[1];
    val_h[1] = val_h[0];
    val_h[0] = temp16;

    results[i] = val;
  }
  done = 1;
  return 0;
}
