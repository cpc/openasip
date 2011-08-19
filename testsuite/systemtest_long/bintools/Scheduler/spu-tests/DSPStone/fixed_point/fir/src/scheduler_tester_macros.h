// ------- target independent data printing macros for verifying ----

#ifndef __TCE__
//#include <stdio.h>
#define OUTPUT_CHAR(x) putchar(x)
#else
#include "tceops.h"
#define OUTPUT_CHAR(x) {_TCE_STDOUT(x);}
#endif

#define OUTPUT_HEX(x) {((x) > 9)?(OUTPUT_CHAR((x)+'7')):(OUTPUT_CHAR((x)+'0'));}
#define OUTPUT_HEXBYTE(x) {OUTPUT_HEX((x)>>4);OUTPUT_HEX((x)&0xf);}

#define OUTPUT_VAR_MACRO(x) {\
int intVal = *(int*)(&x);\
switch (sizeof(x)) {\
 case 1:\
     OUTPUT_HEXBYTE(intVal>>24);\
     break;\
 case 2:\
     OUTPUT_HEXBYTE((intVal>>24)&0xff);\
     OUTPUT_HEXBYTE((intVal>>16)&0xff);\
     break;\
 case 4: {\
     int intVal = *(int*)(&x);\
     OUTPUT_HEXBYTE((intVal>>24)&0xff);\
     OUTPUT_HEXBYTE((intVal>>16)&0xff);\
     OUTPUT_HEXBYTE((intVal>>8)&0xff);\
     OUTPUT_HEXBYTE(intVal&0xff);\
     break;\
 }\
}\
}

// do outputting in function, to make debugging easier
// if performance is really needed use OUTPUT_VAR_MACRO instead
void output_8bit(unsigned char val) __attribute__((noinline));
void output_16bit(unsigned short val) __attribute__((noinline));
void output_32bit(unsigned int val) __attribute__((noinline));

void output_8bit(unsigned char val) {
    OUTPUT_VAR_MACRO(val);
}

void output_16bit(unsigned short val) {
    OUTPUT_VAR_MACRO(val);
}

void output_32bit(unsigned int val) {
    OUTPUT_VAR_MACRO(val);
}

// target independent big endian hex form for 1,2 and 4 byte variables
#define OUTPUT_VAR(x) {\
int intVal = *(int*)(&x);\
switch (sizeof(x)) {\
 case 1: output_8bit(intVal);break;\
 case 2: output_16bit(intVal);break;\
 case 4: output_32bit(intVal);break;\
}\
}


#define START_PROFILING
#define END_PROFILING
