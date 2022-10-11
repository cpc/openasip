/*
    Copyright (c) 2002-2014 Tampere University.

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
 * @file MoveSlot.cc
 *
 * Implementation of MoveSlot class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @author Pekka Jääskeläinen 2014
 * @note rating: red
 */

#include "MoveSlot.hh"
#include "BinaryEncoding.hh"
#include "GuardField.hh"
#include "NullGuardField.hh"
#include "SourceField.hh"
#include "NullSourceField.hh"
#include "DestinationField.hh"
#include "NullDestinationField.hh"
#include "ImmediateControlField.hh"
#include "NullInstructionField.hh"
#include "Application.hh"
#include "ObjectState.hh"
#include "BEMTester.hh"

using std::string;

const std::string MoveSlot::OSNAME_MOVE_SLOT = "move_slot";
const std::string MoveSlot::OSKEY_BUS_NAME = "bus_name";

/**
 * The constructor.
 *
 * Registers the move slot to the parent binary encoding automatically.
 * The slot is added as the leftmost field of the instruction word.
 *
 * @param busName Name of the bus programmed by this move slot.
 * @param parent The parent BinaryEncoding.
 * @exception ObjectAlreadyExists If the parent binary encoding has a move
 *                                slot with the same bus name already.
 */
MoveSlot::MoveSlot(const std::string& busName, BinaryEncoding& parent)
    : InstructionField(&parent),
      name_(busName),
      guardField_(NULL),
      sourceField_(NULL),
      destinationField_(NULL) {
    setParent(NULL);
    parent.addMoveSlot(*this);
    setParent(&parent);
}

/**
 * The constructor.
 *
 * Loads the state of the move slot from the given ObjectState tree.
 *
 * @param state The ObjectState tree.
 * @param parent The parent binary encoding map.
 * @exception ObjectStateLoadingException If an error occurs while loading
 *                                        the state.
 */
MoveSlot::MoveSlot(const ObjectState* state, BinaryEncoding& parent)
    : InstructionField(state, &parent),
      name_(""),
      guardField_(NULL),
      sourceField_(NULL),
      destinationField_(NULL) {
    loadState(state);
    setParent(NULL);
    parent.addMoveSlot(*this);
    setParent(&parent);
}

/**
 * The destructor.
 */
MoveSlot::~MoveSlot() {

    deleteGuardField();
    deleteSourceField();
    deleteDestinationField();

    BinaryEncoding* parent = this->parent();
    setParent(NULL);
    parent->removeMoveSlot(*this);
}


/**
 * Returns the parent binary encoding.
 *
 * @return The parent binary encoding.
 */
BinaryEncoding*
MoveSlot::parent() const {
    InstructionField* parent = InstructionField::parent();
    if (parent == NULL) {
        return NULL;
    } else {
        BinaryEncoding* bem = dynamic_cast<BinaryEncoding*>(parent);
        assert(bem != NULL);
        return bem;
    }
}


/**
 * Returns the bus name programmed by this move slot in the instruction word.
 *
 * @return Name of the bus.
 */
std::string
MoveSlot::name() const {
    return name_;
}


/**
 * Sets the name of the bus programmed by this move slot.
 *
 * @param name Name of the bus.
 * @exception ObjectAlreadyExists If there is a move slot that programs the
 *                                given bus already.
 */
void
MoveSlot::setName(const std::string& name) {
    if (name == this->name()) {
        return;
    }

    if (parent()->hasMoveSlot(name)) {
        const string procName = "MoveSlot::setName";
        throw ObjectAlreadyExists(__FILE__, __LINE__, procName);
    }

    name_ = name;
}

/**
 * Adds the given guard field to the move slot.
 *
 * This method is to be called from the constructor of GuardField.
 *
 * @param field The guard field to be added.
 * @exception ObjectAlreadyExists If the move slot has a guard field already.
 */
