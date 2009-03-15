/*
 * Stub version of kill.
 */

#include "config.h"
#include <_ansi.h>
#include <_syslist.h>
#include <errno.h>
#undef errno
extern int errno;
/*#include <assert.h>*/
#include "warning.h"

int
_DEFUN (_kill, (pid, sig),
        int pid  _AND
        int sig)
{
  errno = ENOSYS;
/*  assert(0 && "_kill is not defined in libgloss/libnosys");*/
  return -1;
}

/* stub_warning(_kill) */
