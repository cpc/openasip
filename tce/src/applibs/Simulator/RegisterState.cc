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
 * @file RegisterState.cc
 *
 * Definition of RegisterState class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka Jääskeläinen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "RegisterState.hh"
#include "Application.hh"

using std::string;

//////////////////////////////////////////////////////////////////////////////
// RegisterState
//////////////////////////////////////////////////////////////////////////////


/**
 * Constructor.
 *
 * @param width The width of the register.
 */
RegisterState::RegisterState(int width) : 
    StateData(), value_(*(new SimValue(width))), shared_(false) {
}

/**
 * Constructor for RegisterState which shares the actual register storage.
 *
 * @param sharedRegister The register which is shared with this.
 */
RegisterState::RegisterState(SimValue& sharedRegister) : 
    StateData(), value_(sharedRegister), shared_(true) {
}

/**
 * Destructor.
 */
RegisterState::~RegisterState() {
    if (!shared_) {
        delete &value_;
    }
}

/**
 * Sets the value for the register.
 *
 * @param value Value to be set.
 */
void
RegisterState::setValue(const SimValue& value) {
    value_ = value;
}

/**
 * Returns the value of the register.
 *
 * @return The value of the register.
 */
const SimValue&
RegisterState::value() const {
    return value_;
}

//////////////////////////////////////////////////////////////////////////////
// NullRegisterState
//////////////////////////////////////////////////////////////////////////////

NullRegisterState* NullRegisterState::instance_ = NULL;

/**
 * Returns instance of NullRegisterState.
 *
 * @return The instance of NullRegisterState.
 */
NullRegisterState&
NullRegisterState::instance() {
    if (instance_ == NULL) {
        instance_ = new NullRegisterState();
    }
    return *instance_;
}

/**
 * Constructor.
 */
NullRegisterState::NullRegisterState() : RegisterState(0) {
}

/**
 * Destructor.
 */
NullRegisterState::~NullRegisterState() {
}

/**
 * Aborts the program with error message.
 */
void
NullRegisterState::setValue(const SimValue&) {
    Application::abortWithError("setValue()");
}

/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
const SimValue&
NullRegisterState::value() const {
    Application::abortWithError("value()");
    return NullSimValue::instance();
}

