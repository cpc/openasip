/*
 * Stub version of execve.
 */

#include <_ansi.h>
#include <_syslist.h>
#include <errno.h>
#undef errno
extern int errno;

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
