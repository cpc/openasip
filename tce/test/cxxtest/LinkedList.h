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
#ifndef __cxxtest__LinkedList_h__
#define __cxxtest__LinkedList_h__

#include <cxxtest/Flags.h>

namespace CxxTest 
{
    struct List;
    class Link;

    struct List
    {
        Link *_head;
        Link *_tail;

        void initialize();

        Link *head();
        const Link *head() const;
        Link *tail();
        const Link *tail() const;

        bool empty() const;
        unsigned size() const;
        Link *nth( unsigned n );

        void activateAll();
        void leaveOnly( const Link &link );
    };

    class Link
    {       
    public:
        Link();
        virtual ~Link();

        bool active() const;
        void setActive( bool value = true );

        Link *justNext();
        Link *justPrev();
        
        Link *next();
        Link *prev();
        const Link *next() const;
        const Link *prev() const;

        virtual bool setUp() = 0;
        virtual bool tearDown() = 0;

        void attach( List &l );
        void detach( List &l );

    private:
        Link *_next;
        Link *_prev;
        bool _active;

        Link( const Link & );
        Link &operator=( const Link & );
    };
}

#endif // __cxxtest__LinkedList_h__

