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
#ifndef __cxxtest__TestListener_h__
#define __cxxtest__TestListener_h__

//
// TestListener is the base class for all "listeners",
// i.e. classes that receive notifications of the
// testing process.
//
// The names of the parameters are in comments to avoid
// "unused parameter" warnings.
//

#include <cxxtest/Descriptions.h>

namespace CxxTest
{
    class TestListener
    {
    public:
        TestListener() {}
        virtual ~TestListener() {}
        
        virtual void enterWorld( const WorldDescription & /*desc*/ ) {}
        virtual void enterSuite( const SuiteDescription & /*desc*/ ) {}
        virtual void enterTest( const TestDescription & /*desc*/ ) {}
        virtual void trace( const char * /*file*/, unsigned /*line*/,
                            const char * /*expression*/ ) {}
        virtual void warning( const char * /*file*/, unsigned /*line*/,
                              const char * /*expression*/ ) {}
        virtual void failedTest( const char * /*file*/, unsigned /*line*/,
                                 const char * /*expression*/ ) {}
        virtual void failedAssert( const char * /*file*/, unsigned /*line*/,
                                   const char * /*expression*/ ) {}
        virtual void failedAssertEquals( const char * /*file*/, unsigned /*line*/,
                                         const char * /*xStr*/, const char * /*yStr*/,
                                         const char * /*x*/, const char * /*y*/ ) {}
        virtual void failedAssertSameData( const char * /*file*/, unsigned /*line*/,
                                           const char * /*xStr*/, const char * /*yStr*/,
                                           const char * /*sizeStr*/, const void * /*x*/,
                                           const void * /*y*/, unsigned /*size*/ ) {}
        virtual void failedAssertDelta( const char * /*file*/, unsigned /*line*/,
                                        const char * /*xStr*/, const char * /*yStr*/,
                                        const char * /*dStr*/, const char * /*x*/,
                                        const char * /*y*/, const char * /*d*/ ) {}
        virtual void failedAssertDiffers( const char * /*file*/, unsigned /*line*/,
                                          const char * /*xStr*/, const char * /*yStr*/,
                                          const char * /*value*/ ) {}
        virtual void failedAssertLessThan( const char * /*file*/, unsigned /*line*/,
                                           const char * /*xStr*/, const char * /*yStr*/,
                                           const char * /*x*/, const char * /*y*/ ) {}
        virtual void failedAssertLessThanEquals( const char * /*file*/, unsigned /*line*/,
                                                 const char * /*xStr*/, const char * /*yStr*/,
                                                 const char * /*x*/, const char * /*y*/ ) {}
        virtual void failedAssertPredicate( const char * /*file*/, unsigned /*line*/,
                                            const char * /*predicate*/, const char * /*xStr*/, const char * /*x*/ ) {}
        virtual void failedAssertRelation( const char * /*file*/, unsigned /*line*/,
                                           const char * /*relation*/, const char * /*xStr*/, const char * /*yStr*/,
                                           const char * /*x*/, const char * /*y*/ ) {}
        virtual void failedAssertThrows( const char * /*file*/, unsigned /*line*/,
                                         const char * /*expression*/, const char * /*type*/,
                                         bool /*otherThrown*/ ) {}
        virtual void failedAssertThrowsNot( const char * /*file*/, unsigned /*line*/,
                                            const char * /*expression*/ ) {}
        virtual void leaveTest( const TestDescription & /*desc*/ ) {}
        virtual void leaveSuite( const SuiteDescription & /*desc*/ ) {}
        virtual void leaveWorld( const WorldDescription & /*desc*/ ) {}
    };
}

#endif // __cxxtest__TestListener_h__
