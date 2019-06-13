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
 * @file BaseRegisterFile.cc
 *
 * Implementation of abstract BaseRegisterFile class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen-no.spam-tut.fi)
 * @note reviewed 10 Jun 2004 by vpj, am, tr, ll
 * @note rating: red
 */

#include "BaseRegisterFile.hh"
#include "Machine.hh"
#include "Bus.hh"
#include "Guard.hh"
#include "RegisterFile.hh"
#include "Application.hh"
#include "ObjectState.hh"

using std::string;

namespace TTAMachine {

// initialization of static data members
const string BaseRegisterFile::OSNAME_BASE_REGISTER_FILE = "baseregfile";
const string BaseRegisterFile::OSKEY_SIZE = "size";
const string BaseRegisterFile::OSKEY_WIDTH = "width";

/**
 * Constructor.
 *
 * @param name Name of the register file.
 * @param size Number of registers in the register file.
 * @param width Bit width of the registers in the register file.
 * @exception OutOfRange If the given size or width is out of range.
 * @exception InvalidName If the given name is not a valid component name.
 */
BaseRegisterFile::BaseRegisterFile(const std::string& name, int size, int width)
    : Unit(name), size_(size), width_(width) {
    setNumberOfRegisters(size);
    setWidth(width);
}

/**
 * Constructor.
 *
 * Loads the state of the object from the given ObjectState instance. Does
 * not load references to other components.
 *
 * @param state The ObjectState instance from which the state is loaded.
 * @exception ObjectStateLoadingException If the given ObjectState instance
 *                                        is invalid.
 */
BaseRegisterFile::BaseRegisterFile(const ObjectState* state)
    : Unit(state), size_(0), width_(0) {
    loadStateWithoutReferences(state);
}

/**
 * Destructor.
 */
BaseRegisterFile::~BaseRegisterFile() {
}


/**
 * Sets the number of registers in the register file.
 *
 * @param registers The new amount of registers.
 * @exception OutOfRange If the given number of registers is less or equal
 *                       to zero.
 */
void
BaseRegisterFile::setNumberOfRegisters(int registers) {
    if (registers <= 0) {
        string procName = "BaseRegisterFile::setNumberOfRegisters";
        throw OutOfRange(__FILE__, __LINE__, procName);
    }

    size_ = registers;
}

/**
 * Sets the bit width of the registers.
 *
 * @param width The new bit width.
 * @exception OutOfRange If the given width is less or equal to zero.
 */
void
BaseRegisterFile::setWidth(int width) {
    if (width <= 0) {
        string procName = "BaseRegisterFile::setWidth";
        throw OutOfRange(__FILE__, __LINE__, procName);
    }

    width_ = width;
}

/**
 * Returns the requested port.
 *
 * @param name Name of the port.
 * @return The requested port.
 * @exception InstanceNotFound If a port is not found by the given name.
 */
RFPort*
BaseRegisterFile::port(const std::string& name) const {
    Port* port = Unit::port(name);
    RFPort* rfPort = dynamic_cast<RFPort*>(port);
    assert(rfPort != NULL);
    return rfPort;
}

/**
 * Returns a port by the given index.
 *
 * The index must be greater or equal to 0 and smaller than the number of
 * ports in the unit.
 *
 * @param index Index.
 * @return The port by the given index.
 * @exception OutOfRange If the given index is out of range.
 */
RFPort*
BaseRegisterFile::port(int index) const {
    Port* port = Unit::port(index);
    RFPort* rfPort = dynamic_cast<RFPort*>(port);
    assert(rfPort != NULL);
    return rfPort;
}

/**
 * Saves the state of the object into an ObjectState tree.
 *
 * @return The newly created ObjectState tree.
 */
ObjectState*
BaseRegisterFile::saveState() const {
    ObjectState* state = Unit::saveState();
    state->setName(OSNAME_BASE_REGISTER_FILE);
    state->setAttribute(OSKEY_SIZE, size_);
    state->setAttribute(OSKEY_WIDTH, width_);
    return state;
}


/**
 * Loads its the from the given ObjectState instance.
 *
 * @param state The ObjectState instance.
 * @exception ObjectStateLoadingException If the given ObjectState instance
 *                                        is invalid.
 */
void
BaseRegisterFile::loadState(const ObjectState* state) {
    loadStateWithoutReferences(state);
    Unit::loadState(state);
}

/**
 * Loads its state from the given ObjectState instance without references
 * to other components.
 *
 * @param state The ObjectState instance.
 * @exception ObjectStateLoadingException If the given ObjectState instance
 *                                        is invalid.
 */
void
BaseRegisterFile::loadStateWithoutReferences(const ObjectState* state) {
    try {
        setNumberOfRegisters(state->intAttribute(OSKEY_SIZE));
        setWidth(state->intAttribute(OSKEY_WIDTH));
    } catch (...) {
        string procName = "BaseRegisterFile::loadStateWithoutReferences";
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }
}
}