void
MoveSlot::setGuardField(GuardField& field) {
    // verify that this is called from GuardField constructor
    assert(field.parent() == NULL);

    if (hasGuardField()) {
        const string procName = "MoveSlot::setGuardField";
        throw ObjectAlreadyExists(__FILE__, __LINE__, procName);
    }

    guardField_ = &field;
}

/**
 * Removes the guard field from the move slot.
 *
 * This method is to be called from the destructor of GuardField.
 */
void
MoveSlot::unsetGuardField() {
    assert(hasGuardField());
    assert(guardField().parent() == NULL);
    guardField_ = NULL;
}


/**
 * Tells whether the move slot has a guard field.
 *
 * @return True if the move has a guard field, otherwise false.
 */
bool
MoveSlot::hasGuardField() const {
    return guardField_ != NULL;
}


/**
 * Returns the guard field of the move slot.
 *
 * Returns NullGuardField instance if the guard field is missing.
 *
 * @return The guard field of the move slot.
 */
GuardField&
MoveSlot::guardField() const {
    if (hasGuardField()) {
        return *guardField_;
    } else {
        return NullGuardField::instance();
    }
}

/**
 * Adds the given source field to the move slot.
 *
 * This method is to be called from the constructor of SourceField.
 *
 * @param field The source field to be added.
 * @exception ObjectAlreadyExists If the move slot has a source field
 *                                already.
 */
void
MoveSlot::setSourceField(SourceField& field) {
    // verify that this is called from SourceField constructor
    assert(field.parent() == NULL);

    if (hasSourceField()) {
        const string procName = "MoveSlot::setSourceField";
        throw ObjectAlreadyExists(__FILE__, __LINE__, procName);
    }

    sourceField_ = &field;
}

/**
 * Removes the source field from the move slot.
 *
 * This method is to be called from the destructor of SourceField.
 */
void
MoveSlot::unsetSourceField() {
    assert(hasSourceField());
    assert(sourceField().parent() == NULL);
    sourceField_ = NULL;
}


/**
 * Tells whether the move slot has a source field.
 *
 * @return True if the move slot has a source field, otherwise false.
 */
bool
MoveSlot::hasSourceField() const {
    return sourceField_ != NULL;
}


/**
 * Returns the source field of the move slot.
 *
 * Returns NullSourceField if the move slot does not have a source field.
 *
 * @return The source field of the move slot.
 */
SourceField&
MoveSlot::sourceField() const {
    if (hasSourceField()) {
        return *sourceField_;
    } else {
        return NullSourceField::instance();
    }
}


/**
 * Adds the given destination field to the move slot.
 *
 * This method is to be called from the constructor of DestinationField.
 *
 * @param field The destination field to be added.
 * @exception ObjectAlreadyExists If the move slot has a destination field
 *                                already.
 */
void
MoveSlot::setDestinationField(DestinationField& field) {
    // verify that this is called from DestinationField constructor
    assert(field.parent() == NULL);

    if (hasDestinationField()) {
        const string procName = "MoveSlot::setDestinationField";
        throw ObjectAlreadyExists(__FILE__, __LINE__, procName);
    }

    destinationField_ = &field;
}

/**
 * Removes the destination field from the move slot.
 *
 * This method is to be called from the destructor of DestinationField.
 */
void
MoveSlot::unsetDestinationField() {
    assert(hasDestinationField());
    assert(destinationField().parent() == NULL);
    destinationField_ = NULL;
}


/**
 * Tells whether the move slot has a destination field.
 *
 * @return True if the move slot has a destination field, otherwise false.
 */
bool
MoveSlot::hasDestinationField() const {
    return destinationField_ != NULL;
}


/**
 * Returns the destination field of the move slot.
 *
 * Returns NullDestinationField if the move slot does not have a destination
 * field.
 *
 * @return The destination field of the move slot.
 */
DestinationField&
MoveSlot::destinationField() const {
    if (hasDestinationField()) {
        return *destinationField_;
    } else {
        return NullDestinationField::instance();
    }
}


