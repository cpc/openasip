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
 * @file BaseFUPort.cc
 *
 * Implementation of BaseFUPort class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include <string>

#include "BaseFUPort.hh"
#include "FunctionUnit.hh"

using std::string;

namespace TTAMachine {

// initialisation of static data members
const string BaseFUPort::OSKEY_WIDTH = "width";

/**
 * The constructor.
 *
 * @param name Name of the port.
 * @param width Bit width of the port.
 * @param parent The function unit to which the port belongs.
 * @exception InvalidName If the given name is not a valid component name.
 * @exception ComponentAlreadyExists If the function unit already has another
 *                                   port by the same name or another port
 *                                   that sets operation code.
 * @exception OutOfRange If the given bit width is less or equal to zero.
 */
BaseFUPort::BaseFUPort(
    const std::string& name,
    int width,
    FunctionUnit& parent)
    throw (InvalidName, ComponentAlreadyExists, OutOfRange) :
    Port(name, *static_cast<Unit*>(&parent)), width_(width) {

    if (width <= 0) {
        string procName = "BaseFUPort::BaseFUPort";
        throw OutOfRange(__FILE__, __LINE__, procName);
    }
}


/**
 * The constructor.
 *
 * Loads the state of the object from the given ObjectState tree.
 *
 * @param state The ObjectState tree.
 * @param Unit The parent unit of the port.
 * @exception ObjectStateLoadingException If an error occurs while loading
 *                                        the state.
 */
BaseFUPort::BaseFUPort(const ObjectState* state, Unit& parent)
    throw (ObjectStateLoadingException) : Port(state, parent) {

    loadStateWithoutReferences(state);
}


/**
 * The destructor.
 */
BaseFUPort::~BaseFUPort() {
}


/**
 * Returns the parent function unit of the port.
 *
 * Returns NULL in case not registered to a function unit.
 *
 * @return The parent function unit of the port. 
 */
FunctionUnit*
BaseFUPort::parentUnit() const {
    Unit* parentUnit = Port::parentUnit();
    FunctionUnit* fu = dynamic_cast<FunctionUnit*>(parentUnit);
    return fu;
}


/**
 * Returns the bit width of the port.
 *
 * @return Bit width of the port.
 */
int
BaseFUPort::width() const {
    return width_;
}


/**
 * Sets the bit width of the port.
 *
 * @param width The new bit width.
 * @exception OutOfRange If the given bit width is less or equal to zero.
 */
void
BaseFUPort::setWidth(int width)
    throw (OutOfRange) {

    if (width <= 0) {
        string procName = "FUPort::setWidth";
        throw OutOfRange(__FILE__, __LINE__, procName);
    }

    width_ = width;
}


/**
 * Saves the contents of the function unit port to an ObjectState object.
 *
 * @return The newly created ObjectState object.
 */
ObjectState*
BaseFUPort::saveState() const {
    ObjectState* fuPort = Port::saveState();
    fuPort->setAttribute(OSKEY_WIDTH, width_);
    return fuPort;
}


/**
 * Loads the state of the function unit port from the given ObjectState
 * instance.
 *
 * @param state The ObjectState instance.
 * @exception ObjectStateLoadingException If the given ObjectState instance
 *                                        is invalid or if connections to
 *                                        socket cannot be made.
 */
void
BaseFUPort::loadState(const ObjectState* state)
    throw (ObjectStateLoadingException) {

    loadStateWithoutReferences(state);
    Port::loadState(state);
}


/**
 * Loads its state from the given ObjectState instance but does not create
 * connections to sockets.
 *
 * @param state The ObjectState instance.
 * @exception ObjectStateLoadingException If the given ObjectState instance
 *                                        is invalid.
 */
void
BaseFUPort::loadStateWithoutReferences(const ObjectState* state)
    throw (ObjectStateLoadingException) {

    const string procName = "BaseFUPort::loadStateWithoutReferences";

    try {
        setWidth(state->intAttribute(OSKEY_WIDTH));
    } catch (const Exception& e) {
        throw ObjectStateLoadingException(
            __FILE__, __LINE__, procName, e.errorMessage());
    }
}

}
