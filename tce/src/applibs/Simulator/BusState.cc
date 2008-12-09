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
 * @file BusState.cc
 *
 * Definition of BusState class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "BusState.hh"
#include "Application.hh"
#include "SimValue.hh"
#include "BaseType.hh"

using std::string;

//////////////////////////////////////////////////////////////////////////////
// BusState
//////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param width Bit width of the bus.
 */
BusState::BusState(int width) : 
    RegisterState(width), squashed_(false) {
}

/**
 * Destructor.
 */
BusState::~BusState() {
}

/**
 * Sets the value of the bus.
 *
 * Does not extend the value.
 *
 * @param value New value.
 */
void
BusState::setValue(const SimValue& value) {
    RegisterState::setValue(value);
}

/**
 * Sets whether the last executed move scheduled to this bus was squashed.
 *
 * @param isSquashed True in case this bus was squashed.
 */
void 
BusState::setSquashed(bool isSquashed) {
    squashed_ = isSquashed;
}

/**
 * Returns true if the last executed move scheduled to this bus was squashed.
 *
 * @param isSquashed True in case this bus was squashed.
 */
bool 
BusState::isSquashed() const {
    return squashed_;
}

//////////////////////////////////////////////////////////////////////////////
// NullBusState
//////////////////////////////////////////////////////////////////////////////

NullBusState* NullBusState::instance_ = NULL;

/**
 * Returns the instance of NullBusState.
 *
 * @return The instance of NullBusState.
 */
NullBusState&
NullBusState::instance() {
    if (instance_ == NULL) {
        instance_ = new NullBusState();
    }
    return *instance_;
}

/**
 * Constructor.
 */
NullBusState::NullBusState() : BusState(0) {
}

/**
 * Destructor.
 */
NullBusState::~NullBusState() {
}

/**
 * Aborts the program with error message.
 */
void
NullBusState::setValue(const SimValue&) {
    Application::abortWithError("setValue()");
}

/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
const SimValue&
NullBusState::value() const {
    Application::abortWithError("value()");
    return NullSimValue::instance();
}
