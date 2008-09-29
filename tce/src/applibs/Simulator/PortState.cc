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
