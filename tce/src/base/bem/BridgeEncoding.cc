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
 * @file BridgeEncoding.cc
 *
 * Implementation of BridgeEncoding class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include <string>

#include "BridgeEncoding.hh"
#include "SourceField.hh"
#include "Application.hh"

using std::string;

const std::string BridgeEncoding::OSNAME_BRIDGE_ENCODING = "bridge_encoding";
const std::string BridgeEncoding::OSKEY_BRIDGE_NAME = "bridge_name";


/**
 * The constructor.
 *
 * Creates an encoding for the given bridge and registers it into the given
 * source field.
 *
 * @param name Name of the bridge.
 * @param encoding The encoding.
 * @param extraBits The number of extra (zero) bits in the encoding.
 * @param parent The parent source field.
 * @exception ObjectAlreadyExists If the parent source field has an encoding
 *                                for this bridge already or if the encoding
 *                                is ambiguous with some other encoding.
 */
BridgeEncoding::BridgeEncoding(
    const std::string& name,
    unsigned int encoding,
    unsigned int extraBits,
    SourceField& parent)
    throw (ObjectAlreadyExists) :
    Encoding(encoding, extraBits, NULL), bridge_(name) {

    parent.addBridgeEncoding(*this);
    setParent(&parent);
}


/**
 * The constructor.
 *
 * Loads the state of the object from the given ObjectState instance.
 *
 * @param state The ObjectState instance.
 * @param parent The parent source field.
 * @exception ObjectStateLoadingException If an error occurs while loading
 *                                        the state.
 * @exception ObjectAlreadyExists If the parent source field has an encoding
 *                                for this bridge already or if the encoding
 *                                is ambiguous with some other encoding.
 */
BridgeEncoding::BridgeEncoding(const ObjectState* state, SourceField& parent)
    throw (ObjectStateLoadingException, ObjectAlreadyExists) :
    Encoding(state, NULL), bridge_("") {

    const string procName = "BridgeEncoding::BridgeEncoding";

    if (state->name() != OSNAME_BRIDGE_ENCODING) {
	throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }

    try {
	bridge_ = state->stringAttribute(OSKEY_BRIDGE_NAME);
    } catch (const Exception& exception) {
	throw ObjectStateLoadingException(
	    __FILE__, __LINE__, procName, exception.errorMessage());
    }

    parent.addBridgeEncoding(*this);
    setParent(&parent);
}


/**
 * The destructor.
 */
BridgeEncoding::~BridgeEncoding() {
    SourceField* parent = this->parent();
    setParent(NULL);
    parent->removeBridgeEncoding(*this);
}


/**
 * Returns the parent source field.
 *
 * @return The parent source field.
 */
SourceField*
BridgeEncoding::parent() const {
    InstructionField* parent = Encoding::parent();
    if (parent != NULL) {
        SourceField* sField = dynamic_cast<SourceField*>(parent);
        assert(sField != NULL);
        return sField;
    } else {
        return NULL;
    }
}


/**
 * Returns the name of the bridge.
 *
 * @return The name of the bridge.
 */
std::string
BridgeEncoding::bridgeName() const {
    return bridge_;
}


/**
 * Returns the position of the bridge encoding within the source field.
 *
 * @return The position.
 */
int
BridgeEncoding::bitPosition() const {
    if (parent()->componentIDPosition() == BinaryEncoding::RIGHT) {
        return 0;
    } else {
        return parent()->width() - parent()->extraBits() - width();
    }
}

    
/**
 * Saves the state of the object to an ObjectState instance.
 *
 * @return The newly created ObjectState instance.
 */
ObjectState*
BridgeEncoding::saveState() const {
    ObjectState* state = Encoding::saveState();
    state->setName(OSNAME_BRIDGE_ENCODING);
    state->setAttribute(OSKEY_BRIDGE_NAME, bridgeName());
    return state;
}
