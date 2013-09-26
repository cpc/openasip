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
 * @file UniversalFunctionUnit.cc
 *
 * Implementation of UniversalFunctionUnit class.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen-no.spam-tut.fi)
 * @author Pekka J‰‰skel‰inen 2006 (pekka.jaaskelainen-no.spam-tut.fi)
 * @note rating: yellow
 */

#include "UniversalFunctionUnit.hh"
#include "UniversalMachine.hh"
#include "UniversalFUPort.hh"
#include "HWOperation.hh"
#include "SmartHWOperation.hh"
#include "OperationPool.hh"
#include "Operation.hh"
#include "TCEString.hh"
#include "Conversion.hh"

using std::string;
using namespace TTAMachine;

const string UniversalFunctionUnit::OC_SETTING_PORT_32 = "OCSetting32";
const string UniversalFunctionUnit::OC_SETTING_PORT_64 = "OCSetting64";
const string UniversalFunctionUnit::OPERATIONS_OF_32_BITS[] = {
    "ADD", "SUB", "LDW", "LDQ", "LDH", "STW", "STQ", "STH", "EQ", "GT",
    "GTU", "SHL", "SHR", "SHRU", "AND", "IOR", "XOR", "SYS", "JUMP", "CALL",
    "MIN", "MAX", "MINU", "MAXU", "SXQW", "SXHW", "NEG", "MUL", "DIV",
    "DIVU", "CFI", "CIF", "ROTL", "ROTR", "ABS", "LDQU", "LDHU"};


/**
 * The constructor.
 *
 * @param name Name of the function unit.
 * @param opPool The operation pool from which the operations are obtained on
 *               demand.
 * @exception InvalidName If the given name is not a valid component name.
 */
UniversalFunctionUnit::UniversalFunctionUnit(
    const std::string& name,
    OperationPool& opPool)
    throw (InvalidName) :
    FunctionUnit(name), opPool_(opPool) {
}


/**
 * The destructor.
 */
UniversalFunctionUnit::~UniversalFunctionUnit() {
}


/**
 * Tells whether the universal function unit has an operation of the given 
 * name. 
 *
 * Returns true even if the operation is not loaded yet but it exists in
 * the operation pool.
 *
 * @param name Name of the operation.
 */
bool
UniversalFunctionUnit::hasOperation(const std::string& name) const {
    return &opPool_.operation(name.c_str()) != &NullOperation::instance();
}


/**
 * Returns an operation by the given name.
 *
 * Adds the operation to the function unit if it doen't exist yet but
 * exist in the operation pool.
 *
 * @param name Name of the operation.
 * @exception InstanceNotFound If an operation by the given name does not
 *                             exist in the operation pool.
 * @return The operation.
 */
TTAMachine::HWOperation*
UniversalFunctionUnit::operation(const std::string& name) const
    throw (InstanceNotFound) {

    const string procName = "UniversalFunctionUnit::operation";

    if (FunctionUnit::hasOperation(name)) {
        return FunctionUnit::operation(name);
    } else {
        Operation& oper = opPool_.operation(name.c_str());

        if (&oper == &NullOperation::instance()) {
            throw InstanceNotFound(__FILE__, __LINE__, procName);
        } else {
            // need to take non-const pointer of this FU to call non-const
            // method addOperation
            UniversalFunctionUnit* thisFU =
                const_cast<UniversalFunctionUnit*>(this);
            SmartHWOperation& hwOper = thisFU->addSupportedOperation(oper);
            return &hwOper;
        }
    }
}


/**
 * Aborts the program. It should not be needed to add pipeline
 * elements to universal function unit.
 *
 * @param element Never used.
 * @exception ComponentAlreadyExists Never thrown.
 */
void
UniversalFunctionUnit::addPipelineElement(PipelineElement&)
    throw (ComponentAlreadyExists) {

    const string procName = "UniversalFunctionUnit::addPipelineElement";
    const string errorMsg =
        "Tried to add pipeline element to UniversalFunctionUnit!";
    Application::writeToErrorLog(__FILE__, __LINE__, procName, errorMsg);
    Application::abortProgram();
}


/**
 * Aborts the program. It is not allowed to load UniversalFunctionUnit from
 * an ObjectState instance. DO NOT CALL THIS METHOD!
 *
 * @param state Never used.
 * @exception ObjectStateLoadingException Never thrown.
 */
void
UniversalFunctionUnit::loadState(const ObjectState*)
    throw (ObjectStateLoadingException) {

    const string procName = "UniversalFunctionUnit::loadState";
    const string errorMsg =
        "Tried to load state of UniversalFunctionUnit from an ObjectState "
        "tree!";
    Application::writeToErrorLog(__FILE__, __LINE__, procName, errorMsg);
    Application::abortProgram();
}


/**
 * Returns the number of ports of the given bit width.
 *
 * @param width The bit width.
 * @return The number of ports of the given bit width.
 * @exception OutOfRange If the given bit width is out of range.
 */
int
UniversalFunctionUnit::portCountWithWidth(int width) const
    throw (OutOfRange) {

    if (width < 1) {
        const string procName = "UniversalFunctionUnit::portCount";
        throw OutOfRange(__FILE__, __LINE__, procName);
    }

    int count(0);

    for (int i = 0; i < FunctionUnit::portCount(); i++) {
        if (FunctionUnit::port(i)->width() == width) {
            count++;
        }
    }

    return count;
}


