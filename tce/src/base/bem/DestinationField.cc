/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file DestinationField.cc
 *
 * Implementation of DestinationField class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>

#include "DestinationField.hh"
#include "MoveSlot.hh"
#include "Application.hh"
#include "ObjectState.hh"

using std::string;

const std::string DestinationField::OSNAME_DESTINATION_FIELD = "dest_field";

/**
 * The constructor.
 *
 * Registers the destination field to the given move slot automatically.
 *
 * @param socketIDPos Position of the socket ID within the destination field.
 * @param parent The parent move slot.
 * @exception ObjectAlreadyExists If the parent move slot already has a
 *                                destination field.
 * @exception IllegalParameters If the given socket ID position is not the same
 *                              with other destination fields.
 */
DestinationField::DestinationField(
    BinaryEncoding::Position socketIDPos,
    MoveSlot& parent)
throw (ObjectAlreadyExists, IllegalParameters) :
SlotField(socketIDPos, parent) {

    BinaryEncoding* bem = parent.parent();
    for (int i = 0; i < bem->moveSlotCount(); i++) {
	MoveSlot& slot = bem->moveSlot(i);
	if (slot.hasDestinationField() &&
	    slot.destinationField().componentIDPosition() != socketIDPos) {
	    const string procName = "SourceField::SourceField";
	    throw IllegalParameters(__FILE__, __LINE__, procName);
	} else {
	    break;
	}
    }

    setParent(NULL);
    parent.setDestinationField(*this);
    setParent(&parent);
}


/**
 * The constructor.
 *
 * Loads the state of the object from the given ObjectState tree.
 *
 * @param state The ObjectState tree.
 * @param parent The parent move slot.
 * @exception ObjectStateLoadingException If an error occurs while loading the
 *                                        state.
 * @exception ObjectAlreadyExists If the parent move slot already has a
 *                                destination field.
 */
DestinationField::DestinationField(const ObjectState* state, MoveSlot& parent)
throw (ObjectStateLoadingException, ObjectAlreadyExists) :
SlotField(state, parent) {

    if (state->name() != OSNAME_DESTINATION_FIELD) {
	const string procName = "DestinationField::DestinationField";
	throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }

    setParent(NULL);
    parent.setDestinationField(*this);
    setParent(&parent);
}


/**
 * The destructor.
 */
DestinationField::~DestinationField() {
    MoveSlot* parent = this->parent();
    assert(parent != NULL);
    setParent(NULL);
    parent->unsetDestinationField();
}


/**
 * Saves the state of the object to an ObjectState tree.
 *
 * @return The newly created ObjectState tree.
 */
ObjectState*
DestinationField::saveState() const {
    ObjectState* state = SlotField::saveState();
    state->setName(OSNAME_DESTINATION_FIELD);
    return state;
}
