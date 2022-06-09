/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file example.cc
 *
 * Operations for testing purposes.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 */

#include "OSAL.hh"

OPERATION(TESTADD)

TRIGGER
  IO(3) = INT(1) + INT(2);
  RETURN_READY;
END_TRIGGER;

END_OPERATION(TESTADD)

OPERATION(TESTSUB)

TRIGGER
  IO(3) = INT(1) - INT(2);
  RETURN_READY;
END_TRIGGER;

END_OPERATION(TESTSUB)

OPERATION(TESTMUL)

TRIGGER
  IO(3) = INT(1) * INT(2);
  RETURN_READY;
END_TRIGGER;

END_OPERATION(TESTMUL)

OPERATION(TESTSTORE)

TRIGGER
    MEMORY.write(INT(1), 4, UINT(2));
END_TRIGGER;

END_OPERATION(TESTSTORE)

OPERATION(TESTLOAD)

TRIGGER
  ULongWord data;
  MEMORY.read(INT(1), 4, data);
  IO(2) = data;
END_TRIGGER;

END_OPERATION(TESTLOAD)
