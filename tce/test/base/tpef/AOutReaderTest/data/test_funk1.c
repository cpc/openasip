/**
 * @file test_funk1.c
 *
 * Test data for AOutReaderTest.
 *
 * @author Mikael Lepistö 2004 (tmlepist@cs.tut.fi) 
 */

#include "test_header.h"

FlagWord testFunction1(unsigned int word) {
    FlagWord temp;
    temp.flagWord = word;
    return temp;
}