/**
 * By the given index, returns a port which has the given bit width.
 *
 * @param index The index.
 * @param width The bit width.
 * @return A port of the given bit width.
 * @exception OutOfRange If the given bit width or index is out of range.
 */
FUPort&
UniversalFunctionUnit::portWithWidth(int index, int width) const
    throw (OutOfRange) {

    if (width < 1 || index >= portCountWithWidth(width)) {
        const string procName = "UniversalFunctionUnit::port";
        throw OutOfRange(__FILE__, __LINE__, procName);
    }

    int count(0);
    for (int i = 0; i < FunctionUnit::portCount(); i++) {
        FUPort* port = 
            static_cast<const FunctionUnit*>(this)->operationPort(i);
        if (port->width() == width) {
            count++;
            if (count > index) {
                return *port;
            }
        }
    }

    assert(false);

    // dummy return to avoid compilation warning
    FUPort* port = NULL;
    return *port;
}


/**
 * Adds the given operation to the function unit.
 *
 * Adds input and/or output ports if there is not enough ports for the
 * operands of the given operation. Operand bindings and pipeline are not
 * created.
 *
 * @param operation The operation to be added.
 * @return The added operation.
 */
SmartHWOperation&
UniversalFunctionUnit::addSupportedOperation(const Operation& operation) {

    int inputCount = operation.numberOfInputs();
    int outputCount = operation.numberOfOutputs();

    int operandWidth = is32BitOperation(operation.name()) ? 32 : 64;

    ensureInputPorts(operandWidth, inputCount);
    ensureOutputPorts(operandWidth, outputCount);

    // create the operation
    SmartHWOperation* hwOper = NULL;
    try {
        hwOper = new SmartHWOperation(operation, *this);
        return *hwOper;
    } catch (const InvalidName& exception) {
        const string procName = "UniversalFunctionUnit::addOperation";
        const string errorMsg =
            "Operation pool contained an operation with illegal"
            "name: " + string(operation.name());
        Application::writeToErrorLog(__FILE__, __LINE__, procName, errorMsg);
        Application::abortProgram();
    } catch (...) {
        assert(false);
    }

    assert(false);

    // dummy return to avoid compilation warning
    return *hwOper;
}


/**
 * Ensures that there is at least the given number of input ports of the
 * given width.
 *
 * Adds input ports if there is not enough ports.
 *
 * @param width Bit width of the ports to be ensured.
 * @param count The number of ports to be ensured.
 */
void
UniversalFunctionUnit::ensureInputPorts(int width, int count) {

    assert(width == 32 || width == 64);

    Socket* inputSocket = machine()->socketNavigator().item(
        UM_INPUT_SOCKET_NAME);

    int portCount = this->portCountWithWidth(width);

    // create opcode setting port if there are no ports at all
    if (count > 0 && portCount == 0) {
        string portName =
            width == 32 ? OC_SETTING_PORT_32 : OC_SETTING_PORT_64;
        FUPort* ocPort = new UniversalFUPort(
            portName, width, *this, true, true);
        portCount++;
        ocPort->attachSocket(*inputSocket);
    }

    // check the amount of matches
    int matches(1);
    for (int i = 1; i < portCount; i++) {
        FUPort& port = this->portWithWidth(i, width);
        assert(port.width() == width);
        if (port.inputSocket() != NULL) {
            matches++;
        }
    }

    // create new ports as necessary
    while (matches < count) {
        string portName = "i" + Conversion::toString(width) + "_" +
            Conversion::toString(matches);
        FUPort* newPort = new UniversalFUPort(
            portName, width, *this, false, false);
        newPort->attachSocket(*inputSocket);
        matches++;
    }
}


/**
 * Ensures that there is at least the given number of output ports of the
 * given width.
 *
 * Adds output ports if there is not enough ports.
 *
 * @param width Bit width of the ports to be ensured.
 * @param count The number of ports to be ensured.
 */
void
UniversalFunctionUnit::ensureOutputPorts(int width, int count) {

    Socket* outputSocket = 
        machine()->socketNavigator().item(UM_OUTPUT_SOCKET_NAME);

    // check the amount of matches
    int matches(0);
    for (int i = 0; i < portCountWithWidth(width); i++) {
        FUPort& port = this->portWithWidth(i, width);
        assert(port.width() == width);
        if (port.outputSocket() != NULL) {
            matches++;
        }
    }

    // create new ports as necessary
    while (matches < count) {
        string portName = "output" + Conversion::toString(width) + "_" +
            Conversion::toString(matches + 1);
        FUPort* newPort = new UniversalFUPort(
            portName, width, *this, false, false);
        newPort->attachSocket(*outputSocket);
        matches++;
    }
}


/**
 * Tells whether operation of the given name has 32 bits wide operands and
 * return value.
 *
 * @param opName Name of the operation.
 * @return True if the operation has 32 bits wide operands and return value,
 *         otherwise false.
 */
bool
UniversalFunctionUnit::is32BitOperation(const std::string& opName) {
    int numberOfElements = sizeof(OPERATIONS_OF_32_BITS) / sizeof(string);
    for (int i = 0; i < numberOfElements; i++) {
        if (OPERATIONS_OF_32_BITS[i] == opName) {
            return true;
        }
    }
    return false;
}
