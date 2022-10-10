/*
 * Stub version of kill.
 */

#include <_ansi.h>
#include <_syslist.h>
#include <errno.h>
#undef errno
extern int errno;

int
_DEFUN (_kill, (pid, sig),
        int pid  _AND
        int sig)
{
  errno = ENOSYS;
  return -1;
}

