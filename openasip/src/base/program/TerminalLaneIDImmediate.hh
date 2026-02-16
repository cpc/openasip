/*
    Copyright (c) 2002-2013 Tampere University.

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
 * @file TerminalLaneIDImmediate.hh
 *
 * Declaration of TerminalLaneIDImmediate class.
 *
 * @author Pekka Jääskeläinen 2013
 * @note rating: red
 */

#ifndef TTA_TERMINAL_LANEID_IMMEDIATE_HH
#define TTA_TERMINAL_LANEID_IMMEDIATE_HH

#include "TerminalImmediate.hh"
#include "AnnotatedInstructionElement.hh"

namespace TTAProgram {

/**
 * Represents a special inline immediate that is fixed to the lane id of
 * the scheduled move in a clustered/SMMT machine.
 */
class TerminalLaneIDImmediate : public TerminalImmediate {
public:
    TerminalLaneIDImmediate(SimValue value);
    virtual ~TerminalLaneIDImmediate();

    virtual SimValue value() const;

    virtual Terminal* copy() const;
    virtual bool equals(const Terminal& other) const;

    virtual TCEString toString() const;

    TerminalLaneIDImmediate&
    operator=(const TerminalLaneIDImmediate&) = delete;

protected:
    /// Value of the inline immediate.
    SimValue value_;
};
}

#endif
