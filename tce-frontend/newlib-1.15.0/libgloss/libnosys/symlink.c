/*
 * Stub version of symlink.
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
_DEFUN (_symlink, (path1, path2),
        const char *path1 _AND
        const char *path2)
{
  errno = ENOSYS;
/*  assert(0 && "_symlink is not defined in libgloss/libnosys");*/
  return -1;
}

/* stub_warning(_symlink) */
