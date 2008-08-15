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
#ifndef __cxxtest__TeeListener_h__
#define __cxxtest__TeeListener_h__

//
// A TeeListener notifies two "reular" TestListeners
//

#include <cxxtest/TestListener.h>
#include <cxxtest/TestListener.h>

namespace CxxTest
{
    class TeeListener : public TestListener
    {
    public:
        TeeListener()
        {
            setFirst( _dummy );
            setSecond( _dummy );
        }

        virtual ~TeeListener()
        {
        }

        void setFirst( TestListener &first )
        {
            _first = &first;
        }

        void setSecond( TestListener &second )
        {
            _second = &second;
        }

        void enterWorld( const WorldDescription &d )
        {
            _first->enterWorld( d );
            _second->enterWorld( d );
        }

        void enterSuite( const SuiteDescription &d )
        {
            _first->enterSuite( d );
            _second->enterSuite( d );
        }
        
        void enterTest( const TestDescription &d )
        {
            _first->enterTest( d );
            _second->enterTest( d );
        }
        
        void trace( const char *file, unsigned line, const char *expression )
        {
            _first->trace( file, line, expression );
            _second->trace( file, line, expression );
        }
        
        void warning( const char *file, unsigned line, const char *expression )
        {
            _first->warning( file, line, expression );
            _second->warning( file, line, expression );
        }
        
        void failedTest( const char *file, unsigned line, const char *expression )
        {
            _first->failedTest( file, line, expression );
            _second->failedTest( file, line, expression );
        }
        
        void failedAssert( const char *file, unsigned line, const char *expression )
        {
            _first->failedAssert( file, line, expression );
            _second->failedAssert( file, line, expression );
        }
        
        void failedAssertEquals( const char *file, unsigned line,
                                 const char *xStr, const char *yStr,
                                 const char *x, const char *y )
        {
            _first->failedAssertEquals( file, line, xStr, yStr, x, y );
            _second->failedAssertEquals( file, line, xStr, yStr, x, y );
        }

        void failedAssertSameData( const char *file, unsigned line,
                                   const char *xStr, const char *yStr,
                                   const char *sizeStr, const void *x,
                                   const void *y, unsigned size )
        {
            _first->failedAssertSameData( file, line, xStr, yStr, sizeStr, x, y, size );
            _second->failedAssertSameData( file, line, xStr, yStr, sizeStr, x, y, size );
        }
        
        void failedAssertDelta( const char *file, unsigned line,
                                const char *xStr, const char *yStr, const char *dStr,
                                const char *x, const char *y, const char *d )
        {
            _first->failedAssertDelta( file, line, xStr, yStr, dStr, x, y, d );
            _second->failedAssertDelta( file, line, xStr, yStr, dStr, x, y, d );
        }
        
        void failedAssertDiffers( const char *file, unsigned line,
                                  const char *xStr, const char *yStr,
                                  const char *value )
        {
            _first->failedAssertDiffers( file, line, xStr, yStr, value );
            _second->failedAssertDiffers( file, line, xStr, yStr, value );
        }
        
        void failedAssertLessThan( const char *file, unsigned line,
                                   const char *xStr, const char *yStr,
                                   const char *x, const char *y )
        {
            _first->failedAssertLessThan( file, line, xStr, yStr, x, y );
            _second->failedAssertLessThan( file, line, xStr, yStr, x, y );
        }
        
        void failedAssertLessThanEquals( const char *file, unsigned line,
                                         const char *xStr, const char *yStr,
                                         const char *x, const char *y )
        {
            _first->failedAssertLessThanEquals( file, line, xStr, yStr, x, y );
            _second->failedAssertLessThanEquals( file, line, xStr, yStr, x, y );
        }
        
        void failedAssertPredicate( const char *file, unsigned line,
                                    const char *predicate, const char *xStr, const char *x )
        {
            _first->failedAssertPredicate( file, line, predicate, xStr, x );
            _second->failedAssertPredicate( file, line, predicate, xStr, x );
        }
        
        void failedAssertRelation( const char *file, unsigned line,
                                   const char *relation, const char *xStr, const char *yStr,
                                   const char *x, const char *y )
        {
            _first->failedAssertRelation( file, line, relation, xStr, yStr, x, y );
            _second->failedAssertRelation( file, line, relation, xStr, yStr, x, y );
        }
        
        void failedAssertThrows( const char *file, unsigned line,
                                 const char *expression, const char *type,
                                 bool otherThrown )
        {
            _first->failedAssertThrows( file, line, expression, type, otherThrown );
            _second->failedAssertThrows( file, line, expression, type, otherThrown );
        }
        
        void failedAssertThrowsNot( const char *file, unsigned line,
                                    const char *expression )
        {
            _first->failedAssertThrowsNot( file, line, expression );
            _second->failedAssertThrowsNot( file, line, expression );
        }
        
        void leaveTest( const TestDescription &d )
        {
            _first->leaveTest(d);
            _second->leaveTest(d);
        }
        
        void leaveSuite( const SuiteDescription &d )
        {
            _first->leaveSuite(d);
            _second->leaveSuite(d);
        }
        
        void leaveWorld( const WorldDescription &d )
        {
            _first->leaveWorld(d);
            _second->leaveWorld(d);
        }

    private:
        TestListener *_first, *_second;
        TestListener _dummy;
    };
};


#endif // __cxxtest__TeeListener_h__
