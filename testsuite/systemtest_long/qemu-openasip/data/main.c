#include "crc.h"

#define UART_BASE 0x10000000
volatile char* const UART0 = (char*)UART_BASE;

void
uart_putc(char c) {
    *UART0 = c;
}

void
uart_puts(const char* s) {
    while (*s) uart_putc(*s++);
}

void
u32_to_hex(unsigned int val, char* buf) {
    static const char hex_chars[] = "0123456789ABCDEF";
    buf[0] = '0';
    buf[1] = 'x';
    for (int i = 0; i < 8; i++) {
        buf[9 - i] = hex_chars[val & 0xF];
        val >>= 4;
    }
    buf[10] = '\0';
}

unsigned char test[] = "TCE rocks!";
#define LENGTH 10

int
main(void) {
    char buf[12];

    uart_puts("CHECK_VALUE: ");
    u32_to_hex(CHECK_VALUE, buf);
    uart_puts(buf);
    uart_puts("\r\n");

    crc slowcrc = crcSlow(test, LENGTH);
    crc fastcrc = crcFast(test, LENGTH);

    uart_puts("Slow CRC: ");
    u32_to_hex(slowcrc, buf);
    uart_puts(buf);
    uart_puts("\r\n");

    uart_puts("Fast CRC: ");
    u32_to_hex(fastcrc, buf);
    uart_puts(buf);
    uart_puts("\r\n");
    while (1)
        ;
    return 0;
}
