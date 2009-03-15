/*
 * Stub version of open.
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
_DEFUN (_open, (file, flags, mode),
        char *file  _AND
        int   flags _AND
        int   mode)
{
  errno = ENOSYS;
/*  assert(0 && "_open is not defined in libgloss/libnosys");*/
  return -1;
}

/* stub_warning(_open) */
