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
 * @file FUImplementation.cc
 *
 * Implementation of FUImplementation class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>
#include <algorithm>

#include "FUImplementation.hh"
#include "FUEntry.hh"
#include "FUExternalPort.hh"
#include "FUPortImplementation.hh"
#include "BlockImplementationFile.hh"

#include "SequenceTools.hh"
#include "MapTools.hh"
#include "ContainerTools.hh"
#include "StringTools.hh"
#include "MathTools.hh"

using std::string;

namespace HDB {

/**
 * The constructor.
 *
 * @param name Name of the module.
 * @param opcodePort Name of the opcode port.
 * @param clkPort Name of the clock port.
 * @param rstPort Name of the reset port.
 * @param glockPort Name of the global lock port.
 * @param glockReqPort Name of the global lock request port.
 */
FUImplementation::FUImplementation(
    const std::string& name,
    const std::string& opcodePort,
    const std::string& clkPort,
    const std::string& rstPort,
    const std::string& glockPort,
    const std::string& glockReqPort) :
    HWBlockImplementation(name, clkPort, rstPort, glockPort),
    opcodePort_(opcodePort), glockReqPort_(glockReqPort) {
}


/**
 * Copy constructor.
 *
 * @param original FUImplementation to copy.
 */
FUImplementation::FUImplementation(const FUImplementation& original) :
    HWBlockImplementation(original) {

    opcodePort_ = original.opcodePort_;
    glockReqPort_ = original.glockReqPort_;
    opcodes_ = original.opcodes_;
    parameters_ = original.parameters_;

    // Deep copy architecture ports.
    for (int i = 0; i < original.architecturePortCount(); i++) {

        FUPortImplementation* portCopy =
            new FUPortImplementation(original.architecturePort(i));

        addArchitecturePort(portCopy);
    }

    // Deep copy external ports.
    for (int i = 0; i < original.externalPortCount(); i++) {

        FUExternalPort* portCopy =
            new FUExternalPort(original.externalPort(i));

        addExternalPort(portCopy);
    }
}



/**
 * The destructor.
 *
 * Deletes all the ports.
 */
FUImplementation::~FUImplementation() {
    SequenceTools::deleteAllItems(externalPorts_);
    SequenceTools::deleteAllItems(ports_);
}

/**
 * Sets the name of the opcode port.
 *
 * @param name Name of the port.
 */
void
FUImplementation::setOpcodePort(const std::string& name) {
    opcodePort_ = name;
}


/**
 * Returns the name of the opcode port.
 *
 * @return The name of the opcode port.
 */
std::string
FUImplementation::opcodePort() const {
    return opcodePort_;
}


/**
 * Sets the name of the global lock request port.
 *
 * @param name Name of the port.
 */
void
FUImplementation::setGlockReqPort(const std::string& name) {
    glockReqPort_ = name;
}


/**
 * Returns the name of the global lock request port.
 *
 * @return The name of the global lock request port.
 */
std::string
FUImplementation::glockReqPort() const {
    return glockReqPort_;
}


/**
 * Sets operation code for the given operation.
 *
 * @param operation Name of the operation.
 * @param opcode The operation code.
 */
void
FUImplementation::setOpcode(const std::string& operation, int opcode) {
    unsetOpcode(operation);
    string opName = StringTools::stringToUpper(operation);
    opcodes_.insert(std::pair<std::string, int>(opName, opcode));
}


/**
 * Unsets the opcode of the given operation.
 *
 * @param operation Name of the operation.
 */
void
FUImplementation::unsetOpcode(const std::string& operation) {
    string opName = StringTools::stringToUpper(operation);
    opcodes_.erase(opName);
}


/**
 * Returns the number of opcodes in the FU.
 *
 * @return The number of opcodes.
 */
int
FUImplementation::opcodeCount() const {
    return opcodes_.size();
}


/**
 * By the given index, returns an operation that has an opcode.
 *
 * @param index The index.
 * @exception OutOfRange If the given index is negative or not smaller than
 *                       the number of opcodes.
 */
std::string
FUImplementation::opcodeOperation(int index) const
    throw (OutOfRange) {

    if (index < 0 || index >= opcodeCount()) {
        throw OutOfRange(__FILE__, __LINE__, __func__);
    }

    OpcodeMap::const_iterator iter = opcodes_.begin();
    for (int i = 0; i < index; i++) {
        iter++;
    }

    return iter->first;
}


/**
 * Tells whether there is an opcode defined for the given operation.
 *
 * @param operation Name of the operation.
 * @return True if there is an opcode, otherwise false.
 */
bool
FUImplementation::hasOpcode(const std::string& operation) const {
    string opName = StringTools::stringToUpper(operation);
    return MapTools::containsKey(opcodes_, opName);
}


/**
 * Returns the opcode of the given operation.
 *
 * @param operation The operation.
 * @return The operation code.
 * @exception KeyNotFound If there is no opcode defined for the given 
 *                             operation.
 */
int
FUImplementation::opcode(const std::string& operation) const 
    throw (KeyNotFound) {

    string opName = StringTools::stringToUpper(operation);
    return MapTools::valueForKey<int>(opcodes_, opName);
}

/**
 * Returns the maximum bitwidth of an opcode in this FU implementation.
 *
 * The bitwidth is not stored explicitly in HDB, this method is provided
 * only for convenience.
 *
 * @return The maximum bitwidth needed by an opcode in this FU.
 *
 */
int 
FUImplementation::maxOpcodeWidth() const {
    int maxFound = 0;
    for (OpcodeMap::const_iterator i = opcodes_.begin(); i != opcodes_.end();
         ++i) {
        maxFound = std::max(MathTools::requiredBits((*i).second), maxFound);
    }
    return maxFound;
}

   

/**
 * Adds the given architectural port.
 *
 * @param port The port to be added.
 */
void
FUImplementation::addArchitecturePort(FUPortImplementation* port) {
    ports_.push_back(port);
}


/**
 * Deletes the given architectural port.
 *
 * @param port The port to delete.
 * @exception InstanceNotFound If the given port is not in this FU
 *                             implementation.
 */
void
FUImplementation::deleteArchitecturePort(FUPortImplementation* port) 
    throw (InstanceNotFound) {

    bool removed = ContainerTools::deleteValueIfExists(ports_, port);
    if (!removed) {
        throw InstanceNotFound(__FILE__, __LINE__, __func__);
    }
}


/**
 * Adds the given external port.
 *
 * @param port The port to be added.
 */
void
FUImplementation::addExternalPort(FUExternalPort* port) {
    externalPorts_.push_back(port);
}


/**
 * Deletes the given external port.
 *
 * @param port The port to delete.
 * @exception InstanceNotFound If the given port is not in this FU
 *                             implementation.
 */
void
FUImplementation::deleteExternalPort(FUExternalPort* port)
    throw (InstanceNotFound) {

    bool removed = ContainerTools::deleteValueIfExists(externalPorts_, port);
    if (!removed) {
        throw InstanceNotFound(__FILE__, __LINE__, __func__);
    }
}


/**
 * Returns the number of architectural ports.
 *
 * @return The number of ports.
 */
int
FUImplementation::architecturePortCount() const {
    return ports_.size();
}


/**
 * Returns the number of external ports.
 *
 * @return The number of external ports.
 */
int
FUImplementation::externalPortCount() const {
    return externalPorts_.size();
}


/**
 * Returns the architectural port implementation at the given position.
 *
 * @param index The position index.
 * @return The port implementation.
 * @exception OutOfRange If the given index is negative or not smaller the
 *                       number of architectural ports.
 */
FUPortImplementation&
FUImplementation::architecturePort(int index) const
    throw (OutOfRange) {

    if (index < 0 || index >= architecturePortCount()) {
        const string procName = "FUImplementation::architecturePort";
        throw OutOfRange(__FILE__, __LINE__, procName);
    }

    return *ports_[index];
}

/**
 * Returns the implementation of a port for the given port architecture
 * name.
 *
 * @param architectureName Name of the port in architecture table.
 * @return The port implementation.
 * @exception InstanceNotFound If no port with the given architecture name
 *                             is found.
 */
FUPortImplementation& 
FUImplementation::portImplementationByArchitectureName(
    const std::string& architectureName) const
    throw (InstanceNotFound) {
    for (int i = 0; i < architecturePortCount(); ++i) {
        if (ports_[i]->architecturePort() == architectureName)
            return *ports_[i];
    }
    throw InstanceNotFound(
        __FILE__, __LINE__, __func__, 
        "No port implementation with the given architecture name found.");
}


/**
 * Returns the external port at the given position.
 *
 * @param index The position index.
 * @return The port.
 * @exception OutOfRange If the given index is negative or not smaller the
 *                       number of external ports.
 */
FUExternalPort&
FUImplementation::externalPort(int index) const
    throw (OutOfRange) {

    if (index < 0 || index >= externalPortCount()) {
        const string procName = "FUImplementation::externalPort";
        throw OutOfRange(__FILE__, __LINE__, procName);
    }

    return *externalPorts_[index];
}


/**
 * Adds the given parameter for the implementation.
 *
 * @param name Name of the parameter.
 * @param type Type of the parameter.
 * @param value Value of the parameter.
 * @exception IllegalParameters If the FU implementation contains a
 *                              parameter with the same name already.
 */
void
FUImplementation::addParameter(
    const std::string& name,
    const std::string& type,
    const std::string& value) 
    throw (IllegalParameters) {

    if (hasParameter(name)) {
        throw IllegalParameters(__FILE__, __LINE__, __func__);
    } else {
        Parameter param = {name, type, value};
        parameters_.push_back(param);
    }
}


/**
 * Removes the parameter of the given name.
 *
 * @param name Name of the parameter.
 */
void
FUImplementation::removeParameter(const std::string& name) {
    for (ParameterTable::iterator iter = parameters_.begin();
         iter != parameters_.end(); iter++) {
        if (iter->name == name) {
            parameters_.erase(iter);
            return;
        }
    }
}
    

/**
 * Returns the number of parameters.
 *
 * @return The number of parameters.
 */
int
FUImplementation::parameterCount() const {
    return parameters_.size();
}


/**
 * Returns a parameter by the given index.
 *
 * @param index The index.
 * @return The parameter.
 * @exception OutOfRange If the index is negative or not smaller than the
 *                       number of parameters.
 */
FUImplementation::Parameter
FUImplementation::parameter(int index) const
    throw (OutOfRange) {
    
    if (index < 0 || index >= parameterCount()) {
        throw OutOfRange(__FILE__, __LINE__, __func__);
    }

    return parameters_[index];
}


/**
 * Tells whether the implementation has the given parameter.
 *
 * @param name Name of the parameter.
 * @return True if the implementation has the parameter, otherwise false.
 */
bool
FUImplementation::hasParameter(const std::string& name) const {
    for (ParameterTable::const_iterator iter = parameters_.begin();
         iter != parameters_.end(); iter++) {
        if (iter->name == name) {
            return true;
        }
    }
    return false;
}
}
