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
#ifndef __cxxtest__DummyDescriptions_h__
#define __cxxtest__DummyDescriptions_h__

//
// DummyTestDescription, DummySuiteDescription and DummyWorldDescription
//

#include <cxxtest/Descriptions.h>

namespace CxxTest 
{
    class DummyTestDescription : public TestDescription
    {
    public:
        DummyTestDescription();
        
        const char *file() const;
        unsigned line() const;
        const char *testName() const;
        const char *suiteName() const;
        bool setUp();
        void run();
        bool tearDown();

        TestDescription *next();
        const TestDescription *next() const;
    };

    class DummySuiteDescription : public SuiteDescription
    {      
    public:
        DummySuiteDescription();
        
        const char *file() const;
        unsigned line() const;
        const char *suiteName() const;
        TestSuite *suite() const;
        unsigned numTests() const;
        const TestDescription &testDescription( unsigned ) const;
        SuiteDescription *next();
        TestDescription *firstTest();
        const SuiteDescription *next() const;
        const TestDescription *firstTest() const;
        void activateAllTests();
        bool leaveOnly( const char * /*testName*/ );
        
        bool setUp();
        bool tearDown();

    private:
        DummyTestDescription _test;
    };

    class DummyWorldDescription : public WorldDescription
    {
    public:
        DummyWorldDescription();
        
        unsigned numSuites( void ) const;
        unsigned numTotalTests( void ) const;
        const SuiteDescription &suiteDescription( unsigned ) const;
        SuiteDescription *firstSuite();
        const SuiteDescription *firstSuite() const;
        void activateAllTests();
        bool leaveOnly( const char * /*suiteName*/, const char * /*testName*/ = 0 );
            
        bool setUp();
        bool tearDown();

    private:
        DummySuiteDescription _suite;
    };
}

#endif // __cxxtest__DummyDescriptions_h__

