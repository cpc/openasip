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
 * @file ICDecoderEstimatorPlugin.cc
 *
 * Implementation of ICDecoderEstimatorPlugin class.
 *
 * @author Pekka Jääskeläinen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "ICDecoderEstimatorPlugin.hh"
#include "ControlUnit.hh"
#include "FUPort.hh"
#include "SpecialRegisterPort.hh"
#include "HWOperation.hh"
#include "ExecutionPipeline.hh"

namespace CostEstimator {
/**
 * Constructor.
 *
 * @param name The name of the cost estimation plugin in the HDB.
 * @param dataSource The HDB that stores the cost estimation data of
 *                   the plugin. This reference can also be used to add new 
 *                   data, in case required by the algorithm.
 */
ICDecoderEstimatorPlugin::ICDecoderEstimatorPlugin(
    const std::string& name) :
    CostEstimationPlugin(name) {
}

/**
 * Destructor.
 */
ICDecoderEstimatorPlugin::~ICDecoderEstimatorPlugin() {
}

/**
 * Estimates the delay of the given path in the interconnection network.
 *
 * The estimated path starts from source socket and traverses through a bus 
 * to a target socket. The delay caused by reading and writing from ports are
 * not included in this value. The default implementation returns false to 
 * signal that the estimation functionality is not implemented by the plugin.
 *
 * @param hdbRegistry The registry for accessing HDBs.
 * @param path The path.
 * @param machineImplementation The implementation of the machine.
 * @param sourceSocketImplementation The implementation identification of
 *                                   the source socket.
 * @param busImplementation The implementation identification of the bus.
 * @param destinationSocketImplementation The implementation identification
 *                                        of the destination socket.
 * @param delay The calculated delay should be stored in this argument.
 * @return True in case the delay can be estimated, false if it cannot.
 */
bool 
ICDecoderEstimatorPlugin::estimateICDelayOfPath(
    HDB::HDBRegistry&,
    const TransportPath&,
    const IDF::MachineImplementation&,
    const IDF::SocketImplementationLocation&,
    const IDF::BusImplementationLocation&,
    const IDF::SocketImplementationLocation&,
        DelayInNanoSeconds&) {

    return false;
}

/**
 * Estimates the area of the IC of the given machine.
 *
 * @param hdbRegistry The registry for accessing HDBs.
 * @param machine Architecture of the machine.
 * @param machineImplementation The implementation of the machine.
 * @param area The calculated area should be stored in this argument.
 * @return True in case the area can be estimated, false if it cannot.
 */
bool 
ICDecoderEstimatorPlugin::estimateICArea(
    HDB::HDBRegistry&,
    const TTAMachine::Machine&,
    const IDF::MachineImplementation&,
    AreaInGates&) {

    return false;
}

/**
 * Estimates the energy consumed by the IC of the given machine by running
 * the given program.
 *
 * @param hdbRegistry The registry for accessing HDBs.
 * @param machine Architecture of the processor.
 * @param machineImplementation Implementation defintions of the processor.
 * @param traceDB The simulation trace database obtained from running the
 *                program.
 * @param energy The calculated energy should be stored in this argument.
 * @return True in case energy can be estimated, false if it cannot.
 */
bool 
ICDecoderEstimatorPlugin::estimateICEnergy(
    HDB::HDBRegistry&,
    const TTAMachine::Machine&,
    const IDF::MachineImplementation&,
    const TTAProgram::Program&,
    const ExecutionTrace&,
    EnergyInMilliJoules&) {

    return false;
}

/**
 * Generates a control unit.
 *
 * @return The control unit.
 */
TTAMachine::ControlUnit* 
ICDecoderEstimatorPlugin::generateControlUnit() {

    const std::string RA_PORT_NAME = "ra";
    const std::string OP_PORT_NAME = "pc";
    const std::string OPNAME_JUMP = "jump";
    const std::string OPNAME_CALL = "call";
    const std::string COMP_DEFAULT_NAME_GCU = "gcu";

    // Add default ports.
    TTAMachine::ControlUnit* gcu = new TTAMachine::ControlUnit(
        COMP_DEFAULT_NAME_GCU, 3, 1);
    
    TTAMachine::FUPort* opPort = 
        new TTAMachine::FUPort(OP_PORT_NAME, 32, *gcu, true, true);
    TTAMachine::SpecialRegisterPort* raPort =
        new TTAMachine::SpecialRegisterPort(RA_PORT_NAME, 32, *gcu);
    
    gcu->setReturnAddressPort(*raPort);
    
    // Add default operations.
    TTAMachine::HWOperation* jump = 
        new TTAMachine::HWOperation(OPNAME_JUMP, *gcu);
    jump->bindPort(1, *opPort);
    jump->pipeline()->addPortRead(1, 0, 1);

    TTAMachine::HWOperation* call = 
        new TTAMachine::HWOperation(OPNAME_CALL, *gcu);
    call->bindPort(1, *opPort);
    call->pipeline()->addPortRead(1, 0, 1);

    return gcu;
}

}

