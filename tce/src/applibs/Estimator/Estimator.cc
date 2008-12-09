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
 * @file Estimator.cc
 *
 * Implementation of Estimator class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 *
 * @note rating: red
 */

#include <algorithm>
#include "boost/format.hpp"

#include "Estimator.hh"
#include "Application.hh"
#include "Machine.hh"
#include "FUCostEstimationPlugin.hh"
#include "NullFUImplementationLocation.hh"
#include "NullRFImplementationLocation.hh"
#include "NullUnitImplementationLocation.hh"
#include "MachineImplementation.hh"
#include "HDBManager.hh"
#include "HDBRegistry.hh"
#include "CostFunctionPlugin.hh"
#include "FUEntry.hh"
#include "RFEntry.hh"
#include "Segment.hh"
#include "FUPort.hh"
#include "RFPort.hh"
#include "ControlUnit.hh"
#include "FullyConnectedCheck.hh"
#include "MachineCheckResults.hh"

using namespace HDB;

namespace CostEstimator {
    
/**
 * Constructor.
 */
Estimator::Estimator() {
}
    
/**
 * Destructor.
 */
Estimator::~Estimator() {
}

/**
 * Estimates the total area of given machine.
 * 
 * Area estimate is calculated as a sum of areas of different machine parts.
 *
 * @param machine The machine architecture to estimate.
 * @param machineImplementation The machine implementation information. 
 * @exception CannotEstimateCost In case cost cannot be estimated for some 
 *                               reason. Reason is given in error message.
 */
AreaInGates 
Estimator::totalArea(
    const TTAMachine::Machine& machine,
    const IDF::MachineImplementation& machineImplementation)
    throw (CannotEstimateCost) {
    
    return totalAreaOfFunctionUnits(machine, machineImplementation) +
        totalAreaOfRegisterFiles(machine, machineImplementation) +
        icArea(machine, machineImplementation);
}

/**
 * Estimates the total area of all function units in the given machine.
 *
 * GCU is not treated as an FU. It should be estimated as a part of IC.
 *
 * @param machine The machine architecture to estimate.
 * @param machineImplementation The machine implementation information.
 * @exception CannotEstimateCost In case cost cannot be estimated for some 
 *                               reason. Reason is given in error message.
 */
AreaInGates 
Estimator::totalAreaOfFunctionUnits(
    const TTAMachine::Machine& machine,
    const IDF::MachineImplementation& machineImplementation)
    throw (CannotEstimateCost) {
    
    AreaInGates total = 0.0;
    TTAMachine::Machine::FunctionUnitNavigator nav = 
        machine.functionUnitNavigator();
    for (int i = 0; i < nav.count(); ++i) {
        TTAMachine::FunctionUnit* fuArchitecture = nav.item(i);
        IDF::FUImplementationLocation* fuImplementation = 
            &IDF::NullFUImplementationLocation::instance();
        
        if (machineImplementation.hasFUImplementation(
                fuArchitecture->name())) {
            fuImplementation = &machineImplementation.fuImplementation(
                fuArchitecture->name());
        }
        total += functionUnitArea(*fuArchitecture, *fuImplementation);
    }
    // GCU is not included in the MOM FU navigator, so we have to treat it
    // separately
    // GCU should be estimated as a part of IC.
    /*
    TTAMachine::FunctionUnit* fuArchitecture = machine.controlUnit();
    IDF::FUImplementationLocation* fuImplementation = 
        &IDF::NullFUImplementationLocation::instance();

    if (machineImplementation.hasFUImplementation(
            fuArchitecture->name())) {
        fuImplementation = &machineImplementation.fuImplementation(
            fuArchitecture->name());
    }
    total += functionUnitArea(*fuArchitecture, *fuImplementation);
    */
    return total;
}

/**
 * Estimates the total area of all register files in the given machine.
 *
 * @param machine The machine architecture to estimate.
 * @param machineImplementation The machine implementation information.
 * @exception CannotEstimateCost In case cost cannot be estimated for some 
 *                               reason. Reason is given in error message.
 */
AreaInGates 
Estimator::totalAreaOfRegisterFiles(
    const TTAMachine::Machine& machine,
    const IDF::MachineImplementation& machineImplementation)
    throw (CannotEstimateCost) {

    AreaInGates total = 0.0;
    TTAMachine::Machine::RegisterFileNavigator nav = 
        machine.registerFileNavigator();
    for (int i = 0; i < nav.count(); ++i) {
        TTAMachine::BaseRegisterFile* rfArchitecture = nav.item(i);
        IDF::RFImplementationLocation* rfImplementation = 
            &IDF::NullRFImplementationLocation::instance();

        if (machineImplementation.hasRFImplementation(
                rfArchitecture->name())) {
            rfImplementation = &machineImplementation.rfImplementation(
                rfArchitecture->name());
        }
        total += registerFileArea(*rfArchitecture, *rfImplementation);
    }
    return total;
}

/**
 * Estimates the area consumed by the interconnection network.
 *
 * @param machine The machine of which IC area to estimate.
 * @param machineImplementation The machine implementation information.
 * @exception CannotEstimateCost In case cost cannot be estimated for some 
 *                               reason. Reason is given in error message.
 */
AreaInGates 
Estimator::icArea(
    const TTAMachine::Machine& machine,
    const IDF::MachineImplementation& machineImplementation)
    throw (CannotEstimateCost) {

    if (!machineImplementation.hasICDecoderPluginName() ||
        !machineImplementation.hasICDecoderPluginFile() ||
        !machineImplementation.hasICDecoderHDB()) {
        throw CannotEstimateCost(
            __FILE__, __LINE__, __func__, 
            "Missing IC&decoder plugin information.");
    }        

    AreaInGates area = 0.0;
    try {

        ICDecoderEstimatorPlugin& plugin = 
            icDecoderEstimatorPluginRegistry_.plugin(
                machineImplementation.icDecoderPluginFile(),
                machineImplementation.icDecoderPluginName());

        if (!plugin.estimateICArea(
                HDBRegistry::instance(), machine, machineImplementation,
                area)) {
            throw CannotEstimateCost(
                __FILE__, __LINE__, __func__, 
                (boost::format(
                    "The IC&decoder estimator plugin '%s' unable to estimate "
                    "area of IC.") % 
                 machineImplementation.icDecoderPluginName()).str());
        }
    } catch (const Exception& e) {
        throw CannotEstimateCost(
            __FILE__, __LINE__, __func__, 
            std::string("Error while using ICDecoder estimation plugin. ") +
            e.errorMessage());
    }
    return area;
}

/**
 * Loads an FU cost estimation plugin for the given FU implementation.
 *
 * @param implementationEntry The implementation of the FU.
 * @return The plugin.
 * @exception Exception In case the plugin was not found or could not be
 *                      loaded.
 */
FUCostEstimationPlugin& 
Estimator::fuCostFunctionPluginOfImplementation(
    const IDF::FUImplementationLocation& implementationEntry)
    throw (Exception) {
 
    std::string pluginFileName = "";
    std::string pluginName = "";
    int pluginId = -1;
    HDB::HDBManager* theHDB = NULL;
    try {
        // use the HDB to find the estimation plugin from the plugin
        // registry
        theHDB = &HDBRegistry::instance().hdb(implementationEntry.hdbFile());
        HDB::FUEntry* fuEntry = theHDB->fuByEntryID(implementationEntry.id());

        if (fuEntry == NULL || !fuEntry->hasCostFunction()) {
            delete fuEntry;
            fuEntry = NULL;
            throw Exception(
                __FILE__, __LINE__, __func__, 
                (boost::format(
                    "Function unit entry %d does not have cost "
                    "estimation plugin set.") % 
                 implementationEntry.id()).str());
        }

        HDB::CostFunctionPlugin& pluginData = fuEntry->costFunction();
        pluginFileName = pluginData.pluginFilePath();
        pluginName = pluginData.name();
        pluginId = pluginData.id();

        try {
            return fuEstimatorPluginRegistry_.plugin(
                pluginFileName, pluginName);
        } catch (const Exception& e) {
            throw CannotEstimateCost(
                __FILE__, __LINE__, __func__, 
                std::string("Unable to open FU estimation plugin '") + 
                pluginFileName + "'. " + e.errorMessage());
        }
    } catch (const Exception& e) {
        throw Exception(
            __FILE__, __LINE__, __func__, e.errorMessage());
    }
    // avoid warnings with some compilers
    throw 1; 
}

/**
 * Loads a RF cost estimation plugin for the given RF implementation.
 *
 * @param implementationEntry The implementation of the RF.
 * @return The plugin.
 * @exception Exception In case the plugin was not found or could not be
 *                      loaded.
 */
RFCostEstimationPlugin& 
Estimator::rfCostFunctionPluginOfImplementation(
    const IDF::RFImplementationLocation& implementationEntry)
    throw (Exception) {
 
    std::string pluginFileName = "";
    std::string pluginName = "";
    int pluginId = -1;
    HDB::HDBManager* theHDB = NULL;
    try {
        // use the HDB to find the estimation plugin from the plugin
        // registry
        theHDB = &HDBRegistry::instance().hdb(implementationEntry.hdbFile());
        HDB::RFEntry* rfEntry = theHDB->rfByEntryID(implementationEntry.id());

        if (rfEntry == NULL || !rfEntry->hasCostFunction()) {
            delete rfEntry;
            rfEntry = NULL;
            throw Exception(
                __FILE__, __LINE__, __func__, 
                (boost::format(
                    "Register file entry %d does not have cost "
                    "estimation plugin set.") % implementationEntry.id()).str());
        }

        HDB::CostFunctionPlugin& pluginData = rfEntry->costFunction();
        pluginFileName = pluginData.pluginFilePath();
        pluginName = pluginData.name();
        pluginId = pluginData.id();

        try {
            return rfEstimatorPluginRegistry_.plugin(
                pluginFileName, pluginName);
        } catch (const Exception& e) {
            throw CannotEstimateCost(
                __FILE__, __LINE__, __func__, 
                std::string("Unable to open RF estimation plugin '") + 
                pluginFileName + "'. " + e.errorMessage());
        }
    } catch (const Exception& e) {
        throw Exception(
            __FILE__, __LINE__, __func__, e.errorMessage());
    }
    // avoid warnings with some compilers
    throw 1; 
}

/**
 * Estimates the area of the given function unit.
 *
 * @param architecture The FU architecture of which area to estimate.
 * @param implementationEntry The implementation information of FU. Can be
 *                       an instance of NullFUImplementationLocation.
 * @exception CannotEstimateCost In case the area could not be estimated. 
 * @return Estimate of area.
 */
AreaInGates 
Estimator::functionUnitArea(
    const TTAMachine::FunctionUnit& architecture,
    const IDF::FUImplementationLocation& implementationEntry) 
    throw (CannotEstimateCost) {

    try {
        AreaInGates area = 0.0;
        HDB::HDBManager& hdb = HDBRegistry::instance().hdb(
            implementationEntry.hdbFile());
        if (!fuCostFunctionPluginOfImplementation(implementationEntry).
            estimateArea(architecture, implementationEntry, area, hdb)) {
            throw CannotEstimateCost(
                __FILE__, __LINE__, __func__,
                std::string(
                    "Plugin was unable to estimate area of function unit '") +
                architecture.name() + ".");
        }
        return area;
    } catch (const Exception& e) {
        throw CannotEstimateCost(
            __FILE__, __LINE__, __func__,
            std::string("Unable to estimate area of function unit '") +
            architecture.name() + "'. " + e.errorMessage());
    }
}

/**
 * Estimates the area of the given register file.
 *
 * @param architecture The RF architecture of which area to estimate.
 * @param implementationEntry The implementation information of RF. Can be
 *                       an instance of NullRFImplementationLocation.
 * @exception CannotEstimateCost In case the area could not be estimated. 
 * @return Estimate of area.
 */
AreaInGates 
Estimator::registerFileArea(
    const TTAMachine::BaseRegisterFile& architecture,
    const IDF::RFImplementationLocation& implementationEntry) 
    throw (CannotEstimateCost) {

    try {
        AreaInGates area = 0.0;
        HDB::HDBManager& hdb = HDBRegistry::instance().hdb(
            implementationEntry.hdbFile());
        if (!rfCostFunctionPluginOfImplementation(implementationEntry).
            estimateArea(architecture, implementationEntry, area, hdb)) {
            throw CannotEstimateCost(
                __FILE__, __LINE__, __func__,
                std::string("Plugin was unable to estimate area."));
        }
        return area;
    } catch (const Exception& e) {
        throw CannotEstimateCost(
            __FILE__, __LINE__, __func__,
            std::string("Unable to estimate area of register file '") +
            architecture.name() + "'. " + e.errorMessage());
    }
    return 0.0;
}

/**
 * Estimates the total energy consumed by the given processor running the
 * given program.
 *
 * @param machine Architecture of the processor.
 * @param machineImplementation Implementation information of the processor.
 * @param traceDB The execution trace database obtained from simulating the
 *                program.
 * @return Energy in milli joules.
 * @exception CannotEstimateCost If the energy could not be estimated.
 */
EnergyInMilliJoules 
Estimator::totalEnergy(
    const TTAMachine::Machine& machine,
    const IDF::MachineImplementation& machineImplementation,
    const TTAProgram::Program& program,
    const ExecutionTrace& traceDB)
    throw (CannotEstimateCost) {

    return totalEnergyOfFunctionUnits(
        machine, machineImplementation, program, traceDB) +
        totalEnergyOfRegisterFiles(
            machine, machineImplementation, program, traceDB) +
        icEnergy(machine, machineImplementation, program, traceDB);
}

/**
 * Estimates the total energy consumed by the interconnection network
 * of the given machine running the given program.
 *
 * @param machine Architecture of the processor.
 * @param machineImplementation Implementation information of the processor.
 * @param traceDB The simulation trace database obtained from simulating the
 *                program.
 * @return Energy in milli joules.
 * @exception CannotEstimateCost If the energy could not be estimated.
 */
EnergyInMilliJoules 
Estimator::icEnergy(
    const TTAMachine::Machine& machine,
    const IDF::MachineImplementation& machineImplementation,
    const TTAProgram::Program& program,
    const ExecutionTrace& traceDB)
    throw (CannotEstimateCost) {

    if (!machineImplementation.hasICDecoderPluginName() ||
        !machineImplementation.hasICDecoderPluginFile() ||
        !machineImplementation.hasICDecoderHDB()) {
        throw CannotEstimateCost(
            __FILE__, __LINE__, __func__, 
            "Missing IC&decoder plugin information.");
    }        

    EnergyInMilliJoules energy = 0.0;
    try {

        ICDecoderEstimatorPlugin& plugin = 
            icDecoderEstimatorPluginRegistry_.plugin(
                machineImplementation.icDecoderPluginFile(),
                machineImplementation.icDecoderPluginName());

        if (!plugin.estimateICEnergy(
                HDBRegistry::instance(), machine, machineImplementation,
                program, traceDB, energy)) {
            throw CannotEstimateCost(
                __FILE__, __LINE__, __func__, 
                (boost::format(
                    "The IC&decoder estimator plugin '%s' could not estimate "
                    "energy for IC.") % 
                    machineImplementation.icDecoderPluginName()).str());

        }
    } catch (const Exception& e) {
        throw CannotEstimateCost(
            __FILE__, __LINE__, __func__, 
            std::string("Error while using ICDecoder estimation plugin. ") +
            e.errorMessage());
    }
    return energy;
}

/**
 * Estimates the energy consumed by the given FU when running the given
 * program.
 *
 * @param architecture The FU architecture of which area to estimate.
 * @param implementationEntry The implementation information of FU. Can be
 *                       an instance of NullFUImplementationLocation.
 * @param program The program of which energy to calculate.
 * @param traceDB The simulation trace database of the program running in the
 *                target architecture.
 * @return Estimate of consumed energy.
 * @exception CannotEstimateCost In case the energy could not be estimated. 
 */
EnergyInMilliJoules 
Estimator::functionUnitEnergy(
    const TTAMachine::FunctionUnit& architecture,
    const IDF::FUImplementationLocation& implementationEntry,
    const TTAProgram::Program& program,
    const ExecutionTrace& traceDB)
    throw (CannotEstimateCost) {  

    try {
        AreaInGates area = 0.0;
        HDB::HDBManager& hdb = HDBRegistry::instance().hdb(
            implementationEntry.hdbFile());
        if (!fuCostFunctionPluginOfImplementation(implementationEntry).
            estimateEnergy(
                architecture, implementationEntry, program, traceDB, area, 
                hdb)) {
            throw CannotEstimateCost(
                __FILE__, __LINE__, __func__,
                std::string("Plugin was unable to estimate energy."));
        }
        return area;
    } catch (const Exception& e) {
        throw CannotEstimateCost(
            __FILE__, __LINE__, __func__,
            std::string("Unable to estimate energy of function unit '") +
            architecture.name() + "'. " + e.errorMessage());
    }

    return 0.0;
}

/**
 * Estimates the energy consumed by the given register file when running 
 * the given program.
 *
 * @param architecture The RF architecture of which area to estimate.
 * @param implementationEntry The implementation information of RF. Can be
 *                       an instance of NullRFImplementationLocation.
 * @param program The program of which energy to calculate.
 * @param traceDB The simulation trace database of the program running in the
 *                target architecture.
 * @return Estimate of consumed energy.
 * @exception CannotEstimateCost In case the energy could not be estimated. 
 */
EnergyInMilliJoules 
Estimator::registerFileEnergy(
    const TTAMachine::BaseRegisterFile& architecture,
    const IDF::RFImplementationLocation& implementationEntry,
    const TTAProgram::Program& program,
    const ExecutionTrace& traceDB)
    throw (CannotEstimateCost) {

    try {
        AreaInGates area = 0.0;
        HDB::HDBManager& hdb = HDBRegistry::instance().hdb(
            implementationEntry.hdbFile());
        if (!rfCostFunctionPluginOfImplementation(implementationEntry).
            estimateEnergy(
                architecture, implementationEntry, program, traceDB, area,
                hdb)) {
            throw CannotEstimateCost(
                __FILE__, __LINE__, __func__,
                std::string("Plugin was unable to estimate energy."));
        }
        return area;
    } catch (const Exception& e) {
        throw CannotEstimateCost(
            __FILE__, __LINE__, __func__,
            std::string("Unable to estimate energy of register file '") +
            architecture.name() + "'. " + e.errorMessage());
    }
    return 0.0;
}

/**
 * Estimates the total energy consumed by all function units in the machine
 * (GCU not included).
 * 
 * Energy estimate is calculated as a sum of consumed energies of all FUs.
 *
 * @param machine The machine architecture to estimate.
 * @param machineImplementation The machine implementation information.
 * @param program The program of which energy to calculate.
 * @param traceDB The simulation trace database of the program running in the
 *                target architecture.
 * @exception CannotEstimateCost In case cost cannot be estimated for some 
 *                               reason. Reason is given in error message.
 */
EnergyInMilliJoules 
Estimator::totalEnergyOfFunctionUnits(
    const TTAMachine::Machine& machine,
    const IDF::MachineImplementation& machineImplementation,
    const TTAProgram::Program& program,
    const ExecutionTrace& traceDB)
    throw (CannotEstimateCost) {

    EnergyInMilliJoules total = 0.0;
    TTAMachine::Machine::FunctionUnitNavigator nav = 
        machine.functionUnitNavigator();
    for (int i = 0; i < nav.count(); ++i) {
        TTAMachine::FunctionUnit* fuArchitecture = nav.item(i);
        IDF::FUImplementationLocation* fuImplementation = 
            &IDF::NullFUImplementationLocation::instance();

        if (machineImplementation.hasFUImplementation(
                fuArchitecture->name())) {
            fuImplementation = &machineImplementation.fuImplementation(
                fuArchitecture->name());
        }
        total += functionUnitEnergy(
            *fuArchitecture, *fuImplementation, program, traceDB);
    }
    // GCU is not included in the MOM FU navigation, so we have to treat it
    // separately
    // GCU should be estimated by icdecoder
    /*
    TTAMachine::FunctionUnit* fuArchitecture = machine.controlUnit();
    IDF::FUImplementationLocation* fuImplementation = 
        &IDF::NullFUImplementationLocation::instance();

    if (machineImplementation.hasFUImplementation(
            fuArchitecture->name())) {
        fuImplementation = &machineImplementation.fuImplementation(
            fuArchitecture->name());
    }
    total += functionUnitEnergy(
        *fuArchitecture, *fuImplementation, program, traceDB);
    */
    return total;
}

/**
 * Estimates the total energy consumed by all register files in the machine.
 * 
 * Energy estimate is calculated as a sum of energies of RFs.
 *
 * @param machine The machine architecture to estimate.
 * @param machineImplementation The machine implementation information.
 * @param program The program of which energy to calculate.
 * @param traceDB The simulation trace database of the program running in the
 *                target architecture.
 * @exception CannotEstimateCost In case cost cannot be estimated for some 
 *                               reason. Reason is given in error message.
 */
EnergyInMilliJoules 
Estimator::totalEnergyOfRegisterFiles(
    const TTAMachine::Machine& machine,
    const IDF::MachineImplementation& machineImplementation,
    const TTAProgram::Program& program,
    const ExecutionTrace& traceDB)
    throw (CannotEstimateCost) {

    EnergyInMilliJoules total = 0.0;
    TTAMachine::Machine::RegisterFileNavigator nav = 
        machine.registerFileNavigator();
    for (int i = 0; i < nav.count(); ++i) {
        TTAMachine::BaseRegisterFile* rfArchitecture = nav.item(i);
        IDF::RFImplementationLocation* rfImplementation = 
            &IDF::NullRFImplementationLocation::instance();

        if (machineImplementation.hasRFImplementation(
                rfArchitecture->name())) {
            rfImplementation = &machineImplementation.rfImplementation(
                rfArchitecture->name());
        }
        total += registerFileEnergy(
            *rfArchitecture, *rfImplementation, program, traceDB);
    }
    return total;
}

/**
 * Estimates the input delay of the given function unit port.
 *
 * @param port The architecture of FU port of which area to estimate.
 * @param implementationEntry The implementation information of the FU the port 
 *                       belongs to.
 * @exception CannotEstimateCost In case the input delay could not be 
 *                               estimated. 
 * @return Estimate of input delay of the given FU port.
 */
DelayInNanoSeconds 
Estimator::functionUnitPortWriteDelay(
    const TTAMachine::FUPort& port,
    const IDF::FUImplementationLocation& implementationEntry) 
    throw (CannotEstimateCost) {

    try {
        DelayInNanoSeconds delay = 0.0;
        HDB::HDBManager& hdb = HDBRegistry::instance().hdb(
            implementationEntry.hdbFile());
        if (!fuCostFunctionPluginOfImplementation(implementationEntry).
            estimatePortWriteDelay(port, implementationEntry, delay, hdb)) {
            throw CannotEstimateCost(
                __FILE__, __LINE__, __func__,
                (boost::format(
                    "Plugin was unable to estimate input delay of port "
                    "%s::%s.") % port.parentUnit()->name() % port.name()).
                str());
        }
        return delay;
    } catch (const Exception& e) {
        throw CannotEstimateCost(
            __FILE__, __LINE__, __func__,
            std::string(
                "Input delay estimation could not be done for the given "
                "function unit entry. ") +
            e.errorMessage());
    }
    return 0.0;
}

/**
 * Estimates the output delay of the given function unit port.
 *
 * @param port The architecture of the FU port of which area to estimate.
 * @param implementationEntry The implementation information of the FU port
 * belongs to.
 * @exception CannotEstimateCost In case the output delay could not be 
 * estimated. 
 * @return Estimate of output delay of the given FU port.
 */
DelayInNanoSeconds 
Estimator::functionUnitPortReadDelay(
    const TTAMachine::FUPort& port,
    const IDF::FUImplementationLocation& implementationEntry) 
    throw (CannotEstimateCost) {

    try {
        DelayInNanoSeconds delay = 0.0;
        HDB::HDBManager& hdb = HDBRegistry::instance().hdb(
            implementationEntry.hdbFile());
        if (!fuCostFunctionPluginOfImplementation(implementationEntry).
            estimatePortReadDelay(port, implementationEntry, delay, hdb)) {
            throw CannotEstimateCost(
                __FILE__, __LINE__, __func__,
                (boost::format(
                    "Plugin was unable to estimate output delay of port "
                    "%s::%s.") % port.parentUnit()->name() % port.name()).
                str());
        }
        return delay;
    } catch (const Exception& e) {
        throw CannotEstimateCost(
            __FILE__, __LINE__, __func__,
            std::string(
                "Output delay estimation could not be done for the given "
                "function unit entry. ") + e.errorMessage());
    }
    return 0.0;
}

/**
 * Estimates the input delay of the given register file port.
 *
 * @param port The RF port of which input delay to estimate.
 * @param implementationEntry The implementation information of the RF the
 *                       port belongs to.
 * @exception CannotEstimateCost In case the input delay could not be 
 *                               estimated. 
 * @return Estimate of input delay of the given RF port.
 */
DelayInNanoSeconds 
Estimator::registerFilePortWriteDelay(
    const TTAMachine::RFPort& port,
    const IDF::RFImplementationLocation& implementationEntry) 
    throw (CannotEstimateCost) {

    try {
        DelayInNanoSeconds delay = 0.0;
        HDB::HDBManager& hdb = HDBRegistry::instance().hdb(
            implementationEntry.hdbFile());
        if (!rfCostFunctionPluginOfImplementation(implementationEntry).
            estimatePortWriteDelay(port, implementationEntry, delay, hdb)) {
            throw CannotEstimateCost(
                __FILE__, __LINE__, __func__,
                (boost::format(
                    "Plugin was unable to estimate input delay of port "
                    "%s::%s.") % port.parentUnit()->name() % port.name()).
                str());
        }
        return delay;
    } catch (const Exception& e) {
        throw CannotEstimateCost(
            __FILE__, __LINE__, __func__,
            std::string(
                "Input delay estimation could not be done for a "
                "register file entry. ") + e.errorMessage());
    }
    return 0.0;
}

/**
 * Estimates the output delay of the given register file port.
 *
 * @param port The RF port of which output delay to estimate.
 * @param implementationEntry The implementation information of RF. 
 * @exception CannotEstimateCost In case the output delay could not be 
 *                               estimated. 
 * @return Estimate of output delay of the given RF.
 */
DelayInNanoSeconds 
Estimator::registerFilePortReadDelay(
    const TTAMachine::RFPort& port,
    const IDF::RFImplementationLocation& implementationEntry) 
    throw (CannotEstimateCost) {

    try {
        DelayInNanoSeconds delay = 0.0;
        HDB::HDBManager& hdb = HDBRegistry::instance().hdb(
            implementationEntry.hdbFile());
        if (!rfCostFunctionPluginOfImplementation(implementationEntry).
            estimatePortReadDelay(port, implementationEntry, delay, hdb)) {
            throw CannotEstimateCost(
                __FILE__, __LINE__, __func__,
                (boost::format(
                    "Plugin was unable to estimate output delay of port "
                    "%s::%s.") % port.parentUnit()->name() % port.name()).
                str());
        }
        return delay;
    } catch (const Exception& e) {
        throw CannotEstimateCost(
            __FILE__, __LINE__, __func__,
            std::string(
                "Output delay estimation could not be done for the given "
                "register file entry. ") + e.errorMessage());
    }
    return 0.0;
}

/**
 * Estimates the maximum computation delay of the given function unit.
 *
 * The maximum computation delay is the longest stage in the FU unit's
 * pipeline. It's used mainly for calculating the maximum clock frequency
 * of target architecture by finding the longest path of the machine.
 *
 * @param architecture The FU architecture of which area to estimate.
 * @param implementationEntry The implementation information of FU. 
 * @exception CannotEstimateCost In case the computation delay could not be 
 *                               estimated. 
 * @return Estimate of computation delay of the given FU.
 */
DelayInNanoSeconds 
Estimator::functionUnitMaximumComputationDelay(
    const TTAMachine::FunctionUnit& architecture,
    const IDF::FUImplementationLocation& implementationEntry) 
    throw (CannotEstimateCost) {

    try {
        DelayInNanoSeconds delay = 0.0;
        HDB::HDBManager& hdb = HDBRegistry::instance().hdb(
            implementationEntry.hdbFile());
        if (!fuCostFunctionPluginOfImplementation(implementationEntry).
            estimateMaximumComputationDelay(
                architecture, implementationEntry, delay, hdb)) {
            throw CannotEstimateCost(
                __FILE__, __LINE__, __func__,
                std::string(
                    "Plugin was unable to estimate computation delay of FU ") +
                architecture.name());
        }
        return delay;
    } catch (const Exception& e) {
        throw CannotEstimateCost(
            __FILE__, __LINE__, __func__,
            std::string(
                "Computation delay estimation could not be done for the given "
                "function unit entry. ") +
            e.errorMessage());
    }
    return 0.0;
}

/**
 * Estimates the maximum computation delay of the given register file.
 *
 * The maximum computation delay is the longest stage in a RF's execution.
 * It's used mainly for calculating the maximum clock frequency of target 
 * architecture.
 *
 * @param architecture The RF architecture of which area to estimate.
 * @param implementationEntry The implementation information of RF.
 * @exception CannotEstimateCost In case the computation delay could not be 
 *                               estimated. 
 * @return Estimate of computation delay of the given RF.
 */
DelayInNanoSeconds 
Estimator::registerFileMaximumComputationDelay(
    const TTAMachine::BaseRegisterFile& architecture,
    const IDF::RFImplementationLocation& implementationEntry) 
    throw (CannotEstimateCost) {

    try {
        DelayInNanoSeconds delay = 0.0;
        HDB::HDBManager& hdb = HDBRegistry::instance().hdb(
            implementationEntry.hdbFile());
        if (!rfCostFunctionPluginOfImplementation(implementationEntry).
            estimateMaximumComputationDelay(
                architecture, implementationEntry, delay, hdb)) {
            throw CannotEstimateCost(
                __FILE__, __LINE__, __func__,
                std::string(
                    "Plugin was unable to estimate computation delay of RF ") +
                architecture.name());
        }
        return delay;
    } catch (const Exception& e) {
        throw CannotEstimateCost(
            __FILE__, __LINE__, __func__,
            std::string(
                "Computation delay estimation could not be done for the given "
                "register file entry. ") +
            e.errorMessage());
    }
    return 0.0;
}


/**
 * Calculates the longest path of the machine.
 *
 * The longest path is the longest delay in the machine. It can be a path 
 * in interconnection network or a computation delay of an FU.
 *
 * @param machine Machine to calculate the longest path for.
 * @param machineImplementation Implementation identifier for the machine.
 * @return The delay of the longest path.
 */
DelayInNanoSeconds 
Estimator::longestPath(
    const TTAMachine::Machine& machine,
    const IDF::MachineImplementation& machineImplementation)
    throw (CannotEstimateCost, Exception) {

//#define LONGEST_PATH_DEBUGGING 

    DelayInNanoSeconds maximumICDelay = 0.0;

    // find all the paths in IC
    TransportPathList* icPaths = findAllICPaths(machine);

    // find if there is bus that is fully connected
    std::string fBusName = "";
    TTAMachine::Machine::BusNavigator busNav = machine.busNavigator();
    TTAMachine::Machine::SocketNavigator sNav = machine.socketNavigator();
    for (int i = 0; i < busNav.count(); ++i) {
        const TTAMachine::Bus& bus = *busNav.item(i);
        const TTAMachine::Segment& segment = *bus.segment(0);

        // check if bus is fully connected
        if (segment.connectionCount() == sNav.count()) {
            fBusName = bus.name();
            break;
        }
    }

    // calculate delays for all the paths in the machine
    for (TransportPathList::iterator i = icPaths->begin(); 
        i != icPaths->end(); ++i) {

        // if fully connected bus was found, check only paths that contain it
        if (fBusName != "" && fBusName != (i->bus()).name()) {
            continue;
        }

        DelayInNanoSeconds readDelay = 0, writeDelay = 0, busDelay = 0,
            pathDelay = 0;

        TransportPath& path = *i;
        
        // calculate the port->socket, socket->port part by using FU/RF plugins
        const TTAMachine::Port& sourcePort = path.sourcePort();

        if (dynamic_cast<const TTAMachine::FUPort*>(&sourcePort) != NULL) {

            const TTAMachine::FUPort& sourceFUPort = 
                dynamic_cast<const TTAMachine::FUPort&>(sourcePort);
            const TTAMachine::FunctionUnit& fu = *sourceFUPort.parentUnit();
            if (!machineImplementation.hasFUImplementation(fu.name())) {
                throw CannotEstimateCost(
                    __FILE__, __LINE__, __func__, 
                    (boost::format(
                        "Implementation information missing for function unit "
                        "'%s'.") % fu.name()).str());
            }
            // read delay
            readDelay = functionUnitPortReadDelay(
                sourceFUPort, machineImplementation.fuImplementation(
                    fu.name()));
        } else if (dynamic_cast<const TTAMachine::RFPort*>(&sourcePort) 
                   != NULL) {
            const TTAMachine::RFPort& sourceRFPort = 
                dynamic_cast<const TTAMachine::RFPort&>(sourcePort);
            const TTAMachine::BaseRegisterFile& rf = 
                *sourceRFPort.parentUnit();
            if (machineImplementation.hasRFImplementation(rf.name())) {
                // read delay
                readDelay = registerFilePortReadDelay( 
                    sourceRFPort, machineImplementation.rfImplementation(
                        rf.name()));
            } else if (machineImplementation.hasIUImplementation(rf.name())) {
                // read delay
                readDelay = registerFilePortReadDelay( 
                    sourceRFPort, machineImplementation.iuImplementation(
                        rf.name()));
            } else {
                throw CannotEstimateCost(
                    __FILE__, __LINE__, __func__, 
                    (boost::format(
                        "Implementation information missing for register file "
                        "'%s'.") % rf.name()).str());
            }
        } else if (dynamic_cast<const TTAMachine::BaseFUPort*>(
                       &sourcePort) != NULL) {
            // @todo What to do with the GCU port?
            readDelay = 0.0;
        } else {
            throw CannotEstimateCost(
                __FILE__, __LINE__, __func__, "Unsupported port type.");
        }

        const TTAMachine::Port& destinationPort = path.destinationPort();

        if (dynamic_cast<const TTAMachine::FUPort*>(&destinationPort) 
            != NULL) {

            const TTAMachine::FUPort& destinationFUPort = 
                dynamic_cast<const TTAMachine::FUPort&>(destinationPort);
            
            const TTAMachine::FunctionUnit& fu = 
                *destinationFUPort.parentUnit();

            if (dynamic_cast<const TTAMachine::ControlUnit*>(&fu) != NULL) {
                // @todo gcu port
                writeDelay = 0.0;
            } else if (!machineImplementation.hasFUImplementation(fu.name())) {
                throw CannotEstimateCost(
                    __FILE__, __LINE__, __func__, 
                    (boost::format(
                        "Implementation information missing for function "
                        "unit '%s'.") % fu.name()).str());
            } else {
                // write delay
                writeDelay = functionUnitPortWriteDelay( 
                    destinationFUPort, machineImplementation.fuImplementation(
                        fu.name()));
            }
        } else if (dynamic_cast<const TTAMachine::RFPort*>(&destinationPort) 
                   != NULL) {
            const TTAMachine::RFPort& destinationRFPort = 
                dynamic_cast<const TTAMachine::RFPort&>(destinationPort);

            const TTAMachine::BaseRegisterFile& rf = 
                *destinationRFPort.parentUnit();
            if (!machineImplementation.hasRFImplementation(rf.name())) {
                throw CannotEstimateCost(
                    __FILE__, __LINE__, __func__, 
                    (boost::format(
                        "Implementation information missing for register file "
                        "'%s'.") % rf.name()).str());
            }

            // write delay
            writeDelay += registerFilePortWriteDelay( 
                destinationRFPort, machineImplementation.rfImplementation(
                    rf.name()));
        } else if (dynamic_cast<const TTAMachine::BaseFUPort*>(
                       &destinationPort) != NULL) {
            // @todo What to do with the GCU port?
            writeDelay = 0.0;
        } else {
            throw CannotEstimateCost(
                __FILE__, __LINE__, __func__, "Unsupported port type.");
        }
        

        if (!machineImplementation.hasICDecoderPluginName() ||
            !machineImplementation.hasICDecoderPluginFile() ||
            !machineImplementation.hasICDecoderHDB()) {
            throw CannotEstimateCost(
                __FILE__, __LINE__, __func__, 
                "Missing IC&decoder plugin information.");
        }        

        // Find out if there are idf entries to sockets and bus

        IDF::SocketImplementationLocation* sourceSocketImplementation = NULL;
        if (!machineImplementation.hasSocketImplementation(
                path.sourceSocket().name())) {
            sourceSocketImplementation = 
                &IDF::NullUnitImplementationLocation::instance();
        } else {
            sourceSocketImplementation =
                &machineImplementation.socketImplementation(
                    path.sourceSocket().name());
        }
        IDF::SocketImplementationLocation* destinationSocketImplementation =
            NULL;
        if (!machineImplementation.hasSocketImplementation(
                path.destinationSocket().name())) {
            destinationSocketImplementation = 
                &IDF::NullUnitImplementationLocation::instance();
        } else {
            destinationSocketImplementation =
                &machineImplementation.socketImplementation(
                    path.destinationSocket().name());
        }
        IDF::BusImplementationLocation* busImplementation = NULL;
        if (!machineImplementation.hasBusImplementation(path.bus().name())) {
            busImplementation = 
                &IDF::NullUnitImplementationLocation::instance();
        } else {
            busImplementation = 
                &machineImplementation.busImplementation(path.bus().name());
        }

        // calculate the socket->bus->socket part by using a ICDecoder plugin
        busDelay = 
            estimateSocketToSocketDelayOfPath(
                machineImplementation.icDecoderPluginFile(), 
                machineImplementation.icDecoderPluginName(),
                path,
                machineImplementation,
                *sourceSocketImplementation,
                *busImplementation,
                *destinationSocketImplementation);

        // the total delay of the path
        pathDelay = readDelay + busDelay + writeDelay;

        maximumICDelay = std::max(maximumICDelay, pathDelay);

#ifdef LONGEST_PATH_DEBUGGING
        const TTAMachine::Bus& bus = path.bus();
        Application::logStream() 
            << sourcePort.parentUnit()->name() << "::" 
            << sourcePort.name() << " (" << readDelay << "), " 
            << bus.name() << " (" << busDelay << "), "
            << destinationPort.parentUnit()->name() << "::" 
            << destinationPort.name() << " (" << writeDelay << ") = "
            << pathDelay << std::endl;
#endif
    } // end of a for statement to calculate delays for all paths

    // find out if the longest path is in a function unit
    DelayInNanoSeconds maximumFUDelay = 0.0;

    // go through all function units and compute the computation delays
    // for all of them and store the largest found delay
    TTAMachine::Machine::FunctionUnitNavigator fuNav = 
        machine.functionUnitNavigator();
    for (int i = 0; i < fuNav.count(); ++i) {
        const TTAMachine::FunctionUnit& fu = *fuNav.item(i);

        if (!machineImplementation.hasFUImplementation(fu.name())) {
            throw CannotEstimateCost(
                __FILE__, __LINE__, __func__, 
                (boost::format(
                    "Implementation information missing for function unit "
                    "'%s'.") % fu.name()).str());
        }
        DelayInNanoSeconds delay = 
            functionUnitMaximumComputationDelay(
                fu, machineImplementation.fuImplementation(fu.name()));
        maximumFUDelay = std::max(maximumFUDelay, delay);

#ifdef LONGEST_PATH_DEBUGGING
        Application::logStream()
            << "maximum computation delay of " << fu.name() << " = " 
            << delay << " ns" << std::endl;
#endif
    }

    // GCU not treated as a regular FU
    // GCU should be estimated with icdecoder
    /*
    const TTAMachine::FunctionUnit& gcu = *machine.controlUnit();

    if (!machineImplementation.hasFUImplementation(gcu.name())) {
        throw CannotEstimateCost(
            __FILE__, __LINE__, __func__, 
            (boost::format(
                "Implementation information missing for function unit "
                "'%s'.") % gcu.name()).str());
    }

    DelayInNanoSeconds delay = 
        functionUnitMaximumComputationDelay(
            gcu, machineImplementation.fuImplementation(gcu.name()));
        
    maximumFUDelay = std::max(maximumFUDelay, delay);
    
#ifdef LONGEST_PATH_DEBUGGING
    Application::logStream()
        << "maximum computation delay of " << gcu.name() << " (GCU) = " 
        << delay << " ns" << std::endl;
#endif
    */
    // find out if the longest path is in a register file
    DelayInNanoSeconds maximumRFDelay = 0.0;

    // go through all register files and compute the computation delays
    // for all of them and store the maximum of the calculated delay
    TTAMachine::Machine::RegisterFileNavigator rfNav = 
        machine.registerFileNavigator();
    for (int i = 0; i < rfNav.count(); ++i) {
        const TTAMachine::BaseRegisterFile& rf = *rfNav.item(i);

        if (!machineImplementation.hasRFImplementation(rf.name())) {
            throw CannotEstimateCost(
                __FILE__, __LINE__, __func__, 
                (boost::format(
                    "Implementation information missing for register file "
                    "'%s'.") % rf.name()).str());
        }

        DelayInNanoSeconds delay = 
            registerFileMaximumComputationDelay(
                rf, machineImplementation.rfImplementation(rf.name()));
        
        maximumRFDelay = std::max(maximumRFDelay, delay);

#ifdef LONGEST_PATH_DEBUGGING
    Application::logStream()
        << "maximum computation delay of " << rf.name() << " = " 
        << delay << " ns" << std::endl;
#endif
    }
       
    delete icPaths;
    icPaths = NULL;

    return std::max(maximumICDelay, std::max(maximumFUDelay, maximumRFDelay));
}

/**
 * Finds all paths in the interconnection network of the given machine.
 *
 * @param machine Machine to search the paths in.
 * @return List of transport paths. Becomes property of the client.
 * @exception IllegalMachine In case the machine is badly formed, for example,
 *                           has features that are not supported yet.
 */
TransportPathList*
Estimator::findAllICPaths(const TTAMachine::Machine& machine) 
    throw (IllegalMachine) {

    TransportPathList* paths = new TransportPathList();

    TTAMachine::Machine::BusNavigator busNav = machine.busNavigator();
    for (int bi = 0; bi < busNav.count(); ++bi) {
        typedef std::set<const TTAMachine::Socket*> SocketSet;
        SocketSet outputSockets, inputSockets;

        const TTAMachine::Bus& bus = *busNav.item(bi);
        if (bus.segmentCount() > 1) {
            throw IllegalMachine(
                __FILE__, __LINE__, __func__,
                "Segmented buses not supported yet.");
        }

        const TTAMachine::Segment& segment = *bus.segment(0);

        // fetch all input and output sockets connected to the bus (segment)
        for (int ci = 0; ci < segment.connectionCount(); ++ci) {
            const TTAMachine::Socket& socket = *segment.connection(ci);
            if (socket.direction() == TTAMachine::Socket::INPUT) {
                inputSockets.insert(&socket);
            } else if (socket.direction() == TTAMachine::Socket::OUTPUT) {
                outputSockets.insert(&socket);
            } else {
                throw IllegalMachine(
                    __FILE__, __LINE__, __func__,
                    "Unsupported socket direction.");
            }
        }

        for (SocketSet::iterator osi = outputSockets.begin(); 
             osi != outputSockets.end(); ++osi) {
            const TTAMachine::Socket& outputSocket = **osi;

            // iterate through all (output) ports connected to the socket
            for (int opi = 0; opi < outputSocket.portCount(); ++opi) {
                const TTAMachine::Port& outputPort = *outputSocket.port(opi);

                // iterate through all input sockets connected to the bus
                for (SocketSet::iterator isi = inputSockets.begin();
                     isi != inputSockets.end(); ++isi) {
                    const TTAMachine::Socket& inputSocket = **isi;

                    // iterate through all endpoints of the path (input ports)
                    for (int ipi = 0; ipi < inputSocket.portCount(); ++ipi) {
                        const TTAMachine::Port& inputPort = *inputSocket.port(
                            ipi);
                        TransportPath path(
                            outputPort, outputSocket, bus, inputSocket,
                            inputPort);
                        paths->push_back(path);
#if 0
                        Application::logStream()
                            << "path: {" << outputPort.parentUnit()->name()
                            << "::" << outputPort.name() << ", "
                            << outputSocket.name() << ", "
                            << bus.name() << ", "
                            << inputSocket.name() << ", "
                            << inputPort.parentUnit()->name()
                            << "::" << inputPort.name() << "}" << std::endl;
#endif
                    }
                }
            }
        }
    }
    return paths;
}

/**
 * Estimates the delay of a single socket-to-socket path.
 *
 * Delegates the task to the given IC&decoder plugin.
 *
 * @param pluginPath The path of the IC&decoder estimator plugin to use.
 * @param pluginName The name of the IC&decoder estimator plugin to use.
 * @param pluginDataHDB The HDB from which the plugin should fetch its data.
 * @param path The path to estimate.
 * @param machineImplementation Implementation of the machine.
 * @param sourceSocketImplementation The implementation descriptor of source 
 *        socket.
 * @param busImplementation The implementation descriptor of bus.
 * @param destinationSocketImplementation The implementation descriptor of
 *        destination socket.

 */
DelayInNanoSeconds 
Estimator::estimateSocketToSocketDelayOfPath(
    const std::string pluginPath,
    const std::string pluginName,
    const TransportPath& path,
    const IDF::MachineImplementation& machineImplementation,
    const IDF::SocketImplementationLocation& sourceSocketImplementation,
    const IDF::BusImplementationLocation& busImplementation,
    const IDF::SocketImplementationLocation& 
        destinationSocketImplementation)
    throw (CannotEstimateCost) {

    DelayInNanoSeconds delay = 0.0;
    try {

        ICDecoderEstimatorPlugin& plugin = 
            icDecoderEstimatorPluginRegistry_.plugin(
                pluginPath, pluginName);

        if (!plugin.estimateICDelayOfPath(
                HDBRegistry::instance(), path, machineImplementation,
                sourceSocketImplementation, 
                busImplementation, destinationSocketImplementation, delay)) {
            throw CannotEstimateCost(
                __FILE__, __LINE__, __func__, 
                (boost::format(
                    "The IC&decoder estimator plugin '%s' could not estimate "
                    "delay of a path.") % pluginName).str());
        }
    } catch (const Exception& e) {
        throw CannotEstimateCost(
            __FILE__, __LINE__, __func__, 
            std::string("Error while using ICDecoder estimation plugin. ") +
            e.errorMessage());
    }
    return delay;
}

}

