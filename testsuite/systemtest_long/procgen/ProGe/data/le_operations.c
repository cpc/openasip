// Tests little-endian load and store operations.
#include <stdio.h>


volatile int sInt = 0xFFFFFFFF;
volatile short sShort = 0xFFFF;
volatile unsigned uShort = 0xFFFF;
volatile char sChar = 0xFF;
volatile unsigned uChar = 0xFF;

int
main() {

    char* psChar = (char*)&sInt;
    *(psChar) = 0xBE;
    *(psChar+1) = 0xBA;
    *(psChar+2) = 0xFE;
    *(psChar+3) = 0xCA;

    unsigned id = 0;
    iprintf("%2d: %08X\n", id++, sInt);

    sInt = 0xC001CAFE;
    iprintf("%2d: %08X\n", id++, *(psChar));
    iprintf("%2d: %08X\n", id++, *(psChar+1));
    iprintf("%2d: %08X\n", id++, *(psChar+2));
    iprintf("%2d: %08X\n", id++, *(psChar+3));

    unsigned char* puChar = (unsigned char*)&sInt;
    iprintf("%2d: %08X\n", id++, *(puChar));
    iprintf("%2d: %08X\n", id++, *(puChar+1));
    iprintf("%2d: %08X\n", id++, *(puChar+2));
    iprintf("%2d: %08X\n", id++, *(puChar+3));

    psChar = (char*)&sShort;
    *(psChar) = 0xFE;
    *(psChar+1) = 0xCA;
    iprintf("%2d: %08X\n", id++, sShort);
    puChar = (unsigned char*)&uShort;
    *(puChar) = 0xFE;
    *(puChar+1) = 0xCA;
    iprintf("%2d: %08X\n", id++, uShort);

    sShort = 0xBABE;
    iprintf("%2d: %08X\n", id++, *(psChar));
    iprintf("%2d: %08X\n", id++, *(psChar+1));
    uShort = 0xBABE;
    iprintf("%2d: %08X\n", id++, *(puChar));
    iprintf("%2d: %08X\n", id++, *(puChar+1));

    return 0;
}
