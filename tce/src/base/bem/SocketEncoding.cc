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
 * @file SocketEncoding.cc
 *
 * Implementation of SocketEncoding class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>

#include "SocketEncoding.hh"
#include "SlotField.hh"
#include "SocketCodeTable.hh"
#include "MoveSlot.hh"
#include "NullSocketCodeTable.hh"
#include "Application.hh"
#include "MathTools.hh"

using std::string;

const std::string SocketEncoding::OSNAME_SOCKET_ENCODING = "socket_encoding";
const std::string SocketEncoding::OSKEY_SOCKET_NAME = "socket_name";
const std::string SocketEncoding::OSKEY_SC_TABLE = "sc_table";

/**
 * The constructor.
 *
 * Creates a socket encoding and registers it into the given slot field. The
 * socket is identified by name. The (fixed) part of the socket encoding and
 * the parent slot field are given as parameters.
 *
 * @param name Name of the socket.
 * @param encoding Fixed part of the encoding.
 * @param extraBits The number of extra (zero) bits in the encoding.
 * @param parent The parent slot field.
 * @exception ObjectAlreadyExists If the parent slot field already has an
 *                                encoding for the same socket, or if the
 *                                given encoding is ambiguous with another
 *                                encoding in the parent slot field.
 */
SocketEncoding::SocketEncoding(
    const std::string& name,
    unsigned int encoding,
    unsigned int extraBits,
    SlotField& parent)
    throw (ObjectAlreadyExists) :
    Encoding(encoding, extraBits, NULL), name_(name), socketCodes_(NULL) {

    parent.addSocketEncoding(*this);
    setParent(&parent);
}


/**
 * The constructor.
 *
 * Loads the state of the object from the given ObjectState instance.
 *
 * @param state The ObjectState instance.
 * @param parent The parent slot field.
 * @exception ObjectStateLoadingException If an error occurs while loading
 *                                        the state.
 * @exception ObjectAlreadyExists If the parent slot field already has an
 *                                encoding for the same socket, or if the
 *                                given encoding is ambiguous with another
 *                                encoding in the parent slot field.
 */
SocketEncoding::SocketEncoding(const ObjectState* state, SlotField& parent)
    throw (ObjectStateLoadingException, ObjectAlreadyExists) :
    Encoding(state, &parent), name_(""), socketCodes_(NULL) {

    const string procName = "SocketEncoding::SocketEncoding";

    try {
	name_ = state->stringAttribute(OSKEY_SOCKET_NAME);
	if (state->hasAttribute(OSKEY_SC_TABLE)) {
	    string tableName = state->stringAttribute(OSKEY_SC_TABLE);
	    BinaryEncoding* bem = parent.parent()->parent();
	    if (&bem->socketCodeTable(tableName) ==
		&NullSocketCodeTable::instance()) {
		throw ObjectStateLoadingException(
		    __FILE__, __LINE__, procName);
	    } else {
		setSocketCodes(bem->socketCodeTable(tableName));
	    }
	}

    } catch (const Exception& exception) {
	throw ObjectStateLoadingException(
	    __FILE__, __LINE__, procName, exception.errorMessage());
    }

    setParent(NULL);
    parent.addSocketEncoding(*this);
    setParent(&parent);
}


/**
 * The destructor.
 */
SocketEncoding::~SocketEncoding() {
    SlotField* parent = this->parent();
    setParent(NULL);
    parent->removeSocketEncoding(*this);
}


/**
 * Returns the parent slot field.
 *
 * @return The parent slot field.
 */
SlotField*
SocketEncoding::parent() const {
    InstructionField* parent = Encoding::parent();
    if (parent != NULL) {
        SlotField* sField = dynamic_cast<SlotField*>(parent);
        assert(sField != NULL);
        return sField;
    } else {
        return NULL;
    }
}


/**
 * Returns the name of the socket.
 *
 * @return The name of the socket.
 */
std::string
SocketEncoding::socketName() const {
    return name_;
}


/**
 * Attaches a set of socket codes to the socket encoding.
 *
 * @param codeTable The socket code table to be attached.
 */
void
SocketEncoding::setSocketCodes(SocketCodeTable& codeTable) {
    socketCodes_ = &codeTable;
}


/**
 * Removes the socket codes from the socket encoding.
 */
void
SocketEncoding::unsetSocketCodes() {
    socketCodes_ = NULL;
}


/**
 * Tells whether the socket encoding contains a socket code table.
 *
 * @return True if the socket encoding contains a socket code table, otherwise
 *         false.
 */
bool
SocketEncoding::hasSocketCodes() const {
    return socketCodes_ != NULL;
}


/**
 * Returns the socket code table.
 *
 * Returns a NullSocketCodeTable instance if the socket does not have a socket
 * code table set.
 *
 * @return The socket code table.
 */
SocketCodeTable&
SocketEncoding::socketCodes() const {
    if (!hasSocketCodes()) {
	return NullSocketCodeTable::instance();
    } else {
	return *socketCodes_;
    }
}


/**
 * Returns the position of the socket ID within the slot field.
 *
 * The position is always 0 if socket ID is in the right end of the
 * field. However, if socket code is in the right end of the field,
 * the position of the socket ID depends on the bit width of the
 * socket code.
 *
 * @return The position of the socket ID.
 */
int
SocketEncoding::socketIDPosition() const {
    if (parent()->componentIDPosition() == BinaryEncoding::RIGHT) {
        return 0;
    } else {
        return parent()->width() - parent()->extraBits() - socketIDWidth();
    }
}


/**
 * Returns the bit width of the fixed part of the socket encoding.
 *
 * @return The bit width.
 */
int
SocketEncoding::socketIDWidth() const {
    return MathTools::requiredBits(encoding()) + extraBits();
}


/**
 * Returns the number of bits required to encode all the destinations of this
 * socket.
 *
 * @return The number of bits.
 */
int
SocketEncoding::width() const {
    int width = socketIDWidth();
    if (hasSocketCodes()) {
	width += socketCodes().width();
    }
    return width;
}


/**
 * Returns always 0.
 *
 * @return The position.
 */
int
SocketEncoding::bitPosition() const {
    return 0;
}


/**
 * Saves the state of the socket encoding to an ObjectState instance.
 *
 * @return The newly created ObjectState instance.
 */
ObjectState*
SocketEncoding::saveState() const {
    ObjectState* state = Encoding::saveState();
    state->setName(OSNAME_SOCKET_ENCODING);
    state->setAttribute(OSKEY_SOCKET_NAME, socketName());
    if (hasSocketCodes()) {
	state->setAttribute(OSKEY_SC_TABLE, socketCodes().name());
    }
    return state;
}
