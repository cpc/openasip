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
 * @file RFPortCode.cc
 *
 * Implementation of RFPortCode class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
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
