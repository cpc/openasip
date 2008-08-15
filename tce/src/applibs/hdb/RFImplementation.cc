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
 * @file RFImplementation.cc
 *
 * Implementation of RFImplementation class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
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
 */
RFImplementation::RFImplementation(
    const std::string& moduleName,
    const std::string& clkPort,
    const std::string& rstPort,
    const std::string& glockPort,
    const std::string& sizeParam,
    const std::string& widthParam,
    const std::string& guardPort) :
    HWBlockImplementation(moduleName, clkPort, rstPort, glockPort), 
    sizeParam_(sizeParam), widthParam_(widthParam), guardPort_(guardPort) {
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
