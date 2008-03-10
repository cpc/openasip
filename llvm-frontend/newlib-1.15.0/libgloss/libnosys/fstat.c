/*
 * Stub version of fstat.
 */

#include "config.h"
#include <_ansi.h>
#include <_syslist.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#undef errno
extern int errno;
/*#include <assert.h>*/
#include "warning.h"

int
_DEFUN (_fstat, (fildes, st),
        int          fildes _AND
        struct stat *st)
{
  errno = ENOSYS;
/*  assert(0 && "_fstat is not defined in libgloss/libnosys");*/
  return -1;
}

/* stub_warning(_fstat) */
