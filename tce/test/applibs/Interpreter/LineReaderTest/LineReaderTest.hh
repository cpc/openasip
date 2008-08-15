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
 * @file LineReaderTest.hh 
 *
 * A test suite for LineReader.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note reviewed 2 June 2004 by jm, pj, tr, jn
 * @note rating: green
 */

#ifndef TTA_LINE_READER_TEST_HH
#define TTA_LINE_READER_TEST_HH

#include <TestSuite.h>
#include <string>
#include <cstdio>

#include "EditLineReader.hh"
#include "Exception.hh"
#include "FileSystem.hh"

using std::string;


/**
 * Class that tests LineReaders.
 */
class LineReaderTest : public CxxTest::TestSuite {

public:
    void setUp();
    void tearDown();

    void testReadLine();
    void testInitialize();
    void testConfirmation();
    void testConfirmation2();

private:
    /// File from which input for LineReader is read.
    static const string INPUT_FILE;
};


const string LineReaderTest::INPUT_FILE =
    "data" + FileSystem::DIRECTORY_SEPARATOR + "input";


/**
 * Called before each test.
 */
void
LineReaderTest::setUp() {
}


/**
 * Called after each test.
 */
void
LineReaderTest::tearDown() {
}


/**
 * Tests that line is read correctly.
 */
void
LineReaderTest::testReadLine() {
    
    EditLineReader reader;
    FILE* in = fopen(INPUT_FILE.c_str(), "r");
    reader.initialize("", in);
    string line;
    TS_ASSERT_THROWS_NOTHING(line = reader.readLine());
    TS_ASSERT_EQUALS(line, "yes yes yes\n");
    fclose(in);
}


/**
 * Tests that without initialization, an exception is thrown.
 */
void
LineReaderTest::testInitialize() {
    EditLineReader reader;
    TS_ASSERT_THROWS(reader.readLine(), ObjectNotInitialized);
    TS_ASSERT_THROWS(reader.charQuestion("", ""), ObjectNotInitialized);
    TS_ASSERT_THROWS(reader.confirmation(""), ObjectNotInitialized);
}


/**
 * Tests that reading of one character works.
 */
void
LineReaderTest::testConfirmation() {
    EditLineReader reader;
    FILE* in = fopen(INPUT_FILE.c_str(), "r");
    reader.initialize("", in);
    bool result = false;
    
    TS_ASSERT_THROWS_NOTHING(result = reader.confirmation(""));
    TS_ASSERT_EQUALS(result, true);
    
    // now test that default character is returned, if none is given
    TS_ASSERT_THROWS_NOTHING(result = reader.confirmation(""));
    TS_ASSERT_EQUALS(result, false);
    
    fclose(in);
}


/**
 * Tests that confirmation works also when default character is '\0'.
 *
 * Then confirmation is asked until a legal answer is given.
 */
void
LineReaderTest::testConfirmation2() {
    EditLineReader reader;
    FILE* in = fopen(INPUT_FILE.c_str(), "r");
    reader.initialize("", in);
    bool result = false;
    TS_ASSERT_THROWS_NOTHING(result = reader.confirmation("", '\0', 'y', 'n'));
    TS_ASSERT_EQUALS(result, true);
    fclose(in);
}

#endif
