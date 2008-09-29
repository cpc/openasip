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
 * @file LongImmediateUnitState
 *
 * Definition of LongImmediateUnitState class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <string>

#include "LongImmediateUnitState.hh"
#include "LongImmediateRegisterState.hh"
#include "SequenceTools.hh"
#include "Application.hh"
#include "Exception.hh"

using std::string;

//////////////////////////////////////////////////////////////////////////////
// LongImmediateUnitState
//////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param size Size of the unit.
 * @param latency Latency of the unit.
 * @param name Name of the unit.
 * @param width The bit width of the registers in the unit.
 * @param signExtend Whether the values written to the registers should be sign 
 *                   extended.
 */
LongImmediateUnitState::LongImmediateUnitState(
    int size, 
    int latency, 
    const std::string& name,
    int width,
    bool signExtend) :
    ClockedState(), latency_(latency), name_(name) {

    for (int i = 0; i < size; i++) {
        registers_.push_back(
            new LongImmediateRegisterState(this, i, width, signExtend));
    }
    SimValue val(width);
    values_.resize(size, val);
}

/**
 * Destructor.
 */
LongImmediateUnitState::~LongImmediateUnitState() {
    clear();
}

/**
 * Clears the containers.
 */
void
LongImmediateUnitState::clear() {
    SequenceTools::deleteAllItems(registers_);
    SequenceTools::deleteAllItems(queue_);
    values_.clear();
}

/**
 * Returns the register value of the given index.
 *
 * @param index Index of the register.
 * @return Register value of the given index.
 * @exception OutOfRange If index is out of range.
 */
SimValue&
LongImmediateUnitState::registerValue(int index)
    throw (OutOfRange) {

    if (index < 0 || index > static_cast<int>(values_.size()) - 1) {
        string msg = "Index out of range.";
        throw OutOfRange(__FILE__, __LINE__, __func__, msg);
    }
    return values_[index];
}

/**
 * Sets the register value.
 *
 * @param index Index of the register.
 * @param value Value to be set.
 * @exception OutOfRange If index is out of range.
 */
void
LongImmediateUnitState::setRegisterValue(int index, const SimValue& value)
    throw (OutOfRange) {

    if (index < 0 || index > static_cast<int>(values_.size()) - 1) {
        string msg = "Index out of range.";
        throw OutOfRange(__FILE__, __LINE__, __func__, msg);
    }

    // iu latency (cycles) is fixed to 1
    values_[index] = value;
}

/**
 * End of clock cycle.
 *
 * Nothing is done.
 */
void
LongImmediateUnitState::endClock() {
}

/**
 * Advances clock by one cycle.
 *
 * Register values are updated. This method is used to simulate the latency of
 * the immediate unit state: register values are not updated immediately in 
 * case the latency is greater than zero.
 */
void
LongImmediateUnitState::advanceClock() {

    ItemQueue::iterator iter = queue_.begin();
    while (iter != queue_.end()) {
   
        (*iter)->timer_++;
        if ((*iter)->timer_ == latency_) {
            values_[(*iter)->index_] = (*iter)->value_;
            delete *iter;
            iter = queue_.erase(iter);
        } else {
            iter++;
        }
    }
}

/**
 * Returns the register of the given index.
 *
 * @param index Index of the register.
 * @return Register of the given index.
 * @exception OutOfRange If index is out of range.
 */
LongImmediateRegisterState&
LongImmediateUnitState::immediateRegister(int i)
    throw (OutOfRange) {

    if (i < 0 || i > static_cast<int>(registers_.size()) - 1) {
	string msg = "Index out of range.";
        throw OutOfRange(__FILE__, __LINE__, __func__, msg);
    }
    return *registers_[i];
}

/**
 * Returns the count of the registers in the unit.
 *
 * @return The count of the immediate registers.
 */
int
LongImmediateUnitState::immediateRegisterCount() const {
    return registers_.size();
}


//////////////////////////////////////////////////////////////////////////////
// NullLongImmediateUnitState
//////////////////////////////////////////////////////////////////////////////

NullLongImmediateUnitState* NullLongImmediateUnitState::instance_ = NULL;

/**
 * Returns NullLongImmediateUnitState instance.
 *
 * @return NullLongImmediateUnitState instance.
 */
NullLongImmediateUnitState&
NullLongImmediateUnitState::instance() {
    if (instance_ == NULL) {
        instance_ = new NullLongImmediateUnitState();
    }
    return *instance_;
}

/**
 * Constructor.
 */
NullLongImmediateUnitState::NullLongImmediateUnitState() :
    LongImmediateUnitState(0, 0, "<NULL>", 0, false) {
}

/**
 * Destructor.
 */
NullLongImmediateUnitState::~NullLongImmediateUnitState() {
}

/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 * @exception OutOfRange Never throws.
 */
SimValue&
NullLongImmediateUnitState::registerValue(int)
    throw (OutOfRange) {

    Application::abortWithError("registerValue()");
    return NullSimValue::instance();
}

/**
 * Aborts the program with error message.
 *
 * @exception OutOfRange Never throws.
 */
void
NullLongImmediateUnitState::setRegisterValue(int, SimValue&)
    throw (OutOfRange) {

    Application::abortWithError("setRegisterValue()");
}

/**
 * Aborts the program with error message.
 */
void
NullLongImmediateUnitState::endClock() {
    Application::abortWithError("endClock()");
}

/**
 * Aborts the program with error message.
 */
void
NullLongImmediateUnitState::advanceClock() {
    Application::abortWithError("advanceClock()");
}

/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
LongImmediateRegisterState&
NullLongImmediateUnitState::immediateRegister(int)
    throw (OutOfRange) {
    Application::abortWithError("immediateRegister()");
    return *(new LongImmediateRegisterState(NULL, 0, 0, false));
}

/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
int
NullLongImmediateUnitState::immediateRegisterCount() const {
    Application::abortWithError("immediateRegisterCount()");
    return 0;
}
