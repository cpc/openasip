/*
 * Stub version of getpid.
 */

#include <_ansi.h>
#include <_syslist.h>
#include <errno.h>
#undef errno
extern int errno;

int
_DEFUN (_getpid, (),
        _NOARGS)
{
  errno = ENOSYS;
/*  assert(0 && "_getpid is not defined in libgloss/libnosys");*/
  return -1;
}
