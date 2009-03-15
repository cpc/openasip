/*
 * Stub version of readlink.
 */

#include "config.h"
#include <_ansi.h>
#include <_syslist.h>
#include <errno.h>
#include <sys/types.h>
#undef errno
extern int errno;
/*#include <assert.h>*/
#include "warning.h"

int
_DEFUN (_readlink, (path, buf, bufsize),
        const char *path _AND
        char *buf _AND
        size_t bufsize)
{
  errno = ENOSYS;
/*  assert(0 && "_readlink is not defined in libgloss/libnosys");*/
  return -1;
}

/* stub_warning(_readlink) */
