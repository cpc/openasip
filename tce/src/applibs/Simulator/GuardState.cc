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
 * @file GuardState.cc
 *
 * Definition of GuardState class.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "Application.hh"
#include "GuardState.hh"
#include "SimValue.hh"

using std::vector;
using std::string;

//////////////////////////////////////////////////////////////////////////////
// GuardState
//////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param targetRegister The target register this guard watches.
 * @param latency The total guard latency modeled.
 * @param lock Global lock signal (currently unused).
 */
GuardState::GuardState(
    const ReadableState& targetRegister, 
    int latency, 
    GlobalLock&) :
    target_(&targetRegister) {

    for (int i = 0; i < latency; ++i) {
        history_.push_back(targetRegister.value());
    }
    position_ = 0;
}

/**
 * Constructor.
 *
 * Creates an empty GuardState, only to be used by subclasses, i.e.,
 * NullGuardState.
 */
GuardState::GuardState() : target_(NULL), position_(-1) {
}

/**
 * Destructor.
 */
GuardState::~GuardState() {
}

/**
 * Does nothing.
 *
 * The guard history is updated in advanceClock().
 */
void 
GuardState::endClock() {
}

/**
 * Updates the guard value history.
 */
void
GuardState::advanceClock() {
    history_[position_] = target_->value();
    position_ = (position_ + 1) % history_.size();
}

/**
 * Returns the current value of the guard, taking the latency in account.
 *
 * @return The current value of the guard.
 */
const SimValue&
GuardState::value() const {
    return history_[position_];
}

//////////////////////////////////////////////////////////////////////////////
// NullGuardState
//////////////////////////////////////////////////////////////////////////////

NullGuardState NullGuardState::instance_;

/**
 * Returns the instance of NullGuardState.
 *
 * @return Instance of NullGuardState.
 */
NullGuardState&
NullGuardState::instance() {
    return instance_;
}

/**
 * Constructor.
 */
NullGuardState::NullGuardState() {
}

/**
 * Destructor.
 */
NullGuardState::~NullGuardState() {
}

//////////////////////////////////////////////////////////////////////////////
// OneClockGuardState
//////////////////////////////////////////////////////////////////////////////
/**
 * Constructor.
 * 
 * @param targetRegister The targer register this guard watches.
 */
OneClockGuardState::OneClockGuardState(const ReadableState& targetRegister) :
    target_(&targetRegister) {
}

/**
 * Destructor.
 */
OneClockGuardState::~OneClockGuardState() {
}


/**
 * Does nothing.
 */
void 
OneClockGuardState::endClock() {
}

/**
 * Does nothing.
 */
void
OneClockGuardState::advanceClock() {
}

/**
 * Returns the current value of the guard.
 *
 * @return The current value of the guard.
 */
const SimValue&
OneClockGuardState::value() const {
    return target_->value();
}

