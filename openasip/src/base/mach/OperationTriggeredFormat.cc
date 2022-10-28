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
 * @file OperationTriggeredFormat.cc
 *
 * Implementation of OperationTriggeredFormat class.
 *
 * @author Kari Hepola 2022 (kari.hepola@tuni.fi)
 * @note rating: red
 */

#include <stdio.h>

#include "OperationTriggeredFormat.hh"
#include "OperationTriggeredOperand.hh"
#include "MOMTextGenerator.hh"
#include "ObjectState.hh"
#include "Machine.hh"

namespace TTAMachine {

const std::string OperationTriggeredFormat::OSNAME_FORMAT = "ota-format";

const std::string OperationTriggeredFormat::OSKEY_OPERATION = "ota-operation";

/**
 * The constructor.
 *
 * Registers the operation code encoding to the parent
 * binary encoding automatically.
 *
 * @param parent The parent OperationTriggeredFormat.
 */

OperationTriggeredFormat::OperationTriggeredFormat(
    const std::string& name, Machine& owner)
    : Component(name) {
    setMachine(owner);
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

OperationTriggeredFormat::OperationTriggeredFormat(
    const ObjectState* state, Machine& owner)
    : Component(state) {
    const std::string procName =
        "OperationTriggeredFormat::OperationTriggeredFormat";
    try {
        setMachine(owner);
    } catch (const ComponentAlreadyExists&) {
        MOMTextGenerator textGenerator;
        boost::format errorMsg =
            textGenerator.text(MOMTextGenerator::TXT_IT_EXISTS_BY_NAME);
        errorMsg % name();
        throw ObjectStateLoadingException(
            __FILE__, __LINE__, procName, errorMsg.str());
    }
    try {
        loadState(state);
    } catch (const ObjectStateLoadingException&) {
        unsetMachine();
        throw;
    }
}

/**
 * Destructor.
 */
OperationTriggeredFormat::~OperationTriggeredFormat() {
    for (unsigned int i = 0; i < operands_.size(); i++) {
        delete operands_.at(i);
    }
    unsetMachine();
}

std::vector<std::string>
OperationTriggeredFormat::operations() const {
    return operations_;
}

void
OperationTriggeredFormat::addOperation(const std::string& op) {
    operations_.push_back(op);
}

void
OperationTriggeredFormat::removeOperation(const std::string& op) {
    for (unsigned int i = 0; i < operations_.size(); i++) {
        if (operations_.at(i) == op) {
            operations_.erase(operations_.begin() + i);
        }
    }
}

int
OperationTriggeredFormat::operationCount() const {
    return operations_.size();
}

std::string
OperationTriggeredFormat::operation(int index) const {
    if (index > operationCount() - 1) {
        const std::string msg = "Operation index out of range.";
        throw OutOfRange(__FILE__, __LINE__, __func__, msg);
    }
    return operations_.at(index);
}

bool
OperationTriggeredFormat::hasOperation(const std::string& opName) const {
    for (const std::string& op : operations_) {
        if (op == opName) {
            return true;
        }
    }
    return false;
}

std::vector<OperationTriggeredOperand*>
OperationTriggeredFormat::operands() const {
    return operands_;
}

void
OperationTriggeredFormat::addOperand(OperationTriggeredOperand& op) {
    operands_.push_back(&op);
}

/**
 * Adds the OperationTriggeredFormat to the given machine.
 *
 * @param machine Machine to which the OperationTriggeredFormat Format is
 * added.
 * @exception ComponentAlreadyExists If there already is another
 * OperationTriggeredFormat Format by the same name or another empty
 * OperationTriggeredFormat Format in the given machine.
 */
void
OperationTriggeredFormat::setMachine(Machine& machine) {
    machine.addOperationTriggeredFormat(*this);
    internalSetMachine(machine);
}

/**
 * Removes the OperationTriggeredFormat Format from its machine.
 *
 * The OperationTriggeredFormat Format is also deleted because it cannot be
 * alone. It must be registered to a machine.
 */
void
OperationTriggeredFormat::unsetMachine() {
    Machine* mach = machine();
    assert(mach != NULL);
    internalUnsetMachine();
    mach->deleteOperationTriggeredFormat(*this);
}

void
OperationTriggeredFormat::loadState(const ObjectState* state) {
    operations_.clear();
    operands_.clear();
    const std::string procName = "OperationTriggeredOperand::loadState";

    ObjectState* newState = new ObjectState(*state);

    if (newState->name() != OSNAME_FORMAT) {
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }
    Component::loadState(newState);
    try {
        for (int i = 0; i < newState->childCount(); i++) {
            ObjectState* child = newState->child(i);
            if (child->name() == OperationTriggeredOperand::OSNAME_OPERAND) {
                new OperationTriggeredOperand(child, *this);
            } else if (
                child->name() == OperationTriggeredFormat::OSKEY_OPERATION) {
                operations_.push_back(child->stringValue());
            }
        }
    } catch (const Exception& exception) {
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
OperationTriggeredFormat::saveState() const {
    ObjectState* state = Component::saveState();
    state->setName(OSNAME_FORMAT);
    for (unsigned int i = 0; i < operands_.size(); i++) {
        ObjectState* operandObject = operands_.at(i)->saveState();
        state->addChild(operandObject);
    }
    for (unsigned int i = 0; i < operations_.size(); i++) {
        ObjectState* operationObject = new ObjectState(OSKEY_OPERATION);
        operationObject->setValue(operations_.at(i));
        state->addChild(operationObject);
    }

    return state;
}
}  // namespace TTAMachine