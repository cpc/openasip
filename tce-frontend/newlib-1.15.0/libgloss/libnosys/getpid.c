/*
 * Stub version of getpid.
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
_DEFUN (_getpid, (),
        _NOARGS)
{
  errno = ENOSYS;
/*  assert(0 && "_getpid is not defined in libgloss/libnosys");*/
  return -1;
}

/* stub_warning(_getpid) */
