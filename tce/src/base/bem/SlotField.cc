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
 * @file SlotField.cc
 *
 * Implementation of SlotField class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include "SlotField.hh"
#include "SocketEncoding.hh"
#include "NullSocketEncoding.hh"
#include "NOPEncoding.hh"
#include "NullNOPEncoding.hh"
#include "MoveSlot.hh"
#include "GuardField.hh"
#include "SourceField.hh"
#include "DestinationField.hh"
#include "ContainerTools.hh"
#include "Application.hh"
#include "BEMTester.hh"
#include "SequenceTools.hh"
#include "ObjectState.hh"

using std::string;

const std::string SlotField::OSNAME_SLOT_FIELD = "slot_field";
const std::string SlotField::OSKEY_COMPONENT_ID_POSITION = "comp_id_pos";

/**
 * The constructor.
 *
 * @param componentIDPos Position of the socket (or bridge) ID within the
 *                       source or destination field.
 * @param parent The parent move slot.
 */
SlotField::SlotField(
    BinaryEncoding::Position componentIDPos,
    MoveSlot& parent) :
    InstructionField(&parent), nopEncoding_(NULL), 
    componentIDPos_(componentIDPos) {
}


/**
 * The constructor.
 *
 * Loads the state of the object from the given ObjectState tree.
 *
 * @param state The ObjectState tree.
 * @param parent The parent move slot.
 * @exception ObjectStateLoadingException If an error occurs while loading
 *                                        the state.
 */
SlotField::SlotField(const ObjectState* state, MoveSlot& parent)
    throw (ObjectStateLoadingException) :
    InstructionField(state, &parent), nopEncoding_(NULL), 
    componentIDPos_(BinaryEncoding::LEFT) {

    loadState(state);
}


/**
 * The destructor.
 */
SlotField::~SlotField() {
    clearSocketEncodings();
    clearNoOperationEncoding();
}


/**
 * Returns the parent move slot.
 */
MoveSlot*
SlotField::parent() const {
    InstructionField* parent = InstructionField::parent();
    if (parent == NULL) {
	return NULL;
    } else {
	MoveSlot* slot = dynamic_cast<MoveSlot*>(parent);
	assert(slot != NULL);
	return slot;
    }
}


/**
 * Adds the given socket encoding.
 *
 * This method is to be called from the constructor of SocketEncoding.
 *
 * @param encoding The socket encoding to be added.
 * @exception ObjectAlreadyExists If the field already has an encoding for
 *                                the same socket or if the encoding is
 *                                already assigned to another socket.
 */
void
SlotField::addSocketEncoding(SocketEncoding& encoding)
    throw (ObjectAlreadyExists) {

    // verify that this is called from SocketEncoding constructor
    assert(encoding.parent() == NULL);

    if (hasSocketEncoding(encoding.socketName()) ||
	!BEMTester::canAddComponentEncoding(
	    *this, encoding.encoding(), encoding.extraBits())) {
	const string procName = "SlotField::addSocketEncoding";
	throw ObjectAlreadyExists(__FILE__, __LINE__, procName);
    }

    encodings_.push_back(&encoding);
}


/**
 * Removes the given socket encoding.
 *
 * This method is to be called from SocketEncoding destructor.
 *
 * @param encoding The socket encoding to be removed.
 */
void
SlotField::removeSocketEncoding(SocketEncoding& encoding) {
    // verify that this is called from SocketEncoding destructor
    assert(encoding.parent() == NULL);
    assert(ContainerTools::removeValueIfExists(encodings_, &encoding));
}


/**
 * Returns the number of sockets that are encoded in this field.
 *
 * @return The number of sockets.
 */
int
SlotField::socketEncodingCount() const {
    return encodings_.size();
}


/**
 * Returns the socket encoding stored at the given position.
 *
 * @param index The position.
 * @return The socket encoding.
 * @exception OutOfRange If the given index is negative or not smaller than
 *                       the number of sockets encoded in the field.
 */
SocketEncoding&
SlotField::socketEncoding(int index) const
    throw (OutOfRange) {

    if (index < 0 || index >= socketEncodingCount()) {
	const string procName = "SlotField::socketEncoding";
	throw OutOfRange(__FILE__, __LINE__, procName);
    }

    return *encodings_[index];
}


/**
 * Tells whether the slot field has an encoding for the socket with the given
 * name.
 *
 * @param socket Name of the socket.
 * @return True if the slot field has an encoding for the given socket,
 *         otherwise false.
 */
bool
SlotField::hasSocketEncoding(const std::string& socket) const {
    for (SocketEncodingTable::const_iterator iter = encodings_.begin();
	 iter != encodings_.end(); iter++) {
	SocketEncoding* encoding = *iter;
	if (encoding->socketName() == socket) {
	    return true;
	}
    }
    return false;
}


/**
 * Returns the socket encoding of the socket with the given name.
 *
 * Returns a NullSocketEncoding instance if this field does not encode the
 * socket.
 *
 * @param socket Name of the socket.
 * @return The socket encoding of the given socket.
 */
SocketEncoding&
SlotField::socketEncoding(const std::string& socket) const {
    for (SocketEncodingTable::const_iterator iter = encodings_.begin();
	 iter != encodings_.end(); iter++) {
	SocketEncoding* encoding = *iter;
	if (encoding->socketName() == socket) {
	    return **iter;
	}
    }

    return NullSocketEncoding::instance();
}


