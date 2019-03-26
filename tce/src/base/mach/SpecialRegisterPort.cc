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
 * @file SpecialRegisterPort.cc
 *
 * Implementation of SpecialRegisterPort class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>

#include "SpecialRegisterPort.hh"
#include "ControlUnit.hh"
#include "ObjectState.hh"

using std::string;

namespace TTAMachine {

const std::string SpecialRegisterPort::OSNAME_SPECIAL_REG_PORT = "sr_port";

/**
 * The constructor.
 *
 * @param name Name of the port.
 * @param width Bit width of the port.
 * @param parent The function unit to which the port belongs.
 * @exception InvalidName If the given name is not a valid component name.
 * @exception OutOfRange If the given bit width is less or equal to zero.
 * @exception ComponentAlreadyExists If the function unit already has another
 *                                   port by the same name or another port
 *                                   that sets operation code.
 */
SpecialRegisterPort::SpecialRegisterPort(
    const std::string& name,
    int width,
    ControlUnit& parent)
    throw (InvalidName, OutOfRange, ComponentAlreadyExists) :
    BaseFUPort(name, width, parent) {
}


/**
 * The constructor.
 *
 * Loads the state of the object from the given ObjectState tree.
 *
 * @param state The ObjectState tree.
 * @param parent The parent unit of the port.
 * @exception ObjectStateLoadingException If an error occurs while loading
 *                                        the state.
 */
SpecialRegisterPort::SpecialRegisterPort(
    const ObjectState* state,
    Unit& parent)
    throw (ObjectStateLoadingException) :
    BaseFUPort(state, parent) {
}


/**
 * The destructor.
 */
SpecialRegisterPort::~SpecialRegisterPort() {
}


/**
 * Always returns false. SpecialRegisterPort cannot be triggering.
 *
 * @return false
 */
bool
SpecialRegisterPort::isTriggering() const {
    return false;
}


/**
 * Always returns false. SpecialRegisterPort cannot be operation code
 * setting.
 *
 * @return false
 */
bool
SpecialRegisterPort::isOpcodeSetting() const {
    return false;
}


/**
 * Saves the state of the object to an ObjectState tree and returns it.
 *
 * @return The newly created ObjectState tree.
 */
ObjectState*
SpecialRegisterPort::saveState() const {
    ObjectState* state = BaseFUPort::saveState();
    state->setName(OSNAME_SPECIAL_REG_PORT);
    return state;
}


/**
 * Loads the state of the object from the given ObjectState tree.
 *
 * @param state An ObjectState instance representing state of a
 *              SpecialRegisterPort.
 * @exception ObjectStateLoadingException If the given ObjectState instance
 *                                        is invalid.
 */
void
SpecialRegisterPort::loadState(const ObjectState* state)
    throw (ObjectStateLoadingException) {

    if (state->name() != OSNAME_SPECIAL_REG_PORT) {
        const string procName = "SpecialRegisterPort::loadState";
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }

    BaseFUPort::loadState(state);
}

}
