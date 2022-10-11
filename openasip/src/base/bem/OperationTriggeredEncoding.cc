/*
    Copyright (c) 2002-2021 Tampere University.

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
 * @file OperationTriggeredEncoding.cc
 *
 * Implementation of OperationTriggeredEncoding class.
 *
 * @author Kari Hepola 2021 (kari.hepola@tuni.fi)
 * @note rating: red
 */

#include "OperationTriggeredEncoding.hh"
#include "OperationTriggeredField.hh"
#include "InstructionFormat.hh"
#include "ObjectState.hh"
#include <algorithm>

const std::string OperationTriggeredEncoding::OSNAME_OTA_ENCODING =
    "ota-encoding";

const std::string OperationTriggeredEncoding::OSKEY_OTA_ENCODING_NAME =
    "name";

/**
 * The constructor.
 *
 * Adds the instruction format to the parent binary encoding automatically.
 *
 * @param name Name of the instruction format.
 * @param parent The parent Instruction Format.
 */

OperationTriggeredEncoding::OperationTriggeredEncoding(
    const std::string& name, InstructionFormat& parent)
    : name_(name) {
    parent.addEncoding(*this);
}

/**
 * The constructor.
 *
 * Loads the state of the instruction format from the given ObjectState tree.
 *
 * @param state The ObjectState tree.
 * @param parent The parent binary encoding map.
 * @exception ObjectStateLoadingException If an error occurs while loading
              the state.
 */

OperationTriggeredEncoding::OperationTriggeredEncoding(
    const ObjectState* state, InstructionFormat& parent) {
    parent.addEncoding(*this);
    loadState(state);
}

/**
 * The destructor
 *
 */

OperationTriggeredEncoding::~OperationTriggeredEncoding() {
    for (unsigned int i = 0; i < fields_.size(); i++) {
        delete fields_.at(i);
    }
}

/**
 * Returns the name of the instruction format.
 *
 * @return The name of the instruction format
 */

std::string
OperationTriggeredEncoding::name() const {
    return name_;
}

/**
 * Sets the name of the instruction format.
 *
 * @param name The name of the instruction format.
 */

void
OperationTriggeredEncoding::setName(const std::string& name) {
    name_ = name;
}

/**
 * Returns the number of operation triggered fields.
 *
 * @return The number of child fields.
 */

int
OperationTriggeredEncoding::childFieldCount() const {
    return fields_.size();
}

/**
 * Returns the bit width of the instruction format.
 *
 * @return Bit width of the instruction format.
 */

int
OperationTriggeredEncoding::width() const {
    int width = 0;
    for (unsigned int i = 0; i < fields_.size(); i++) {
        OperationTriggeredField* field = fields_.at(i);
        width += field->width();
    }
    return width;
}

void
OperationTriggeredEncoding::addField(OperationTriggeredField& field) {
    // TODO: Inspect that field is valid
    fields_.push_back(&field);
}

/**
 * Loads the state of the instruction format from the given ObjectState tree.
 *
 * @param state The ObjectState tree.
 * @exception ObjectStateLoadingException If an error occurs while loading
 *                                        the state.
 */

void
OperationTriggeredEncoding::loadState(const ObjectState* state) {
    ObjectState* newState = new ObjectState(*state);
    try {
        setName(newState->stringAttribute(OSKEY_OTA_ENCODING_NAME));
        for (int i = 0; i < newState->childCount(); i++) {
            ObjectState* child = newState->child(i);
            if (child->name() == OperationTriggeredField::OSNAME_FIELD) {
                new OperationTriggeredField(child, *this);
            }
        }
    } catch (const Exception& exception) {
        const std::string procName = "OperationTriggeredEncoding::loadState";
        throw ObjectStateLoadingException(
            __FILE__, __LINE__, procName, exception.errorMessage());
    }
    delete newState;
}

/**
 * Saves the state of the instruction format to an ObjectState tree.
 *
 * @return The newly created ObjectState tree.
 */

ObjectState*
OperationTriggeredEncoding::saveState() const {
    ObjectState* state = new ObjectState(OSNAME_OTA_ENCODING);
    state->setAttribute(OSKEY_OTA_ENCODING_NAME, name_);
    for (unsigned int i = 0; i < fields_.size(); i++) {
        OperationTriggeredField* it = fields_.at(i);
        ObjectState* fieldObj = it->saveState();
        state->addChild(fieldObj);
    }
    return state;
}