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
 * @file RFImplementation.cc
 *
 * Implementation of RFImplementation class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>

#include "RFImplementation.hh"
#include "RFEntry.hh"
#include "RFExternalPort.hh"
#include "RFPortImplementation.hh"
#include "BlockImplementationFile.hh"
#include "SequenceTools.hh"
#include "ContainerTools.hh"

using std::string;

namespace HDB {

/**
 * The constructor.
 *
 * @param moduleName Name of the module.
 * @param clkPort Name of the clock port.
 * @param rstPort Name of the reset port.
 * @param glockPort Name of the global lock port.
 * @param sizeParam Name of the parameter that defines the size of the 
 *                  register file.
 * @param widthParam Name of the parameter that defines the width of the
 *                   register file.
 * @param guardPort Name of the guard port.
 * @param sacParam Flag for separate address cycle. Default value is false.
 */
RFImplementation::RFImplementation(
    const std::string& moduleName,
    const std::string& clkPort,
    const std::string& rstPort,
    const std::string& glockPort,
    const std::string& sizeParam,
    const std::string& widthParam,
    const std::string& guardPort,
    bool sacParam) :
    HWBlockImplementation(moduleName, clkPort, rstPort, glockPort), 
    sizeParam_(sizeParam), widthParam_(widthParam), guardPort_(guardPort),
    sepAddrCycleParam_(sacParam) {
}


/**
 * Copy constructor.
 *
 * @param original RFImplementation to copy.
 */
RFImplementation::RFImplementation(const RFImplementation& original):
    HWBlockImplementation(original) {

    sizeParam_ = original.sizeParam_;
    widthParam_ = original.widthParam_;
    guardPort_ = original.guardPort_;
    sepAddrCycleParam_ = original.sepAddrCycleParam_;

    // Deep copy ports.
    for (int i = 0; i < original.portCount(); i++) {
        RFPortImplementation* p = new RFPortImplementation(original.port(i));
        addPort(p);
    }

    // Copy parameters.
    parameters_ = original.parameters_;

    // Deep copy external ports.
    for (int i = 0; i < original.externalPortCount(); i++) {
        RFExternalPort* p = new RFExternalPort(original.externalPort(i));
        addExternalPort(p);
    }

}


/**
 * The destructor.
 */
RFImplementation::~RFImplementation() {
    SequenceTools::deleteAllItems(ports_);
    SequenceTools::deleteAllItems(externalPorts_);
}

/**
 * Sets the name of the size parameter.
 *
 * @param sizeParam Name of the size parameter.
 */
void
RFImplementation::setSizeParameter(const std::string& sizeParam) {
    sizeParam_ = sizeParam;
}


/**
 * Returns the name of the size parameter.
 *
 * @return The name of the size parameter.
 */
std::string
RFImplementation::sizeParameter() const {
    return sizeParam_;
}


/**
 * Sets the name of the width parameter.
 *
 * @param widthParam Name of the width parameter.
 */
void
RFImplementation::setWidthParameter(const std::string& widthParam) {
    widthParam_ = widthParam;
}


/**
 * Returns the name of the width parameter.
 *
 * @return The name of the width parameter.
 */
std::string
RFImplementation::widthParameter() const {
    return widthParam_;
}


/**
 * Sets the name of the guard port.
 *
 * @param guardPort Name of the guard port.
 */
void
RFImplementation::setGuardPort(const std::string& guardPort) {
    guardPort_ = guardPort;
}


/**
 * Returns the name of the guard port.
 *
 * @return The name of the guard port.
 */
std::string
RFImplementation::guardPort() const {
    return guardPort_;
}

/**
 * Sets flag for separate address cycle.
 *
 * @param enable Flag value.
 */
void RFImplementation::setSeparateAddressCycleParameter(bool enable) {
    sepAddrCycleParam_ = enable;
}

/**
 * Returns flag for separate address cycle.
 *
 * @return The flag value. True if enabled.
 */
bool RFImplementation::separateAddressCycleParameter() const {
    return sepAddrCycleParam_;
}

/**
 * Adds a new port to the RF implementation.
 *
 * @param port The port to be added.
 */
void
RFImplementation::addPort(RFPortImplementation* port) {
    ports_.push_back(port);
}

/**
 * Adds a new external port to the RF implementation.
 *
 * @param extPort The external port to be added.
 */
void
RFImplementation::addExternalPort(RFExternalPort* extPort) {
    externalPorts_.push_back(extPort);
}


/**
 * Deletes the given port from the RF implementation.
 *
 * @param port The port to delete.
 * @exception InstanceNotFound If the given port is not in this RF
 *                             implementation.
 */
void
RFImplementation::deletePort(RFPortImplementation* port) {
    bool deleted = ContainerTools::deleteValueIfExists(ports_, port);
    if (!deleted) {
        throw InstanceNotFound(__FILE__, __LINE__, __func__);
    }
}

/**
 * Deletes the given external port.
 *
 * @param port The port to delete.
 * @exception InstanceNotFound If the given port is not in this RF
 *                             implementation.
 */
void
RFImplementation::deleteExternalPort(RFExternalPort* port) {
    bool removed = ContainerTools::deleteValueIfExists(externalPorts_, port);
    if (!removed) {
        throw InstanceNotFound(__FILE__, __LINE__, __func__);
    }
}

/**
 * Returns the number of ports.
 *
 * @return The number of ports.
 */
int
RFImplementation::portCount() const {
    return ports_.size();
}


/**
 * Returns the number of external ports.
 *
 * @return The number of external ports.
 */
int
RFImplementation::externalPortCount() const {
    return externalPorts_.size();

}


/**
 * Returns the port at the given position.
 *
 * @param index The position index.
 * @return The port.
 * @exception OutOfRange If the index is negative or not smaller than the
 *                       number of ports.
 */
RFPortImplementation&
RFImplementation::port(int index) const {
    if (index < 0 || index >= portCount()) {
        const string procName = "RFImplementation::port";
        throw OutOfRange(__FILE__, __LINE__, procName);
    }

    return *ports_[index];
}

/**
 * Returns the external port at the given position.
 *
 * @param index The position index.
 * @return The port.
 * @exception OutOfRange If the given index is negative or not smaller the
 *                       number of external ports.
 */
RFExternalPort&
RFImplementation::externalPort(int index) const {
    if (index < 0 || index >= externalPortCount()) {
        const string procName = "RFImplementation::externalPort";
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
 * @exception IllegalParameters If the RF implementation contains a
 *                              parameter with the same name already.
 */
void
RFImplementation::addParameter(
    const std::string& name, const std::string& type,
    const std::string& value) {
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
RFImplementation::removeParameter(const std::string& name) {
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
RFImplementation::parameterCount() const {
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
RFImplementation::Parameter
RFImplementation::parameter(int index) const {
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
RFImplementation::hasParameter(const std::string& name) const {
    for (ParameterTable::const_iterator iter = parameters_.begin();
        iter != parameters_.end(); iter++) {
        if (iter->name == name) {
            return true;
        }
    }
    return false;
}

}



