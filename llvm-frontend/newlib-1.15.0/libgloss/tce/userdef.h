
/*
 *	User defined operations support for gcc-move
 */


#ifdef __TCE__

/**
 * Macros for writing and reading of user define operations.
 */

/* Generates assembly code that writes to any input register. */
#define __userdef_write_input__(oper, type, input) \
({  asm type("%0 -> " #oper ";" : : "fri" (input));  })

/* A shortcut for the previous using 'volatile' as type. */
#define WRITETO(REGISTER, INPUT) \
    __userdef_write_input__(REGISTER, volatile, INPUT)


/* Generates assembly code that reads any output register. */
#define __userdef_read_output__(oper, type, output ) \
({  asm type(#oper " -> %0;" : "=fr" (output): );  })


/* Shortcut for reading an integer from an output register (untested). */
#define READINT(REGISTER, OUTPUT) \
    __userdef_read_output__(REGISTER, volatile, OUTPUT)


/*{int temp__; asm volatile(#REGISTER " -> %0;" : "=r" (temp__): );  temp__} */


/* Shortcut for reading an double from an output register (untested). */
#define READDBL(REGISTER) \
({double temp__; asm volatile(#REGISTER " -> %0;" : "=f" (temp__): );  } temp__;)

 
/* 
#define READDBL(REGISTER)

/*
 * Convention: the macros for user-defined instructions having multiple
 * results do not return any value.  The results are passed as reference
 * arguments and modified.
 */
/*

#define __userdef_88__(oper, type, r1, r2, r3, r4, r5, r6, r7, r8, i1, i2, i3, i4, i5, i6, i7, i8 )     \
({  asm type("%8 -> usr" #oper "_o;" "%9 -> usr" #oper "_o1;" "%10 -> usr" #oper "_o2;" "%11 -> usr" #oper "_o3;" "%12 -> usr" #oper "_o4;" "%13 -> usr" #oper "_o5;" "%14 -> usr" #oper "_o6;" "%15 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; " "usr" #oper "_r2 -> %2; " "usr" #oper "_r3 -> %3; " "usr" #oper "_r4 -> %4; " "usr" #oper "_r5 -> %5; " "usr" #oper "_r6 -> %6; " "usr" #oper "_r7 -> %7; "  : "=r" (r1), "=r" (r2), "=r" (r3), "=r" (r4), "=r" (r5), "=r" (r6), "=r" (r7), "=r" (r8) : "ri" (i1), "ri" (i2), "ri" (i3), "ri" (i4), "ri" (i5), "ri" (i6), "ri" (i7), "ri" (i8));  })

#define __userdef_87__(oper, type, r1, r2, r3, r4, r5, r6, r7, i1, i2, i3, i4, i5, i6, i7, i8 )     \
({  asm type("%7 -> usr" #oper "_o;" "%8 -> usr" #oper "_o1;" "%9 -> usr" #oper "_o2;" "%10 -> usr" #oper "_o3;" "%11 -> usr" #oper "_o4;" "%12 -> usr" #oper "_o5;" "%13 -> usr" #oper "_o6;" "%14 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; " "usr" #oper "_r2 -> %2; " "usr" #oper "_r3 -> %3; " "usr" #oper "_r4 -> %4; " "usr" #oper "_r5 -> %5; " "usr" #oper "_r6 -> %6; "  : "=r" (r1), "=r" (r2), "=r" (r3), "=r" (r4), "=r" (r5), "=r" (r6), "=r" (r7) : "ri" (i1), "ri" (i2), "ri" (i3), "ri" (i4), "ri" (i5), "ri" (i6), "ri" (i7), "ri" (i8));  })

#define __userdef_86__(oper, type, r1, r2, r3, r4, r5, r6, i1, i2, i3, i4, i5, i6, i7, i8 )     \
({  asm type("%6 -> usr" #oper "_o;" "%7 -> usr" #oper "_o1;" "%8 -> usr" #oper "_o2;" "%9 -> usr" #oper "_o3;" "%10 -> usr" #oper "_o4;" "%11 -> usr" #oper "_o5;" "%12 -> usr" #oper "_o6;" "%13 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; " "usr" #oper "_r2 -> %2; " "usr" #oper "_r3 -> %3; " "usr" #oper "_r4 -> %4; " "usr" #oper "_r5 -> %5; "  : "=r" (r1), "=r" (r2), "=r" (r3), "=r" (r4), "=r" (r5), "=r" (r6) : "ri" (i1), "ri" (i2), "ri" (i3), "ri" (i4), "ri" (i5), "ri" (i6), "ri" (i7), "ri" (i8));  })

#define __userdef_85__(oper, type, r1, r2, r3, r4, r5, i1, i2, i3, i4, i5, i6, i7, i8 )     \
({  asm type("%5 -> usr" #oper "_o;" "%6 -> usr" #oper "_o1;" "%7 -> usr" #oper "_o2;" "%8 -> usr" #oper "_o3;" "%9 -> usr" #oper "_o4;" "%10 -> usr" #oper "_o5;" "%11 -> usr" #oper "_o6;" "%12 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; " "usr" #oper "_r2 -> %2; " "usr" #oper "_r3 -> %3; " "usr" #oper "_r4 -> %4; "  : "=r" (r1), "=r" (r2), "=r" (r3), "=r" (r4), "=r" (r5) : "ri" (i1), "ri" (i2), "ri" (i3), "ri" (i4), "ri" (i5), "ri" (i6), "ri" (i7), "ri" (i8));  })

#define __userdef_84__(oper, type, r1, r2, r3, r4, i1, i2, i3, i4, i5, i6, i7, i8 )     \
({  asm type("%4 -> usr" #oper "_o;" "%5 -> usr" #oper "_o1;" "%6 -> usr" #oper "_o2;" "%7 -> usr" #oper "_o3;" "%8 -> usr" #oper "_o4;" "%9 -> usr" #oper "_o5;" "%10 -> usr" #oper "_o6;" "%11 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; " "usr" #oper "_r2 -> %2; " "usr" #oper "_r3 -> %3; "  : "=r" (r1), "=r" (r2), "=r" (r3), "=r" (r4) : "ri" (i1), "ri" (i2), "ri" (i3), "ri" (i4), "ri" (i5), "ri" (i6), "ri" (i7), "ri" (i8));  })

#define __userdef_83__(oper, type, r1, r2, r3, i1, i2, i3, i4, i5, i6, i7, i8 )     \
({  asm type("%3 -> usr" #oper "_o;" "%4 -> usr" #oper "_o1;" "%5 -> usr" #oper "_o2;" "%6 -> usr" #oper "_o3;" "%7 -> usr" #oper "_o4;" "%8 -> usr" #oper "_o5;" "%9 -> usr" #oper "_o6;" "%10 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; " "usr" #oper "_r2 -> %2; "  : "=r" (r1), "=r" (r2), "=r" (r3) : "ri" (i1), "ri" (i2), "ri" (i3), "ri" (i4), "ri" (i5), "ri" (i6), "ri" (i7), "ri" (i8));  })

#define __userdef_82__(oper, type, r1, r2, i1, i2, i3, i4, i5, i6, i7, i8 )     \
({  asm type("%2 -> usr" #oper "_o;" "%3 -> usr" #oper "_o1;" "%4 -> usr" #oper "_o2;" "%5 -> usr" #oper "_o3;" "%6 -> usr" #oper "_o4;" "%7 -> usr" #oper "_o5;" "%8 -> usr" #oper "_o6;" "%9 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; "  : "=r" (r1), "=r" (r2) : "ri" (i1), "ri" (i2), "ri" (i3), "ri" (i4), "ri" (i5), "ri" (i6), "ri" (i7), "ri" (i8));  })

#define __userdef_78__(oper, type, r1, r2, r3, r4, r5, r6, r7, r8, i1, i2, i3, i4, i5, i6, i7 )     \
({  asm type("%8 -> usr" #oper "_o;" "%9 -> usr" #oper "_o1;" "%10 -> usr" #oper "_o2;" "%11 -> usr" #oper "_o3;" "%12 -> usr" #oper "_o4;" "%13 -> usr" #oper "_o5;" "%14 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; " "usr" #oper "_r2 -> %2; " "usr" #oper "_r3 -> %3; " "usr" #oper "_r4 -> %4; " "usr" #oper "_r5 -> %5; " "usr" #oper "_r6 -> %6; " "usr" #oper "_r7 -> %7; "  : "=r" (r1), "=r" (r2), "=r" (r3), "=r" (r4), "=r" (r5), "=r" (r6), "=r" (r7), "=r" (r8) : "ri" (i1), "ri" (i2), "ri" (i3), "ri" (i4), "ri" (i5), "ri" (i6), "ri" (i7));  })

#define __userdef_77__(oper, type, r1, r2, r3, r4, r5, r6, r7, i1, i2, i3, i4, i5, i6, i7 )     \
({  asm type("%7 -> usr" #oper "_o;" "%8 -> usr" #oper "_o1;" "%9 -> usr" #oper "_o2;" "%10 -> usr" #oper "_o3;" "%11 -> usr" #oper "_o4;" "%12 -> usr" #oper "_o5;" "%13 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; " "usr" #oper "_r2 -> %2; " "usr" #oper "_r3 -> %3; " "usr" #oper "_r4 -> %4; " "usr" #oper "_r5 -> %5; " "usr" #oper "_r6 -> %6; "  : "=r" (r1), "=r" (r2), "=r" (r3), "=r" (r4), "=r" (r5), "=r" (r6), "=r" (r7) : "ri" (i1), "ri" (i2), "ri" (i3), "ri" (i4), "ri" (i5), "ri" (i6), "ri" (i7));  })

#define __userdef_76__(oper, type, r1, r2, r3, r4, r5, r6, i1, i2, i3, i4, i5, i6, i7 )     \
({  asm type("%6 -> usr" #oper "_o;" "%7 -> usr" #oper "_o1;" "%8 -> usr" #oper "_o2;" "%9 -> usr" #oper "_o3;" "%10 -> usr" #oper "_o4;" "%11 -> usr" #oper "_o5;" "%12 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; " "usr" #oper "_r2 -> %2; " "usr" #oper "_r3 -> %3; " "usr" #oper "_r4 -> %4; " "usr" #oper "_r5 -> %5; "  : "=r" (r1), "=r" (r2), "=r" (r3), "=r" (r4), "=r" (r5), "=r" (r6) : "ri" (i1), "ri" (i2), "ri" (i3), "ri" (i4), "ri" (i5), "ri" (i6), "ri" (i7));  })

#define __userdef_75__(oper, type, r1, r2, r3, r4, r5, i1, i2, i3, i4, i5, i6, i7 )     \
({  asm type("%5 -> usr" #oper "_o;" "%6 -> usr" #oper "_o1;" "%7 -> usr" #oper "_o2;" "%8 -> usr" #oper "_o3;" "%9 -> usr" #oper "_o4;" "%10 -> usr" #oper "_o5;" "%11 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; " "usr" #oper "_r2 -> %2; " "usr" #oper "_r3 -> %3; " "usr" #oper "_r4 -> %4; "  : "=r" (r1), "=r" (r2), "=r" (r3), "=r" (r4), "=r" (r5) : "ri" (i1), "ri" (i2), "ri" (i3), "ri" (i4), "ri" (i5), "ri" (i6), "ri" (i7));  })

#define __userdef_74__(oper, type, r1, r2, r3, r4, i1, i2, i3, i4, i5, i6, i7 )     \
({  asm type("%4 -> usr" #oper "_o;" "%5 -> usr" #oper "_o1;" "%6 -> usr" #oper "_o2;" "%7 -> usr" #oper "_o3;" "%8 -> usr" #oper "_o4;" "%9 -> usr" #oper "_o5;" "%10 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; " "usr" #oper "_r2 -> %2; " "usr" #oper "_r3 -> %3; "  : "=r" (r1), "=r" (r2), "=r" (r3), "=r" (r4) : "ri" (i1), "ri" (i2), "ri" (i3), "ri" (i4), "ri" (i5), "ri" (i6), "ri" (i7));  })

#define __userdef_73__(oper, type, r1, r2, r3, i1, i2, i3, i4, i5, i6, i7 )     \
({  asm type("%3 -> usr" #oper "_o;" "%4 -> usr" #oper "_o1;" "%5 -> usr" #oper "_o2;" "%6 -> usr" #oper "_o3;" "%7 -> usr" #oper "_o4;" "%8 -> usr" #oper "_o5;" "%9 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; " "usr" #oper "_r2 -> %2; "  : "=r" (r1), "=r" (r2), "=r" (r3) : "ri" (i1), "ri" (i2), "ri" (i3), "ri" (i4), "ri" (i5), "ri" (i6), "ri" (i7));  })

#define __userdef_72__(oper, type, r1, r2, i1, i2, i3, i4, i5, i6, i7 )     \
({  asm type("%2 -> usr" #oper "_o;" "%3 -> usr" #oper "_o1;" "%4 -> usr" #oper "_o2;" "%5 -> usr" #oper "_o3;" "%6 -> usr" #oper "_o4;" "%7 -> usr" #oper "_o5;" "%8 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; "  : "=r" (r1), "=r" (r2) : "ri" (i1), "ri" (i2), "ri" (i3), "ri" (i4), "ri" (i5), "ri" (i6), "ri" (i7));  })

#define __userdef_68__(oper, type, r1, r2, r3, r4, r5, r6, r7, r8, i1, i2, i3, i4, i5, i6 )     \
({  asm type("%8 -> usr" #oper "_o;" "%9 -> usr" #oper "_o1;" "%10 -> usr" #oper "_o2;" "%11 -> usr" #oper "_o3;" "%12 -> usr" #oper "_o4;" "%13 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; " "usr" #oper "_r2 -> %2; " "usr" #oper "_r3 -> %3; " "usr" #oper "_r4 -> %4; " "usr" #oper "_r5 -> %5; " "usr" #oper "_r6 -> %6; " "usr" #oper "_r7 -> %7; "  : "=r" (r1), "=r" (r2), "=r" (r3), "=r" (r4), "=r" (r5), "=r" (r6), "=r" (r7), "=r" (r8) : "ri" (i1), "ri" (i2), "ri" (i3), "ri" (i4), "ri" (i5), "ri" (i6));  })

#define __userdef_67__(oper, type, r1, r2, r3, r4, r5, r6, r7, i1, i2, i3, i4, i5, i6 )     \
({  asm type("%7 -> usr" #oper "_o;" "%8 -> usr" #oper "_o1;" "%9 -> usr" #oper "_o2;" "%10 -> usr" #oper "_o3;" "%11 -> usr" #oper "_o4;" "%12 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; " "usr" #oper "_r2 -> %2; " "usr" #oper "_r3 -> %3; " "usr" #oper "_r4 -> %4; " "usr" #oper "_r5 -> %5; " "usr" #oper "_r6 -> %6; "  : "=r" (r1), "=r" (r2), "=r" (r3), "=r" (r4), "=r" (r5), "=r" (r6), "=r" (r7) : "ri" (i1), "ri" (i2), "ri" (i3), "ri" (i4), "ri" (i5), "ri" (i6));  })

#define __userdef_66__(oper, type, r1, r2, r3, r4, r5, r6, i1, i2, i3, i4, i5, i6 )     \
({  asm type("%6 -> usr" #oper "_o;" "%7 -> usr" #oper "_o1;" "%8 -> usr" #oper "_o2;" "%9 -> usr" #oper "_o3;" "%10 -> usr" #oper "_o4;" "%11 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; " "usr" #oper "_r2 -> %2; " "usr" #oper "_r3 -> %3; " "usr" #oper "_r4 -> %4; " "usr" #oper "_r5 -> %5; "  : "=r" (r1), "=r" (r2), "=r" (r3), "=r" (r4), "=r" (r5), "=r" (r6) : "ri" (i1), "ri" (i2), "ri" (i3), "ri" (i4), "ri" (i5), "ri" (i6));  })

#define __userdef_65__(oper, type, r1, r2, r3, r4, r5, i1, i2, i3, i4, i5, i6 )     \
({  asm type("%5 -> usr" #oper "_o;" "%6 -> usr" #oper "_o1;" "%7 -> usr" #oper "_o2;" "%8 -> usr" #oper "_o3;" "%9 -> usr" #oper "_o4;" "%10 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; " "usr" #oper "_r2 -> %2; " "usr" #oper "_r3 -> %3; " "usr" #oper "_r4 -> %4; "  : "=r" (r1), "=r" (r2), "=r" (r3), "=r" (r4), "=r" (r5) : "ri" (i1), "ri" (i2), "ri" (i3), "ri" (i4), "ri" (i5), "ri" (i6));  })

#define __userdef_64__(oper, type, r1, r2, r3, r4, i1, i2, i3, i4, i5, i6 )     \
({  asm type("%4 -> usr" #oper "_o;" "%5 -> usr" #oper "_o1;" "%6 -> usr" #oper "_o2;" "%7 -> usr" #oper "_o3;" "%8 -> usr" #oper "_o4;" "%9 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; " "usr" #oper "_r2 -> %2; " "usr" #oper "_r3 -> %3; "  : "=r" (r1), "=r" (r2), "=r" (r3), "=r" (r4) : "ri" (i1), "ri" (i2), "ri" (i3), "ri" (i4), "ri" (i5), "ri" (i6));  })

#define __userdef_63__(oper, type, r1, r2, r3, i1, i2, i3, i4, i5, i6 )     \
({  asm type("%3 -> usr" #oper "_o;" "%4 -> usr" #oper "_o1;" "%5 -> usr" #oper "_o2;" "%6 -> usr" #oper "_o3;" "%7 -> usr" #oper "_o4;" "%8 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; " "usr" #oper "_r2 -> %2; "  : "=r" (r1), "=r" (r2), "=r" (r3) : "ri" (i1), "ri" (i2), "ri" (i3), "ri" (i4), "ri" (i5), "ri" (i6));  })

#define __userdef_62__(oper, type, r1, r2, i1, i2, i3, i4, i5, i6 )     \
({  asm type("%2 -> usr" #oper "_o;" "%3 -> usr" #oper "_o1;" "%4 -> usr" #oper "_o2;" "%5 -> usr" #oper "_o3;" "%6 -> usr" #oper "_o4;" "%7 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; "  : "=r" (r1), "=r" (r2) : "ri" (i1), "ri" (i2), "ri" (i3), "ri" (i4), "ri" (i5), "ri" (i6));  })

#define __userdef_58__(oper, type, r1, r2, r3, r4, r5, r6, r7, r8, i1, i2, i3, i4, i5 )     \
({  asm type("%8 -> usr" #oper "_o;" "%9 -> usr" #oper "_o1;" "%10 -> usr" #oper "_o2;" "%11 -> usr" #oper "_o3;" "%12 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; " "usr" #oper "_r2 -> %2; " "usr" #oper "_r3 -> %3; " "usr" #oper "_r4 -> %4; " "usr" #oper "_r5 -> %5; " "usr" #oper "_r6 -> %6; " "usr" #oper "_r7 -> %7; "  : "=r" (r1), "=r" (r2), "=r" (r3), "=r" (r4), "=r" (r5), "=r" (r6), "=r" (r7), "=r" (r8) : "ri" (i1), "ri" (i2), "ri" (i3), "ri" (i4), "ri" (i5));  })

#define __userdef_57__(oper, type, r1, r2, r3, r4, r5, r6, r7, i1, i2, i3, i4, i5 )     \
({  asm type("%7 -> usr" #oper "_o;" "%8 -> usr" #oper "_o1;" "%9 -> usr" #oper "_o2;" "%10 -> usr" #oper "_o3;" "%11 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; " "usr" #oper "_r2 -> %2; " "usr" #oper "_r3 -> %3; " "usr" #oper "_r4 -> %4; " "usr" #oper "_r5 -> %5; " "usr" #oper "_r6 -> %6; "  : "=r" (r1), "=r" (r2), "=r" (r3), "=r" (r4), "=r" (r5), "=r" (r6), "=r" (r7) : "ri" (i1), "ri" (i2), "ri" (i3), "ri" (i4), "ri" (i5));  })

#define __userdef_56__(oper, type, r1, r2, r3, r4, r5, r6, i1, i2, i3, i4, i5 )     \
({  asm type("%6 -> usr" #oper "_o;" "%7 -> usr" #oper "_o1;" "%8 -> usr" #oper "_o2;" "%9 -> usr" #oper "_o3;" "%10 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; " "usr" #oper "_r2 -> %2; " "usr" #oper "_r3 -> %3; " "usr" #oper "_r4 -> %4; " "usr" #oper "_r5 -> %5; "  : "=r" (r1), "=r" (r2), "=r" (r3), "=r" (r4), "=r" (r5), "=r" (r6) : "ri" (i1), "ri" (i2), "ri" (i3), "ri" (i4), "ri" (i5));  })

#define __userdef_55__(oper, type, r1, r2, r3, r4, r5, i1, i2, i3, i4, i5 )     \
({  asm type("%5 -> usr" #oper "_o;" "%6 -> usr" #oper "_o1;" "%7 -> usr" #oper "_o2;" "%8 -> usr" #oper "_o3;" "%9 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; " "usr" #oper "_r2 -> %2; " "usr" #oper "_r3 -> %3; " "usr" #oper "_r4 -> %4; "  : "=r" (r1), "=r" (r2), "=r" (r3), "=r" (r4), "=r" (r5) : "ri" (i1), "ri" (i2), "ri" (i3), "ri" (i4), "ri" (i5));  })

#define __userdef_54__(oper, type, r1, r2, r3, r4, i1, i2, i3, i4, i5 )     \
({  asm type("%4 -> usr" #oper "_o;" "%5 -> usr" #oper "_o1;" "%6 -> usr" #oper "_o2;" "%7 -> usr" #oper "_o3;" "%8 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; " "usr" #oper "_r2 -> %2; " "usr" #oper "_r3 -> %3; "  : "=r" (r1), "=r" (r2), "=r" (r3), "=r" (r4) : "ri" (i1), "ri" (i2), "ri" (i3), "ri" (i4), "ri" (i5));  })

#define __userdef_53__(oper, type, r1, r2, r3, i1, i2, i3, i4, i5 )     \
({  asm type("%3 -> usr" #oper "_o;" "%4 -> usr" #oper "_o1;" "%5 -> usr" #oper "_o2;" "%6 -> usr" #oper "_o3;" "%7 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; " "usr" #oper "_r2 -> %2; "  : "=r" (r1), "=r" (r2), "=r" (r3) : "ri" (i1), "ri" (i2), "ri" (i3), "ri" (i4), "ri" (i5));  })

#define __userdef_52__(oper, type, r1, r2, i1, i2, i3, i4, i5 )     \
({  asm type("%2 -> usr" #oper "_o;" "%3 -> usr" #oper "_o1;" "%4 -> usr" #oper "_o2;" "%5 -> usr" #oper "_o3;" "%6 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; "  : "=r" (r1), "=r" (r2) : "ri" (i1), "ri" (i2), "ri" (i3), "ri" (i4), "ri" (i5));  })

#define __userdef_48__(oper, type, r1, r2, r3, r4, r5, r6, r7, r8, i1, i2, i3, i4 )     \
({  asm type("%8 -> usr" #oper "_o;" "%9 -> usr" #oper "_o1;" "%10 -> usr" #oper "_o2;" "%11 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; " "usr" #oper "_r2 -> %2; " "usr" #oper "_r3 -> %3; " "usr" #oper "_r4 -> %4; " "usr" #oper "_r5 -> %5; " "usr" #oper "_r6 -> %6; " "usr" #oper "_r7 -> %7; "  : "=r" (r1), "=r" (r2), "=r" (r3), "=r" (r4), "=r" (r5), "=r" (r6), "=r" (r7), "=r" (r8) : "ri" (i1), "ri" (i2), "ri" (i3), "ri" (i4));  })

#define __userdef_47__(oper, type, r1, r2, r3, r4, r5, r6, r7, i1, i2, i3, i4 )     \
({  asm type("%7 -> usr" #oper "_o;" "%8 -> usr" #oper "_o1;" "%9 -> usr" #oper "_o2;" "%10 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; " "usr" #oper "_r2 -> %2; " "usr" #oper "_r3 -> %3; " "usr" #oper "_r4 -> %4; " "usr" #oper "_r5 -> %5; " "usr" #oper "_r6 -> %6; "  : "=r" (r1), "=r" (r2), "=r" (r3), "=r" (r4), "=r" (r5), "=r" (r6), "=r" (r7) : "ri" (i1), "ri" (i2), "ri" (i3), "ri" (i4));  })

#define __userdef_46__(oper, type, r1, r2, r3, r4, r5, r6, i1, i2, i3, i4 )     \
({  asm type("%6 -> usr" #oper "_o;" "%7 -> usr" #oper "_o1;" "%8 -> usr" #oper "_o2;" "%9 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; " "usr" #oper "_r2 -> %2; " "usr" #oper "_r3 -> %3; " "usr" #oper "_r4 -> %4; " "usr" #oper "_r5 -> %5; "  : "=r" (r1), "=r" (r2), "=r" (r3), "=r" (r4), "=r" (r5), "=r" (r6) : "ri" (i1), "ri" (i2), "ri" (i3), "ri" (i4));  })

#define __userdef_45__(oper, type, r1, r2, r3, r4, r5, i1, i2, i3, i4 )     \
({  asm type("%5 -> usr" #oper "_o;" "%6 -> usr" #oper "_o1;" "%7 -> usr" #oper "_o2;" "%8 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; " "usr" #oper "_r2 -> %2; " "usr" #oper "_r3 -> %3; " "usr" #oper "_r4 -> %4; "  : "=r" (r1), "=r" (r2), "=r" (r3), "=r" (r4), "=r" (r5) : "ri" (i1), "ri" (i2), "ri" (i3), "ri" (i4));  })

#define __userdef_44__(oper, type, r1, r2, r3, r4, i1, i2, i3, i4 )     \
({  asm type("%4 -> usr" #oper "_o;" "%5 -> usr" #oper "_o1;" "%6 -> usr" #oper "_o2;" "%7 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; " "usr" #oper "_r2 -> %2; " "usr" #oper "_r3 -> %3; "  : "=r" (r1), "=r" (r2), "=r" (r3), "=r" (r4) : "ri" (i1), "ri" (i2), "ri" (i3), "ri" (i4));  })

#define __userdef_43__(oper, type, r1, r2, r3, i1, i2, i3, i4 )     \
({  asm type("%3 -> usr" #oper "_o;" "%4 -> usr" #oper "_o1;" "%5 -> usr" #oper "_o2;" "%6 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; " "usr" #oper "_r2 -> %2; "  : "=r" (r1), "=r" (r2), "=r" (r3) : "ri" (i1), "ri" (i2), "ri" (i3), "ri" (i4));  })

#define __userdef_42__(oper, type, r1, r2, i1, i2, i3, i4 )     \
({  asm type("%2 -> usr" #oper "_o;" "%3 -> usr" #oper "_o1;" "%4 -> usr" #oper "_o2;" "%5 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; "  : "=r" (r1), "=r" (r2) : "ri" (i1), "ri" (i2), "ri" (i3), "ri" (i4));  })

#define __userdef_38__(oper, type, r1, r2, r3, r4, r5, r6, r7, r8, i1, i2, i3 )     \
({  asm type("%8 -> usr" #oper "_o;" "%9 -> usr" #oper "_o1;" "%10 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; " "usr" #oper "_r2 -> %2; " "usr" #oper "_r3 -> %3; " "usr" #oper "_r4 -> %4; " "usr" #oper "_r5 -> %5; " "usr" #oper "_r6 -> %6; " "usr" #oper "_r7 -> %7; "  : "=r" (r1), "=r" (r2), "=r" (r3), "=r" (r4), "=r" (r5), "=r" (r6), "=r" (r7), "=r" (r8) : "ri" (i1), "ri" (i2), "ri" (i3));  })

#define __userdef_37__(oper, type, r1, r2, r3, r4, r5, r6, r7, i1, i2, i3 )     \
({  asm type("%7 -> usr" #oper "_o;" "%8 -> usr" #oper "_o1;" "%9 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; " "usr" #oper "_r2 -> %2; " "usr" #oper "_r3 -> %3; " "usr" #oper "_r4 -> %4; " "usr" #oper "_r5 -> %5; " "usr" #oper "_r6 -> %6; "  : "=r" (r1), "=r" (r2), "=r" (r3), "=r" (r4), "=r" (r5), "=r" (r6), "=r" (r7) : "ri" (i1), "ri" (i2), "ri" (i3));  })

#define __userdef_36__(oper, type, r1, r2, r3, r4, r5, r6, i1, i2, i3 )     \
({  asm type("%6 -> usr" #oper "_o;" "%7 -> usr" #oper "_o1;" "%8 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; " "usr" #oper "_r2 -> %2; " "usr" #oper "_r3 -> %3; " "usr" #oper "_r4 -> %4; " "usr" #oper "_r5 -> %5; "  : "=r" (r1), "=r" (r2), "=r" (r3), "=r" (r4), "=r" (r5), "=r" (r6) : "ri" (i1), "ri" (i2), "ri" (i3));  })

#define __userdef_35__(oper, type, r1, r2, r3, r4, r5, i1, i2, i3 )     \
({  asm type("%5 -> usr" #oper "_o;" "%6 -> usr" #oper "_o1;" "%7 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; " "usr" #oper "_r2 -> %2; " "usr" #oper "_r3 -> %3; " "usr" #oper "_r4 -> %4; "  : "=r" (r1), "=r" (r2), "=r" (r3), "=r" (r4), "=r" (r5) : "ri" (i1), "ri" (i2), "ri" (i3));  })

#define __userdef_34__(oper, type, r1, r2, r3, r4, i1, i2, i3 )     \
({  asm type("%4 -> usr" #oper "_o;" "%5 -> usr" #oper "_o1;" "%6 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; " "usr" #oper "_r2 -> %2; " "usr" #oper "_r3 -> %3; "  : "=r" (r1), "=r" (r2), "=r" (r3), "=r" (r4) : "ri" (i1), "ri" (i2), "ri" (i3));  })

#define __userdef_33__(oper, type, r1, r2, r3, i1, i2, i3 )     \
({  asm type("%3 -> usr" #oper "_o;" "%4 -> usr" #oper "_o1;" "%5 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; " "usr" #oper "_r2 -> %2; "  : "=r" (r1), "=r" (r2), "=r" (r3) : "ri" (i1), "ri" (i2), "ri" (i3));  })

#define __userdef_32__(oper, type, r1, r2, i1, i2, i3 )     \
({  asm type("%2 -> usr" #oper "_o;" "%3 -> usr" #oper "_o1;" "%4 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; "  : "=r" (r1), "=r" (r2) : "ri" (i1), "ri" (i2), "ri" (i3));  })

#define __userdef_28__(oper, type, r1, r2, r3, r4, r5, r6, r7, r8, i1, i2 )     \
({  asm type("%8 -> usr" #oper "_o;" "%9 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; " "usr" #oper "_r2 -> %2; " "usr" #oper "_r3 -> %3; " "usr" #oper "_r4 -> %4; " "usr" #oper "_r5 -> %5; " "usr" #oper "_r6 -> %6; " "usr" #oper "_r7 -> %7; "  : "=r" (r1), "=r" (r2), "=r" (r3), "=r" (r4), "=r" (r5), "=r" (r6), "=r" (r7), "=r" (r8) : "ri" (i1), "ri" (i2));  })

#define __userdef_27__(oper, type, r1, r2, r3, r4, r5, r6, r7, i1, i2 )     \
({  asm type("%7 -> usr" #oper "_o;" "%8 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; " "usr" #oper "_r2 -> %2; " "usr" #oper "_r3 -> %3; " "usr" #oper "_r4 -> %4; " "usr" #oper "_r5 -> %5; " "usr" #oper "_r6 -> %6; "  : "=r" (r1), "=r" (r2), "=r" (r3), "=r" (r4), "=r" (r5), "=r" (r6), "=r" (r7) : "ri" (i1), "ri" (i2));  })

#define __userdef_26__(oper, type, r1, r2, r3, r4, r5, r6, i1, i2 )     \
({  asm type("%6 -> usr" #oper "_o;" "%7 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; " "usr" #oper "_r2 -> %2; " "usr" #oper "_r3 -> %3; " "usr" #oper "_r4 -> %4; " "usr" #oper "_r5 -> %5; "  : "=r" (r1), "=r" (r2), "=r" (r3), "=r" (r4), "=r" (r5), "=r" (r6) : "ri" (i1), "ri" (i2));  })

#define __userdef_25__(oper, type, r1, r2, r3, r4, r5, i1, i2 )     \
({  asm type("%5 -> usr" #oper "_o;" "%6 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; " "usr" #oper "_r2 -> %2; " "usr" #oper "_r3 -> %3; " "usr" #oper "_r4 -> %4; "  : "=r" (r1), "=r" (r2), "=r" (r3), "=r" (r4), "=r" (r5) : "ri" (i1), "ri" (i2));  })

#define __userdef_24__(oper, type, r1, r2, r3, r4, i1, i2 )     \
({  asm type("%4 -> usr" #oper "_o;" "%5 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; " "usr" #oper "_r2 -> %2; " "usr" #oper "_r3 -> %3; "  : "=r" (r1), "=r" (r2), "=r" (r3), "=r" (r4) : "ri" (i1), "ri" (i2));  })

#define __userdef_23__(oper, type, r1, r2, r3, i1, i2 )     \
({  asm type("%3 -> usr" #oper "_o;" "%4 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; " "usr" #oper "_r2 -> %2; "  : "=r" (r1), "=r" (r2), "=r" (r3) : "ri" (i1), "ri" (i2));  })

#define __userdef_22__(oper, type, r1, r2, i1, i2 )     \
({  asm type("%2 -> usr" #oper "_o;" "%3 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; "  : "=r" (r1), "=r" (r2) : "ri" (i1), "ri" (i2));  })

#define __userdef_18__(oper, type, r1, r2, r3, r4, r5, r6, r7, r8, i1 )     \
({  asm type("%8 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; " "usr" #oper "_r2 -> %2; " "usr" #oper "_r3 -> %3; " "usr" #oper "_r4 -> %4; " "usr" #oper "_r5 -> %5; " "usr" #oper "_r6 -> %6; " "usr" #oper "_r7 -> %7; "  : "=r" (r1), "=r" (r2), "=r" (r3), "=r" (r4), "=r" (r5), "=r" (r6), "=r" (r7), "=r" (r8) : "ri" (i1));  })

#define __userdef_17__(oper, type, r1, r2, r3, r4, r5, r6, r7, i1 )     \
({  asm type("%7 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; " "usr" #oper "_r2 -> %2; " "usr" #oper "_r3 -> %3; " "usr" #oper "_r4 -> %4; " "usr" #oper "_r5 -> %5; " "usr" #oper "_r6 -> %6; "  : "=r" (r1), "=r" (r2), "=r" (r3), "=r" (r4), "=r" (r5), "=r" (r6), "=r" (r7) : "ri" (i1));  })

#define __userdef_16__(oper, type, r1, r2, r3, r4, r5, r6, i1 )     \
({  asm type("%6 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; " "usr" #oper "_r2 -> %2; " "usr" #oper "_r3 -> %3; " "usr" #oper "_r4 -> %4; " "usr" #oper "_r5 -> %5; "  : "=r" (r1), "=r" (r2), "=r" (r3), "=r" (r4), "=r" (r5), "=r" (r6) : "ri" (i1));  })

#define __userdef_15__(oper, type, r1, r2, r3, r4, r5, i1 )     \
({  asm type("%5 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; " "usr" #oper "_r2 -> %2; " "usr" #oper "_r3 -> %3; " "usr" #oper "_r4 -> %4; "  : "=r" (r1), "=r" (r2), "=r" (r3), "=r" (r4), "=r" (r5) : "ri" (i1));  })

#define __userdef_14__(oper, type, r1, r2, r3, r4, i1 )     \
({  asm type("%4 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; " "usr" #oper "_r2 -> %2; " "usr" #oper "_r3 -> %3; "  : "=r" (r1), "=r" (r2), "=r" (r3), "=r" (r4) : "ri" (i1));  })

#define __userdef_13__(oper, type, r1, r2, r3, i1 )     \
({  asm type("%3 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; " "usr" #oper "_r2 -> %2; "  : "=r" (r1), "=r" (r2), "=r" (r3) : "ri" (i1));  })

#define __userdef_12__(oper, type, r1, r2, i1 )     \
({  asm type("%2 -> usr" #oper "_t; " "usr" #oper "_r -> %0; " "usr" #oper "_r1 -> %1; "  : "=r" (r1), "=r" (r2) : "ri" (i1));  })



#define __userdef_81__(oper, type, i1, i2, i3, i4, i5, i6, i7, i8 )     \
({  int result; asm type("%1 -> usr" #oper "_o;" "%2 -> usr" #oper "_o1;" "%3 -> usr" #oper "_o2;" "%4 -> usr" #oper "_o3;" "%5 -> usr" #oper "_o4;" "%6 -> usr" #oper "_o5;" "%7 -> usr" #oper "_o6;" "%8 -> usr" #oper "_t; usr " #oper "_r -> %0"  : "=r" (result) : "ri" (i1), "ri" (i2), "ri" (i3), "ri" (i4), "ri" (i5), "ri" (i6), "ri" (i7), "ri" (i8)); result;  })

#define __userdef_71__(oper, type, i1, i2, i3, i4, i5, i6, i7 )     \
({  int result; asm type("%1 -> usr" #oper "_o;" "%2 -> usr" #oper "_o1;" "%3 -> usr" #oper "_o2;" "%4 -> usr" #oper "_o3;" "%5 -> usr" #oper "_o4;" "%6 -> usr" #oper "_o5;" "%7 -> usr" #oper "_t; usr " #oper "_r -> %0"  : "=r" (result) : "ri" (i1), "ri" (i2), "ri" (i3), "ri" (i4), "ri" (i5), "ri" (i6), "ri" (i7)); result;  })

#define __userdef_61__(oper, type, i1, i2, i3, i4, i5, i6 )     \
({  int result; asm type("%1 -> usr" #oper "_o;" "%2 -> usr" #oper "_o1;" "%3 -> usr" #oper "_o2;" "%4 -> usr" #oper "_o3;" "%5 -> usr" #oper "_o4;" "%6 -> usr" #oper "_t; usr " #oper "_r -> %0"  : "=r" (result) : "ri" (i1), "ri" (i2), "ri" (i3), "ri" (i4), "ri" (i5), "ri" (i6)); result;  })

#define __userdef_51__(oper, type, i1, i2, i3, i4, i5 )     \
({  int result; asm type("%1 -> usr" #oper "_o;" "%2 -> usr" #oper "_o1;" "%3 -> usr" #oper "_o2;" "%4 -> usr" #oper "_o3;" "%5 -> usr" #oper "_t; usr " #oper "_r -> %0"  : "=r" (result) : "ri" (i1), "ri" (i2), "ri" (i3), "ri" (i4), "ri" (i5)); result;  })

#define __userdef_41__(oper, type, i1, i2, i3, i4 )     \
({  int result; asm type("%1 -> usr" #oper "_o;" "%2 -> usr" #oper "_o1;" "%3 -> usr" #oper "_o2;" "%4 -> usr" #oper "_t; usr " #oper "_r -> %0"  : "=r" (result) : "ri" (i1), "ri" (i2), "ri" (i3), "ri" (i4)); result;  })

#define __userdef_31__(oper, type, i1, i2, i3 )     \
({  int result; asm type("%1 -> usr" #oper "_o;" "%2 -> usr" #oper "_o1;" "%3 -> usr" #oper "_t; usr " #oper "_r -> %0"  : "=r" (result) : "ri" (i1), "ri" (i2), "ri" (i3)); result;  })

#define __userdef_21__(oper, type, i1, i2 )     \
({  int result; asm type("%1 -> usr" #oper "_o;" "%2 -> usr" #oper "_t; usr " #oper "_r -> %0"  : "=r" (result) : "ri" (i1), "ri" (i2)); result;  })

#define __userdef_11__(oper, type, i1 )     \
({  int result; asm type("%1 -> usr" #oper "_t; usr " #oper "_r -> %0"  : "=r" (result) : "ri" (i1)); result;  })



#define __userdef_80__(oper, type, i1, i2, i3, i4, i5, i6, i7, i8 )     \
({  asm type("%0 -> usr" #oper "_o;" "%1 -> usr" #oper "_o1;" "%2 -> usr" #oper "_o2;" "%3 -> usr" #oper "_o3;" "%4 -> usr" #oper "_o4;" "%5 -> usr" #oper "_o5;" "%6 -> usr" #oper "_o6;" "%7 -> usr" #oper "_t; "  :  : "ri" (i1), "ri" (i2), "ri" (i3), "ri" (i4), "ri" (i5), "ri" (i6), "ri" (i7), "ri" (i8));  })

#define __userdef_70__(oper, type, i1, i2, i3, i4, i5, i6, i7 )     \
({  asm type("%0 -> usr" #oper "_o;" "%1 -> usr" #oper "_o1;" "%2 -> usr" #oper "_o2;" "%3 -> usr" #oper "_o3;" "%4 -> usr" #oper "_o4;" "%5 -> usr" #oper "_o5;" "%6 -> usr" #oper "_t; "  :  : "ri" (i1), "ri" (i2), "ri" (i3), "ri" (i4), "ri" (i5), "ri" (i6), "ri" (i7));  })

#define __userdef_60__(oper, type, i1, i2, i3, i4, i5, i6 )     \
({  asm type("%0 -> usr" #oper "_o;" "%1 -> usr" #oper "_o1;" "%2 -> usr" #oper "_o2;" "%3 -> usr" #oper "_o3;" "%4 -> usr" #oper "_o4;" "%5 -> usr" #oper "_t; "  :  : "ri" (i1), "ri" (i2), "ri" (i3), "ri" (i4), "ri" (i5), "ri" (i6));  })

#define __userdef_50__(oper, type, i1, i2, i3, i4, i5 )     \
({  asm type("%0 -> usr" #oper "_o;" "%1 -> usr" #oper "_o1;" "%2 -> usr" #oper "_o2;" "%3 -> usr" #oper "_o3;" "%4 -> usr" #oper "_t; "  :  : "ri" (i1), "ri" (i2), "ri" (i3), "ri" (i4), "ri" (i5));  })

#define __userdef_40__(oper, type, i1, i2, i3, i4 )     \
({  asm type("%0 -> usr" #oper "_o;" "%1 -> usr" #oper "_o1;" "%2 -> usr" #oper "_o2;" "%3 -> usr" #oper "_t; "  :  : "ri" (i1), "ri" (i2), "ri" (i3), "ri" (i4));  })

#define __userdef_30__(oper, type, i1, i2, i3 )     \
({  asm type("%0 -> usr" #oper "_o;" "%1 -> usr" #oper "_o1;" "%2 -> usr" #oper "_t; "  :  : "ri" (i1), "ri" (i2), "ri" (i3));  })

#define __userdef_20__(oper, type, i1, i2 )     \
({  asm type("%0 -> usr" #oper "_o;" "%1 -> usr" #oper "_t; "  :  : "ri" (i1), "ri" (i2));  })

#define __userdef_10__(oper, type, i1 )     \
({  asm type("%0 -> usr" #oper "_t; "  :  : "ri" (i1));  })

*/

#define PURE_FUNCTION		/**/
#define CONTAINS_STATE		volatile

#endif
