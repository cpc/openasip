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
#include "ObjectState.hh"

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
    : Port(name, parent) {}

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
RFPort::RFPort(const ObjectState* state, Unit& parent) : Port(state, parent) {}

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
    return static_cast<BaseRegisterFile*>(Port::parentUnit());
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
RFPort::loadState(const ObjectState* state) {
    if (state->name() != OSNAME_RFPORT) {
        const string procName = "RFPort::loadState";
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }

    Port::loadState(state);
}
}
