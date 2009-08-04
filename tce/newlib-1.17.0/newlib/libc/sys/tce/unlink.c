/*
 * Stub version of unlink.
 */

#include <_ansi.h>
#include <_syslist.h>
#include <errno.h>
#undef errno
extern int errno;

int
_DEFUN (_unlink, (name),
        char *name)
{
  errno = ENOSYS;
  return -1;
}

