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
 * @file BinaryEncoding.cc
 *
 * Implementation of BinaryEncoding class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>

#include "BinaryEncoding.hh"
#include "MoveSlot.hh"
#include "ImmediateSlotField.hh"
#include "LImmDstRegisterField.hh"
#include "NullImmediateControlField.hh"
#include "NullInstructionField.hh"
#include "SocketCodeTable.hh"
#include "NullSocketCodeTable.hh"
#include "ContainerTools.hh"
#include "SequenceTools.hh"
#include "Application.hh"
#include "ObjectState.hh"

using std::string;

const std::string BinaryEncoding::OSNAME_BEM = "bem";

/**
 * The constructor.
 *
 * Creates an empty BEM.
 */
BinaryEncoding::BinaryEncoding() :
    InstructionField(NULL), immediateField_(NULL) {
}


/**
 * The constructor.
 *
 * Loads the state of the object from the given ObjectState tree.
 *
 * @exception ObjectStateLoadingException If an error occurs while loading
 *                                        the state.
 */
BinaryEncoding::BinaryEncoding(const ObjectState* state)
    : InstructionField(NULL), immediateField_(NULL) {
    loadState(state);
}

/**
 * The destructor.
 */
BinaryEncoding::~BinaryEncoding() {
    if (hasImmediateControlField()) {
	delete immediateField_;
    }
    deleteMoveSlots();
    deleteImmediateSlots();
    deleteLongImmDstRegisterFields();
    deleteSocketCodes();
}


/**
 * Returns the number of move slots in the instruction.
 *
 * @return The number of move slots.
 */
int
BinaryEncoding::moveSlotCount() const {
    return moveSlots_.size();
}


/**
 * Returns the move slot at given index.
 *
 * The index does not have to reflect the actual order within the TTA
 * instruction word.
 *
 * @param index The index.
 * @return The move slot at given index.
 * @exception OutOfRange If the index is negative or is not smaller than the
 *                       number of move slots of the TTA instruction word.
 */
MoveSlot&
BinaryEncoding::moveSlot(int index) const {
    if (index < 0 || index >= moveSlotCount()) {
	const string procName = "BinaryEncoding::moveSlot";
	throw OutOfRange(__FILE__, __LINE__, procName);
    }

    return *moveSlots_[index];
}

/**
 * Tells whether the encoding map contains a move slot with the given (bus)
 * name.
 *
 * @param name The bus name.
 * @return True if there is a move slot with the given name, otherwise false.
 */
bool
BinaryEncoding::hasMoveSlot(const std::string& name) const {

    for (MoveSlotContainer::const_iterator iter = moveSlots_.begin();
	 iter != moveSlots_.end(); iter++) {

	MoveSlot* slot = *iter;
	if (slot->name() == name) {
	    return true;
	}
    }

    return false;
}


/**
 * Returns the move slot that programs the bus identified by the given name.
 *
 * @param The bus name.
 * @return The move slot.
 * @exception InstanceNotFound If the encoding map does not contain a move
 *                             slot with the given name.
 */
MoveSlot&
BinaryEncoding::moveSlot(const std::string& name) const {
    for (MoveSlotContainer::const_iterator iter = moveSlots_.begin();
	 iter != moveSlots_.end(); iter++) {

	MoveSlot* slot = *iter;
	if (slot->name() == name) {
	    return *slot;
	}
    }

    const string procName = "BinaryEncoding::moveSlot";
    throw InstanceNotFound(__FILE__, __LINE__, procName);
}

/**
 * Adds the given move slot to the encoding map.
 *
 * This method is to be called from the constructor of MoveSlot.
 *
 * @param slot The move slot to be added.
 * @exception ObjectAlreadyExists If the encoding map contains a slot with
 *                                the same name already.
 */
void
BinaryEncoding::addMoveSlot(MoveSlot& slot) {
    // verify that this is called from MoveSlot constructor
    assert(slot.parent() == NULL);

    if (hasMoveSlot(slot.name())) {
	const string procName = "BinaryEncoding::addMoveSlot";
	throw ObjectAlreadyExists(__FILE__, __LINE__, procName);
    }

    moveSlots_.push_back(&slot);
}

/**
 * Removes the given move slot from the encoding map.
 *
 * This method is to be called from the destructor of MoveSlot.
 *
 * @param slot The move slot to be removed.
 */