/**
 * Returns the number of child fields (guard, source, destination) within the
 * move slot.
 *
 * @return The number of child fields.
 */
int
MoveSlot::childFieldCount() const {
    int count(0);
    if (hasGuardField()) {
        count++;
    }
    if (hasSourceField()) {
        count++;
    }
    if (hasDestinationField()) {
        count++;
    }
    return count;
}


/**
 * Returns the child field at the given relative position.
 *
 * @param position The position (0 is the rightmost position).
 * @exception OutOfRange If the position is negative or not smaller than the
 *                       number of child fields.
 */
InstructionField&
MoveSlot::childField(int position) const {
    InstructionField::childField(position);

    if (hasGuardField() && guardField().relativePosition() == position) {
        return guardField();
    }

    if (hasSourceField() && sourceField().relativePosition() == position) {
        return sourceField();
    }

    if (hasDestinationField() &&
        destinationField().relativePosition() == position) {
        return destinationField();
    }

    assert(false);
    return NullInstructionField::instance();
}

/**
 * Returns the bit width of the move slot.
 *
 * @return The bit width of the move slot.
 */
int
MoveSlot::width() const {

    int width(0);

    if (hasGuardField()) {
        width += guardField().width();
    }
    if (hasSourceField()) {
        width += sourceField().width();
    }
    if (hasDestinationField()) {
        width += destinationField().width();
    }

    return width;
}


/**
 * Loads the state of the move slot from the given ObjectState tree.
 *
 * @param state The ObjectState tree.
 * @exception ObjectStateLoadingException If an error occurs while loading
 *                                        the state.
 */
void
MoveSlot::loadState(const ObjectState* state) {
    deleteGuardField();
    deleteSourceField();
    deleteDestinationField();

    ObjectState* newState = new ObjectState(*state);
    reorderSubfields(newState);
    InstructionField::loadState(newState);

    try {
        setName(newState->stringAttribute(OSKEY_BUS_NAME));
        for (int i = 0; i < newState->childCount(); i++) {
            ObjectState* child = newState->child(i);
            if (child->name() == GuardField::OSNAME_GUARD_FIELD) {
                new GuardField(child, *this);
            } else if (child->name() == SourceField::OSNAME_SOURCE_FIELD) {
                new SourceField(child, *this);
            } else if (
                child->name() == DestinationField::OSNAME_DESTINATION_FIELD) {
                new DestinationField(child, *this);
            }
        }
    } catch (const Exception& exception) {
        const string procName = "MoveSlot::loadState";
        throw ObjectStateLoadingException(
            __FILE__, __LINE__, procName, exception.errorMessage());
    }

    delete newState;
}

/**
 * Saves the state of the move slot to an ObjectState tree.
 *
 * @return The newly created ObjectState tree.
 */
ObjectState*
MoveSlot::saveState() const {

    ObjectState* state = InstructionField::saveState();
    state->setName(OSNAME_MOVE_SLOT);
    state->setAttribute(OSKEY_BUS_NAME, name());

    if (hasGuardField()) {
        state->addChild(guardField().saveState());
    }
    if (hasSourceField()) {
        state->addChild(sourceField().saveState());
    }
    if (hasDestinationField()) {
        state->addChild(destinationField().saveState());
    }

    return state;
}


/**
 * Deletes the guard field of the move slot.
 */
void
MoveSlot::deleteGuardField() {
    if (guardField_ != NULL) {
        delete guardField_;
        guardField_ = NULL;
    }
}


/**
 * Deletes the source field of the move slot.
 */
void
MoveSlot::deleteSourceField() {
    if (sourceField_ != NULL) {
        delete sourceField_;
        sourceField_ = NULL;
    }
}

/**
 * Deletes the destination field of the move slot.
 */
void
MoveSlot::deleteDestinationField() {
    if (destinationField_ != NULL) {
        delete destinationField_;
        destinationField_ = NULL;
    }
}