/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
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
