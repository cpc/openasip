#define __shared_mem__ __attribute__((address_space(1)))
#include "tceops.h"

#define N 64

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
  volatile __shared_mem__ int* val = (volatile __shared_mem__ int*)0x04;
  for(int i=0; i<N; ++i)
  {
    val[i] = i*4 + ((i*4+1) << 8) + ((i*4+2) << 16) + ((i*4+3) << 24);
  }
  return 0;
}
