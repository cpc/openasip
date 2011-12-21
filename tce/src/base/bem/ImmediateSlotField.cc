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
 * @file ImmediateSlotField.cc
 *
 * Implementation of ImmediateSlotField class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>

#include "Application.hh"
#include "ImmediateSlotField.hh"
#include "BinaryEncoding.hh"
#include "ObjectState.hh"

using std::string;

const std::string ImmediateSlotField::OSNAME_IMMEDIATE_SLOT_FIELD = 
    "imm_slot_field";
const std::string ImmediateSlotField::OSKEY_NAME = "name";
const std::string ImmediateSlotField::OSKEY_WIDTH = "width";

/**
 * The constructor.
 *
 * @param name Name of the immediate slot programmed by this field.
 * @param width Bit width of the field.
 * @parent The parent binary encoding map.
 * @exception OutOfRange If the bit width is smaller than 1.
 * @exception ObjectAlreadyExists If the parent binary encoding already has
 *                                an immediate slot with the given name.
 */
ImmediateSlotField::ImmediateSlotField(
    const std::string& name,
    int width,
    BinaryEncoding& parent)
    throw (OutOfRange, ObjectAlreadyExists) :
    InstructionField(&parent), name_(name), width_(width) {

    if (width < 1) {
        const string procName = "ImmediateSlotField::ImmediateSlotField";
        throw OutOfRange(__FILE__, __LINE__, procName);
    }

    setParent(NULL);
    parent.addImmediateSlot(*this);
    setParent(&parent);
}


/**
 * The constructor.
 *
 * Loads the state of the object from the given ObjectState instance.
 *
 * @param state The ObjectState instance.
 * @exception ObjectStateLoadingException If an error occurs while loading
 *                                        the state.
 */
ImmediateSlotField::ImmediateSlotField(
    const ObjectState* state,
    BinaryEncoding& parent)
    throw (ObjectStateLoadingException) :
    InstructionField(state, &parent), name_(""), width_(0) {

    loadState(state);
    setParent(NULL);
    parent.addImmediateSlot(*this);
    setParent(&parent);
}


/**
 * The destructor.
 */
ImmediateSlotField::~ImmediateSlotField() {
    BinaryEncoding* parent = this->parent();
    assert(parent != NULL);
    setParent(NULL);
    parent->removeImmediateSlot(*this);
}


/**
 * Returns the parent binary encoding map.
 *
 * @return The parent binary encoding map.
 */
BinaryEncoding*
ImmediateSlotField::parent() const {
    InstructionField* parent = InstructionField::parent();
    if (parent == NULL) {
        return NULL;
    } else {
        BinaryEncoding* bemParent = dynamic_cast<BinaryEncoding*>(parent);
        assert(bemParent != NULL);
        return bemParent;
    }
}


/**
 * Returns the name of the immediate slot programmed by this field.
 *
 * @return The name of the immediate slot.
 */
std::string
ImmediateSlotField::name() const {
    return name_;
}


/**
 * Sets the name of the immediate slot programmed by this field.
 *
 * @param name The name.
 * @exception ObjectAlreadyExists If the parent binary encoding has an
 *                                immediate slot field for the given 
 *                                immediate slot already.
 */
void
ImmediateSlotField::setName(const std::string& name)
    throw (ObjectAlreadyExists) {

    if (name == this->name()) {
        return;
    }

    if (parent()->hasImmediateSlot(name)) {
        const string procName = "ImmediateSlotField::setName";
        throw ObjectAlreadyExists(__FILE__, __LINE__, procName);
    } else {
        name_ = name;
    }
}
    

/**
 * Returns 0 always since immediate slot does not have any child fields.
 *
 * @return 0.
 */
int
ImmediateSlotField::childFieldCount() const {
    return 0;
}


/**
 * Returns the bit width of the field.
 *
 * @return The bit width.
 */
int
ImmediateSlotField::width() const {
    return width_;
}


/**
 * Sets the bit width of the field.
 *
 * @param width The new bit width.
 * @exception OutOfRange If the given width is smaller than 1.
 */
void
ImmediateSlotField::setWidth(int width)
    throw (OutOfRange) {

    if (width < 1) {
        const string procName = "ImmediateSlotField::setWidth";
        throw OutOfRange(__FILE__, __LINE__, procName);
    }
    width_ = width;
}


/**
 * Saves the state of the object to an ObjectState instance.
 *
 * @return The newly created ObjectState instance.
 */
ObjectState*
ImmediateSlotField::saveState() const {
    ObjectState* state = InstructionField::saveState();
    state->setName(OSNAME_IMMEDIATE_SLOT_FIELD);
    state->setAttribute(OSKEY_NAME, name());
    state->setAttribute(OSKEY_WIDTH, width());
    return state;
}


/**
 * Loads the state of the object from the given ObjectState instance.
 *
 * @param state The ObjectState instance.
 * @exception ObjectStateLoadingException If an error occurs while loading
 *                                        the state.
 */
void
ImmediateSlotField::loadState(const ObjectState* state)
    throw (ObjectStateLoadingException) {

    const string procName = "ImmediateSlotField::loadState";

    if (state->name() != OSNAME_IMMEDIATE_SLOT_FIELD) {
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }

    InstructionField::loadState(state);

    try {
        setName(state->stringAttribute(OSKEY_NAME));
        setWidth(state->intAttribute(OSKEY_WIDTH));
    } catch (const Exception&) {
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }
}
