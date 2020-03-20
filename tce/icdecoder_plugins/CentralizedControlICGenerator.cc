/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file CentralizedControlICGenerator.cc
 *
 * Implementation of CentralizedControlICGenerator class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include "CentralizedControlICGenerator.hh"
#include "NetlistPort.hh"
#include "MapTools.hh"
#include "Application.hh"

using namespace ProGe;
using namespace TTAMachine;

/**
 * The constructor.
 */
CentralizedControlICGenerator::CentralizedControlICGenerator() :
    glockPort_(0) {
}


/**
 * The destructor.
 */
CentralizedControlICGenerator::~CentralizedControlICGenerator() {
}


/**
 * Returns the short immediate data port for the given bus.
 *
 * @param busName Name of the bus.
 * @return The netlist port that was added by the IC generator.
 * @exception InstanceNotFound If the port was not created.
 */
ProGe::NetlistPort&
CentralizedControlICGenerator::simmDataPort(const std::string& busName) const {
    try {
        return *MapTools::valueForKey<NetlistPort*>(
            simmDataPortMap_, busName);
    } catch (const Exception&) {
        throw InstanceNotFound(__FILE__, __LINE__, __func__);
    }
}

/**
 * Returns the short immediate control port for the given bus.
 *
 * @param busName Name of the bus.
 * @return The netlist port that was added by the IC generator.
 * @exception InstanceNotFound If the port was not created.
 */
ProGe::NetlistPort&
CentralizedControlICGenerator::simmCntrlPort(const std::string& busName) const {
    try {
        return *MapTools::valueForKey<NetlistPort*>(
            simmCntrlPortMap_, busName);
    } catch (const Exception&) {
        throw InstanceNotFound(__FILE__, __LINE__, __func__);
    }
}

/**
 * Returns the bus control port of the given socket in the IC block.
 *
 * @param socketName Name of the socket.
 * @return The netlist port that was added by the IC generator.
 * @exception InstanceNotFound If the port was not created.
 */
ProGe::NetlistPort&
CentralizedControlICGenerator::busCntrlPortOfSocket(
    const std::string& socketName) const {
    try {
        return *MapTools::valueForKey<NetlistPort*>(busCntrlPortMap_, socketName);
    } catch (const Exception&) {
        throw InstanceNotFound(__FILE__, __LINE__, __func__);
    }
}

/**
 * Returns the data control port of the given socket in the IC block.
 *
 * @param socketName Name of the socket.
 * @return The netlist port that was added by the IC generator.
 * @exception InstanceNotFound If the port was not created.
 */
ProGe::NetlistPort&
CentralizedControlICGenerator::dataCntrlPortOfSocket(
    const std::string& socketName) const {
    try {
        return *MapTools::valueForKey<NetlistPort*>(dataCntrlPortMap_, socketName);
    } catch (const Exception&) {
        throw InstanceNotFound(__FILE__, __LINE__, __func__);
    }
}

/**
 * Returns true if IC has glock port.
 */
bool
CentralizedControlICGenerator::hasGlockPort() const {
    return (glockPort_ != 0);
}


/**
 * Returns reference to NetlistPort of glock.
 *
 * @exception InstanceNotFound If IC does not have glock port.
 */
ProGe::NetlistPort&
CentralizedControlICGenerator::glockPort() const {
    if (!hasGlockPort()) {
        throw InstanceNotFound(__FILE__, __LINE__, __func__,
            "IC does not have glock port.");
    } else {
        return *glockPort_;
    }
}

/**
 * Maps the given netlist port as the short immediate data port of the 
 * given bus.
 *
 * @param busName The bus.
 * @param port The netlist port.
 */
void
CentralizedControlICGenerator::mapSImmDataPort(
    const std::string& busName, 
    ProGe::NetlistPort& port) {

    assert(!MapTools::containsKey(simmDataPortMap_, busName));
    simmDataPortMap_.insert(
        std::pair<std::string, NetlistPort*>(busName, &port));
}


/**
 * Maps the given netlist port as the short immediate control port of the 
 * given bus.
 *
 * @param busName The bus.
 * @param port The netlist port.
 */
void
CentralizedControlICGenerator::mapSImmCntrlPort(
    const std::string& busName, 
    ProGe::NetlistPort& port) {

    assert(!MapTools::containsKey(simmCntrlPortMap_, busName));
    simmCntrlPortMap_.insert(
        std::pair<std::string, NetlistPort*>(busName, &port));
}


/**
 * Maps the given bus control port for the given socket.
 *
 * @param socketName Name of the socket.
 * @param port The netlist port.
 */
void
CentralizedControlICGenerator::mapBusCntrlPortOfSocket(
    const std::string& socketName,
    ProGe::NetlistPort& port) {

    assert(!MapTools::containsKey(busCntrlPortMap_, socketName));
    busCntrlPortMap_.insert(
        std::pair<std::string, NetlistPort*>(socketName, &port));
}


/**
 * Maps the given data control port for the given socket.
 *
 * @param socketName Name of the socket.
 * @param port The netlist port.
 */
void 
CentralizedControlICGenerator::mapDataCntrlPortOfSocket(
    const std::string& socketName,
    ProGe::NetlistPort& port) {

    assert(!MapTools::containsKey(dataCntrlPortMap_, socketName));
    dataCntrlPortMap_.insert(
        std::pair<std::string, NetlistPort*>(socketName, &port));
}    


/**
 * Sets optional glock port of IC.
 *
 * @param glockPort The glock port.
 */
void
CentralizedControlICGenerator::setGlockPort(ProGe::NetlistPort& glockPort) {
    assert(glockPort_ == 0);
    glockPort_ = &glockPort;
}

