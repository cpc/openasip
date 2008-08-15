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
#ifndef __CXXTEST__GUI_H
#define __CXXTEST__GUI_H

//
// GuiListener is a simple base class for the differes GUIs
// GuiTuiRunner<GuiT, TuiT> combines a GUI with a text-mode error formatter
//

#include <cxxtest/TeeListener.h>

namespace CxxTest
{
    class GuiListener : public TestListener
    {
    public:
        GuiListener() : _state( GREEN_BAR ) {}
        virtual ~GuiListener() {}

        virtual void runGui( int &argc, char **argv, TestListener &listener )
        {
            enterGui( argc, argv );
            TestRunner::runAllTests( listener );
            leaveGui();            
        }

        virtual void enterGui( int & /*argc*/, char ** /*argv*/ ) {}
        virtual void leaveGui() {}
        
        //
        // The easy way is to implement these functions:
        //      
        virtual void guiEnterWorld( unsigned /*numTotalTests*/ ) {}
        virtual void guiEnterSuite( const char * /*suiteName*/ ) {}
        virtual void guiEnterTest( const char * /*suiteName*/, const char * /*testName*/ ) {}
        virtual void yellowBar() {}
        virtual void redBar() {}

        //
        // The hard way is this:
        //
        void enterWorld( const WorldDescription &d ) { guiEnterWorld( d.numTotalTests() ); }
        void enterSuite( const SuiteDescription &d ) { guiEnterSuite( d.suiteName() ); }
        void enterTest( const TestDescription &d ) { guiEnterTest( d.suiteName(), d.testName() ); }
        void leaveTest( const TestDescription & ) {}
        void leaveSuite( const SuiteDescription & ) {}
        void leaveWorld( const WorldDescription & ) {}
        
        void warning( const char * /*file*/, unsigned /*line*/, const char * /*expression*/ )
        {
            yellowBarSafe();
        }
        
        void failedTest( const char * /*file*/, unsigned /*line*/, const char * /*expression*/ )
        {
            redBarSafe();
        }
        
        void failedAssert( const char * /*file*/, unsigned /*line*/, const char * /*expression*/ )
        {
            redBarSafe();
        }
        
        void failedAssertEquals( const char * /*file*/, unsigned /*line*/,
                                 const char * /*xStr*/, const char * /*yStr*/,
                                 const char * /*x*/, const char * /*y*/ )
        {
            redBarSafe();
        }

        void failedAssertSameData( const char * /*file*/, unsigned /*line*/,
                                   const char * /*xStr*/, const char * /*yStr*/,
                                   const char * /*sizeStr*/, const void * /*x*/,
                                   const void * /*y*/, unsigned /*size*/ )
        {
            redBarSafe();
        }
        
        void failedAssertDelta( const char * /*file*/, unsigned /*line*/,
                                const char * /*xStr*/, const char * /*yStr*/, const char * /*dStr*/,
                                const char * /*x*/, const char * /*y*/, const char * /*d*/ )
        {
            redBarSafe();
        }
        
        void failedAssertDiffers( const char * /*file*/, unsigned /*line*/,
                                  const char * /*xStr*/, const char * /*yStr*/,
                                  const char * /*value*/ )
        {
            redBarSafe();
        }
        
        void failedAssertLessThan( const char * /*file*/, unsigned /*line*/,
                                   const char * /*xStr*/, const char * /*yStr*/,
                                   const char * /*x*/, const char * /*y*/ )
        {
            redBarSafe();
        }
        
        void failedAssertLessThanEquals( const char * /*file*/, unsigned /*line*/,
                                         const char * /*xStr*/, const char * /*yStr*/,
                                         const char * /*x*/, const char * /*y*/ )
        {
            redBarSafe();
        }
        
        void failedAssertPredicate( const char * /*file*/, unsigned /*line*/,
                                    const char * /*predicate*/, const char * /*xStr*/, const char * /*x*/ )
        {
            redBarSafe();
        }
        
        void failedAssertRelation( const char * /*file*/, unsigned /*line*/,
                                   const char * /*relation*/, const char * /*xStr*/, const char * /*yStr*/,
                                   const char * /*x*/, const char * /*y*/ )
        {
            redBarSafe();
        }
        
        void failedAssertThrows( const char * /*file*/, unsigned /*line*/,
                                 const char * /*expression*/, const char * /*type*/,
                                 bool /*otherThrown*/ )
        {
            redBarSafe();
        }
        
        void failedAssertThrowsNot( const char * /*file*/, unsigned /*line*/,
                                    const char * /*expression*/ )
        {
            redBarSafe();
        }

    protected:
        void yellowBarSafe()
        {
            if ( _state < YELLOW_BAR ) {
                yellowBar();
                _state = YELLOW_BAR;
            }
        }

        void redBarSafe()
        {
            if ( _state < RED_BAR ) {
                redBar();
                _state = RED_BAR;
            }
        }
        
    private:
        enum { GREEN_BAR, YELLOW_BAR, RED_BAR } _state;
    };

    template<class GuiT, class TuiT>
    class GuiTuiRunner : public TeeListener
    {
        int &_argc;
        char **_argv;
        GuiT _gui;
        TuiT _tui;
        
    public:
        GuiTuiRunner( int &argc, char **argv ) :
            _argc( argc ),
            _argv( argv )
        {  
            setFirst( _gui );
            setSecond( _tui );
        }

        int run()
        {
            _gui.runGui( _argc, _argv, *this );
            return tracker().failedTests();
        }
    };
};

#endif //__CXXTEST__GUI_H
