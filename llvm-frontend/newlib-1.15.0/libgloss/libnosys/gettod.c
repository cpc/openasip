/*
 * Stub version of gettimeofday.
 */

#include "config.h"
#include <_ansi.h>
#include <_syslist.h>
#include <sys/time.h>
#include <sys/times.h>
#include <errno.h>
#undef errno
extern int errno;
/*#include <assert.h>*/
#include "warning.h"

struct timeval;
struct timezone;

int
_DEFUN (_gettimeofday, (ptimeval, ptimezone),
        struct timeval  *ptimeval  _AND
        struct timezone *ptimezone)
{
  errno = ENOSYS;
/*  assert(0 && "_gettimeofday is not defined in libgloss/libnosys");*/
  return -1;
}

/* stub_warning(_gettimeofday) */
