/**
 * @file test_main.c
 *
 * Test data for AOutReaderTest.
 *
 * @author Mikael Lepistö 2004 (tmlepist@cs.tut.fi) 
 */

#include <stdlib.h> /* atoi */
#include "test_header.h"

int main(int argc, char *argv[]) {   
    return testFunction2(testFunction1(atoi(argv[1])));
}
