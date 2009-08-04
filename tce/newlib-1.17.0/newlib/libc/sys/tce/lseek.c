/*
 * Stub version of lseek.
 */

#include <_ansi.h>
#include <_syslist.h>
#include <errno.h>
#undef errno
extern int errno;

int
_DEFUN (_lseek, (file, ptr, dir),
        int   file  _AND
        int   ptr   _AND
        int   dir)
{
  errno = ENOSYS;
  return -1;
}
