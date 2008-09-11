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
 * @file UnreachableStreamTest.hh
 *
 * A test suite for UnreachableStream.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme-no.spam-tut.fi)
 * @note reviewed 7 November 2003 by am, ml, jn, pj
 */

#ifndef TTA_UNREACHABLE_STREAM_TEST_HH
#define TTA_UNREACHABLE_STREAM_TEST_HH

#include <string>
#include <TestSuite.h>
#include "Exception.hh"

using std::string;

/**
 * Implements the tests needed to verify correct operation of Exception.
 */
class UnreachableStreamTest : public CxxTest::TestSuite {
public:
    void testThrow();
    void testThrowWithDefaultParam();

private:
    /// Test file name.
    static const string testFile_;
    /// Test line number.
    static const int testLineNum_;
    /// Test procedure name.
    static const string testProcedure_;
    /// Name for an unknown procedure.
    static const string unknownProcedure_;
    /// Test input file.
    static const string testInputFile_;
};

const string UnreachableStreamTest::testFile_ = "UnreachableStreamTest.h";
const int UnreachableStreamTest::testLineNum_ = 13;
const string UnreachableStreamTest::testProcedure_ =
    "UnreachableStreamTest::testThrow()";
const string UnreachableStreamTest::unknownProcedure_ = "(unknown)";
const string UnreachableStreamTest::testInputFile_ = "testinputfile";

/**
 * Tests that correct information is passed when throwing exception.
 */
inline void
UnreachableStreamTest::testThrow() {
    try {
        throw UnreachableStream(testFile_, testLineNum_, testProcedure_,
                                testInputFile_);
    } catch (const UnreachableStream& error) {
        TS_ASSERT_EQUALS(error.fileName(), testFile_);
        TS_ASSERT_EQUALS(error.lineNum(), testLineNum_);
        TS_ASSERT_EQUALS(error.procedureName(), testProcedure_);
        TS_ASSERT_EQUALS(error.errorMessage(), testInputFile_);
    }
}

/**
 * Tests that default parameters are used correctly.
 */
inline void
UnreachableStreamTest::testThrowWithDefaultParam() {
    try {
        throw UnreachableStream(testFile_, testLineNum_);
    } catch (const UnreachableStream& error) {
        TS_ASSERT_EQUALS(error.fileName(), testFile_);
        TS_ASSERT_EQUALS(error.lineNum(), testLineNum_);
        TS_ASSERT_EQUALS(error.procedureName(), unknownProcedure_);
        TS_ASSERT_EQUALS(error.errorMessage(), "");
    }
}

#endif
