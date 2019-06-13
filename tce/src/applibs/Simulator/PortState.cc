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
 * @file PortState.cc
 *
 * Declaration of PortState class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2006 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "PortState.hh"
#include "Application.hh"
#include "SimValue.hh"
#include "FUState.hh"

using std::string;

//////////////////////////////////////////////////////////////////////////////
// PortState
//////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param parent The parent FU state object of the port.
 * @param width The bit width of the port.
 */
PortState::PortState(FUState& parent, int width) : 
    RegisterState(width), parent_(&parent) {
}

/**
 * Constructor for RegisterState which shares the actual register storage.
 *
 * @param parent The parent FU state object of the port.
 * @param sharedRegister The register which is shared with this.
 */
PortState::PortState(
    FUState& parent, SimValue& sharedRegister) :
    RegisterState(sharedRegister), parent_(&parent) {
    
}


/**
 * Destructor.
 */ 
PortState::~PortState() {
}

/**
 * Returns the parent of the port.
 *
 * @return The parent of the port.
 */
FUState&
PortState::parent() const {
    return *parent_;
}

//////////////////////////////////////////////////////////////////////////////
// NullPortState
//////////////////////////////////////////////////////////////////////////////

NullPortState* NullPortState::instance_ = NULL;

/**
 * Returns the instance of NullPortState.
 *
 * @return The instance of NullPortState.
 */
NullPortState&
NullPortState::instance() {
    if (instance_ == NULL) {
        instance_ = new NullPortState();
    }
    return *instance_;
}

/**
 * Constructor.
 */
NullPortState::NullPortState() : PortState(NullFUState::instance(), 0) {
}

/**
 * Destructor.
 */
NullPortState::~NullPortState() {
}

/**
 * Aborts the program with error message.
 */
void
NullPortState::setValue(const SimValue&) {
    Application::abortWithError("setValue()");
}

/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
const SimValue&
NullPortState::value() const {
    Application::abortWithError("value()");
    return NullSimValue::instance();
}

/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
FUState&
NullPortState::parent() const {
    Application::abortWithError("parent()");
    return NullFUState::instance();
}
