/*
 * Stub version of stat.
 */

#include <_ansi.h>
#include <_syslist.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#undef errno
extern int errno;

int
_DEFUN (_stat, (file, st),
        const char  *file _AND
        struct stat *st)
{
  errno = ENOSYS;
  return -1;
}

