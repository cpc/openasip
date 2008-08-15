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
#ifndef __cxxtest_TestRunner_h__
#define __cxxtest_TestRunner_h__

//
// TestRunner is the class that runs all the tests.
// To use it, create an object that implements the TestListener
// interface and call TestRunner::runAllTests( myListener );
// 

#include <cxxtest/TestListener.h>
#include <cxxtest/RealDescriptions.h>
#include <cxxtest/TestSuite.h>
#include <cxxtest/TestTracker.h>

namespace CxxTest 
{
    class TestRunner
    {
    public:
        static void runAllTests( TestListener &listener )
        {
            tracker().setListener( &listener );
            _TS_TRY { TestRunner().runWorld(); }
            _TS_LAST_CATCH( { tracker().failedTest( __FILE__, __LINE__, "Exception thrown from world" ); } );
            tracker().setListener( 0 );
        }

        static void runAllTests( TestListener *listener )
        {
            if ( listener ) {
                listener->warning( __FILE__, __LINE__, "Deprecated; Use runAllTests( TestListener & )" );
                runAllTests( *listener );
            }
        }        
    
    private:
        void runWorld()
        {
            RealWorldDescription wd;
            WorldGuard sg;
            
            tracker().enterWorld( wd );
            if ( wd.setUp() ) {
                for ( SuiteDescription *sd = wd.firstSuite(); sd; sd = sd->next() )
                    if ( sd->active() )
                        runSuite( *sd );
            
                wd.tearDown();
            }
            tracker().leaveWorld( wd );
        }
    
        void runSuite( SuiteDescription &sd )
        {
            StateGuard sg;
            
            tracker().enterSuite( sd );
            if ( sd.setUp() ) {
                for ( TestDescription *td = sd.firstTest(); td; td = td->next() )
                    if ( td->active() )
                        runTest( *td );

                sd.tearDown();
            }
            tracker().leaveSuite( sd );
        }

        void runTest( TestDescription &td )
        {
            StateGuard sg;
            
            tracker().enterTest( td );
            if ( td.setUp() ) {
                td.run();
                td.tearDown();
            }
            tracker().leaveTest( td );
        }
        
        class StateGuard
        {
#ifdef _CXXTEST_HAVE_EH
            bool _abortTestOnFail;
#endif // _CXXTEST_HAVE_EH
            unsigned _maxDumpSize;
            
        public:
            StateGuard()
            {
#ifdef _CXXTEST_HAVE_EH
                _abortTestOnFail = abortTestOnFail();
#endif // _CXXTEST_HAVE_EH
                _maxDumpSize = maxDumpSize();
            }
            
            ~StateGuard()
            {
#ifdef _CXXTEST_HAVE_EH
                setAbortTestOnFail( _abortTestOnFail );
#endif // _CXXTEST_HAVE_EH
                setMaxDumpSize( _maxDumpSize );
            }
        };

        class WorldGuard : public StateGuard
        {
        public:
            WorldGuard() : StateGuard()
            {
#ifdef _CXXTEST_HAVE_EH
                setAbortTestOnFail( CXXTEST_DEFAULT_ABORT );
#endif // _CXXTEST_HAVE_EH
                setMaxDumpSize( CXXTEST_MAX_DUMP_SIZE );
            }
        };
    };

    //
    // For --no-static-init
    //
    void initialize();
}


#endif // __cxxtest_TestRunner_h__
