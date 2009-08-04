/*
 * Stub version of close.
 */

#include <_ansi.h>
#include <_syslist.h>
#include <errno.h>
#undef errno
extern int errno;

int
_DEFUN (_close, (fildes),
        int fildes)
{
  errno = ENOSYS;
/*  assert(0 && "_close is not defined in newlib/libgloss/libnosys/close.c");*/
  return -1;
}
