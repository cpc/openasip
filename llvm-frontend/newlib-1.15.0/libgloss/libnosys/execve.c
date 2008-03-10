/*
 * Stub version of execve.
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
_DEFUN (_execve, (name, argv, env),
        char  *name  _AND
        char **argv  _AND
        char **env)
{
  errno = ENOSYS;
/*  assert(0 && "_execve is not defined in libgloss/libnosys");*/
  return -1;
}

/* stub_warning(_execve) */
