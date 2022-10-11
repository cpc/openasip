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
 * @file BuslessExecutableMove.hh
 *
 * Declaration of BuslessExecutableMove class.
 *
 * @author Pekka Jääskeläinen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_BUSLESS_EXECUTABLE_MOVE_HH
#define TTA_BUSLESS_EXECUTABLE_MOVE_HH

#include "ExecutableMove.hh"
#include "InlineImmediateValue.hh"

class ReadableState;
class BusState;
class WritableState;
class SimValue;


/**
 * Represents an interpreted move which does not utilize a transport bus.
 *
 * Moves that utilize this class include control unit internal moves such
 * as control flow operations (jump, call) with immediate or RA source. 
 * This class can be used also for simulating data transports of sequential 
 * code, as writing to (the universal) bus is not necessary in that case.
 */
class BuslessExecutableMove : public ExecutableMove {
public:
    BuslessExecutableMove(
        const ReadableState& src, 
        WritableState& dst);

    BuslessExecutableMove(
        const ReadableState& src,
        WritableState& dst,
        const ReadableState& guardReg,
        bool negated);

    BuslessExecutableMove(
        InlineImmediateValue* immediateSource,
        WritableState& dst,
        const ReadableState& guardReg,
        bool negated);

    BuslessExecutableMove(
        InlineImmediateValue* immediateSource,
        WritableState& dst);
    
    virtual ~BuslessExecutableMove();

    virtual void executeRead();
    virtual void executeWrite();

private:
    /// Copying not allowed.
    BuslessExecutableMove(const BuslessExecutableMove&);
    /// Assignment not allowed.
    BuslessExecutableMove& operator=(const BuslessExecutableMove&);
};

#endif
