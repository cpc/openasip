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
 * @file ExecutableMove.cc
 *
 * Definition of ExecutableMove class.
 *
 * @author Jussi Nyk�nen 2005 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J��skel�inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "ExecutableMove.hh"
#include "ReadableState.hh"
#include "WritableState.hh"
#include "BusState.hh"
#include "SimValue.hh"

// This should be defined if guards block bus writes, that is, in case
// guard definition evaluates to false, even the bus is not written.
// At least this is the case in MOVE, therefore this needs to be defined
// to be able to compare bus traces between MOVE and TCE easily.
const bool GUARD_BLOCKS_BUS_WRITE = true;

/**
 * Constructor.
 *
 * @param src Source of the move.
 * @param bus Bus of the move.
 * @param dst Destination of the move.
 */
ExecutableMove::ExecutableMove(
    const ReadableState& src,
    BusState& bus,
    WritableState& dst) : 
    src_(&src), bus_(&bus), dst_(&dst), 
    guardReg_(NULL), guarded_(false), negated_(false), 
    executionCount_(0), squashed_(false),
    inlineImmediate_(NULL) {
}

/**
 * Constructor.
 *
 * @param src Source of the move.
 * @param bus Bus of the move.
 * @param dst Destination of the move.
 * @param guardReg Guard register.
 * @param negated True if guard is reversed.
 */
ExecutableMove::ExecutableMove(
    const ReadableState& src,
    BusState& bus,
    WritableState& dst,
    const ReadableState& guardReg,
    bool negated) : 
    src_(&src), bus_(&bus), dst_(&dst), guardReg_(&guardReg),
    guarded_(true), negated_(negated), 
    executionCount_(0), squashed_(false),
    inlineImmediate_(NULL) {
}

/**
 * Constructor.
 *
 * Used to construct a move that has an inline immediate as the source.
 * The inline immediate becomes property of the created ExecutableMove
 * and is deleted by it.
 *
 * @param immediateSource The inline immediate source of the move.
 * @param bus Bus of the move.
 * @param dst Destination of the move.
 * @param guardReg Guard register.
 * @param negated True if guard is reversed.
 */
ExecutableMove::ExecutableMove(
    InlineImmediateValue* immediateSource,
    BusState& bus,
    WritableState& dst,
    const ReadableState& guardReg,
    bool negated) : 
    src_(immediateSource), bus_(&bus), dst_(&dst), guardReg_(&guardReg),
    guarded_(true), negated_(negated), 
    executionCount_(0), squashed_(false),
    inlineImmediate_(immediateSource) {
}

/**
 * Constructor.
 *
 * Used to construct a move that has an inline immediate as the source.
 * The inline immediate becomes property of the created ExecutableMove
 * and is deleted by it.
 *
 * @param immediateSource The inline immediate source of the move.
 * @param bus Bus of the move.
 * @param dst Destination of the move.
 * @param guardReg Guard register.
 * @param negated True if guard is reversed.
 */
ExecutableMove::ExecutableMove(
    InlineImmediateValue* immediateSource,
    BusState& bus,
    WritableState& dst) : 
    src_(immediateSource), bus_(&bus), dst_(&dst), 
    guardReg_(NULL), guarded_(false), negated_(false),
    executionCount_(0), squashed_(false),
    inlineImmediate_(immediateSource) {
}


/**
 * Destructor.
 */
ExecutableMove::~ExecutableMove() {
    delete inlineImmediate_;
    inlineImmediate_ = NULL;
}

/**
 * Evaluates the possible guard of the move, i.e., decides whether the
 * move is squashed at the simulated cycle or not.
 *
 * This must be called by the ExecutableInstruction before any 
 * executeRead()/executeWrite() is called in order not to overwrite 
 * the old guard value in case of the default 1-latency guards.
 */
void
ExecutableMove::evaluateGuard() {
    if (guarded_) {
        const SimValue& regValue = guardReg_->value();
        squashed_ = !((!negated_ && regValue.sIntWordValue() != 0) ||
                      (negated_ && regValue.sIntWordValue() == 0));
        bus_->setSquashed(squashed_);
    } else {
        bus_->setSquashed(false);
    }
}

/**
 * Copies the value of the move source to the transport bus.
 */
void
ExecutableMove::executeRead() {

    if (guarded_ && GUARD_BLOCKS_BUS_WRITE && squashed_) 
        return;
    bus_->setValueInlined(src_->value());
}

/**
 * Writes the value of the bus to the destination.
 *
 * If the move is guarded, the value is written to destination only if guard 
 * expression is true.
 *
 */
void
ExecutableMove::executeWrite() {

    if (guarded_ && squashed_) 
        return;

    dst_->setValue(bus_->value());
    executionCount_++;
}

/**
 * Resets the execution counter of this move.
 */
void
ExecutableMove::resetExecutionCount() {
    executionCount_ = 0;
}

/**
 * Returns the count of executions of this move.
 *
 * This count does not include squashed executions.
 *
 * @return Count of executions of this move.
 */
ClockCycleCount 
ExecutableMove::executionCount() const {
    return executionCount_;
}

/**
 * Returns true in case move with the move was squashed the last time
 * the instruction was executed.
 *
 * Being squashed means that the move is guarded and the guard expression
 * evaluated to false.
 *
 * @return True in case move was squashed last time its instruction was 
 *              executed.
 */
inline bool 
ExecutableMove::squashed() const {
    return squashed_;
}

/**
 * A dummy constructor for being used with DummyExecutableMove
 */
ExecutableMove::ExecutableMove() :
    src_(0),
    bus_(&NullBusState::instance()), 
    dst_(0),
    guarded_(false), 
    negated_(false) {
    inlineImmediate_ = NULL;
} 
        


/**
 * Constructor for SimpleExecutableMove that sets the execution count
 * 
 * @param executionCount the execution count
 */
DummyExecutableMove::DummyExecutableMove(ClockCycleCount executionCount) {
    executionCount_ = executionCount;
}
