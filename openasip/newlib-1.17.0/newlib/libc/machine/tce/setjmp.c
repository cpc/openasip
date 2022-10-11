#include <setjmp.h>

void
longjmp (jmp_buf env, int val)
{
  asm volatile (".longjmp" : : "ri" (env), "ri" (val));
}
