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
 * @file FixedRegisters.cc
 *
 * Definition of FixedRegisters class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "FixedRegisters.hh"
#include "SimValue.hh"

/**
 * Constructor.
 */
FixedRegisters::FixedRegisters() : 
    stackPointer_(NULL), intReturnValue_(NULL), floatReturnValue_(NULL) {
}

/**
 * Destructor.
 */
FixedRegisters::~FixedRegisters() {
}

/**
 * Returns the stack pointer value.
 *
 * @return Stack pointer value.
 */
SimValue&
FixedRegisters::stackPointer() const {
    if (stackPointer_ == NULL) {
        return NullSimValue::instance();
    }
    return *stackPointer_;
}

/**
 * Returns integer argument with a given index.
 *
 * If index is out of range, returns NullSimValue.
 *
 * @return Integer argument with a given index.
 */
SimValue&
FixedRegisters::integerArgumentRegister(int index) const {
    if (index < 0 || index > static_cast<int>(integerArgs_.size()) - 1) {
        return NullSimValue::instance();
    }
    return *integerArgs_[index];
}

/**
 * Returns float argument register with a given index.
 *
 * If index is out of range, returns NullSimValue.
 *
 * @return Float argument register with a given index.
 */
SimValue&
FixedRegisters::floatArgumentRegister(int index) const {
    if (index < 0 || index > static_cast<int>(floatArgs_.size()) - 1) {
        return NullSimValue::instance();
    }
    return *floatArgs_[index];
}

/**
 * Returns integer return value.
 *
 * @return Integer return value.
 */
SimValue&
FixedRegisters::integerReturnValue() const {
    if (intReturnValue_ == NULL) {
        return NullSimValue::instance();
    }
    return *intReturnValue_;
}

/**
 * Returns float return value.
 *
 * @return Float return value.
 */
SimValue&
FixedRegisters::floatReturnValue() const {
    if (floatReturnValue_ == NULL) {
        return NullSimValue::instance();
    }
    return *floatReturnValue_;
}

/**
 * Sets stack pointer to new value.
 *
 * @param value New value.
 */
void
FixedRegisters::setStackPointer(SimValue& value) {
    stackPointer_ = &value;
}

/**
 * Adds new integer argument register.
 *
 * @param value New integer argument register value.
 */
void
FixedRegisters::addIntegerArgumentRegister(SimValue& value) {
    integerArgs_.push_back(&value);
}

/**
 * Adds new float argument register.
 *
 * @param value New float argument register value.
 */
void
FixedRegisters::addFloatArgumentRegister(SimValue& value) {
    floatArgs_.push_back(&value);
}

/**
 * Sets integer return value to new value.
 *
 * @param value New value.
 */
void
FixedRegisters::setIntegerReturnValue(SimValue& value) {
    intReturnValue_ = &value;
}

/**
 * Sets float return value to new value.
 *
 * @param value New value.
 */
void
FixedRegisters::setFloatReturnValue(SimValue& value) {
    floatReturnValue_ = &value;
}
