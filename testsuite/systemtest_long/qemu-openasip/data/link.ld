ENTRY(_start)

MEMORY {
    CODE (rx)  : ORIGIN = 0x80000000, LENGTH = 512K
    DATA (rw)  : ORIGIN = 0x80080000, LENGTH = 512K
}

SECTIONS {
    /* Code sections (read-execute) */
    .text : {
        *(.text.init)
        *(.text*)
    } > CODE

    .rodata : {
        *(.rodata*)
    } > CODE
    
    /* Data sections (read-write) */
    .data : {
        *(.data*)
    } > DATA
    
    .bss : {
        *(.bss*)
        *(COMMON)
    } > DATA
}