void
BinaryEncoding::removeMoveSlot(MoveSlot& slot) {
    // verify that this is called from MoveSlot destructor.
    assert(slot.parent() == NULL);
    assert(ContainerTools::removeValueIfExists(moveSlots_, &slot));
}


/**
 * Returns the number of immediate slots in the instruction.
 *
 * @return The number of immediate slots.
 */
int
BinaryEncoding::immediateSlotCount() const {
    return immediateSlots_.size();
}


/**
 * Returns the immediate slot at given index.
 *
 * The index does not have to reflect the actual order within the TTA
 * instruction word.
 *
 * @param index The index.
 * @return The immediate slot at given index.
 * @exception OutOfRange If the index is negative or is not smaller than the
 *                       number of immediate slots in the TTA instruction
 *                       word.
 */
ImmediateSlotField&
BinaryEncoding::immediateSlot(int index) const {
    if (index < 0 || index >= immediateSlotCount()) {
	const string procName = "BinaryEncoding::immediateSlot";
	throw OutOfRange(__FILE__, __LINE__, procName);
    }

    return *immediateSlots_[index];
}

/**
 * Tells whether the encoding map contains an immediate slot with the given
 * name.
 *
 * @param name The name of the immediate slot.
 * @return True if there is an immediate slot with the given name, otherwise
 *         false.
 */
bool
BinaryEncoding::hasImmediateSlot(const std::string& name) const {

    for (ImmediateSlotContainer::const_iterator iter = 
             immediateSlots_.begin(); iter != immediateSlots_.end(); 
         iter++) {

	ImmediateSlotField* slot = *iter;
	if (slot->name() == name) {
	    return true;
	}
    }

    return false;
}


/**
 * Returns the immediate slot of the given name.
 *
 * @param name The name of the immediate slot.
 * @return The immediate slot.
 * @exception InstanceNotFound If the encoding map does not contain an
 *                             immediate slot with the given name.
 */
ImmediateSlotField&
BinaryEncoding::immediateSlot(const std::string& name) const {
    for (ImmediateSlotContainer::const_iterator iter = 
             immediateSlots_.begin(); iter != immediateSlots_.end(); 
         iter++) {

	ImmediateSlotField* slot = *iter;
	if (slot->name() == name) {
	    return *slot;
	}
    }

    const string procName = "BinaryEncoding::immediateSlot";
    throw InstanceNotFound(__FILE__, __LINE__, procName);
}

/**
 * Adds the given immediate slot to the encoding map.
 *
 * This method is to be called from the constructor of ImmediateSlotField.
 *
 * @param slot The immediate slot to be added.
 * @exception ObjectAlreadyExists If the encoding map contains an immediate
 *                                slot with the same name already.
 */
void
BinaryEncoding::addImmediateSlot(ImmediateSlotField& slot) {
    // verify that this is called from MoveSlot constructor
    assert(slot.parent() == NULL);

    if (hasImmediateSlot(slot.name())) {
	const string procName = "BinaryEncoding::addImmediateSlot";
	throw ObjectAlreadyExists(__FILE__, __LINE__, procName);
    }

    immediateSlots_.push_back(&slot);
}

/**
 * Removes the given immediate slot from the encoding map.
 *
 * This method is to be called from the destructor of ImmediateSlotField.
 *
 * @param slot The immediate slot to be removed.
 */
void
BinaryEncoding::removeImmediateSlot(ImmediateSlotField& slot) {
    // verify that this is called from ImmediateSlotField destructor.
    assert(slot.parent() == NULL);
    assert(ContainerTools::removeValueIfExists(immediateSlots_, &slot));
}


/**
 * Tells whether the instruction word has an immediate control field.
 *
 * @return True if the instruction word has an immediate control field,
 *         otherwise false.
 */
bool
BinaryEncoding::hasImmediateControlField() const {
    return immediateField_ != NULL;
}


/**
 * Returns the immediate control field of the instruction word, if one exists.
 *
 * Returns a NullImmediateControlField instance otherwise (a single-template
 * instruction).
 *
 * @return The immediate control field.
 */
ImmediateControlField&
BinaryEncoding::immediateControlField() const {
    if (hasImmediateControlField()) {
	return *immediateField_;
    } else {
	return NullImmediateControlField::instance();
    }
}


