// ------- target independent data printing macros for verifying ----

#ifndef __TCE__
#define OUTPUT_CHAR(x) putchar(x)
#else
#include "tceops.h"
#define OUTPUT_CHAR(x) {_TCE_STDOUT(x);}
#endif

#define OUTPUT_HEX(x) {((x) > 9)?(OUTPUT_CHAR((x)+'7')):(OUTPUT_CHAR((x)+'0'));}
#define OUTPUT_HEXBYTE(x) {OUTPUT_HEX((x)>>4);OUTPUT_HEX((x)&0xf);}

// target independent big endian hex form for 1,2 and 4 byte variables
#define OUTPUT_VAR(x) {\
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

#define START_PROFILING
#define END_PROFILING
