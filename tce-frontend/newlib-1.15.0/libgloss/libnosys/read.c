/*
 * Stub version of read.
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
_DEFUN (_read, (file, ptr, len),
        int   file  _AND
        char *ptr   _AND
        int   len)
{
  errno = ENOSYS;
/*  assert(0 && "_read is not defined in libgloss/libnosys");*/
  return -1;
}

/* stub_warning(_read) */
