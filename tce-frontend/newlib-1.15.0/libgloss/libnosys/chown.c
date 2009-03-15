/*
 * Stub version of chown.
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
_DEFUN (_chown, (path, owner, group),
        const char *path  _AND
        uid_t owner _AND
        gid_t group)
{
  errno = ENOSYS;
/*  assert(0 && "_chown not defined in libgloss/libnosys/chown.c");*/
  return -1;
}

/* stub_warning(_chown) */
