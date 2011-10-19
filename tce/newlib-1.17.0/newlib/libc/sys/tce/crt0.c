/* libc/sys/linux/crt0.c - Run-time initialization */

/* FIXME: This should be rewritten in assembler and
          placed in a subdirectory specific to a platform.
          There should also be calls to run constructors. */

/* Written 2000 by Werner Almesberger */


#include <stdio.h>
#include <signal.h>

#undef __MORE_OR_LESS_ORIGINAL_FROM_NEWLIB
#ifndef __MORE_OR_LESS_ORIGINAL_FROM_NEWLIB

void _start(void) __attribute__((noinline));
void _exit(int) __attribute__((noinline,noreturn));

/* Clang C++ support needs this. -fno-cxa-atexit 
   does not work in 2.7. */
void*   __dso_handle = (void*) &__dso_handle;

void _start(void)
{
    asm __volatile__ (".call_global_ctors");
    // for initing signal table...
    //__init_signal();

    int retval = main();

    // note: atexit() not called as our targets are not supposed to exit,
    // destructors called just for completeness for simulations
    // this does not work with Clang 2.7 as it registers the destructors
    // to atexit() instead.
    asm __volatile__ (".call_global_dtors");

    _exit(retval);
}

volatile char __dummy__;

/* override normal exit. simulator should stop when _exit() is seen */
void _exit(int retval) {
    /* less instructions are generated, when this while exists */
    /* volatile write to workaround LLVM bug #965 */
    while(1) __dummy__ = 0; 

}

#else
#include <stdlib.h>
#include <time.h>
#include <string.h>

extern char **environ;

/* extern int main(int argc,char **argv,char **envp); */
/* extern int main(int argc, char* argv[]); */
/* extern int main(); */

extern char _end;
extern char __bss_start;

void _start(int args)
{
    /*
     * The argument block begins above the current stack frame, because we
     * have no return address. The calculation assumes that sizeof(int) ==
     * sizeof(void *). This is okay for i386 user space, but may be invalid in
     * other cases.
     */
    int *params = &args-1;
    int argc = *params;
    char **argv = (char **) (params+1);

    environ = argv+argc+1;

    /*initialize timezone info */
    /*     tzset(); */
/*     exit(main(argc,argv,environ));      */
    exit(main(argc,argv));     
}

#endif
