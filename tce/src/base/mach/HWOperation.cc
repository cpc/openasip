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
 * @file HWOperation.cc
 *
 * Implementation of HWOperation class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include "HWOperation.hh"
#include "FunctionUnit.hh"
#include "ExecutionPipeline.hh"
#include "PipelineElement.hh"
#include "FUPort.hh"
#include "MachineTester.hh"
#include "MOMTextGenerator.hh"
#include "MapTools.hh"
#include "ContainerTools.hh"
#include "StringTools.hh"

using std::string;
using std::pair;
using boost::format;

namespace TTAMachine {

// initialization of static data members
const string HWOperation::OSNAME_OPERATION = "operation";
const string HWOperation::OSKEY_NAME = "name";
const string HWOperation::OSNAME_OPERAND_BINDING = "binding";
const string HWOperation::OSKEY_OPERAND = "operand";
const string HWOperation::OSKEY_PORT = "port";

/**
 * Constructor.
 *
 * @param name Name of the operation.
 * @param parent The parent unit.
 * @exception ComponentAlreadyExists If there is already an operation by the
 *                                   same name in the given function unit.
 * @exception InvalidName If the given name is not valid for a component.
 */
HWOperation::HWOperation(const string& name, FunctionUnit& parent)
    throw (ComponentAlreadyExists, InvalidName) :
    SubComponent(), name_(name), pipeline_(NULL), parent_(NULL) {

    name_ = StringTools::stringToLower(name);

    if (!MachineTester::isValidComponentName(name_)) {
        const string procName = "HWOperation::HWOperation";
        MOMTextGenerator textGen;
        format errorMsg = textGen.text(MOMTextGenerator::TXT_INVALID_NAME);
        errorMsg % name;
        throw InvalidName(__FILE__, __LINE__, procName, errorMsg.str());
    }

    parent.addOperation(*this);
    parent_ = &parent;
    pipeline_ = new ExecutionPipeline(*this);
}


/**
 * Constructor.
 *
 * Loads the state of the operation from the given ObjectState instance.
 *
 * @param state The ObjectState instance.
 * @param parent The parent unit.
 * @exception ObjectStateLoadingException If an error occurs while loading
 *                                        the state.
 */
HWOperation::HWOperation(const ObjectState* state, FunctionUnit& parent)
    throw (ObjectStateLoadingException) :
    SubComponent(), name_(""), pipeline_(NULL), parent_(&parent) {

    const string procName = "HWOperation::HWOperation";

    // set name
    try {
        setName(state->stringAttribute(OSKEY_NAME));
    } catch (const Exception& exception) {
        throw ObjectStateLoadingException(
            __FILE__, __LINE__, procName, exception.errorMessage());
    }

    parent_ = NULL;

    // register to parent unit
    parent.addOperation(*this);
    parent_ = &parent;

    try {
        loadState(state);
    } catch (const ObjectStateLoadingException& e) {
        if (pipeline_ != NULL) {
            delete pipeline_;
        }
        throw;
    }
}


/**
 * Destructor.
 */
HWOperation::~HWOperation() {
    delete pipeline_;
    FunctionUnit* parent = parentUnit();
    parent_ = NULL;
    parent->deleteOperation(*this);
}


/**
 * Returns the name of the operation.
 *
 * @return The name of the operation.
 */
const string&
HWOperation::name() const {
    return name_;
}


/**
 * Sets the name of the operation.
 *
 * @param name The new name.
 * @exception ComponentAlreadyExists If another operation exists by the same
 *                                   name in the function unit.
 * @exception InvalidName If the given name is not valid.
 */
void
HWOperation::setName(const std::string& name)
    throw (ComponentAlreadyExists, InvalidName) {

    string lowerName = StringTools::stringToLower(name);

    if (lowerName == this->name()) {
        return;
    }

    const string procName = "HWOperation::setName";

    if (parentUnit()->hasOperation(lowerName)) {
        MOMTextGenerator textGenerator;
        format text = textGenerator.text(
            MOMTextGenerator::TXT_OPERATION_EXISTS_BY_SAME_NAME);
        text % name % parentUnit()->name();
        throw ComponentAlreadyExists(
            __FILE__, __LINE__, procName, text.str());
    }

    if (!MachineTester::isValidComponentName(lowerName)) {
        MOMTextGenerator textGen;
        format errorMsg = textGen.text(MOMTextGenerator::TXT_INVALID_NAME);
        errorMsg % lowerName;
        throw InvalidName(__FILE__, __LINE__, procName, errorMsg.str());
    }

    name_ = lowerName;
}


/**
 * Returns the parent unit of the operation.
 *
 * @return The parent unit of the operation.
 */
FunctionUnit*
HWOperation::parentUnit() const {
    return parent_;
}


/**
 * Returns the pipeline used by this operation.
 *
 * @return The pipeline used by this operation.
 */
ExecutionPipeline*
HWOperation::pipeline() const {
    return pipeline_;
}


/**
 * Returns the number of cycles used during execution of the operation.
 *
 * Different results may have different latencies, thus this is the longest 
 * latency of any result produced by the operation. After this latency the
 * operation execution has been finished and no results will be produced.
 *
 * @return The latency of the operation.
 */
int
HWOperation::latency() const {
    return pipeline_->latency();
}


/**
 * Returns the latency for the given output.
 *
 * @param output The number of the output.
 * @return The latency for the given output.
 * @exception IllegalParameters If the given output is not written in the
 *                              pipeline.
 */
int
HWOperation::latency(int output) const 
    throw (IllegalParameters) {

    return pipeline_->latency(output);
}


/**
 * Returns the slack of the given input.
 *
 * The slack tells how many cycles AFTER the trigger, opcode-setting move is
 * scheduled, can the operand be scheduled (and still affect correctly the
 * result of the operation).
 *
 * @param input The number of the input.
 * @return The slack of the given input.
 * @exception IllegalParameters If the given input is not read in the 
 *                              pipeline.
 */
int
HWOperation::slack(int input) const 
    throw (IllegalParameters) {

    return pipeline_->slack(input);
}


/**
 * Binds the given operand of the operation to the given port of the
 * function unit.
 *
 * If the given operand is already bound to another port, the old binding is
 * replaced with the new one.
 *
 * @param operand Index of the operand.
 * @param port The port.
 * @exception IllegalRegistration If the given port does not belong to the
 *                                same function unit as this operation.
 * @exception ComponentAlreadyExists If the given port is already reserved
 *                                   for another operand.
 * @exception OutOfRange If the given operand is less than 1.
 */
void
HWOperation::bindPort(int operand, const FUPort& port)
    throw (IllegalRegistration, ComponentAlreadyExists, OutOfRange) {

    const string procName = "HWOperation::bindPort";

    if (operand < 1) {
        throw OutOfRange(__FILE__, __LINE__, procName);
    }

    if (port.parentUnit() != parentUnit()) {
        throw IllegalRegistration(__FILE__, __LINE__, procName);
    }

    if (MapTools::containsValue(operandBinding_, &port) &&
        this->HWOperation::port(operand) != &port) {
        throw ComponentAlreadyExists(__FILE__, __LINE__, procName);
    }

    operandBinding_[operand] = &port;
    port.updateBindingString();
}


/**
 * Unbinds the operand bound to the given port.
 *
 * @param port The port.
 */
void
HWOperation::unbindPort(const FUPort& port) {
    MapTools::removeItemsByValue(operandBinding_, &port);
    port.updateBindingString();
}


/**
 * Returns the port of the function unit that is bound to the given operand.
 *
 * Returns NULL if no port is bound to the given operand.
 *
 * @param operand Index of the operand.
 * @return The port or NULL.
 */
FUPort*
HWOperation::port(int operand) const {
    OperandBindingMap::const_iterator iter = operandBinding_.find(operand);
    if (iter != operandBinding_.end()) {
        return const_cast<FUPort*>((*iter).second);
    } else {
        return NULL;
    }
}


/**
 * Checks whether the given port is bound to one of the operands of the
 * operation.
 *
 * @param port The port.
 * @return True if the port is bound, otherwise false.
 */
bool
HWOperation::isBound(const FUPort& port) const {
    return MapTools::containsValue(operandBinding_, &port);
}


/**
 * Returns the index of the input or output that is bound to the given port.
 *
 * @param port The port.
 * @return The index of the input or output.
 * @exception InstanceNotFound If no io is bound to the given port.
 */
int
HWOperation::io(const FUPort& port) const
    throw (InstanceNotFound) {

    if (!isBound(port)) {
        const string procName = "HWOperation::operand";
        throw InstanceNotFound(__FILE__, __LINE__, procName);
    }

    return MapTools::keyForValue<int>(operandBinding_, &port);
}


/**
 * Saves the contents to an ObjectState tree.
 *
 * @return The newly created ObjectState tree.
 */
ObjectState*
HWOperation::saveState() const {

    ObjectState* operation = new ObjectState(OSNAME_OPERATION);
    operation->setAttribute(OSKEY_NAME, name());

    // save operand binding map
    for (OperandBindingMap::const_iterator iter = operandBinding_.begin();
         iter != operandBinding_.end(); iter++) {
        pair<int, const FUPort*> binding = *iter;
        ObjectState* bindingState = new ObjectState(OSNAME_OPERAND_BINDING);
        operation->addChild(bindingState);
        bindingState->setAttribute(OSKEY_OPERAND, binding.first);
        bindingState->setAttribute(OSKEY_PORT, binding.second->name());
    }

    // save pipeline
    operation->addChild(pipeline_->saveState());

    return operation;
}


/**
 * Loads the state of the operation from the given ObjectState tree.
 *
 * @param state The ObjectState tree.
 * @exception ObjectStateLoadingException If an error occurs while loading
 *                                        the state.
 */
void
HWOperation::loadState(const ObjectState* state)
    throw (ObjectStateLoadingException) {

    const string procName = "HWOperation::loadState";

    if (state->name() != OSNAME_OPERATION) {
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }

    MOMTextGenerator textGenerator;

    try {
        setName(state->stringAttribute(OSKEY_NAME));
    } catch (const Exception& exception) {
        throw ObjectStateLoadingException(
            __FILE__, __LINE__, procName, exception.errorMessage());
    }

    try {
        // load operand binding map
        operandBinding_.clear();
        for (int i = 0; i < state->childCount(); i++) {
            ObjectState* child = state->child(i);
            if (child->name() == OSNAME_OPERAND_BINDING) {
                int operand = child->intAttribute(OSKEY_OPERAND);
                string portName = child->stringAttribute(OSKEY_PORT);

                if (parentUnit()->hasOperationPort(portName)) {
                    FUPort* port = parentUnit()->operationPort(portName);
                    if (isBound(*port)) {
                        format text = textGenerator.text(
                            MOMTextGenerator::TXT_OPERAND_BOUND_TO_PORT);
                        text % name() % portName % parentUnit()->name();
                        throw ObjectStateLoadingException(
                            __FILE__, __LINE__, procName, text.str());
                    } else if (this->port(operand) != NULL) {
                        format text = textGenerator.text(
                            MOMTextGenerator::TXT_OPERAND_ALREADY_BOUND);
                        text % operand % portName % name() %
                            parentUnit()->name() %
                            this->port(operand)->name();
                        throw ObjectStateLoadingException(
                            __FILE__, __LINE__, procName, text.str());
                    } else {
                        bindPort(operand, *port);
                    }
                } else {
                    format text = textGenerator.text(
                        MOMTextGenerator::TXT_OPERATION_REF_LOAD_ERR_PORT);
                    text % portName % name() % parentUnit()->name();
                    throw ObjectStateLoadingException(
                        __FILE__, __LINE__, procName, text.str());
                }

            }
        }

        // load pipeline
        if (pipeline_ != NULL) {
            delete pipeline_;
        }
        ObjectState* pipelineState = state->childByName(
            ExecutionPipeline::OSNAME_PIPELINE);
        pipeline_ = new ExecutionPipeline(*this);
        pipeline()->loadState(pipelineState);

    } catch (const Exception& e) {
        throw ObjectStateLoadingException(
            __FILE__, __LINE__, procName, e.errorMessage());
    }
}

}
