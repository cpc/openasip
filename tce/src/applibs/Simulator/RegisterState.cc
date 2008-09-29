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
 * @file RegisterState.cc
 *
 * Definition of RegisterState class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
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
    value_.setActive();
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

