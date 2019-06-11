/*
    Copyright (c) 2002-2009 Tampere University.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file test_header.h
 *
 * Test data header for AOutReaderTest.
 *
 * @author Mikael Lepistö 2004 (tmlepist-no.spam-cs.tut.fi) 
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
