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
LongImmediateUnitState::registerValue(int index) {
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
LongImmediateUnitState::setRegisterValue(int index, const SimValue& value) {
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
LongImmediateUnitState::immediateRegister(int i) {
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
NullLongImmediateUnitState::registerValue(int) {
    Application::abortWithError("registerValue()");
    return NullSimValue::instance();
}

/**
 * Aborts the program with error message.
 *
 * @exception OutOfRange Never throws.
 */
void
NullLongImmediateUnitState::setRegisterValue(int, const SimValue&) {
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
NullLongImmediateUnitState::immediateRegister(int) {
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