/**
 * Adds the given immediate control field to the instruction word.
 *
 * This method is to be called from the constructor of ImmediateControlField.
 *
 * @param field The immediate control field to be added.
 * @exception ObjectAlreadyExists If the instruction word already has an
 *                                immediate control field.
 */
void
BinaryEncoding::setImmediateControlField(ImmediateControlField& field) {
    // verify that this is called from ImmediateControlField constructor
    assert(field.parent() == NULL);

    if (hasImmediateControlField()) {
	const string procName = "BinaryEncoding::setImmediateControlField";
	throw ObjectAlreadyExists(__FILE__, __LINE__, procName);
    }

    immediateField_ = &field;
}

/**
 * Removes the immediate control field.
 *
 * This method is to be called from the destructor of ImmediateControlField.
 */
void
BinaryEncoding::unsetImmediateControlField() {
    // verify that this is called from ImmediateControlField destructor
    assert(hasImmediateControlField());
    assert(immediateControlField().parent() == NULL);
    immediateField_ = NULL;
}


/**
 * Returns the number of long immediate destination register fields in the
 * instruction.
 *
 * @return The number of fields.
 */
int
BinaryEncoding::longImmDstRegisterFieldCount() const {
    return longImmDstRegFields_.size();
}


/**
 * Returns a long immediate destination register field by the given index.
 *
 * @param index The index.
 * @return The field.
 * @exception OutOfRange If the index is negative or not smaller than the
 *                       number of long immediate destination register
 *                       fields.
 */
LImmDstRegisterField&
BinaryEncoding::longImmDstRegisterField(int index) const {
    if (index < 0 || index >= longImmDstRegisterFieldCount()) {
        throw OutOfRange(__FILE__, __LINE__, __func__);
    }

    return *longImmDstRegFields_[index];
}

/**
 * Returns the long immediate destination register field that gives the
 * destination register of the given immediate unit in the given instruction
 * template.
 *
 * @param iTemp Name of the instruction template.
 * @param dstUnit Name of the immediate unit.
 * @return The long immediate destination register field.
 * @exception InstanceNotFound If there is no such field.
 */
LImmDstRegisterField&
BinaryEncoding::longImmDstRegisterField(
    const std::string& iTemp, const std::string& dstUnit) const {
    int fields = longImmDstRegisterFieldCount();
    for (int i = 0; i < fields; i++) {
        LImmDstRegisterField& field = longImmDstRegisterField(i);
        if (field.usedByInstructionTemplate(iTemp)) {
            if (field.immediateUnit(iTemp) == dstUnit) {
                return field;
            }
        }
    }

    throw InstanceNotFound(__FILE__, __LINE__, __func__);
}

/**
 * Adds the given long immediate destination register field to the
 * instruction format.
 *
 * This method is to be called from the constructor of LImmDstRegisterField
 * only!
 *
 * @param field The field to add.
 */
void
BinaryEncoding::addLongImmDstRegisterField(LImmDstRegisterField& field) {
    assert(field.parent() == NULL);
    longImmDstRegFields_.push_back(&field);
}


/**
 * Removes the given long immediate destination register field from the
 * instruction format.
 *
 * This method is to be called from the destructor of LImmDstRegisterField
 * only!
 *
 * @param field The field to remove.
 */
void
BinaryEncoding::removeLongImmDstRegisterField(LImmDstRegisterField& field) {
    assert(field.parent() == NULL);
    assert(
        ContainerTools::removeValueIfExists(longImmDstRegFields_, &field));
}
    

/**
 * Returns the number of socket code tables contained by the encoding map.
 *
 * @return The number of socket code tables.
 */
int
BinaryEncoding::socketCodeTableCount() const {
    return socketCodes_.size();
}


/**
 * Returns the socket code table at the given index.
 *
 * @param index The index.
 * @exception OutOfRange If the given index is negative or not smaller than
 *                       the number of socket code tables in the encoding
 *                       map.
 */
SocketCodeTable&
BinaryEncoding::socketCodeTable(int index) const {
    if (index < 0 || index >= socketCodeTableCount()) {
	const string procName = "BinaryEncoding::socketCodeTable";
	throw OutOfRange(__FILE__, __LINE__, procName);
    }

    return *socketCodes_[index];
}

/**
 * Returns the socket code table which has the given name.
 *
 * Returns a NullSocketCodeTable instance if there is no such table.
 *
 * @param name Name of the table.
 * @return The socket code table.
 */
