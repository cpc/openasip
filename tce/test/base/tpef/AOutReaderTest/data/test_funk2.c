/**
 * @file test_funk2.c
 *
 * Test data for AOutReaderTest.
 *
 * Hopefully uses some float registers.
 *
 * @author Mikael Lepistö 2004 (tmlepist@cs.tut.fi) 
 */

#include "test_header.h"

int testFunction2(FlagWord flags) {
    float floatVar1;
    float floatVar2;

    floatVar1 = flags.flags.f2;
    floatVar2 = flags.flags.f1 - 10;
    
    floatVar1 = floatVar1 + floatVar2;

    if (flags.flags.f2 != floatVar1) {
	return 0;
    } else {
	return 1;
    }
}

int testFunction3(FlagWord flags) {
    float floatVar1;
    float floatVar2;

    floatVar1 = flags.flags.f2;
    floatVar2 = flags.flags.f1 - 10;
    
    floatVar1 = floatVar1 + floatVar2;

    if (flags.flags.f2 != floatVar1) {
	return 0;
    } else {
	return 1;
    }
}

