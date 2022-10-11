#include <fcntl.h>
#include <unistd.h>
#include "userdef.h"

int open(const char* path, int flags, ...) {
    int fid = 0;
    WRITETO(open_emu.1, path);
    WRITETO(open_emu.2, flags);
    WRITETO(open_emu.3, 0777);
    READINT(open_emu.4, fid);
    return fid;
}

int close(int fd) {
    int retval = 0;
    WRITETO(close_emu.1, fd);
    READINT(close_emu.2, retval);
    return retval;
}

ssize_t write(int fd, const void* buf, size_t count) {
    int wcount = 0;
    WRITETO(write_emu.1, fd);
    WRITETO(write_emu.2, buf);
    WRITETO(write_emu.3, count);
    READINT(write_emu.4, wcount);
    return wcount;
}

ssize_t read(int fd, void *buf, size_t count) {
    int rcount = 0;
    WRITETO(read_emu.1, fd);
    WRITETO(read_emu.2, buf);
    WRITETO(read_emu.3, count);
    READINT(read_emu.4, rcount);
    return rcount;
}

off_t lseek(int fildes, off_t offset, int whence) {
    off_t retval = 0;
    WRITETO(lseek_emu.1, fildes);
    WRITETO(lseek_emu.2, offset);
    WRITETO(lseek_emu.3, whence);
    READINT(lseek_emu.4, retval);
    return retval;
}

