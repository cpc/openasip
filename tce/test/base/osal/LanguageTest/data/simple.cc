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
/**
 * @file simple.cc
 *
 * Test: simple operation definitions.
 *
 * Operations contain no state and define only the main simulation function.
 *
 * @author Pekka J‰‰skel‰inen (pekka.jaaskelainen-no.spam-tut.fi)
 */


#include "OSAL.hh"

OPERATION(ADD4)

TRIGGER

    IO(5) = INT(1) + INT(2) + INT(3) + INT(4);
    RETURN_READY;

END_TRIGGER;

END_OPERATION(ADD4)

OPERATION(FLOATMUL3)

TRIGGER
    IO(4) = FLT(1)*FLT(2)*FLT(3);
    RETURN_READY;
END_TRIGGER;

END_OPERATION(FLOATMUL3)

OPERATION(DOUBLESUB3)

TRIGGER

    // test indirect indexing of operands
    int index = 1;
    DoubleWord one, two, three;
    one = DBL(1);
    two = DBL(index + 1);
    three = DBL(3);
    IO(4) = one - two - three;
    RETURN_READY;

END_TRIGGER;

END_OPERATION(DOUBLESUB3)

// test operation that lacks return statement

OPERATION(MISSING_RETURN_STATEMENT)

TRIGGER
    IO(1) = 3 + 3;
    RETURN_READY;
END_TRIGGER;

END_OPERATION(MISSING_RETURN_STATEMENT)
