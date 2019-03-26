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
 * @file BridgeEncoding.cc
 *
 * Implementation of BridgeEncoding class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>

#include "BridgeEncoding.hh"
#include "SourceField.hh"
#include "Application.hh"
#include "ObjectState.hh"

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
