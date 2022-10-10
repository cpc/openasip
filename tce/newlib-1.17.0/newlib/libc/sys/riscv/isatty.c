/*
 * Stub version of isatty.
 */

#include <_ansi.h>
#include <_syslist.h>
#include <errno.h>
#undef errno
extern int errno;

int
_DEFUN (isatty, (file),
        int file)
{
  errno = ENOSYS;
/*  assert(0 && "isatty is not defined in libgloss/libnosys");*/
  return 0;
}
