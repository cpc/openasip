/*
	setjmp.h
	stubs for future use.
*/

#ifndef _SETJMP_H_
#define _SETJMP_H_

#include "_ansi.h"
#include <machine/setjmp.h>

_BEGIN_STD_C

void	_EXFUN(longjmp,(jmp_buf __jmpb, int __retval));
int	_EXFUN(setjmp,(jmp_buf __jmpb));

#ifdef __TCE__
#define setjmp(env) ({int rv;                                           \
                      asm volatile (".setjmp" : "=r" (rv) : "ri" (env)); \
                      rv;})
#endif

_END_STD_C

#endif /* _SETJMP_H_ */

