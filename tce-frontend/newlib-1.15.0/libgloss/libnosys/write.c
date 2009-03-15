/*
 * Stub version of write.
 */

#include "config.h"
#include <_ansi.h>
#include <_syslist.h>
#include <errno.h>
/*#include <assert.h>*/
#undef errno
extern int errno;
#include "warning.h"

int
_DEFUN (_write, (file, ptr, len),
        int   file  _AND
        char *ptr   _AND
        int   len)
{
    int i;
    for (i = 0; i < len; i++) {                
        asm volatile ("STDOUT"::"ir"((int)ptr[i]));
    }
    
    return len;

#if 0    
    errno = ENOSYS;
/*  assert(0 && "_write is not defined in libgloss/libnosys");*/
    return -1;
#endif
}

/* stub_warning(_write) */

