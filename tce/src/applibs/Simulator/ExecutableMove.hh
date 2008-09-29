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
/**
 * @file ExecutableMove.hh
 *
 * Declaration of ExecutableMove class.
 *
 * @author Jussi Nyk‰nen 2005 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
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
        const ReadableState& src, 
        BusState& bus, 
        WritableState& dst);

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
    /// Copying not allowed.
    ExecutableMove(const ExecutableMove&);
    /// Assignment not allowed.
    ExecutableMove& operator=(const ExecutableMove&);
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
