/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
#ifndef __cxxtest__Descriptions_h__
#define __cxxtest__Descriptions_h__

//
// TestDescription, SuiteDescription and WorldDescription
// hold information about tests so they can be run and reported.
//

#include <cxxtest/LinkedList.h>

namespace CxxTest 
{
    class TestSuite;

    class TestDescription : public Link
    {
    public:
        virtual ~TestDescription();
        
        virtual const char *file() const = 0;
        virtual unsigned line() const = 0;
        virtual const char *testName() const = 0;
        virtual const char *suiteName() const = 0;
        
        virtual void run() = 0;

        virtual const TestDescription *next() const = 0;
        virtual TestDescription *next() = 0;        
    };

    class SuiteDescription : public Link
    {
    public:
        virtual ~SuiteDescription();
        
        virtual const char *file() const = 0;
        virtual unsigned line() const = 0;
        virtual const char *suiteName() const = 0;
        virtual TestSuite *suite() const = 0;
        
        virtual unsigned numTests() const = 0;
        virtual const TestDescription &testDescription( unsigned /*i*/ ) const = 0;

        virtual TestDescription *firstTest() = 0;
        virtual const TestDescription *firstTest() const = 0;
        virtual SuiteDescription *next() = 0;
        virtual const SuiteDescription *next() const = 0;

        virtual void activateAllTests() = 0;
        virtual bool leaveOnly( const char * /*testName*/ ) = 0;
    };

    class WorldDescription : public Link
    {
    public:
        virtual ~WorldDescription();
        
        virtual unsigned numSuites( void ) const = 0;
        virtual unsigned numTotalTests( void ) const = 0;
        virtual const SuiteDescription &suiteDescription( unsigned /*i*/ ) const = 0;

        enum { MAX_STRLEN_TOTAL_TESTS = 32 };
        char *strTotalTests( char * /*buffer*/ ) const;

        virtual SuiteDescription *firstSuite() = 0;
        virtual const SuiteDescription *firstSuite() const = 0;

        virtual void activateAllTests() = 0;
        virtual bool leaveOnly( const char * /*suiteName*/, const char * /*testName*/ = 0 ) = 0;
    };
}

#endif // __cxxtest__Descriptions_h__

