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
 * @file ExecutableMove.hh
 *
 * Declaration of ExecutableMove class.
 *
 * @author Jussi Nykänen 2005 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka Jääskeläinen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_EXECUTABLE_MOVE_HH
#define TTA_EXECUTABLE_MOVE_HH

#include "InlineImmediateValue.hh"
#include "SimulatorConstants.hh"

class ReadableState;
class BusState;
class WritableState;
class SimValue;


/**
 * Represents an interpreted move.
 *
 * Interpreted means that source and destination of the move are already
 * resolved.
 */
class ExecutableMove {
public:
    ExecutableMove(
        const ReadableState& src, BusState& bus, WritableState& dst);

    ExecutableMove(
        const ReadableState& src,
        BusState& bus,
        WritableState& dst,
        const ReadableState& guardReg,
        bool negated);

    ExecutableMove(
        InlineImmediateValue* immediateSource,
        BusState& bus,
        WritableState& dst,
        const ReadableState& guardReg,
        bool negated);

    ExecutableMove(
        InlineImmediateValue* immediateSource,
        BusState& bus,
        WritableState& dst);

    virtual ~ExecutableMove();

    virtual void executeRead();
    virtual void executeWrite();
    virtual void evaluateGuard();
    virtual bool squashed() const;

    ClockCycleCount executionCount() const;
    void resetExecutionCount();

    ExecutableMove(const ExecutableMove&) = delete;
    ExecutableMove& operator=(const ExecutableMove&) = delete;

protected:
    ExecutableMove();

    /// Source of the move.
    const ReadableState* src_;
    /// Bus of the move.
    BusState* bus_;
    /// Destination of the move.
    WritableState* dst_;
    /// Guard of the move.
    const ReadableState* guardReg_;
    /// True if this is a guarded move.
    const bool guarded_;
    /// True if guard is inverted.
    const bool negated_;
    /// The count of times this move has been fully executed (without squash).
    ClockCycleCount executionCount_;
    /// True in case this move was squashed last time it was executed.
    bool squashed_;

private:
    /// If the move source is an inline immediate, the instance
    /// is stored here so it can be deleted in the destructor.
    InlineImmediateValue* inlineImmediate_;
};

/**
 * Extremely simple wrapper for ExecutableMove, takes no params but exec count.
 */
class DummyExecutableMove : public ExecutableMove {
public:
    DummyExecutableMove(ClockCycleCount executionCount);
private:
};

#endif
