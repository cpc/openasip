/*
 * Stub version of wait.
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
_DEFUN (_wait, (status),
        int  *status)
{
  errno = ENOSYS;
/*  assert(0 && "_wait is not defined in libgloss/libnosys");*/
  return -1;
}

/* stub_warning(_wait) */
