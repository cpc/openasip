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

