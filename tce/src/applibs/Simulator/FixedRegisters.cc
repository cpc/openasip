/**
 * @file FixedRegisters.cc
 *
 * Definition of FixedRegisters class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
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
