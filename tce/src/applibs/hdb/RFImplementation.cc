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
}


/**
 * The destructor.
 */
RFImplementation::~RFImplementation() {
    SequenceTools::deleteAllItems(ports_);
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
 * Deletes the given port from the RF implementation.
 *
 * @param port The port to delete.
 * @exception InstanceNotFound If the given port is not in this RF
 *                             implementation.
 */
void
RFImplementation::deletePort(RFPortImplementation* port)
    throw (InstanceNotFound) {

    bool deleted = ContainerTools::deleteValueIfExists(ports_, port);
    if (!deleted) {
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
 * Returns the port at the given position.
 *
 * @param index The position index.
 * @return The port.
 * @exception OutOfRange If the index is negative or not smaller than the
 *                       number of ports.
 */
RFPortImplementation&
RFImplementation::port(int index) const
    throw (OutOfRange) {

    if (index < 0 || index >= portCount()) {
        const string procName = "RFImplementation::port";
        throw OutOfRange(__FILE__, __LINE__, procName);
    }

    return *ports_[index];
}
}