/**
 * Sets the given encoding for no operation.
 *
 * This method is to be called from the constructor of NOPEncoding.
 *
 * @param encoding The encoding to be set.
 * @exception ObjectAlreadyExists If the slot field has a NOP
 *                                encoding already or if the given encoding
 *                                is ambiguous with some other encoding.
 */
void
SlotField::setNoOperationEncoding(NOPEncoding& encoding)
    throw (ObjectAlreadyExists) {

    assert(encoding.parent() == NULL);

    if (hasNoOperationEncoding() ||
        !BEMTester::canAddComponentEncoding(
            *this, encoding.encoding(), encoding.extraBits())) {
        const string procName = "SlotField::setNoOperationEncoding";
        throw ObjectAlreadyExists(__FILE__, __LINE__, procName);
    }

    nopEncoding_ = &encoding;
}


/**
 * Unsets the NOP encoding.
 *
 * This method is to be called from the destructor of NOPEncoding.
 */
void
SlotField::unsetNoOperationEncoding() {
    assert(hasNoOperationEncoding());
    assert(noOperationEncoding().parent() == NULL);
    nopEncoding_ = NULL;
}


/**
 * Tells whether the slot field has a NOP encoding.
 *
 * @return True if the slot field has a NOP encoding, otherwise
 *         false.
 */
bool
SlotField::hasNoOperationEncoding() const {
    return nopEncoding_ != NULL;
}


/**
 * Returns the NOP encoding.
 *
 * Returns NullNOPEncoding instance if the slot field does not have
 * a NOP encoding.
 *
 * @return The NOP encoding.
 */
NOPEncoding&
SlotField::noOperationEncoding() const {
    if (hasNoOperationEncoding()) {
        return *nopEncoding_;
    } else {
        return NullNOPEncoding::instance();
    }
}


/**
 * Returns the position of the component ID within the slot field.
 *
 * @return Position of the component ID.
 */
BinaryEncoding::Position
SlotField::componentIDPosition() const {
    return componentIDPos_;
}


/**
 * Returns the bit width required by the socket encodings.
 *
 * @return The bit width.
 */
int
SlotField::width() const {

    int width(0);
    int encodings = socketEncodingCount();
    for (int i = 0; i < encodings; i++) {
	SocketEncoding& encoding = socketEncoding(i);
	int encodingWidth = encoding.width();
	if (encodingWidth > width) {
	    width = encodingWidth;
	}
    }

    if (hasNoOperationEncoding() && noOperationEncoding().width() > width) {
        width = noOperationEncoding().width();
    }

    return width + extraBits();
}


/**
 * Always returns 0 because slot fields do not have any child fields.
 *
 * @return 0.
 */
int
SlotField::childFieldCount() const {
    return 0;
}


/**
 * Always throws OutOfRange because slot fields do not have any child fields.
 *
 * @return Never returns.
 * @exception OutOfRange Always throws.
 */
InstructionField&
SlotField::childField(int) const
    throw (OutOfRange) {

    const string procName = "SlotField::childField";
    throw OutOfRange(__FILE__, __LINE__, procName);
}


/**
 * Loads the state of the object from the given ObjectState tree.
 *
 * @param state The ObjectState tree.
 * @exception ObjectStateLoadingException If an error occurs while loading
 *                                        the state.
 */
void
SlotField::loadState(const ObjectState* state)
    throw (ObjectStateLoadingException) {

    clearSocketEncodings();
    clearNoOperationEncoding();
    InstructionField::loadState(state);

    const string procName = "SlotField::loadState";

    try {
	componentIDPos_ = static_cast<BinaryEncoding::Position>(
	    state->intAttribute(OSKEY_COMPONENT_ID_POSITION));
	for (int i = 0; i < state->childCount(); i++) {
	    ObjectState* child = state->child(i);
	    if (child->name() == SocketEncoding::OSNAME_SOCKET_ENCODING) {
		new SocketEncoding(child, *this);
            } else if (child->name() == NOPEncoding::OSNAME_NOP_ENCODING) {
                new NOPEncoding(child, *this);
	    }
	}
    } catch (const Exception& exception) {
	throw ObjectStateLoadingException(
	    __FILE__, __LINE__, procName, exception.errorMessage());
    }
}


/**
 * Saves the state of the object to an ObjectState tree.
 *
 * @return The newly created ObjectState tree.
 */
ObjectState*
SlotField::saveState() const {

    ObjectState* state = InstructionField::saveState();
    state->setName(OSNAME_SLOT_FIELD);
    state->setAttribute(OSKEY_COMPONENT_ID_POSITION, componentIDPosition());

    // add socket encodings
    for (int i = 0; i < socketEncodingCount(); i++) {
	SocketEncoding& enc = socketEncoding(i);
	state->addChild(enc.saveState());
    }

    // add NOP encoding
    if (hasNoOperationEncoding()) {
        state->addChild(noOperationEncoding().saveState());
    }

    return state;
}


/**
 * Clears all the socket encodings from the slot field.
 */
void
SlotField::clearSocketEncodings() {
    SequenceTools::deleteAllItems(encodings_);
}


/**
 * Deletes the NOP encoding if one exists.
 */
void
SlotField::clearNoOperationEncoding() {
    if (hasNoOperationEncoding()) {
        delete nopEncoding_;
    }
}
