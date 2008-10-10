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
