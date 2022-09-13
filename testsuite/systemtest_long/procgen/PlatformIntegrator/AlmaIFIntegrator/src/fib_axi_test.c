#define __data__ __attribute__((address_space(5)))
#define __param__ __attribute__((address_space(6)))
#define __private__ __attribute__((address_space(0)))
#include "tceops.h"

#define N 64

volatile __param__ int *results_ext;
volatile __param__ int *results_local;

volatile __param__ int *done;

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
  // These collide with each other if the accesses go to the same memory
  results_ext   = (volatile __param__ int*)0x8000;
  results_local = (volatile __param__ int*)0x118000;
  done = (volatile __param__ int*)0x118180;

  for(int i=0; i<N; ++i)
  {
    results_local[64-i] = fib(i);
  }

  for(int i=0; i<N; ++i)
  {
    results_ext[i+1] = results_local[64-i];
  }

  for(int i=0; i<N; ++i)
  {
    results_local[i+1] = results_ext[i+1];
  }

  _TCE_ECC(0, results_local[66]);
  _TCE_LCC(0, results_local[67]);

  *done = 1;
  return 0;
}
