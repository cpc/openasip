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
 * @file InstructionFormat.cc
 *
 * Implementation of InstructionFormat class.
 *
 * @author Kari Hepola 2021 (kari.hepola@tuni.fi)
 * @note rating: red
 */

#include "InstructionFormat.hh"
#include "OperationTriggeredEncoding.hh"
#include "ObjectState.hh"
#include "BinaryEncoding.hh"
#include "MapTools.hh"
#include "VectorTools.hh"
#include "InstructionField.hh"
#include <algorithm>

const std::string InstructionFormat::OSNAME_INSTRUCTION_FORMAT = "ota-format";

const std::string InstructionFormat::OSKEY_INSTRUCTION_FORMAT_NAME = "name";
const std::string InstructionFormat::OSKEY_OTA_OPERATION_NAME =
    "ota-operation";
const std::string InstructionFormat::OSKEY_OPERATION_NAME = "name";
const std::string InstructionFormat::OSKEY_OPERATION_ENCODING_NAME =
    "encoding";

/**
 * The constructor.
 *
 * Adds the instruction format to the parent binary encoding automatically.
 *
 * @param name Name of the instruction format.
 * @param parent The parent BinaryEncoding.
 */

InstructionFormat::InstructionFormat(
    const std::string& name, BinaryEncoding& parent)
    : name_(name), parent_(NULL) {
    parent.addInstructionFormat(*this);
    parent_ = &parent;
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

InstructionFormat::InstructionFormat(
    const ObjectState* state, BinaryEncoding& parent)
    : parent_(NULL) {
    parent.addInstructionFormat(*this);
    parent_ = &parent;
    loadState(state);
}

/**
 * The destructor
 *
 */

InstructionFormat::~InstructionFormat() {
    /*
    for (unsigned int i = 0; i < encodings_.size(); i++) {
        delete encodings_.at(i);
    }*/
}

InstructionField*
InstructionFormat::parent() const {
    return static_cast<InstructionField*>(parent_);
}

bool
InstructionFormat::hasOperation(const std::string& op) const {
    return MapTools::containsKey(operations_, op);
}

/**
 * Returns the name of the instruction format.
 *
 * @return The name of the instruction format
 */

std::string
InstructionFormat::name() const {
    return name_;
}

/**
 * Sets the name of the instruction format.
 *
 * @param name The name of the instruction format.
 */

void
InstructionFormat::setName(const std::string& name) {
    name_ = name;
}

/**
 * Adds an operation triggered move slot to the instruction format
 *
 * @param encoding The operation triggered move slot to be addeed
 * @exception ObjectAlreadyExists If the slot has already been added to the
 *            instruction format.
 */

void
InstructionFormat::addEncoding(OperationTriggeredEncoding& encoding) {
    if (!VectorTools::containsValue(encodings_, &encoding)) {
        encodings_.push_back(&encoding);
    } else {
        const std::string procName = "InstructionFormat::addEncoding";
        throw ObjectAlreadyExists(__FILE__, __LINE__, procName);
    }
}

/**
 * Adds an operation to the instruction format
 *
 * @param op Operation name
 * @param encoding Encoding of the operation
 * @exception ObjectAlreadyExists If the slot has already been added to the
 *            instruction format.
 */

void
InstructionFormat::addOperation(std::string op, int encoding) {
    if (!MapTools::containsKey(operations_, op)) {
        operations_.insert({op, encoding});
    } else {
        const std::string procName = "InstructionFormat::addOperation";
        throw ObjectAlreadyExists(__FILE__, __LINE__, procName);
    }
}

/**
 * Returns the number of operation triggered encodings.
 *
 * @return The number of child fields.
 */

int
InstructionFormat::childFieldCount() const {
    return encodings_.size();
}

/**
 * Returns the bit width of the instruction format.
 *
 * @return Bit width of the instruction format.
 */

int
InstructionFormat::width() const {
    int width = 0;
    for (unsigned int i = 0; i < encodings_.size(); i++) {
        OperationTriggeredEncoding* slot = encodings_.at(i);
        width += slot->width();
    }
    return width;
}

std::map<std::string, int>
InstructionFormat::operations() const {
    return operations_;
}

int
InstructionFormat::encoding(const std::string& op) const {
    assert(MapTools::containsKey(operations_, op));
    return operations_.at(op);
}

/**
 * Loads the state of the instruction format from the given ObjectState tree.
 *
 * @param state The ObjectState tree.
 * @exception ObjectStateLoadingException If an error occurs while loading
 *                                        the state.
 */

void
InstructionFormat::loadState(const ObjectState* state) {
    ObjectState* newState = new ObjectState(*state);
    try {
        setName(newState->stringAttribute(OSKEY_INSTRUCTION_FORMAT_NAME));
        for (int i = 0; i < newState->childCount(); i++) {
            ObjectState* child = newState->child(i);
            if (child->name() ==
                OperationTriggeredEncoding::OSNAME_OTA_ENCODING) {
                new OperationTriggeredEncoding(child, *this);
            } else if (child->name() == OSKEY_OTA_OPERATION_NAME) {
                addOperation(
                    child->stringAttribute(OSKEY_OPERATION_NAME),
                    child->intAttribute(OSKEY_OPERATION_ENCODING_NAME));
            }
        }
    } catch (const Exception& exception) {
        const std::string procName = "InstructionFormat::loadState";
        throw ObjectStateLoadingException(
            __FILE__, __LINE__, procName, exception.errorMessage());
    }
}

/**
 * Saves the state of the instruction format to an ObjectState tree.
 *
 * @return The newly created ObjectState tree.
 */

ObjectState*
InstructionFormat::saveState() const {
    ObjectState* state = new ObjectState(OSNAME_INSTRUCTION_FORMAT);
    state->setAttribute(OSKEY_INSTRUCTION_FORMAT_NAME, name_);
    for (unsigned int i = 0; i < encodings_.size(); i++) {
        OperationTriggeredEncoding* it = encodings_.at(i);
        ObjectState* obj = it->saveState();
        state->addChild(obj);
    }
    for (auto const& val : operations_) {
        ObjectState* obj = new ObjectState(OSKEY_OTA_OPERATION_NAME);
        obj->setAttribute(OSKEY_OPERATION_NAME, val.first);
        obj->setAttribute(OSKEY_OPERATION_ENCODING_NAME, val.second);
        state->addChild(obj);
    }
    return state;
}