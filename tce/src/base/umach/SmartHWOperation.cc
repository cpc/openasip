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
 * @file SmartHWOperation.cc
 *
 * Implementation of SmartHWOperation class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: yellow
 */

#include "SmartHWOperation.hh"
#include "UniversalFunctionUnit.hh"
#include "Operation.hh"
#include "FUPort.hh"
#include "ExecutionPipeline.hh"
#include "TCEString.hh"

using std::string;
using namespace TTAMachine;

/**
 * The constructor.
 *
 * Creates the pipeline automatically.
 *
 * @param operation The operation to be constructed.
 * @param parent The parent unit of the operation.
 * @exception ComponentAlreadyExists If there is already an operation by the
 *                                   same name in the given function unit.
 * @exception InvalidName If the given operation does not have a valid name.
 */
SmartHWOperation::SmartHWOperation(
    const Operation& operation,
    UniversalFunctionUnit& parent)
    throw (ComponentAlreadyExists, InvalidName) :
    HWOperation(operation.name(), parent), operation_(operation) {

    is32BitOperation_ =
        UniversalFunctionUnit::is32BitOperation(operation.name());

    int inputs = operation.numberOfInputs();
    int outputs = operation.numberOfOutputs();

    // create pipeline
    ExecutionPipeline* pLine = pipeline();
    for (int operand = 1; operand <= inputs; operand++) {
        pLine->addPortRead(operand, 0, 1);
    }
    for (int operand = inputs + 1; operand <= inputs + outputs; operand++) {
        pLine->addPortWrite(operand, 0, 1);
    }
}


/**
 * The destructor.
 */
SmartHWOperation::~SmartHWOperation() {
}


/**
 * Aborts the program. It is not allowed to set the name of SmartHWOperation.
 * DO NOT CALL THIS METHOD!
 *
 * @param name Never used.
 * @exception ComponentAlreadyExists Never thrown.
 * @exception InvalidName Never thrown.
 */
void
SmartHWOperation::setName(const std::string&)
    throw (ComponentAlreadyExists, InvalidName) {

    const string procName = "SmartHWOperation::setName";
    const string errorMsg = "Tried to set name of SmartHWOperation!";
    Application::writeToErrorLog(__FILE__, __LINE__, procName, errorMsg);
    Application::abortProgram();
}


/**
 * Returns the port bound to the given operand. If the given operand is not
 * bound yet, it is automatically bound to a port.
 *
 * The port to which the operand will be bound depends on the bindings
 * of other operands. Input operands are bound to input ports and
 * output operands to output ports, of course. If all the input
 * operands are bound already except the given one, the given operand
 * will be bound to the operation code setting port.
 *
 * @param operand The operand.
 * @return The port to which the operand is bound.
 */
FUPort*
SmartHWOperation::port(int operand) const {

    // if the operand is bound already, just return the port
    if (HWOperation::port(operand) != NULL) {
        return HWOperation::port(operand);
    }

    bool input = true;

    // check whether the operand is input or output operand
    if (operand > operation_.numberOfInputs() +
        operation_.numberOfOutputs()) {
        return NULL;
    } else if (operand > operation_.numberOfInputs()) {
        input = false;
    }

    int bitWidth = is32BitOperation_ ? 32 : 64;
    int portCount = parentUnit()->portCount(bitWidth);

    if (input) {
        if (otherMandatoryInputsBound(operand)) {
            // bind to opcode port if other inputs are bound already
            FUPort* opcodePort;
            if (is32BitOperation_) {
                opcodePort = 
                    static_cast<FunctionUnit*>(parentUnit())->operationPort(
                        UniversalFunctionUnit::OC_SETTING_PORT_32);
            } else {
                opcodePort = 
                    static_cast<FunctionUnit*>(parentUnit())->operationPort(
                        UniversalFunctionUnit::OC_SETTING_PORT_64);
            }
            const_cast<SmartHWOperation*>(this)->
                HWOperation::bindPort(operand, *opcodePort);
            return opcodePort;

        } else {
            // bind to a non-opcode input port otherwise
            for (int i = 0; i < portCount; i++) {
                FUPort& port = parentUnit()->port(i, bitWidth);
                if (!isBound(port) && port.inputSocket() != NULL &&
                    !port.isOpcodeSetting()) {
                    const_cast<SmartHWOperation*>(this)->
                        HWOperation::bindPort(operand, port);
                    return &port;
                }
            }
        }

    } else {
        // bind to some output port
        for (int i = 0; i < portCount; i++) {
            FUPort& port = parentUnit()->port(i, bitWidth);
            if (!isBound(port) && port.outputSocket() != NULL) {
                const_cast<SmartHWOperation*>(this)->
                    HWOperation::bindPort(operand, port);
                return &port;
            }
        }
    }

    assert(false);

    // dummy return to avoid compilation warning
    return NULL;
}


/**
 * Returns the parent unit of the operation.
 *
 * @return The parent unit.
 */
UniversalFunctionUnit*
SmartHWOperation::parentUnit() const {
    FunctionUnit* parent = HWOperation::parentUnit();
    UniversalFunctionUnit* uFU =
        dynamic_cast<UniversalFunctionUnit*>(parent);
    assert(uFU != NULL);
    return uFU;
}


/**
 * Aborts the program. It is not allowed to bind ports manually. DO NOT
 * CALL THIS METHOD!
 *
 * @param operand Never used.
 * @param port Never used.
 * @exception IllegalRegistration Never thrown.
 * @exception ComponentAlreadyExists Never thrown.
 * @exception OutOfRange Never thrown.
 */
void
SmartHWOperation::bindPort(int, const FUPort&)
    throw (IllegalRegistration, ComponentAlreadyExists, OutOfRange) {

    const string procName = "SmartHWOperation::bindPort";
    const string errorMsg =
        "Tried to bind port of SmartHWOperation manually!";
    Application::writeToErrorLog(__FILE__, __LINE__, procName, errorMsg);
    Application::abortProgram();
}


/**
 * Aborts the program. It is not allowed to unbind ports manually. DO NOT
 * CALL THIS METHOD!
 *
 * @param port Never used.
 */
void
SmartHWOperation::unbindPort(const FUPort&) {
    const string procName = "SmartHWOperation::unbindPort";
    const string errorMsg =
        "Tried unbind port of SmartHWOperation manually!";
    Application::writeToErrorLog(__FILE__, __LINE__, procName, errorMsg);
    Application::abortProgram();
}


/**
 * Aborts the program. It is not allowed to load state of SmartHWOperation
 * from an ObjectState tree. DO NOT CALL THIS METHOD!
 *
 * @param state Never used.
 * @exception ObjectStateLoadingException Never thrown.
 */
void
SmartHWOperation::loadState(const ObjectState*)
    throw (ObjectStateLoadingException) {

    const string procName = "SmartHWOperation::loadState";
    const string errorMsg =
        "Tried load state of SmartHWOperation from an ObjectState tree!";
    Application::writeToErrorLog(__FILE__, __LINE__, procName, errorMsg);
    Application::abortProgram();
}


/**
 * Tells whether other input operands except the given one are bound to some
 * port.
 *
 * @param operand The operand.
 * @return True if other inputs are bound, otherwise false.
 */
bool
SmartHWOperation::otherMandatoryInputsBound(int operand) const {

   assert(operand <= operation_.numberOfInputs());

   for (int opIndex = 1; opIndex <= operation_.numberOfInputs();
        opIndex++) {
       if (opIndex == operand) {
           continue;
       } else {
           if (HWOperation::port(opIndex) == NULL) {
               return false;
           }
       }
   }

   return true;
}