SocketCodeTable&
BinaryEncoding::socketCodeTable(const std::string& name) const {

    for (int i = 0; i < socketCodeTableCount(); i++) {
	SocketCodeTable& table = socketCodeTable(i);
	if (table.name() == name) {
	    return table;
	}
    }

    return NullSocketCodeTable::instance();
}


/**
 * Adds the given socket code table to the encoding map.
 *
 * This method is to be called from the constructor of SocketCodeTable.
 *
 * @param table The socket code table to be added.
 * @exception ObjectAlreadyExists If the encoding map already contains a
 *                                socket code table with the same name as
 *                                the given table.
 */
void
BinaryEncoding::addSocketCodeTable(SocketCodeTable& table) {
    // verify that this is called from SocketCodeTable constructor
    assert(table.parent() == NULL);

    if (hasSocketCodeTable(table.name())) {
	const string procName = "BinaryEncoding::addSocketCodeTable";
	throw ObjectAlreadyExists(__FILE__, __LINE__, procName);
    }

    socketCodes_.push_back(&table);
}

/**
 * Removes the given socket code table from the encoding map.
 *
 * This method is to be called from the destructor of SocketCodeTable.
 *
 * @param table The socket code table to be removed.
 */
void
BinaryEncoding::removeSocketCodeTable(SocketCodeTable& table) {
    assert(table.parent() == NULL);
    assert(ContainerTools::removeValueIfExists(socketCodes_, &table));
}


/**
 * Returns the number of immediate child fields
 * (move slots + immediate slots + immediate control field).
 *
 * @return The number of child fields.
 */
int
BinaryEncoding::childFieldCount() const {
    int count = moveSlotCount() + immediateSlotCount() + 
        longImmDstRegisterFieldCount();
    if (hasImmediateControlField()) {
	count++;
    }
    return count;
}


/**
 * Returns the child instruction field at the given relative position.
 *
 * Returns a NullInstructionField instance if there is no child field at
 * the given position. This is, however, not possible if the object model is
 * in consistent state.
 *
 * @param position The relative position.
 * @return The instruction field at the given relative position.
 * @exception OutOfRange If the given position is negative or not smaller
 *                       than the number of child fields.
 */
InstructionField&
BinaryEncoding::childField(int position) const {
    if (position < 0 || position >= childFieldCount()) {
	const string procName = "BinaryEncoding::childField";
	throw OutOfRange(__FILE__, __LINE__, procName);
    }

    if (hasImmediateControlField() &&
	immediateControlField().relativePosition() == position) {
	return immediateControlField();
    }

    int moveSlots = moveSlotCount();
    for (int i = 0; i < moveSlots; i++) {
	MoveSlot& slot = moveSlot(i);
	if (slot.relativePosition() == position) {
	    return slot;
	}
    }

    int immediateSlots = immediateSlotCount();
    for (int i = 0; i < immediateSlots; i++) {
        ImmediateSlotField& slot = immediateSlot(i);
        if (slot.relativePosition() == position) {
            return slot;
        }
    }

    int limmDstRegFields = longImmDstRegisterFieldCount();
    for (int i = 0; i < limmDstRegFields; i++) {
        LImmDstRegisterField& field = longImmDstRegisterField(i);
        if (field.relativePosition() == position) {
            return field;
        }
    }

    return NullInstructionField::instance();
}

/**
 * Returns the bit width of the instruction word defined by this encoding
 * map.
 *
 * @return The bit width of the instruction word.
 */
int
BinaryEncoding::width() const {

    int moveSlots = moveSlotCount();
    int immediateSlots = immediateSlotCount();
    int limmDstRegFields = longImmDstRegisterFieldCount();
    int width(0);

    for (int i = 0; i < moveSlots; i++) {
	MoveSlot& slot = moveSlot(i);
	width += slot.width();
    }

    for (int i = 0; i < immediateSlots; i++) {
        ImmediateSlotField& slot = immediateSlot(i);
        width += slot.width();
    }

    for (int i = 0; i < limmDstRegFields; i++) {
        LImmDstRegisterField& field = longImmDstRegisterField(i);
        width += field.width();
    }

    if (hasImmediateControlField()) {
	width += immediateControlField().width();
    }

    width += extraBits();
    return width;
}


