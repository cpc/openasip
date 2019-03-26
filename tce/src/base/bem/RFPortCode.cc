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
 * @file RFPortCode.cc
 *
 * Implementation of RFPortCode class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>

#include "RFPortCode.hh"
#include "SocketCodeTable.hh"
#include "ObjectState.hh"

using std::string;

const std::string RFPortCode::OSNAME_RF_PORT_CODE = "rf_port_code";

/**
 * The constructor.
 *
 * Registers the created instance to the given socket code table
 * automatically.
 *
 * @param regFile Name of the register file.
 * @param encoding The encoding for the register file port.
 * @param extraBits The number of extra zero bits in the encoding.
 * @param indexWidth The number of bits reserved for the register index.
 * @param parent The parent socket code table.
 * @exception ObjectAlreadyExists If the socket code table has an encoding
 *                                defined for the same port already, or if
 *                                the encoding is ambiguous with another
 *                                encoding in the same socket code table.
 * @exception OutOfRange If some of the given values is out of range.
 */
RFPortCode::RFPortCode(
    const std::string& regFile,
    unsigned int encoding,
    unsigned int extraBits,
    int indexWidth,
    SocketCodeTable& parent)
    throw (ObjectAlreadyExists, OutOfRange) :
    PortCode(regFile, encoding, extraBits, indexWidth) {

    setParent(NULL);
    parent.addRFPortCode(*this);
    setParent(&parent);
}


/**
 * The constructor.
 *
 * Creates an RF port code without port encoding. That is, the port
 * code consists of mere register index. This is useful when the
 * parent socket code table does not have other port codes. Registers
 * the created instance to the given socket code table automatically.
 *
 * @param regFile Name of the register file.
 * @param indexWidth The number of bits reserved for the register index.
 * @param parent The parent socket code table.
 * @exception ObjectAlreadyExists If the socket code table has an encoding
 *                                defined for the same port already, or if
 *                                the encoding is ambiguous with another
 *                                encoding in the same socket code table.
 * @exception OutOfRange If the given index width is negative.
 */
RFPortCode::RFPortCode(
    const std::string& regFile,
    int indexWidth,
    SocketCodeTable& parent)
    throw (ObjectAlreadyExists, OutOfRange) :
    PortCode(regFile, indexWidth) {

    setParent(NULL);
    parent.addRFPortCode(*this);
    setParent(&parent);
}


/**
 * The constructor.
 *
 * Loads the state of the object from the given ObjectState tree.
 *
 * @param state The ObjectState tree.
 * @param parent The parent encoding map.
 * @exception ObjectStateLoadingException If an error occurs while loading
 *                                        the state.
 * @exception ObjectAlreadyExists If the socket code table has an encoding
 *                                defined for the same port already, or if
 *                                the encoding is ambiguous with another
 *                                encoding in the same socket code table.
 */
RFPortCode::RFPortCode(const ObjectState* state, SocketCodeTable& parent)
    throw (ObjectStateLoadingException, ObjectAlreadyExists) :
    PortCode(state) {

    if (state->name() != OSNAME_RF_PORT_CODE) {
	const string procName = "RFPortCode::RFPortCode";
	throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }

    setParent(NULL);
    parent.addRFPortCode(*this);
    setParent(&parent);
}


/**
 * The destructor.
 */
RFPortCode::~RFPortCode() {
    SocketCodeTable* parent = this->parent();
    setParent(NULL);
    parent->removeRFPortCode(*this);
}


/**
 * Saves the state of the object to an ObjectState tree.
 *
 * @return The newly created ObjectState tree.
 */
ObjectState*
RFPortCode::saveState() const {
    ObjectState* state = PortCode::saveState();
    state->setName(OSNAME_RF_PORT_CODE);
    return state;
}
