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
 * @file Estimator.hh
 *
 * Declaration of Estimator class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_COST_ESTIMATOR_HH
#define TTA_COST_ESTIMATOR_HH

#include <set>

#include "Exception.hh"
#include "CostEstimatorTypes.hh"
#include "FUImplementationLocation.hh"
#include "RFImplementationLocation.hh"
#include "SocketImplementationLocation.hh"
#include "BusImplementationLocation.hh"
#include "MachineImplementation.hh"
#include "FUCostEstimationPluginRegistry.hh"
#include "RFCostEstimationPluginRegistry.hh"
#include "ICDecoderCostEstimationPluginRegistry.hh"
#include "TransportPath.hh"
#include "FUCostEstimationPlugin.hh"

class ExecutionTrace;
class UtilizationStats;
class FUCostEstimationPlugin;
class RFCostEstimationPlugin;

namespace TTAMachine {
    class Machine;
    class FunctionUnit;
    class BaseRegisterFile;
    class FUPort;
    class RFPort;
    class Port;
}

namespace TTAProgram {
    class Program;
}

namespace CostEstimator {

/**
 * The frontend to hardware cost estimation functionality.
 *
 * This class is used by clients that need to estimate area, energy, or
 * delay of a whole TTA processor or parts of it. 
 *
 * The estimation functions of this interface throw CannotEstimateCost,
 * because it really is an exceptional situation when this class
 * cannot estimate, because it's client's responsibility to make sure
 * all plugins etc. can be found for estimating the machine. In case
 * of single plugins, the situation is not exceptional, but they provide
 * estimation as a "capability", thus they signal explicitly whether they
 * can estimate a cost or not by returning true or false, respectively.
 */
class Estimator {
public:
    Estimator();
    virtual ~Estimator();

    /// area estimation functions

    AreaInGates totalArea(
        const TTAMachine::Machine& machine,
        const IDF::MachineImplementation& machineImplementation)
        throw (CannotEstimateCost);

    AreaInGates totalAreaOfFunctionUnits(
        const TTAMachine::Machine& machine,
        const IDF::MachineImplementation& machineImplementation)
        throw (CannotEstimateCost);

    AreaInGates totalAreaOfRegisterFiles(
        const TTAMachine::Machine& machine,
        const IDF::MachineImplementation& machineImplementation)
        throw (CannotEstimateCost);

    AreaInGates icArea(
        const TTAMachine::Machine& machine,
        const IDF::MachineImplementation& machineImplementation)
        throw (CannotEstimateCost);

    AreaInGates functionUnitArea(
        const TTAMachine::FunctionUnit& architecture,
        const IDF::FUImplementationLocation& implementationEntry) 
        throw (CannotEstimateCost);

    AreaInGates registerFileArea(
        const TTAMachine::BaseRegisterFile& architecture,
        const IDF::RFImplementationLocation& implementationEntry) 
        throw (CannotEstimateCost);

    /// energy estimation functions

    EnergyInMilliJoules totalEnergy(
        const TTAMachine::Machine& machine,
        const IDF::MachineImplementation& machineImplementation,
        const TTAProgram::Program& program,
        const ExecutionTrace& traceDB)
        throw (CannotEstimateCost);

    EnergyInMilliJoules icEnergy(
        const TTAMachine::Machine& machine,
        const IDF::MachineImplementation& machineImplementation,
        const TTAProgram::Program& program,
        const ExecutionTrace& traceDB)
        throw (CannotEstimateCost);

    EnergyInMilliJoules functionUnitEnergy(
        const TTAMachine::FunctionUnit& architecture,
        const IDF::FUImplementationLocation& implementationEntry,
        const TTAProgram::Program& program,
        const ExecutionTrace& traceDB)
        throw (CannotEstimateCost);

    EnergyInMilliJoules registerFileEnergy(
        const TTAMachine::BaseRegisterFile& architecture,
        const IDF::RFImplementationLocation& implementationEntry,
        const TTAProgram::Program& program,
        const ExecutionTrace& traceDB)
        throw (CannotEstimateCost);

    EnergyInMilliJoules totalEnergyOfFunctionUnits(
        const TTAMachine::Machine& machine,
        const IDF::MachineImplementation& machineImplementation,
        const TTAProgram::Program& program,
        const ExecutionTrace& traceDB)
        throw (CannotEstimateCost);

    EnergyInMilliJoules totalEnergyOfRegisterFiles(
        const TTAMachine::Machine& machine,
        const IDF::MachineImplementation& machineImplementation,
        const TTAProgram::Program& program,
        const ExecutionTrace& traceDB)
        throw (CannotEstimateCost);

    /// delay estimation functions

    DelayInNanoSeconds longestPath(
        const TTAMachine::Machine& machine,
        const IDF::MachineImplementation& machineImplementation)
        throw (CannotEstimateCost, Exception);

    DelayInNanoSeconds decompressorDelay(
        const TTAMachine::Machine& machine,
        const IDF::MachineImplementation& machineImplementation)
        throw (CannotEstimateCost);    

    DelayInNanoSeconds functionUnitPortWriteDelay(
        const TTAMachine::FUPort& port,
        const IDF::FUImplementationLocation& implementationEntry) 
        throw (CannotEstimateCost);

    DelayInNanoSeconds functionUnitPortReadDelay(
        const TTAMachine::FUPort& port,
        const IDF::FUImplementationLocation& implementationEntry) 
        throw (CannotEstimateCost);

    DelayInNanoSeconds functionUnitMaximumComputationDelay(
        const TTAMachine::FunctionUnit& architecture,
        const IDF::FUImplementationLocation& implementation) 
        throw (CannotEstimateCost);

    DelayInNanoSeconds registerFileMaximumComputationDelay(
        const TTAMachine::BaseRegisterFile& architecture,
        const IDF::RFImplementationLocation& implementationEntry) 
        throw (CannotEstimateCost);

    DelayInNanoSeconds registerFilePortWriteDelay(
        const TTAMachine::RFPort& port,
        const IDF::RFImplementationLocation& implementationEntry) 
        throw (CannotEstimateCost);

    DelayInNanoSeconds registerFilePortReadDelay(
        const TTAMachine::RFPort& port,
        const IDF::RFImplementationLocation& implementationEntry) 
        throw (CannotEstimateCost);

private:
    static TransportPathList* findAllICPaths(
        const TTAMachine::Machine& machine)
        throw (IllegalMachine);

    DelayInNanoSeconds estimateSocketToSocketDelayOfPath(
        const std::string pluginPath,
        const std::string pluginName,
        const TransportPath& path,
        const IDF::MachineImplementation& machineImplementation,
        const IDF::SocketImplementationLocation& sourceSocketImplementation,
        const IDF::BusImplementationLocation& busImplementation,
        const IDF::SocketImplementationLocation& 
            destinationSocketImplementation)
        throw (CannotEstimateCost);

    FUCostEstimationPlugin& fuCostFunctionPluginOfImplementation(
        const IDF::FUImplementationLocation& implementation)
        throw (Exception);

    RFCostEstimationPlugin& rfCostFunctionPluginOfImplementation(
        const IDF::RFImplementationLocation& implementation)
        throw (Exception);

    /// all accessed FU estimation plugins are stored in this registry
    FUCostEstimationPluginRegistry fuEstimatorPluginRegistry_;
    /// all accessed RF estimation plugins are stored in this registry
    RFCostEstimationPluginRegistry rfEstimatorPluginRegistry_;
    /// all accessed IC&decoder plugins are stored in this registry
    ICDecoderCostEstimationPluginRegistry icDecoderEstimatorPluginRegistry_;
};

}

#endif