/**
 * Loads the state of the binary encoding map from the given ObjectState
 * tree.
 *
 * @param state The ObjectState tree.
 * @exception ObjectStateLoadingException If an error occurs while loading
 *                                        the state.
 */
void
BinaryEncoding::loadState(const ObjectState* state) {
    if (state->name() != OSNAME_BEM) {
	const string procName = "BinaryEncoding::loadState";
	throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }

    // create socket code tables at first
    for (int i = 0; i < state->childCount(); i++) {
	ObjectState* child = state->child(i);
	if (child->name() == SocketCodeTable::OSNAME_SOCKET_CODE_TABLE) {
	    new SocketCodeTable(child, *this);
	}
    }

    // create subfields in the correct order
    ObjectState* newState = new ObjectState(*state);
    reorderSubfields(newState);

    for (int i = 0; i < newState->childCount(); i++) {
        ObjectState* child = newState->child(i);
        if (child->name() == MoveSlot::OSNAME_MOVE_SLOT) {
            new MoveSlot(child, *this);
        } else if (child->name() == 
                   ImmediateSlotField::OSNAME_IMMEDIATE_SLOT_FIELD) {
            new ImmediateSlotField(child, *this);
        } else if (child->name() ==
                   ImmediateControlField::OSNAME_IMM_CONTROL_FIELD) {
            new ImmediateControlField(child, *this);
        } else if (child->name() == 
                   LImmDstRegisterField::OSNAME_LIMM_DST_REGISTER_FIELD) {
            new LImmDstRegisterField(child, *this);
        }
    }
    delete newState;
}

/**
 * Saves the state of the binary encoding map to an ObjectState tree.
 *
 * @return The newly created ObjectState tree.
 */
ObjectState*
BinaryEncoding::saveState() const {

    ObjectState* bem = new ObjectState(OSNAME_BEM);

    // add move slots
    int moveSlots = moveSlotCount();
    for (int i = 0; i < moveSlots; i++) {
	MoveSlot& slot = moveSlot(i);
	bem->addChild(slot.saveState());
    }

    // add immediate slots
    int immediateSlots = immediateSlotCount();
    for (int i = 0; i < immediateSlots; i++) {
        ImmediateSlotField& slot = immediateSlot(i);
        bem->addChild(slot.saveState());
    }

    // add long immediate destination register fields
    int limmDstRegFields = longImmDstRegisterFieldCount();
    for (int i = 0; i < limmDstRegFields; i++) {
        LImmDstRegisterField& field = longImmDstRegisterField(i);
        bem->addChild(field.saveState());
    }

    // add socket code tables
    int tableCount = socketCodeTableCount();
    for (int i = 0; i < tableCount; i++) {
	SocketCodeTable& table = socketCodeTable(i);
	bem->addChild(table.saveState());
    }

    // add immediate control field
    if (hasImmediateControlField()) {
	bem->addChild(immediateControlField().saveState());
    }

    return bem;
}


/**
 * Tells whether the encoding map contains a socket code table with the given
 * name.
 *
 * @param name The name.
 * @return True if the encoding map contains a socket code table with the
 *         given name, otherwise false.
 */
bool
BinaryEncoding::hasSocketCodeTable(const std::string& name) const {

    for (SocketCodeTableContainer::const_iterator iter = 
             socketCodes_.begin();
	 iter != socketCodes_.end(); iter++) {
        
	SocketCodeTable* table = *iter;
	if (table->name() == name) {
	    return true;
	}
    }

    return false;
}


/**
 * Deletes all the move slots contained by the encoding map.
 */
void
BinaryEncoding::deleteMoveSlots() {
    SequenceTools::deleteAllItems(moveSlots_);
}


/**
 * Deletes all the immediate slots contained by the encoding map.
 */
void
BinaryEncoding::deleteImmediateSlots() {
    SequenceTools::deleteAllItems(immediateSlots_);
}


/**
 * Deletes all the long immediate destination register fields contained
 * by the encoding map.
 */
void
BinaryEncoding::deleteLongImmDstRegisterFields() {
    SequenceTools::deleteAllItems(longImmDstRegFields_);
}


/**
 * Deletes all the socket code tables contained by the encoding map.
 */
void
BinaryEncoding::deleteSocketCodes() {
    SequenceTools::deleteAllItems(socketCodes_);
}
