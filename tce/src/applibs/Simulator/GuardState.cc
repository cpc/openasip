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
 */
GuardState::GuardState(
    const ReadableState& targetRegister, 
    int latency) :
    target_(&targetRegister) {
    assert(latency && "Use DirectGuardState for 0-cycle latency");
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
    position_ = (position_ + 1);
    if ((size_t)position_ >= history_.size())
      position_ = 0;
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
// DirectGuardState
//////////////////////////////////////////////////////////////////////////////
/**
 * Constructor.
 * 
 * @param targetRegister The targer register this guard watches.
 */
DirectGuardState::DirectGuardState(const ReadableState& targetRegister) :
    target_(&targetRegister) {
}

/**
 * Destructor.
 */
DirectGuardState::~DirectGuardState() {
}


/**
 * Does nothing.
 */
void 
DirectGuardState::endClock() {
}

/**
 * Does nothing.
 */
void
DirectGuardState::advanceClock() {
}

/**
 * Returns the current value of the guard.
 *
 * @return The current value of the guard.
 */
const SimValue&
DirectGuardState::value() const {
    return target_->value();
}

