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
