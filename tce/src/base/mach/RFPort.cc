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
 * @file RFPort.cc
 *
 * Implementation of RFPort class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>

#include "RFPort.hh"
#include "BaseRegisterFile.hh"

using std::string;

namespace TTAMachine {

const string RFPort::OSNAME_RFPORT = "rf_port";

/**
 * The constructor.
 *
 * @param name Name of the port.
 * @param parent The parent register file of the port.
 * @exception InvalidName If the given name is not a valid component name.
 * @exception ComponentAlreadyExists If there is another port with the same
 *                                   name in the parent unit.
 */
RFPort::RFPort(const std::string& name, BaseRegisterFile& parent)
    throw (InvalidName, ComponentAlreadyExists) :
    Port(name, parent) {
}


/**
 * The constuctor.
 *
 * Loads the state of the object from the given ObjectState tree.
 *
 * @param state The ObjectState tree.
 * @param parent The parent unit of the port.
 * @exception ObjectStateLoadingException If an error occurs while loading
 *                                        the state.
 */
RFPort::RFPort(const ObjectState* state, Unit& parent)
    throw (ObjectStateLoadingException) : Port(state, parent) {
}


/**
 * The destructor.
 */
RFPort::~RFPort() {
}


/**
 * Returns the bit width of the port.
 *
 * @return The bit width of the port.
 */
int
RFPort::width() const {
    return parentUnit()->width();
}


/**
 * Returns the parent register file.
 *
 * @return The parent register file.
 */
BaseRegisterFile*
RFPort::parentUnit() const {
    Unit* parent = Port::parentUnit();
    BaseRegisterFile* rfParent = dynamic_cast<BaseRegisterFile*>(parent);
    assert(rfParent != NULL);
    return rfParent;
}


/**
 * Saves the state of the object to an ObjectState tree and returns it.
 *
 * @return The newly created ObjectState tree.
 */
ObjectState*
RFPort::saveState() const {
    ObjectState* state = Port::saveState();
    state->setName(OSNAME_RFPORT);
    return state;
}


/**
 * Loads the state of the object from the given ObjectState tree.
 *
 * @param state An ObjectState instance representing state of an RFPort.
 * @exception ObjectStateLoadingException If the given ObjectState instance
 *                                        is invalid.
 */
void
RFPort::loadState(const ObjectState* state)
    throw (ObjectStateLoadingException) {

    if (state->name() != OSNAME_RFPORT) {
        const string procName = "RFPort::loadState";
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }

    Port::loadState(state);
}

}
