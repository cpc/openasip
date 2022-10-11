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
 * @file ExceptionTest.hh
 *
 * A test suite for Exception.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme-no.spam-tut.fi)
 * @note reviewed 7 November 2003 by am, ml, jn, pj
 */

#ifndef TTA_EXCEPTION_TEST_HH
#define TTA_EXCEPTION_TEST_HH

#include <TestSuite.h>
#include <string>
#include "Exception.hh"

using std::string;

/**
 * Implements the tests needed to verify correct operation of Exception.
 */
class ExceptionTest : public CxxTest::TestSuite {
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
};

const string ExceptionTest::testFile_ = "ExceptionTest.h";
const int ExceptionTest::testLineNum_ = 13;
const string ExceptionTest::testProcedure_ = "ExceptionTest::testThrow()";
const string ExceptionTest::unknownProcedure_ = "(unknown)";

/**
 * Tests that correct information is passed when throwing exception.
 */
void
ExceptionTest::testThrow() {
    try {
        throw Exception(testFile_, testLineNum_, testProcedure_);
    } catch (const Exception& error) {
        TS_ASSERT_EQUALS(error.fileName(), testFile_);
        TS_ASSERT_EQUALS(error.lineNum(), testLineNum_);
        TS_ASSERT_EQUALS(error.procedureName(), testProcedure_);
    }
}

/**
 * Tests that default parameters are used correctly.
 */
void
ExceptionTest::testThrowWithDefaultParam() {
    try {
        throw Exception(testFile_, testLineNum_);
    } catch (const Exception& error) {
        TS_ASSERT_EQUALS(error.fileName(), testFile_);
        TS_ASSERT_EQUALS(error.lineNum(), testLineNum_);
        TS_ASSERT_EQUALS(error.procedureName(), unknownProcedure_);
    }
}

#endif
