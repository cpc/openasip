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
 * OSAL behavior definition file.
 */

#include "OSAL.hh"
OPERATION(AVALON_READ_IRQ)
TRIGGER
int dummy = INT(1);
dummy=0;
IO(2) = dummy; 
END_TRIGGER
END_OPERATION(AVALON_READ_IRQ)

OPERATION(AVALON_LDW)
TRIGGER
int dummy = INT(1);
dummy=0;
IO(2) = dummy; 
END_TRIGGER
END_OPERATION(AVALON_LDW)

OPERATION(AVALON_LDH)
TRIGGER
int dummy = INT(1);
dummy=0;
IO(2) = dummy; 
END_TRIGGER
END_OPERATION(AVALON_LDH)

OPERATION(AVALON_LDHU)
TRIGGER
int dummy = INT(1);
dummy=0;
IO(2) = dummy; 
END_TRIGGER
END_OPERATION(AVALON_LDHU)

OPERATION(AVALON_LDQ)
TRIGGER
int dummy = INT(1);
dummy=0;
IO(2) = dummy; 
END_TRIGGER
END_OPERATION(AVALON_LDQ)

OPERATION(AVALON_LDQU)
TRIGGER
int dummy = INT(1);
dummy=0;
IO(2) = dummy; 
END_TRIGGER
END_OPERATION(AVALON_LDQU)

OPERATION(AVALON_STW)
TRIGGER
int dummy = INT(1);
int dummy2 = INT(2);
dummy=dummy2;
END_TRIGGER
END_OPERATION(AVALON_STW)

OPERATION(AVALON_STH)
TRIGGER
int dummy = INT(1);
int dummy2 = INT(2);
dummy=dummy2;
END_TRIGGER
END_OPERATION(AVALON_STH)

OPERATION(AVALON_STQ)
TRIGGER
int dummy = INT(1);
int dummy2 = INT(2);
dummy=dummy2;
END_TRIGGER
END_OPERATION(AVALON_STQ)

