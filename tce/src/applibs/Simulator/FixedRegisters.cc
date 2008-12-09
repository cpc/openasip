/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
