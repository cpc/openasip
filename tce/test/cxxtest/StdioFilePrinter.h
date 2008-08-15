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
#ifndef __cxxtest__StdioFilePrinter_h__
#define __cxxtest__StdioFilePrinter_h__

//
// The StdioFilePrinter is a simple TestListener that
// just prints "OK" if everything goes well, otherwise
// reports the error in the format of compiler messages.
// This class uses <stdio.h>, i.e. FILE * and fprintf().
//

#include <cxxtest/ErrorFormatter.h>
#include <stdio.h>

namespace CxxTest 
{
    class StdioFilePrinter : public ErrorFormatter
    {
    public:
        StdioFilePrinter( FILE *o, const char *preLine = ":", const char *postLine = "" ) :
            ErrorFormatter( new Adapter(o), preLine, postLine ) {}
        virtual ~StdioFilePrinter() { delete outputStream(); }

    private:
        class Adapter : public OutputStream
        {
            Adapter( const Adapter & );
            Adapter &operator=( const Adapter & );
            
            FILE *_o;
            
        public:
            Adapter( FILE *o ) : _o(o) {}
            void flush() { fflush( _o ); }
            OutputStream &operator<<( unsigned i ) { fprintf( _o, "%u", i ); return *this; }
            OutputStream &operator<<( const char *s ) { fputs( s, _o ); return *this; }
            OutputStream &operator<<( Manipulator m ) { return OutputStream::operator<<( m ); }
        };
    };
}

#endif // __cxxtest__StdioFilePrinter_h__
