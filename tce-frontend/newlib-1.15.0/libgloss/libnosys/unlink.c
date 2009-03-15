/*
 * Stub version of unlink.
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
_DEFUN (_unlink, (name),
        char *name)
{
  errno = ENOSYS;
/*  assert(0 && "_unlink is not defined in libgloss/libnosys");*/
  return -1;
}

/* stub_warning(_unlink) */
