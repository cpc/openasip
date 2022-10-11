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
 * @file BaseFUPort.cc
 *
 * Implementation of BaseFUPort class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>

#include "BaseFUPort.hh"
#include "FunctionUnit.hh"
#include "ObjectState.hh"

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
BaseFUPort::BaseFUPort(const std::string& name, int width, FunctionUnit& parent)
    : Port(name, *static_cast<Unit*>(&parent)), width_(width) {
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
    : Port(state, parent) {
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
BaseFUPort::setWidth(int width) {
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
BaseFUPort::loadState(const ObjectState* state) {
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
BaseFUPort::loadStateWithoutReferences(const ObjectState* state) {
    const string procName = "BaseFUPort::loadStateWithoutReferences";

    try {
        setWidth(state->intAttribute(OSKEY_WIDTH));
    } catch (const Exception& e) {
        throw ObjectStateLoadingException(
            __FILE__, __LINE__, procName, e.errorMessage());
    }
}
}
