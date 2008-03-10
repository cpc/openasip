/**
 * @file test_header.h
 *
 * Test data header for AOutReaderTest.
 *
 * @author Mikael Lepistö 2004 (tmlepist@cs.tut.fi) 
 */

#ifndef TTA_AOUT_READER_TEST_HEADER
#define TTA_AOUT_READER_TEST_HEADER

/**
 * One can set FlagWord through setting one word or 
 * just some bits of it.
 */
typedef union {
    /**
     * Contains 32bit sliced into smaller pieces.
     */
    struct {
	unsigned f1:1;
	unsigned f2:2;
	unsigned f3:3;
	unsigned f4:5;
	unsigned f5:6;
	unsigned f6:7;
	unsigned f7:8;
    } flags;
    
    /**
     * Or alternative one whole word interface.
     */
    unsigned long flagWord;
} FlagWord;

/* functions */
FlagWord testFunction1(unsigned int word);
int testFunction2(FlagWord flags);

#endif
