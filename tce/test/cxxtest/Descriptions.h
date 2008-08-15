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

