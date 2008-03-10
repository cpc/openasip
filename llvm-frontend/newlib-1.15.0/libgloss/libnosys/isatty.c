/*
 * Stub version of isatty.
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
_DEFUN (isatty, (file),
        int file)
{
  errno = ENOSYS;
/*  assert(0 && "isatty is not defined in libgloss/libnosys");*/
  return 0;
}

/* stub_warning(isatty) */
