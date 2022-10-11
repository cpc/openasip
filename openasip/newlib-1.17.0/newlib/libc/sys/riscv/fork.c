/*
 * Stub version of fork.
 */

#include <_ansi.h>
#include <_syslist.h>
#include <errno.h>
#undef errno
extern int errno;

int
_DEFUN (_fork, (),
        _NOARGS)
{
  errno = ENOSYS;
/*  assert(0 && "_fork is not defined in libgloss/libnosys");*/
  return -1;
}
