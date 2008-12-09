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
 * @file Estimator.hh
 *
 * Declaration of Estimator class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
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
