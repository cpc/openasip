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
 * @file OperationTriggeredOperand.cc
 *
 * Implementation of OperationTriggeredOperand class.
 *
 * @author Kari Hepola 2021 (kari.hepola@tuni.fi)
 * @note rating: red
 */

#include "OperationTriggeredOperand.hh"
#include "ObjectState.hh"
#include "OperationTriggeredFormat.hh"
#include "StringTools.hh"

namespace TTAMachine {

const std::string OperationTriggeredOperand::OSNAME_OPERAND = "operand";
const std::string OperationTriggeredOperand::OSKEY_NAME = "name";
const std::string OperationTriggeredOperand::OSKEY_TYPE = "type";
const std::string OperationTriggeredOperand::OSKEY_DIRECTION = "direction";

/**
 * The constructor.
 *
 * Registers the operation code encoding to the parent
 * binary encoding automatically.
 *
 * @param parent The parent OperationTriggeredFormat.
 */

OperationTriggeredOperand::OperationTriggeredOperand(
    const std::string& name, OperationTriggeredFormat& parent)
    : SubComponent(), name_(name), type_(""), direction_("") {
    name_ = StringTools::stringToLower(name);
    parent.addOperand(*this);
}

/**
 * The constructor
 *
 * Loads the state of the operation code encoding from
 * the given ObjectState tree
 *
 * @param state The ObjectState tree
 * @param parent The parent binary encoding map
 * @exception ObjectStateLoadingException If an error occurs while loading
              the state.
 */

OperationTriggeredOperand::OperationTriggeredOperand(
    const ObjectState* state, OperationTriggeredFormat& parent)
    : SubComponent(), name_(""), type_(""), direction_("") {
    const std::string procName =
        "OperationTriggeredOperand::OperationTriggeredOperand";
    // set name
    try {
        setName(state->stringAttribute(OSKEY_NAME));
    } catch (const Exception& exception) {
        throw ObjectStateLoadingException(
            __FILE__, __LINE__, procName, exception.errorMessage());
    }
    parent.addOperand(*this);
    loadState(state);
}

/**
 * Destructor.
 */
OperationTriggeredOperand::~OperationTriggeredOperand() {
    // TODO
}

/**
 * Returns the name of the operation.
 *
 * @return The name of the operation.
 */
const std::string&
OperationTriggeredOperand::name() const {
    return name_;
}

/**
 * Sets the name of the operand.
 *
 * @param name The new name.
 */
void
OperationTriggeredOperand::setName(const std::string& name) {
    std::string lowerName = StringTools::stringToLower(name);

    if (lowerName == this->name()) {
        return;
    }
    name_ = lowerName;
}

std::string
OperationTriggeredOperand::type() const {
    return type_;
}

void
OperationTriggeredOperand::setType(const std::string& type) {
    type_ = type;
}

std::string
OperationTriggeredOperand::direction() const {
    return direction_;
}

void
OperationTriggeredOperand::setDirection(const std::string& direction) {
    type_ = direction;
}

void
OperationTriggeredOperand::loadState(const ObjectState* state) {
    ObjectState* newState = new ObjectState(*state);
    try {
        type_ = state->stringAttribute(OSKEY_TYPE);
        direction_ = state->stringAttribute(OSKEY_DIRECTION);
    } catch (const Exception& exception) {
        const std::string procName = "OperationTriggeredOperand::loadState";
        throw ObjectStateLoadingException(
            __FILE__, __LINE__, procName, exception.errorMessage());
    }

    delete newState;
}

/**
 * Saves the state of the operation code encoding
 * to an ObjectState tree.
 *
 * @return The newly created ObjectState tree.
 */

ObjectState*
OperationTriggeredOperand::saveState() const {
    ObjectState* state = new ObjectState(OSNAME_OPERAND);
    state->setAttribute(OSKEY_NAME, name_);
    state->setAttribute(OSKEY_TYPE, type_);
    state->setAttribute(OSKEY_DIRECTION, direction_);

    return state;
}
}  // namespace TTAMachine