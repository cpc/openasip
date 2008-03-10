/**
 * @file UnreachableStreamTest.hh
 *
 * A test suite for UnreachableStream.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
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
