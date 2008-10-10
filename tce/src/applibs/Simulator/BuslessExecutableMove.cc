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
 * @file BuslessExecutableMove.cc
 *
 * Definition of BuslessExecutableMove class.
 *
 * @author Pekka Jääskeläinen 2005 (pjaaskel-no.spam-cs.tut.fi)
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
    dst_->setValue(src_->value());
    executionCount_++;
}
