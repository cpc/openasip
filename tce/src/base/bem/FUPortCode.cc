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
 * @file FUPortCode.cc
 *
 * Implementation of FUPortCode class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include "FUPortCode.hh"
#include "SocketCodeTable.hh"
#include "MathTools.hh"
#include "ObjectState.hh"

using std::string;

const std::string FUPortCode::OSNAME_FU_PORT_CODE = "fu_port_code";
const std::string FUPortCode::OSKEY_PORT_NAME = "port";
const std::string FUPortCode::OSKEY_OPERATION_NAME = "operation";

/**
 * The constructor.
 *
 * Creates an encoding for a FU port and registers it into a socket code
 * table. The port is identified by a name string and by the name of its
 * function unit.
 *
 * @param fu Name of the function unit.
 * @param port Name of the port.
 * @param encoding The encoding.
 * @param extraBits The number of extra (zero) bits in the encoding.
 * @param parent The parent socket code table.
 * @exception ObjectAlreadyExists If the given socket code table already has
 *                                a code for the same port or if the encoding
 *                                is ambiguous with another encoding in the
 *                                same socket code table.
 * @exception OutOfRange If some of the given values is out of range.
 */
FUPortCode::FUPortCode(
    const std::string& fu,
    const std::string& port,
    unsigned int encoding,
    unsigned int extraBits,
    SocketCodeTable& parent)
    throw (ObjectAlreadyExists, OutOfRange) :
    PortCode(fu, encoding, extraBits, 0), port_(port), opName_("") {

    parent.addFUPortCode(*this);
    setParent(&parent);
}


/**
 * The constructor.
 *
 * Creates an encoding for a FU port and registers it into a socket code
 * table. The port is identified by a name string, the operation carried by 
 * it and the name of the parent function unit.
 *
 * @param fu Name of the function unit.
 * @param port Name of the port.
 * @param operation Name of the operation carried by the port.
 * @param encoding The encoding for the port + operation.
 * @param extraBits The number of extra zero bits in the encoding.
 * @param parent The parent socket code table.
 * @exception ObjectAlreadyExists If the given socket code table already has
 *                                a code for this port and the operation
 *                                carried or if the encoding is ambiguous
 *                                with another encoding in the same socket
 *                                code table.
 * @exception OutOfRange If some of the given values is out of range.
 */
FUPortCode::FUPortCode(
    const std::string& fu,
    const std::string& port,
    const std::string& operation,
    unsigned int encoding,
    unsigned int extraBits,
    SocketCodeTable& parent)
    throw (ObjectAlreadyExists, OutOfRange) :
    PortCode(fu, encoding, extraBits, 0), port_(port), opName_(operation) {

    parent.addFUPortCode(*this);
    setParent(&parent);
}


/**
 * The constructor.
 *
 * Loads the state of the object from the given ObjectState tree.
 *
 * @param state The ObjectState tree.
 * @param parent The parent socket code table.
 * @exception ObjectStateLoadingException If an error occurs while loading
 *                                        the state.
 * @exception ObjectAlreadyExists If the given socket code table already has
 *                                a code for this port and the operation
 *                                carried or if the encoding is ambiguous
 *                                with another encoding in the same socket
 *                                code table.
 */
FUPortCode::FUPortCode(const ObjectState* state, SocketCodeTable& parent)
    throw (ObjectStateLoadingException, ObjectAlreadyExists) :
    PortCode(state), port_(""), opName_("") {

    const string procName = "FUPortCode::FUPortCode";

    if (state->name() != OSNAME_FU_PORT_CODE) {
	throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }

    if (state->hasAttribute(OSKEY_OPERATION_NAME)) {
        opName_ = state->stringAttribute(OSKEY_OPERATION_NAME);
    }
    port_ = state->stringAttribute(OSKEY_PORT_NAME);

    parent.addFUPortCode(*this);
    setParent(&parent);
}


/**
 * The destructor.
 */
FUPortCode::~FUPortCode() {
    SocketCodeTable* parent = this->parent();
    setParent(NULL);
    parent->removeFUPortCode(*this);
}


/**
 * Returns the name of the port.
 *
 * @return The name of the port.
 */
std::string
FUPortCode::portName() const {
    return port_;
}


/**
 * Returns the name of the operation.
 *
 * @return The name of the operation.
 * @exception InstanceNotFound If this control code identifies a plain FU
 *                             port without operation.
 */
std::string
FUPortCode::operationName() const
    throw (InstanceNotFound) {

    if (opName_ == "") {
	const string procName = "FUPortCode::operationName";
	throw InstanceNotFound(__FILE__, __LINE__, procName);
    }

    return opName_;
}


/**
 * Tells whether this control code identifies also one of the operations
 * carried by the FU port.
 *
 * @return True if the control code identifies an operation, otherwise false.
 */
bool
FUPortCode::hasOperation() const {
    return opName_ != "";
}


/**
 * Saves the state of the object to an ObjectState tree.
 *
 * @return The newly created ObjectState tree.
 */
ObjectState*
FUPortCode::saveState() const {
    ObjectState* state = PortCode::saveState();
    state->setName(OSNAME_FU_PORT_CODE);
    state->setAttribute(OSKEY_PORT_NAME, portName());
    if (hasOperation()) {
	state->setAttribute(OSKEY_OPERATION_NAME, operationName());
    }
    return state;
}
