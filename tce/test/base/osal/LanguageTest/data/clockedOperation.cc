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
 * @file clockedOperation.cc
 *
 * Test definition of operation definition which uses clock signal.
 *
 * @author Pekka Jääskeläinen (pekka.jaaskelainen-no.spam-tut.fi)
 */

#include "OSAL.hh"

/// This operation is a clock counter. The trigger returns the current
/// clock count stored in state. ADVANCE_CLOCK increases the counter.
DEFINE_STATE(CLOCK_COUNTER)

    UIntWord cycleCount;

    INIT_STATE(CLOCK_COUNTER)
        cycleCount = 0;
    END_INIT_STATE;

    ADVANCE_CLOCK
        cycleCount++;
    END_ADVANCE_CLOCK;

END_DEFINE_STATE

OPERATION_WITH_STATE(CLOCK, CLOCK_COUNTER)

    TRIGGER
         IO(1) = STATE.cycleCount;
         RETURN_READY;
    END_TRIGGER;

END_OPERATION_WITH_STATE(CLOCK)
