.global _start

_start:
    li sp, 0x80100000
    call main
    