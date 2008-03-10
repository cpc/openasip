/**
 * @file BuslessExecutableMove.cc
 *
 * Definition of BuslessExecutableMove class.
 *
 * @author Pekka Jääskeläinen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "BuslessExecutableMove.hh"
#include "ReadableState.hh"
#include "WritableState.hh"
#include "BusState.hh"
#include "SimValue.hh"

/**
 * Constructor.
 *
 * @param src Source of the move.
 * @param dst Destination of the move.
 */
BuslessExecutableMove::BuslessExecutableMove(
    const ReadableState& src,
    WritableState& dst) : 
    ExecutableMove(src, NullBusState::instance(), dst) {
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
BuslessExecutableMove::BuslessExecutableMove(
    const ReadableState& src,
    WritableState& dst,
    const ReadableState& guardReg,
    bool negated) : 
    ExecutableMove(src, NullBusState::instance(), dst, guardReg, negated) {
}

/**
 * Constructor.
 *
 * Used to construct a move that has an inline immediate as the source.
 * The inline immediate becomes property of the created BuslessExecutableMove
 * and is deleted by it.
 *
 * @param immediateSource The inline immediate source of the move.
 * @param bus Bus of the move.
 * @param dst Destination of the move.
 * @param guardReg Guard register.
 * @param negated True if guard is reversed.
 */
BuslessExecutableMove::BuslessExecutableMove(
    InlineImmediateValue* immediateSource,
    WritableState& dst,
    const ReadableState& guardReg,
    bool negated) : 
    ExecutableMove(
        immediateSource, NullBusState::instance(), dst, guardReg, negated) {
}

/**
 * Constructor.
 *
 * Used to construct a move that has an inline immediate as the source.
 * The inline immediate becomes property of the created BuslessExecutableMove
 * and is deleted by it.
 *
 * @param immediateSource The inline immediate source of the move.
 * @param bus Bus of the move.
 * @param dst Destination of the move.
 * @param guardReg Guard register.
 * @param negated True if guard is reversed.
 */
BuslessExecutableMove::BuslessExecutableMove(
    InlineImmediateValue* immediateSource,
    WritableState& dst) : 
    ExecutableMove(immediateSource, NullBusState::instance(), dst) {
}


/**
 * Destructor.
 */
BuslessExecutableMove::~BuslessExecutableMove() {
}

/**
 * Does not do anything because value is transferred directly from source to
 * destination without writing to a bus first.
 *
 */
void
BuslessExecutableMove::executeRead() {
}

/**
 * Writes the value from source to the destination.
 *
 * If the move is guarded, the value is written to destination only if guard 
 * expression is true.
 */
void
BuslessExecutableMove::executeWrite() {

    if (guarded_) {
        const SimValue& regValue = guardReg_->value();
        if (!((regValue.uIntWordValue() != 0 && !negated_) ||
              (regValue.uIntWordValue() == 0 && negated_))) {
            // guard expression evaluated to false
            squashed_ = true;
            return; 
        }
    }

    squashed_ = false;
    dst_.setValue(src_.value());
    executionCount_++;
}
