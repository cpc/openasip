/*
 * Stub version of link.
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
_DEFUN (_link, (existing, new),
        char *existing _AND
        char *new)
{
  errno = ENOSYS;
/*  assert(0 && "_link is not defined in libgloss/libnosys");*/
  return -1;
}

/* stub_warning(_link) */
