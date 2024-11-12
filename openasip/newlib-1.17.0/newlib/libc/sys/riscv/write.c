/*
 * Stdout version of write.
 */

#include <_ansi.h>
#include <_syslist.h>
#include <errno.h>

#undef errno
extern int errno;

int
_DEFUN (_write, (file, ptr, len),
        int   file  _AND
        char *ptr   _AND
        int   len)
{
    int i;
    for (i = 0; i < len; i++) {
        _OA_RV_STDOUT((int)ptr[i]);
    }
    return len;
}


