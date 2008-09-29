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
CentralizedControlICGenerator::CentralizedControlICGenerator() {
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
CentralizedControlICGenerator::simmDataPort(
    const std::string& busName) const
    throw (InstanceNotFound) {

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
CentralizedControlICGenerator::simmCntrlPort(
    const std::string& busName) const 
    throw (InstanceNotFound) {

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
    const std::string& socketName) const 
    throw (InstanceNotFound) {

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
    const std::string& socketName) const
    throw (InstanceNotFound) {

    try {
        return *MapTools::valueForKey<NetlistPort*>(dataCntrlPortMap_, socketName);
    } catch (const Exception&) {
        throw InstanceNotFound(__FILE__, __LINE__, __func__);
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
