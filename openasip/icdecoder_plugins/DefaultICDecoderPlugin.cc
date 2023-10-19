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
 * @file DefaultICDecoderPlugin.hh
 *
 * Declaration and implementation of DefaultICDecoderPlugin class.
 *
 * @author Pekka Jääskeläinen 2005 (pekka.jaaskelainen-no.spam-tut.fi)
 * @author Vinogradov Viacheslav(added Verilog generating) 2012 
 * @note rating: red
 */

#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <utility>
#include <iterator>
#include <fstream>
#include <iostream>
#include <map>
#include <math.h>

#include "boost/regex.hpp"
#include "boost/format.hpp"

#include "HDBManager.hh"
#include "HDBRegistry.hh"
#include "Machine.hh"
#include "MachineInfo.hh"
#include "Socket.hh"
#include "Bus.hh"
#include "Segment.hh"
#include "ControlUnit.hh"
#include "Guard.hh"
#include "FUPort.hh"
#include "RFPort.hh"
#include "MachineImplementation.hh"
#include "ICDecoderEstimatorPlugin.hh"
#include "Program.hh"
#include "ExecutionTrace.hh"

#include "ICDecoderGeneratorPlugin.hh"
#include "ProGeTypes.hh"
#include "NetlistBlock.hh"
#include "LoopBufferBlock.hh"
#include "NetlistGenerator.hh"
#include "Netlist.hh"
#include "NetlistPort.hh"
#include "VHDLNetlistWriter.hh"
#include "VerilogNetlistWriter.hh"
#include "CUOpcodeGenerator.hh"
#include "SignalTypes.hh"

#include "DefaultDecoderGenerator.hh"
#include "DefaultICGenerator.hh"

#include "BinaryEncoding.hh"
#include "MoveSlot.hh"
#include "SourceField.hh"
#include "DestinationField.hh"
#include "GuardField.hh"
#include "ImmediateSlotField.hh"
#include "ImmediateControlField.hh"
#include "LImmDstRegisterField.hh"
#include "GuardEncoding.hh"
#include "GPRGuardEncoding.hh"
#include "FUGuardEncoding.hh"
#include "UnconditionalGuardEncoding.hh"
#include "SocketEncoding.hh"
#include "RFPortCode.hh"
#include "IUPortCode.hh"
#include "FUPortCode.hh"
#include "SocketCodeTable.hh"
#include "BlockSourceCopier.hh"

#include "Application.hh"
#include "MathTools.hh"
#include "DataObject.hh"
#include "MapTools.hh"
#include "Conversion.hh"
#include "Environment.hh"
#include "RV32MicroCodeGenerator.hh"

#include "CUOpcodeGenerator.hh"

using namespace CostEstimator;
using std::cerr;
using std::endl;
using std::cout;

const std::string RISCV_SIMM_PORT_OUT_NAME = "simm_out";
const std::string IFETCH_STALL_PORT_NAME = "ifetch_stall";

////////////////////////////////////////////////////////////////////////////
// DefaultICDecoderEstimator
////////////////////////////////////////////////////////////////////////////

/**
 * The default IC&decoder cost estimation plugin.
 *
 */
class DefaultICDecoderEstimator : public ICDecoderEstimatorPlugin {
public:

    /**
     * Constructor.
     *
     * @param name The name of this plugin.
     * @param dataSource The HDB used to fetch cost estimation data.
     */
    DefaultICDecoderEstimator(
        const std::string& name) :
        ICDecoderEstimatorPlugin(name) {
    }

    /**
     * Destructor.
     */
    virtual ~DefaultICDecoderEstimator() {
    }


    /**
     * Estimates the delay of an IC path.
     *
     * The default implementation estimates the delays by summing up delays of
     * the subcomponent of the machine part that is registered to be the 
     * 'delay component'. In practice, an entry named 
     * 'delay_subsocket_reference' or 'delay_subbus_reference' is first
     * fetched from HDB. The entry pointed to by the fetched entry contains
     * the delay data for the socket/bus at hand.
     * 
     * @param path Transport path of which delay is estimated.
     * @param machineImplementation Implementation of the machine.
     * @param sourceSocketImplementation Not used.
     * @param busImplementation Not used.
     * @param destinationSocketImplementation Not used.
     * @param delay Result delay of the transport path.
     * @return True if the estimation succeeded.
     */
    virtual bool estimateICDelayOfPath(
        HDB::HDBRegistry& hdbRegistry,
        const TransportPath& path,
        const IDF::MachineImplementation& machineImplementation,
        const IDF::SocketImplementationLocation&,
        const IDF::BusImplementationLocation&,
        const IDF::SocketImplementationLocation&,
        DelayInNanoSeconds& delay) {
//#define DEBUG_DELAY_ESTIMATION
        try {
            HDB::HDBManager& hdb = 
                hdbRegistry.hdb(machineImplementation.icDecoderHDB());
            DelayInNanoSeconds sourceDelay = 
                delayOfSocket(hdb, path.sourceSocket());
            DelayInNanoSeconds destinationDelay =
                delayOfSocket(hdb, path.destinationSocket());
            DelayInNanoSeconds busDelay = delayOfBus(hdb, path.bus());
            delay = sourceDelay + busDelay + destinationDelay;

#ifdef DEBUG_DELAY_ESTIMATION
            std::cout 
                << "path: {" << path.sourceSocket().name()
                << "," << path.bus().name() << "," 
                << path.destinationSocket().name() << "} "
                << "delays={" << sourceDelay << "," << busDelay << ","
                << destinationDelay << "} = " << delay << std::endl;
#endif            
            return true;
        } catch (const Exception& e) {
            debugLog(e.errorMessage());
            return false;
        }

        return false;
    }
      
    /**
     * Estimates the area of an IC path.
     *
     * The default implementation estimates the area by summing up areas of
     * of all subcomponents of sockets and buses in the machine. Subcomponent 
     * data is entered by user to HDB before estimation. The estimation 
     * algorithm does not figure out the subcomponents, it only sums up the c
     * costs of all subcomponents of a machine part. Subcomponent is a 
     * fanin/fanout combination included in a component. There can be multiple 
     * such combinations due to different widths in inputs and outputs.
     *
     * The subcomponent data is fetched from HDB by first fetching all
     * subcomponent ids of the machine part by looking for entries named
     * 'subsocket_reference' or 'subbus_reference'. The values of these
     * entries are identifiers of subsocket entries in the cost table.
     * Each subsocket entry has a string of name/value pairs as their
     * value. The cost data is stored in the name/value string. The
     * cost data string must be in the following format:
     * 'throughput_delay=53 area=5.2 active_energy=3 idle_energy=3 
     *  control_delay=3'
     */
    virtual bool estimateICArea(
        HDB::HDBRegistry& hdbRegistry,
        const TTAMachine::Machine& machine,
        const IDF::MachineImplementation& machineImplementation,
        AreaInGates& area) {
        area = 0;

//#define DEBUG_AREA_ESTIMATION

        std::string socketName = "";
        try {
            /// @todo each socket may have own HDB associated to their
            /// implementation location object
            HDB::HDBManager& hdb = 
                hdbRegistry.hdb(machineImplementation.icDecoderHDB());
            TTAMachine::Machine::SocketNavigator socketNav = 
                machine.socketNavigator();
            for (int i = 0; i < socketNav.count(); ++i) {
                const TTAMachine::Socket& socket = *socketNav.item(i);
                socketName = "";
                if (socket.direction() == TTAMachine::Socket::INPUT) {
                    socketName = "input_sub_socket";
                } else if (socket.direction() == TTAMachine::Socket::OUTPUT) {
                    socketName = "output_sub_socket";
                } else {
                    // Socket direction is unknown
                    // No need to estimate, since it will not be used.
                    continue;
                }

                FanInFanOutCombinationSet parameterSet =
                    socketParameters(socket);
                // socket fanin, fanout and data width is added to name string
                FanInFanOutCombinationSet::const_iterator iter = 
                    parameterSet.begin();
                std::string socketNameCopy = socketName;
                for (; iter != parameterSet.end(); iter++) {
                    socketName = socketNameCopy + " " +
                        Conversion::toString((*iter).fanIn) + " " +
                        Conversion::toString((*iter).fanOut) + " " +
                        Conversion::toString((*iter).dataWidth);
                    try {
                        DataObject subsocketData =
                            hdb.costEstimationDataValue(socketName, name());
                        area += valueFromKeyValuePairString(
                            "area", subsocketData.stringValue()).doubleValue();
                    } catch (const KeyNotFound& e) {
                        std::set<std::string>::iterator iter = 
                            missingComponents_.find("area" + socketName);
                        if (iter == missingComponents_.end()) {
                            std::cerr << "Warning: Could not get area data "
                                      << "for '"
                                      << socketName << "' from HDB: "
                                      << hdb.fileName() << ". Area of this "
                                      << "socket not counted to total." 
                                      << std::endl;
                            missingComponents_.insert("area" + socketName);
                        }
                    }
                }
            }
        } catch (const Exception& e) {
            HDB::HDBManager& hdb = 
                hdbRegistry.hdb(machineImplementation.icDecoderHDB());
            debugLog(
                std::string("Could not get socket area data '") +
                socketName + "' from HDB: " + hdb.fileName() + 
                e.errorMessage());
            return false;
        }

        std::string busName = "";
        try {
            /// @todo each bus may have own HDB associated to their
            /// implementation location object
            HDB::HDBManager& hdb = 
                hdbRegistry.hdb(machineImplementation.icDecoderHDB());
            TTAMachine::Machine::BusNavigator busNav = 
                machine.busNavigator();
            for (int i = 0; i < busNav.count(); ++i) {
                const TTAMachine::Bus& bus = *busNav.item(i);

                FanInFanOutCombinationSet parameterSet = busParameters(bus);
                // bus fanin, fanout and data width is added to name string
                FanInFanOutCombinationSet::const_iterator iter = 
                    parameterSet.begin();
                for (; iter != parameterSet.end(); iter++) {
                    busName = "sub_bus " +
                        Conversion::toString((*iter).fanIn) + " " +
                        Conversion::toString((*iter).fanOut) + " " +
                        Conversion::toString((*iter).dataWidth);
                    try {
                        DataObject subbusData =
                            hdb.costEstimationDataValue(busName, name());
                        area += valueFromKeyValuePairString(
                            "area", subbusData.stringValue()).doubleValue();
                    } catch (const KeyNotFound& e) {
                        std::set<std::string>::iterator iter = 
                            missingComponents_.find("area" + busName);
                        if (iter == missingComponents_.end()) {
                            std::cerr << "Warning: Could not get area data "
                                      << "for '"
                                      << busName << "' from HDB: "
                                      << hdb.fileName() << ". Area of this "
                                      << "bus not counted to total." 
                                      << std::endl;
                            missingComponents_.insert("area" + busName);
                        }
                    }
                }
            }
        } catch (const Exception& e) {
            HDB::HDBManager& hdb = 
                hdbRegistry.hdb(machineImplementation.icDecoderHDB());
            debugLog(
                std::string("Could not get bus area data '") +
                busName + "' from HDB. " +  hdb.fileName() +
                e.errorMessage());
            return false;
        }

        return true;
    }

    /**
     * Estimates the energy consumed by IC.
     *
     * The default implementation estimates the area by summing up energies of
     * of all subcomponents of sockets and buses. For more information, see 
     * the comment of the area estimation function.
     */
    virtual bool estimateICEnergy(
        HDB::HDBRegistry& hdbRegistry,
        const TTAMachine::Machine& machine,
        const IDF::MachineImplementation& machineImplementation,
        const TTAProgram::Program& /*program*/,
        const ExecutionTrace& traceDB,
        EnergyInMilliJoules& energy) {
    
//#define DEBUG_ENERGY_ESTIMATION

        energy = 0.0;

        ClockCycleCount totalCycles = traceDB.simulatedCycleCount();

        std::string socketName = "";
        try {
            HDB::HDBManager& hdb = 
                hdbRegistry.hdb(machineImplementation.icDecoderHDB());
            TTAMachine::Machine::SocketNavigator socketNav = 
                machine.socketNavigator();
            for (int i = 0; i < socketNav.count(); ++i) {
                const TTAMachine::Socket& socket = *socketNav.item(i);
                socketName = "";
                if (socket.direction() == TTAMachine::Socket::INPUT) {
                    socketName = "input_sub_socket";
                } else if (socket.direction() == TTAMachine::Socket::OUTPUT) {
                    socketName = "output_sub_socket";
                } else {
                    // Socket direction is unknown
                    // No need to estimate, since it will not be used.
                    continue;
                }

                FanInFanOutCombinationSet parameterSet =
                    socketParameters(socket);
                // socket fanin, fanout and data width is added to name string
                FanInFanOutCombinationSet::const_iterator iter = 
                    parameterSet.begin();
                std::string socketNameCopy = socketName;
                for (; iter != parameterSet.end(); iter++) {
                    socketName = socketNameCopy + " " +
                        Conversion::toString((*iter).fanIn) + " " +
                        Conversion::toString((*iter).fanOut) + " " +
                        Conversion::toString((*iter).dataWidth);
                    DataObject subsocketData =
                        hdb.costEstimationDataValue(socketName, name());

                    // sum the active and idle energies of all subsockets
                    // multiplied with write and idle clock counts, 
                    // respectively
                    EnergyInMilliJoules idleEnergy = 
                        valueFromKeyValuePairString(
                            "idle_energy", subsocketData.stringValue()).
                        doubleValue();
                    EnergyInMilliJoules activeEnergy = 
                        valueFromKeyValuePairString(
                            "active_energy", subsocketData.stringValue()).
                        doubleValue();

                    ClockCycleCount activeCycles = 
                        traceDB.socketWriteCount(socket.name());

                    EnergyInMilliJoules totalActiveEnergy = 
                        activeEnergy*activeCycles;

                    ClockCycleCount idleCycles = totalCycles - activeCycles;

                    EnergyInMilliJoules totalIdleEnergy = 
                        idleEnergy*idleCycles;

                    EnergyInMilliJoules totalEnergy = 
                        totalIdleEnergy + totalActiveEnergy;
                    energy += totalEnergy;
#ifdef DEBUG_ENERGY_ESTIMATION
                    std::cout 
                        << "socket " << socket.name() << ", subsocket "
                        << socketName << ": "
                        << "active energy = " << activeEnergy 
                        << " mJ/cycle * " << activeCycles << " cycle = " 
                        << totalActiveEnergy << ", "
                        << "idle energy = " << idleEnergy 
                        << " mJ/cycle * " << idleCycles << " cycle = " 
                        << totalIdleEnergy << " TOTAL = " << totalEnergy
                        << std::endl;
#endif
                }
            }
        } catch (const Exception& e) {                
            debugLog(
                std::string("Could not get socket area data '") +
                socketName + "' from HDB. " + e.errorMessage());
            return false;
        }
        std::string busName = "";
        try {
            HDB::HDBManager& hdb = 
                hdbRegistry.hdb(machineImplementation.icDecoderHDB());
            TTAMachine::Machine::BusNavigator busNav = machine.busNavigator();
            for (int i = 0; i < busNav.count(); ++i) {
                const TTAMachine::Bus& bus = *busNav.item(i);
                
                FanInFanOutCombinationSet parameterSet = busParameters(bus);
                // bus fanin, fanout and data width is added to name string
                FanInFanOutCombinationSet::const_iterator iter = 
                    parameterSet.begin();
                for (; iter != parameterSet.end(); iter++) {
                    busName = "sub_bus " +
                        Conversion::toString((*iter).fanIn) + " " +
                        Conversion::toString((*iter).fanOut) + " " +
                        Conversion::toString((*iter).dataWidth);
                    DataObject subbusData =
                        hdb.costEstimationDataValue(busName, name());

                    // sum the active and idle energies of all subbuses
                    // multiplied with write and idle clock counts, 
                    // respectively
                    EnergyInMilliJoules idleEnergy = 
                        valueFromKeyValuePairString(
                            "idle_energy", subbusData.stringValue()).
                        doubleValue();
                    EnergyInMilliJoules activeEnergy = 
                        valueFromKeyValuePairString(
                            "active_energy", subbusData.stringValue()).
                        doubleValue();

                    ClockCycleCount activeCycles = 
                        traceDB.busWriteCount(bus.name());
                    
                    EnergyInMilliJoules totalActiveEnergy = 
                        activeEnergy*activeCycles;
                    
                    ClockCycleCount idleCycles = totalCycles - activeCycles;
                    
                    EnergyInMilliJoules totalIdleEnergy = 
                        idleEnergy*idleCycles;
                    
                    EnergyInMilliJoules totalEnergy = 
                        totalIdleEnergy + totalActiveEnergy;
                    energy += totalEnergy;
#ifdef DEBUG_ENERGY_ESTIMATION
                    std::cout 
                        << "bus " << bus.name() << ", subbus "
                        << busName << ": "
                        << "active energy = " << activeEnergy 
                        << " mJ/cycle * " << activeCycles << " cycle = " 
                        << totalActiveEnergy << ", "
                        << "idle energy = " << idleEnergy 
                        << " mJ/cycle * " << idleCycles << " cycle = " 
                        << totalIdleEnergy << " TOTAL = " << totalEnergy
                        << std::endl;
#endif
                }
            }
        } catch (const Exception& e) {                
            debugLog(
                std::string("Could not get bus energy data '") + busName + 
                "' from HDB. " + e.errorMessage());
            return false;
        }
        return true;
    }

private:

    /// Set for component names that are missing from HDB.
    std::set<std::string> missingComponents_;

    /**
     * Finds out the delay of the given socket.
     */
    DelayInNanoSeconds
    delayOfSocket(HDB::HDBManager& hdb, const TTAMachine::Socket& socket) {
        DelayInNanoSeconds delay = 0;
        std::string socketName = "";
        try {
            if (socket.direction() == TTAMachine::Socket::INPUT) {
                socketName = "input_sub_socket";
            } else if (socket.direction() == TTAMachine::Socket::OUTPUT) {
                socketName = "output_sub_socket";
            } else {
                // Socket direction is unknown, should never be
                assert(false);
            }
            
            FanInFanOutCombinationSet parameterSet =
                socketParameters(socket);
            // socket fanin, fanout and data width is added to name string
            FanInFanOutCombinationSet::const_iterator iter = 
                parameterSet.begin();
            std::string socketNameCopy = socketName;
            for (; iter != parameterSet.end(); iter++) {
                socketName = socketNameCopy + " " +
                    Conversion::toString((*iter).fanIn) + " " +
                    Conversion::toString((*iter).fanOut) + " " +
                    Conversion::toString((*iter).dataWidth);
                    if (socket.direction() == TTAMachine::Socket::INPUT &&
                            (*iter).fanIn == 1 && (*iter).fanOut == 1) {
                        // input socket 1 1 * --> no delay
                        return 0;
                    }
                try {
                    DataObject subsocketData =
                        hdb.costEstimationDataValue(socketName, name());
                    delay += valueFromKeyValuePairString(
                        "throughput_delay",
                        subsocketData.stringValue()).doubleValue();
                } catch (const KeyNotFound& e) {
                    std::set<std::string>::iterator iter = 
                        missingComponents_.find("tdelay" + socketName);
                    if (iter == missingComponents_.end()) {
                        std::cerr << "Warning: Could not get throughput delay "
                                  << "data for '"
                                  << socketName << "' from HDB: "
                                  << hdb.fileName() << ". This value is "
                                  << "not counted to total." << std::endl;
                        missingComponents_.insert("tdelay" + socketName);
                    }
                }
            }
        } catch (const Exception& e) {
            throw Exception(
                __FILE__, __LINE__, __func__, 
                std::string("Cannot fetch socket delay data '") +
                socketName + "'." + e.errorMessage());
        }
        return delay;
    }

    /**
     * Finds out the delay of the given bus.
     */
    DelayInNanoSeconds
    delayOfBus(HDB::HDBManager& hdb, const TTAMachine::Bus& bus) {
        DelayInNanoSeconds delay = 0;
        std::string busName = "";
        try {
            FanInFanOutCombinationSet parameterSet = busParameters(bus);
            // bus fanin, fanout and data width is added to name string
            FanInFanOutCombinationSet::const_iterator iter = 
                parameterSet.begin();
            for (; iter != parameterSet.end(); iter++) {
                busName = "sub_bus " +
                    Conversion::toString((*iter).fanIn) + " " +
                    Conversion::toString((*iter).fanOut) + " " +
                    Conversion::toString((*iter).dataWidth);

                try {
                    DataObject subbusData =
                        hdb.costEstimationDataValue(busName, name());
                    
                    delay += valueFromKeyValuePairString(
                        "throughput_delay",
                        subbusData.stringValue()).doubleValue();
                } catch (const KeyNotFound& e) {
                    std::set<std::string>::iterator iter = 
                        missingComponents_.find("tdelay" + busName);
                    if (iter == missingComponents_.end()) {
                        std::cerr << "Warning: Could not get throughput delay "
                                  << "data for '"
                                  << busName << "' from HDB: "
                                  << hdb.fileName() << ". This value is "
                                  << "not counted to total." << std::endl;
                        missingComponents_.insert("tdelay" + busName);
                    }
                }
            }
        } catch (const Exception& e) {
            throw Exception(
                __FILE__, __LINE__, __func__, 
                std::string("Cannot fetch bus delay data '") +
                busName + "'." + e.errorMessage());
        }
        return delay;
    }

    /**
     * Finds out the delay of the given socket.
     *
     * Not used with current implementation.
     */
    DelayInNanoSeconds
    delayOfSocket(
        HDB::HDBRegistry& hdbRegistry,
        const IDF::SocketImplementationLocation& implementation) {
        // First fetch the id of the 'delay_subsocket_reference' of the
        // socket. The id is used to fetch the subsocket data which carries
        // the delay data.
        int implementationId = implementation.id();
        
        HDB::HDBManager& hdb = hdbRegistry.hdb(implementation.hdbFile());

        std::string entry = "delay_subsocket_reference";

        try {
            DataObject subsocketReference = 
                hdb.socketCostEstimationData(entry, implementationId, name_);

            int entryId = subsocketReference.integerValue();

            entry = std::string("id=") + Conversion::toString(entryId);
            DataObject subsocketData = hdb.costEstimationDataValue(entryId);
            return valueFromKeyValuePairString(
                "throughput_delay", subsocketData.stringValue()).doubleValue();
        } catch (const Exception& e) {
            throw Exception(
                __FILE__, __LINE__, __func__, 
                std::string("Cannot fetch socket delay data '") +
                entry + "'." + e.errorMessage());
        }
    }

    /**
     * Finds out the delay of the given bus.
     */
    DelayInNanoSeconds
    delayOfBus(
        HDB::HDBRegistry& hdbRegistry,
        const IDF::BusImplementationLocation& implementation) {
        // First fetch the id of the 'delay_subbus_reference' of the
        // socket. The id is used to fetch the subbus data which carries
        // the delay data.
        int implementationId = implementation.id();
        
        HDB::HDBManager& hdb = hdbRegistry.hdb(implementation.hdbFile());

        try {
            return hdb.busCostEstimationData(
                "throughput_delay", implementationId, name_).doubleValue();

        } catch (const Exception& e) {
            throw Exception(
                __FILE__, __LINE__, __func__, 
                std::string("Cannot fetch bus delay data. ") +
                e.errorMessage());
        }
    }

    /**
     * Parses a string of key-value pairs and returns a value with given key.
     *
     * The string must be in format "key1=value1 key2=value2" etc.
     * the value must not have spaces in it.
     *
     * @param keyName The key of the value to fetch from the string.
     * @param keyValuePairString The key-value pair string to parse.
     * @return The value.
     * @exception KeyNotFound In case the key was not found in the string.
     *
     */
    DataObject
    valueFromKeyValuePairString(
        const std::string& keyName, const std::string& keyValuePairString) {
        boost::smatch parsed;
        boost::regex regexp(
            (boost::format("(.*)(%s)=([^\\s]*)(.*)") % keyName).str());
        if (!regex_match(keyValuePairString, parsed, regexp)) {
            throw KeyNotFound(
                __FILE__, __LINE__, __func__, 
                "Key not found in value string.");
        } else {
            if (parsed.size() < 3)
                throw KeyNotFound(__FILE__, __LINE__, __func__);
            DataObject data;
            data.setString(parsed[3]);
            return data;
        }
    }

    struct ICParameters {
        std::size_t fanIn;
        std::size_t fanOut;
        std::size_t dataWidth;
    };

    typedef std::vector<ICParameters> FanInFanOutCombinationSet;

    /**
     * Generates all possible fanin/fanout/data width combinations
     * (subcomponents) for given input and output widths.
     *
     * @param inputWidths Set containing component's input widths.
     * @param outputWidths Set containing component's output widths.
     * @return Set of fanin/fanout/data width combinations.
     */
    FanInFanOutCombinationSet
    generateFanInFanOutCombinations(
        std::vector<std::size_t>& inputWidths,
        std::vector<std::size_t>& outputWidths) {

        std::sort(inputWidths.begin(), inputWidths.end());
        std::sort(outputWidths.begin(), outputWidths.end());

        std::set<std::size_t> allWidthsTemp;
        std::copy(
            inputWidths.begin(), inputWidths.end(), 
            std::insert_iterator<std::set<std::size_t> >(
                allWidthsTemp, allWidthsTemp.begin()));
        std::copy(
            outputWidths.begin(), outputWidths.end(), 
            std::insert_iterator<std::set<std::size_t> >(
                allWidthsTemp, allWidthsTemp.begin()));
        std::vector<std::size_t> allWidths;
        std::copy(
            allWidthsTemp.begin(), allWidthsTemp.end(), 
            std::insert_iterator<std::vector<std::size_t> >(
                allWidths, allWidths.begin()));
            

        FanInFanOutCombinationSet combinations;
        for (std::vector<std::size_t>::iterator i = allWidths.begin(); 
             i != allWidths.end(); ++i) {

            const unsigned int width = *i;

            // how many input widths fits in the current width?
            std::size_t fanIn = 0;
            for (unsigned int n = 0; n < inputWidths.size(); n++) {
                // we can do a trick like this because the vector is ordered
                if (width > inputWidths.at(n) && inputWidths.at(n) > 0)
                    break;
                if (inputWidths.at(n) != 0) {
                    fanIn++;
                }
            }

            // how many output widths fits in the current width?
            std::size_t fanOut = 0;
            for (unsigned int n = 0; n < outputWidths.size(); n++) {
                // we can do a trick like this because the vector is ordered
                if (width > outputWidths.at(n) && outputWidths.at(n) > 0)
                    break;
                if (outputWidths.at(n) != 0) {
                    fanOut++;
                }
            }

            if (fanIn == 0 || fanOut == 0)
                return combinations; //continue;
            ICParameters parameters;
            parameters.fanIn = fanIn;
            parameters.fanOut = fanOut;
            parameters.dataWidth = width;
            combinations.push_back(parameters);
            
            for (unsigned int n = 0; n < inputWidths.size(); n++) {
                if (inputWidths.at(n) > width) {
                    inputWidths.at(n) = inputWidths.at(n) - width;
                } else {
                    inputWidths.at(n) = 0;
                }
            }
            for (unsigned int n = 0; n < outputWidths.size(); n++) {
                if (outputWidths.at(n) > width) {
                    outputWidths.at(n) = outputWidths.at(n) - width;
                } else {
                    outputWidths.at(n) = 0;
                }
            }

            for (unsigned int n = 0; n < allWidths.size(); n++) {
                if (allWidths.at(n) > width) {
                    allWidths.at(n) = allWidths.at(n) - width;
                } else {
                    allWidths.at(n) = 0;
                }
            }

        }

        return combinations;
    }
 

    /**
     * Generates fanin/fanout/width combinations to a socket.
     *
     * @param socket Socket which parameters are generated.
     * @return Set of fanin/fanout/width combinations.
     */
    FanInFanOutCombinationSet
    socketParameters(
        const TTAMachine::Socket& socket) {

        std::vector<std::size_t> inputWidths;
        std::vector<std::size_t> outputWidths;

        if (socket.direction() == TTAMachine::Socket::INPUT) {
            for (int i = 0; i < socket.segmentCount(); i++) {
                inputWidths.push_back(socket.segment(i)->parentBus()->width());
            }
            for (int i = 0; i < socket.portCount(); i++) {
                outputWidths.push_back(socket.port(i)->width());
            }
        } else if (socket.direction() == TTAMachine::Socket::OUTPUT) {
            for (int i = 0; i < socket.segmentCount(); i++) {
                outputWidths.push_back(socket.segment(i)->parentBus()->width());
            }
            for (int i = 0; i < socket.portCount(); i++) {
                inputWidths.push_back(socket.port(i)->width());
            }
        }
        return generateFanInFanOutCombinations(inputWidths, outputWidths);
    }

    /**
     * Generates fanin/fanout/width combinations to a bus.
     *
     * @param bus Bus which parameters are generated.
     * @return Set of fanin/fanout/width combinations.
     */
    FanInFanOutCombinationSet
    busParameters(
        const TTAMachine::Bus& bus) {

        std::vector<std::size_t> inputWidths;
        std::vector<std::size_t> outputWidths;

        TTAMachine::Machine::SocketNavigator socketNav = 
            bus.machine()->socketNavigator();
        for (int i = 0; i < socketNav.count(); ++i) {
            const TTAMachine::Socket& socket = *socketNav.item(i);
            if (bus.isConnectedTo(socket)) {
                std::size_t width = 0;
                for (int port = 0; port < socket.portCount(); port++) {
                    if (static_cast<unsigned int>(
                                socket.port(port)->width()) > width) {
                        width = socket.port(port)->width();
                    }
                }
                if (width == 0) {
                    continue;
                }
                if (socket.direction() == TTAMachine::Socket::OUTPUT) {
                    inputWidths.push_back(width);
                } else {
                    outputWidths.push_back(width);
                }
            }
        }

        return generateFanInFanOutCombinations(inputWidths, outputWidths);
    }
};
////////////////////////////////////////////////////////////////////////////
// DefaultICDecoderGenerator
////////////////////////////////////////////////////////////////////////////

using namespace TTAMachine;
using namespace ProGe;
using std::string;
using std::endl;

const std::string ENABLE_FEATURE = "yes";
const std::string GENERATE_DEBUGGER_PARAM = "debugger";
const std::string GENERATE_DEBUGGER_PARAM_YES = ENABLE_FEATURE;
const std::string GENERATE_DEBUGGER_PARAM_INTERNAL = "internal";
const std::string GENERATE_DEBUGGER_PARAM_EXTERNAL = "external";
const std::string GENERATE_DEBUGGER_PARAM_MINIMAL = "minimal";
const std::string GENERATE_BUS_TRACE_PARAM = "bustrace";
const std::string GENERATE_BUS_TRACE_PARAM_YES = ENABLE_FEATURE;
const std::string GENERATE_LOCK_TRACE_PARAM = "locktrace";
const std::string GENERATE_LOCK_TRACE_PARAM_YES = ENABLE_FEATURE;
const std::string BYPASS_FETCHBLOCK_REG_PARAM = "bypassinstructionregister";
const std::string BYPASS_FETCHBLOCK_REG_PARAM_YES = ENABLE_FEATURE;
const std::string LOCK_TRACE_STARTING_CYCLE = "locktracestartingcycle";
const std::string BUS_TRACE_STARTING_CYCLE = "bustracestartingcycle";
const std::string PLUGIN_DESCRIPTION = 
                           "Generates the IC as as an AND-OR network.";
const std::string NO_SELF_LOCKING_PARAM = "no-self-locking";
const std::string NO_SELF_LOCKING_PARAM_YES = ENABLE_FEATURE;
const std::string SYNC_RESET = "synchronous-reset";
const std::string BRAM_VENDOR = "bram-vendor";
const std::string BRAM_VENDOR_INTEL = "intel";

#include <iostream>
#include <cmath>
using namespace std;
/**
 * Default implementation for IC/decoder generator.
 */
class DefaultICDecoderGenerator : public ICDecoderGeneratorPlugin {
public:
    DefaultICDecoderGenerator(
        const TTAMachine::Machine& machine, const BinaryEncoding& bem)
        : ICDecoderGeneratorPlugin(machine, bem, PLUGIN_DESCRIPTION),
          dbsmBlock(NULL),
          icGenerator_(NULL),
          decoderGenerator_(NULL),
          ttamachine_(machine),
          bem_(bem) {
        addParameter(
            GENERATE_DEBUGGER_PARAM,
            "Generates wires to the internal hardware debugger if "
            "the value is '" + GENERATE_DEBUGGER_PARAM_INTERNAL
            + "', and to the external debugger if the value is '"
            + GENERATE_DEBUGGER_PARAM_EXTERNAL + "'. A minimal set of wires, "
            "for softreset and break, are added if the value is '"
            + GENERATE_DEBUGGER_PARAM_MINIMAL + "'.");
        addParameter(
            GENERATE_BUS_TRACE_PARAM,
            "Generates code that prints bus trace if the value is '" +
                ENABLE_FEATURE + "'.");
        addParameter(
            BUS_TRACE_STARTING_CYCLE,
            "The first cycle for which the bus trace is printed.");
        addParameter(
            GENERATE_LOCK_TRACE_PARAM,
            "Generates code that prints global lock trace if the value is"
            " '" +
                ENABLE_FEATURE + "'.");
        addParameter(
            LOCK_TRACE_STARTING_CYCLE,
            "The first cycle for which the global lock trace is printed. "
            "If value is \"" + BUS_TRACE_STARTING_CYCLE + "\" then the "
            "value is inherited from " + BUS_TRACE_STARTING_CYCLE + ".");
        addParameter(
            BYPASS_FETCHBLOCK_REG_PARAM,
            "-1 delay slot by removing instruction fetch register. "
            "'" +
                ENABLE_FEATURE + "' to enable the feature.");
        addParameter(BRAM_VENDOR,
                "Set FPGA vendor to set specific Block RAM to ensure the "
                "correct inference. Supported values: \"" + BRAM_VENDOR_INTEL
                + "\". By default generates xilinx-compatible BRAMs.");

        icGenerator_ = new DefaultICGenerator(machine);
        decoderGenerator_ = new DefaultDecoderGenerator(
            machine, bem, *icGenerator_);
    }

    virtual ~DefaultICDecoderGenerator() {
        delete icGenerator_;
        delete decoderGenerator_;
    }
    
    /**
     * Completes the given netlist by adding IC block and completing the
     * decoder block by adding the ports connected to IC. Connects also IC to
     * all the units in the machine.
     *
     * @param The netlist to complete.
     * @param generator The netlist generator which generated the netlist.
     */
    virtual void
    completeNetlist(
        NetlistBlock& netlistBlock, const NetlistGenerator& generator) {
        // add interconnection network to the netlist and connect it to the 
        // units
        icGenerator_->addICToNetlist(generator, netlistBlock);

        // complete the decoder block and connect it to the IC and units
        decoderGenerator_->completeDecoderBlock(generator, netlistBlock);

        if (ttamachine_.isRISCVMachine()) {
            addRV32MicroCode(netlistBlock, generator);
        }
    }

    void
    addRV32MicroCode(
        NetlistBlock& netlistBlock, const NetlistGenerator& generator) {
        NetlistBlock& decompressor = generator.instructionDecompressor();
        NetlistBlock& decoder = generator.instructionDecoder();
        NetlistBlock& ifetch = generator.instructionFetch();
        Netlist& netlist = netlistBlock.netlist();

        // Change port width to 32 to match socket width
        ifetch.port("pc_in")->setWidthFormula("32");

        NetlistBlock* microCodeBlock = new NetlistBlock(
            "rv32_microcode_wrapper", "rv32_microcode_wrapper_i");

        // Adds the block to the netlist
        netlistBlock.addSubBlock(microCodeBlock);

        NetlistPort* decoderRISCVSimmPort = new NetlistPort(
            DefaultDecoderGenerator::RISCV_SIMM_PORT_IN_NAME, "32",
            ProGe::BIT_VECTOR, ProGe::IN, decoder);

        NetlistPort* clk = new NetlistPort(
            "clk", "1", ProGe::BIT, ProGe::IN, *microCodeBlock);

        NetlistPort* rst = new NetlistPort(
            "rstx", "1", ProGe::BIT, ProGe::IN, *microCodeBlock);

        NetlistPort* glock = new NetlistPort(
            "glock_in", "1", ProGe::BIT, ProGe::IN, *microCodeBlock);

        netlist.connect(
            *glock, *ifetch.port(DefaultDecoderGenerator::GLOCK_PORT_NAME));

        netlist.connect(*clk, generator.clkPort(netlistBlock));

        netlist.connect(*rst, generator.rstPort(netlistBlock));

        NetlistPort* instructionOut = new NetlistPort(
            "instruction_out", "INSTRUCTIONWIDTH", BIT_VECTOR, ProGe::OUT,
            *microCodeBlock);

        NetlistPort* microCodeImmOut = new NetlistPort(
            RISCV_SIMM_PORT_OUT_NAME, "32", BIT_VECTOR, ProGe::OUT,
            *microCodeBlock);

        NetlistPort* instructionIn = new NetlistPort(
            "instruction_in", "IMEMWIDTHINMAUS*IMEMMAUWIDTH", BIT_VECTOR,
            ProGe::IN, *microCodeBlock);

        netlist.connect(*microCodeImmOut, *decoderRISCVSimmPort);

        netlist.disconnectPorts(
            *ifetch.port(NetlistGenerator::FETCHBLOCK_PORT_NAME),
            *decompressor.port(NetlistGenerator::FETCHBLOCK_PORT_NAME));

        netlist.connect(
            *instructionOut,
            *decompressor.port(NetlistGenerator::FETCHBLOCK_PORT_NAME));

        netlist.connect(
            *ifetch.port(NetlistGenerator::FETCHBLOCK_PORT_NAME),
            *instructionIn);

        decompressor.port(NetlistGenerator::FETCHBLOCK_PORT_NAME)
            ->setWidthFormula("INSTRUCTIONWIDTH");

        NetlistPort* ifetchStallPortIFetch = new NetlistPort(
            IFETCH_STALL_PORT_NAME, "1", ProGe::BIT, ProGe::IN, ifetch);

        NetlistPort* ifetchStallPortMicroCode = new NetlistPort(
            IFETCH_STALL_PORT_NAME, "1", ProGe::BIT, ProGe::OUT,
            *microCodeBlock);

        NetlistPort* rvJumpPortMicroCode = new NetlistPort(
            "rv_jump", "1", ProGe::BIT, ProGe::OUT, *microCodeBlock);

        NetlistPort* rvAuipcPortMicroCode = new NetlistPort(
            "rv_auipc", "1", ProGe::BIT, ProGe::OUT, *microCodeBlock);

        NetlistPort* rvOffsetPortMicroCode = new NetlistPort(
            "rv_offset", "32", BIT_VECTOR, ProGe::OUT, *microCodeBlock);

        NetlistPort* rvOffsetPortIfetch =
            new NetlistPort("rv_offset", "32", BIT_VECTOR, ProGe::IN, ifetch);

        NetlistPort* rvJumpPortIfetch =
            new NetlistPort("rv_jump", "1", ProGe::BIT, ProGe::IN, ifetch);

        NetlistPort* rvAuipcPortIfetch =
            new NetlistPort("rv_auipc", "1", ProGe::BIT, ProGe::IN, ifetch);

        netlist.connect(*rvJumpPortMicroCode, *rvJumpPortIfetch);
        netlist.connect(*rvAuipcPortMicroCode, *rvAuipcPortIfetch);
        netlist.connect(*rvOffsetPortMicroCode, *rvOffsetPortIfetch);

        netlist.connect(*ifetchStallPortMicroCode, *ifetchStallPortIFetch);
    }

    void generateDebuggerCode(const NetlistGenerator& generator) {
        assert(
            generateDebugger() &&
            "Entered debugger connection generation without cause.");

        NetlistBlock* decoderBlock = &generator.instructionDecoder();
        NetlistBlock& toplevelBlock = decoderBlock->parentBlock();
        std::string addrWidthFormula = generator.gcuReturnAddressInPort()
            .widthFormula();
        NetlistBlock* fetchBlock = &generator.instructionFetch();
        NetlistBlock* icBlock = NULL;

        bool is_external = false, is_internal = false, is_minimal = false;

        if (parameterValue(GENERATE_DEBUGGER_PARAM) ==
            GENERATE_DEBUGGER_PARAM_EXTERNAL) {
              is_external = true;
        } else if (parameterValue(GENERATE_DEBUGGER_PARAM) ==
            GENERATE_DEBUGGER_PARAM_MINIMAL) {
              is_minimal = true;
        } else {
              is_internal = true;
        }

        for (std::size_t i = 0; i < toplevelBlock.subBlockCount(); i++) {
            icBlock = &toplevelBlock.subBlock(i);
            if (icBlock->instanceName() == "ic")
                break;
            if (icBlock->moduleName() == "ic")
                break;
        }

        if (is_internal) {
            //Include debugger package with width constants in tta0.vhdl
            toplevelBlock.addPackage("debugger_if");
        }

        NetlistPort* ttaResetPort = new NetlistPort(
            "db_tta_nreset", "1", BIT, ProGe::IN, toplevelBlock);
        NetlistPort *ttaLockcountPort = new NetlistPort("db_lockcnt",
            "64", ProGe::BIT_VECTOR, ProGe::OUT, toplevelBlock);
        NetlistPort *ttaCyclecountPort = new NetlistPort("db_cyclecnt",
            "64", ProGe::BIT_VECTOR, ProGe::OUT, toplevelBlock);

        NetlistPort *ttaPCPort = new NetlistPort("db_pc",
            "IMEMADDRWIDTH", ProGe::BIT_VECTOR, ProGe::OUT, toplevelBlock);

        if (is_minimal || is_external) {

            NetlistPort* dbGlockReqPort = new NetlistPort(
                "db_lockrq", "1", BIT, ProGe::IN, toplevelBlock);

            // Connect to the highest bit of decoder lockrq vector, others are
            // already connected
            NetlistPort* decoderGlockReqPort =
                decoderBlock->port("lock_req");
            int bit = decoderGenerator_->glockRequestWidth()-1;
            toplevelBlock.netlist().connect(
                *dbGlockReqPort, *decoderGlockReqPort, 0, bit, 1);

            // Connect ifetch debug ports
            NetlistPort* ifetchDebugResetPort = new NetlistPort(
                "db_rstx", "1", 1, ProGe::BIT, ProGe::IN, *fetchBlock);
            toplevelBlock.netlist().connect(*ttaResetPort,
                                            *ifetchDebugResetPort);

            NetlistPort* ifetchDebugLockRqPort = new NetlistPort(
                "db_lockreq", "1", 1, ProGe::BIT, ProGe::IN, *fetchBlock);
            toplevelBlock.netlist().connect(*dbGlockReqPort,
                *ifetchDebugLockRqPort);

            NetlistPort* ifetchCyclecountPort = new NetlistPort(
                "db_cyclecnt", "64", 64, ProGe::BIT_VECTOR, ProGe::OUT,
                *fetchBlock);
            toplevelBlock.netlist().connect(
                *ifetchCyclecountPort, *ttaCyclecountPort);
            NetlistPort* ifetchLockcountPort = new NetlistPort(
                "db_lockcnt", "64", 64, ProGe::BIT_VECTOR, ProGe::OUT,
                *fetchBlock);
            toplevelBlock.netlist().connect(
                *ifetchLockcountPort, *ttaLockcountPort);

            NetlistPort* ifetchPCPort = new NetlistPort(
                "db_pc", "IMEMADDRWIDTH", ProGe::BIT_VECTOR, ProGe::OUT,
                *fetchBlock);
            toplevelBlock.netlist().connect(*ifetchPCPort, *ttaPCPort);
        }
        if (!is_minimal) {
            //Add debugger interface ports to tta0 entity
            NetlistPort *ttaPCStartPort = new NetlistPort("db_pc_start",
                "IMEMADDRWIDTH", ProGe::BIT_VECTOR, ProGe::IN, toplevelBlock);
            NetlistPort *ttaBustracePort = new NetlistPort("db_bustraces",
                "BUSTRACE_WIDTH", ProGe::BIT_VECTOR, ProGe::OUT, toplevelBlock);

            // Connect bustraces out and away
            NetlistPort* icBustracePort = icBlock->port("db_bustraces");
            assert(icBustracePort);
            toplevelBlock.netlist().connect(
                *icBustracePort, *ttaBustracePort);

            if (is_external) {
                NetlistPort* dbPCNextPort = new NetlistPort(
                    "db_pc_next", "IMEMADDRWIDTH",
                    ProGe::BIT_VECTOR, ProGe::OUT, toplevelBlock);

                NetlistPort* ifetchPCStartPort = new NetlistPort(
                    "db_pc_start", "IMEMADDRWIDTH", ProGe::BIT_VECTOR,
                    ProGe::IN, *fetchBlock);
                toplevelBlock.netlist().connect(*ifetchPCStartPort,
                                                *ttaPCStartPort);
                NetlistPort* ifetchPCNextPort = new NetlistPort(
                    "db_pc_next", "IMEMADDRWIDTH", ProGe::BIT_VECTOR,
                    ProGe::OUT, *fetchBlock);
                toplevelBlock.netlist().connect(*ifetchPCNextPort,
                                                *dbPCNextPort);

            } else if (is_internal) { // Generate internal debugger
                NetlistPort* ttaBpEnaPort = new NetlistPort(
                    "db_bp_ena", "1+db_breakpoints",
                    ProGe::BIT_VECTOR, ProGe::IN, toplevelBlock);
                NetlistPort* ttaBp0Port = new NetlistPort(
                    "bp_target_cc", "db_breakpoints_cc*db_data_width",
                    ProGe::BIT_VECTOR, ProGe::IN, toplevelBlock);
                NetlistPort* ttaBp41Port = new NetlistPort(
                    "bp_target_pc", "db_breakpoints_pc*IMEMADDRWIDTH",
                    ProGe::BIT_VECTOR, ProGe::IN,
                    toplevelBlock);
                NetlistPort* ttaBpHitPort = new NetlistPort(
                    "db_bp_hit", "2+db_breakpoints",
                    ProGe::BIT_VECTOR, ProGe::OUT, toplevelBlock);
                NetlistPort* ttaContinuePort = new NetlistPort(
                    "db_tta_continue", "1", BIT, ProGe::IN, toplevelBlock);
                NetlistPort* ttaForceBreakPort = new NetlistPort(
                    "db_tta_forcebreak", "1", BIT, ProGe::IN, toplevelBlock);
                NetlistPort* ttaStdoutBreakPort = new NetlistPort(
                    "db_tta_stdoutbreak", "1", BIT, ProGe::IN, toplevelBlock);

                //Build and connect the debugger state machine block in tta0.vhdl
                dbsmBlock = new NetlistBlock("dbsm", "dbsm_1");
                toplevelBlock.addSubBlock(dbsmBlock);

                dbsmBlock->setParameter("cyclecnt_width_g", "integer",
                                        "db_data_width");
                dbsmBlock->setParameter("pc_width_g", "integer",
                                        "IMEMADDRWIDTH");

                NetlistPort* dbsmClkPort = new NetlistPort(
                    "clk", "1", BIT, ProGe::IN, *dbsmBlock);
                toplevelBlock.netlist().connect(
                    *dbsmClkPort, generator.clkPort(toplevelBlock));

                NetlistPort* dbsmNresetPort = new NetlistPort(
                    "nreset", "1", BIT, ProGe::IN, *dbsmBlock);
                toplevelBlock.netlist().connect(
                    *dbsmNresetPort, generator.rstPort(toplevelBlock));

                NetlistPort* dbsmBpEnaPort = new NetlistPort(
                    "bp_ena", "1+db_breakpoints",
                    ProGe::BIT_VECTOR, ProGe::IN, *dbsmBlock);
                toplevelBlock.netlist().connect(*ttaBpEnaPort, *dbsmBpEnaPort);

                NetlistPort* dbsmBp0Port = new NetlistPort(
                    "bp_target_cc", "db_breakpoints_cc*cyclecnt_width_g",
                    ProGe::BIT_VECTOR,
                    ProGe::IN, *dbsmBlock);
                toplevelBlock.netlist().connect(*ttaBp0Port, *dbsmBp0Port);

                NetlistPort* dbsmCyclecountPort = new NetlistPort(
                    "cyclecnt", "cyclecnt_width_g", ProGe::BIT_VECTOR,
                    ProGe::IN, *dbsmBlock);

                NetlistPort* dbsmBp41Port = new NetlistPort(
                    "bp_target_pc", "db_breakpoints_pc*IMEMADDRWIDTH",
                    ProGe::BIT_VECTOR, ProGe::IN, *dbsmBlock);
                toplevelBlock.netlist().connect(*ttaBp41Port, *dbsmBp41Port);

                NetlistPort* dbsmPCNextPort = new NetlistPort(
                    "pc_next", "IMEMADDRWIDTH", ProGe::BIT_VECTOR,
                    ProGe::IN, *dbsmBlock);

                NetlistPort* dbsmContinuePort = new NetlistPort(
                    "tta_continue", "1", BIT, ProGe::IN, *dbsmBlock);
                toplevelBlock.netlist().connect(
                    *ttaContinuePort, *dbsmContinuePort);

                NetlistPort* dbsmForceBreakPort = new NetlistPort(
                    "tta_forcebreak", "1", BIT, ProGe::IN, *dbsmBlock);
                toplevelBlock.netlist().connect(*ttaForceBreakPort,
                    *dbsmForceBreakPort);

                NetlistPort* dbsmStdoutBreakPort = new NetlistPort(
                    "tta_stdoutbreak", "1", BIT, ProGe::IN, *dbsmBlock);
                toplevelBlock.netlist().connect(
                    *ttaStdoutBreakPort, *dbsmStdoutBreakPort);

                NetlistPort* dbsmBpHitPort = new NetlistPort(
                    "bp_hit", "2+db_breakpoints",
                    ProGe::BIT_VECTOR, ProGe::OUT, *dbsmBlock);
                toplevelBlock.netlist().connect(*ttaBpHitPort, *dbsmBpHitPort);

                NetlistPort* dbsmExtlockPort = new NetlistPort(
                    "extlock", "1", BIT, ProGe::IN, *dbsmBlock);
                toplevelBlock.netlist().connect(
                    *toplevelBlock.port("busy"), *dbsmExtlockPort);


                // Connect to the highest bit of decoder lockrq vector, others are
                // already connected
                NetlistPort* dbsmGlockReqPort = new NetlistPort(
                    "bp_lockrq", "1", BIT, ProGe::OUT, *dbsmBlock);
                NetlistPort* decoderGlockReqPort =
                    decoderBlock->port("lock_req");
                int bit = decoderGenerator_->glockRequestWidth()-1;
                toplevelBlock.netlist().connect(
                    *dbsmGlockReqPort, *decoderGlockReqPort, 0, bit, 1);

                // Connect ifetch debug ports
                NetlistPort* ifetchDebugLockRqPort = new NetlistPort(
                    "db_lockreq", "1", 1, ProGe::BIT, ProGe::IN, *fetchBlock);
                toplevelBlock.netlist().connect(*dbsmGlockReqPort,
                    *ifetchDebugLockRqPort);
                NetlistPort* ifetchDebugResetPort = new NetlistPort(
                    "db_rstx", "1", 1, ProGe::BIT, ProGe::IN, *fetchBlock);
                toplevelBlock.netlist().connect(*ttaResetPort,
                    *ifetchDebugResetPort);
                NetlistPort* ifetchPCStartPort = new NetlistPort(
                    "db_pc_start", "IMEMADDRWIDTH", ProGe::BIT_VECTOR, ProGe::IN,
                    *fetchBlock);
                toplevelBlock.netlist().connect(*ifetchPCStartPort,
                                                *ttaPCStartPort);
                NetlistPort* ifetchPCPort = new NetlistPort(
                    "db_pc", "IMEMADDRWIDTH", ProGe::BIT_VECTOR, ProGe::OUT,
                    *fetchBlock);
                toplevelBlock.netlist().connect(*ifetchPCPort, *ttaPCPort);
                NetlistPort* ifetchPCNextPort = new NetlistPort(
                    "db_pc_next", "IMEMADDRWIDTH", ProGe::BIT_VECTOR, ProGe::OUT,
                    *fetchBlock);
                toplevelBlock.netlist().connect(
                    *ifetchPCNextPort, *dbsmPCNextPort);
                NetlistPort* ifetchCyclecountPort = new NetlistPort(
                    "db_cyclecnt", "64", 64, ProGe::BIT_VECTOR, ProGe::OUT,
                    *fetchBlock);
                toplevelBlock.netlist().connect(
                    *ifetchCyclecountPort, *ttaCyclecountPort);
                toplevelBlock.netlist().connect(
                    *ifetchCyclecountPort, *dbsmCyclecountPort);
                NetlistPort* ifetchLockcountPort = new NetlistPort(
                    "db_lockcnt", "64", 64, ProGe::BIT_VECTOR, ProGe::OUT,
                    *fetchBlock);
                toplevelBlock.netlist().connect(
                    *ifetchLockcountPort, *ttaLockcountPort);
                NetlistPort *ttaInstrPort = new NetlistPort(
                    "db_instr", "IMEMWIDTHINMAUS*IMEMMAUWIDTH",
                    ProGe::BIT_VECTOR, ProGe::OUT, toplevelBlock);
                NetlistPort* ifetchFetchblockPort =
                    fetchBlock->port("fetchblock");
                toplevelBlock.netlist().connect(
                    *ifetchFetchblockPort, *ttaInstrPort);

            }
        }

        // Connect decoder softreset
        NetlistPort* decoderDBResetPort = decoderBlock->port("db_tta_nreset");
        decoderDBResetPort->unsetStatic();
        toplevelBlock.netlist().connect(*ttaResetPort, *decoderDBResetPort);
    }

    void
    addDummyIfetchDebugPorts(const NetlistGenerator& generator) {
        NetlistBlock* fetchBlock = &generator.instructionFetch();
        // Connect ifetch debug ports
        NetlistPort* ifetchDebugLockRqPort = new NetlistPort(
            "db_lockreq", "1", 1, ProGe::BIT, ProGe::IN, *fetchBlock);
        ifetchDebugLockRqPort->setToStatic(ProGe::StaticSignal::GND);
        NetlistPort* ifetchDebugResetPort = new NetlistPort(
            "db_rstx", "1", 1, ProGe::BIT, ProGe::IN, *fetchBlock);
        ifetchDebugResetPort->setToStatic(ProGe::StaticSignal::VCC);
        new NetlistPort(
            "db_pc", "IMEMADDRWIDTH", ProGe::BIT_VECTOR, ProGe::OUT,
            *fetchBlock);
        new NetlistPort(
            "db_pc_next", "IMEMADDRWIDTH", ProGe::BIT_VECTOR, ProGe::OUT,
            *fetchBlock);
        new NetlistPort(
            "db_cyclecnt", "32", 32, ProGe::BIT_VECTOR, ProGe::OUT,
            *fetchBlock);
        new NetlistPort(
            "db_lockcnt", "32", 32, ProGe::BIT_VECTOR, ProGe::OUT,
            *fetchBlock);
    }

    /**
     * Generates the interconnection network and instruction decoder to
     * the given destination directory.
     *
     * @param dstDirectory The destination directory.
     * @param generator The netlist generator that generated the netlist.
     */
    virtual void
    generate(
        HDL language, const std::string& dstDirectory,
        const NetlistGenerator& generator,
        const IDF::MachineImplementation& implementation,
        const std::string& entityString) {
        const string DS = FileSystem::DIRECTORY_SEPARATOR;
        const string templateDir = Environment::dataDirPath("ProGe");

        BlockSourceCopier copier(implementation, entityString, language);
        MachineInfo::OperationSet cuOps =
            MachineInfo::getOpset(*generator.context().adf().controlUnit());
        Path proGeDataDir(Environment::dataDirPath("ProGe"));
        HDLTemplateInstantiator& instantiator =
            copier.getTemplateInstatiator();

        if (ttamachine_.isRISCVMachine()) {
            instantiator.replacePlaceholder(
                "ifetch-stall-cond",
                " and " + IFETCH_STALL_PORT_NAME + " = '0'");
            instantiator.replacePlaceholder(
                "ifetch-stall-port-declarations",
                IFETCH_STALL_PORT_NAME + " : in std_logic;");

            ProGe::RV32MicroCodeGenerator* microCodeGen =
                new RV32MicroCodeGenerator(ttamachine_, bem_, entityString);
            microCodeGen->setBypassInstructionRegister(
                bypassInstructionRegister());
            microCodeGen->generateRTL(instantiator, dstDirectory);
        }

        if (generateBusTrace()) {
            icGenerator_->setGenerateBusTrace(true);
            icGenerator_->setBusTraceStartingCycle(busTraceStartingCycle());
        } else {
            icGenerator_->setGenerateBusTrace(false);
        }

        try {
            // Check for illegal combinations in fetch/decode unit options

            if (!generateDebugger()) {
                // Signal declarations as a stand-in for debug ports
                instantiator.replacePlaceholderFromFile(
                    "db-signal-declarations",
                    proGeDataDir / "no_debug_signal_declaration.snippet");
            }

            if (hasSynchronousReset()) {
                if (language == VHDL) {
                    generator.instructionFetch().setParameter(
                        "sync_reset_g", "boolean", "true");
                    instantiator.replacePlaceholder("sync-waitlist", "clk");
                } else {  // language == Verilog
                    instantiator.replacePlaceholder(
                        "update-condition", "always@(posedge clk)");
                }
            } else {
                if (language == VHDL) {
                    instantiator.replacePlaceholder(
                        "sync-waitlist", "clk, rstx");
                } else {  // language == Verilog
                    instantiator.replacePlaceholder(
                        "update-condition",
                        "always@(posedge clk or negedge rstx)");
                }
            }

            if (generateNoSelfLockingFUs()) {
                decoderGenerator_->setGenerateNoLoopbackGlock(true);
                if (language == VHDL) {
                    generator.instructionFetch().setParameter(
                        "no_glock_loopback_g", "std_logic", "'1'");
                }  // todo verilog version
            }

            // Set the pc opcode port width
            std::size_t reqOpcodeWidth =
                CUOpcodeGenerator::gcuOpcodeWidth(ttamachine_);
            instantiator.replacePlaceholder(
                "pc-opcode-len", Conversion::toString(reqOpcodeWidth - 1));

            instantiator.replacePlaceholderFromFile(
                "default-instr-reg-write",
                Path(proGeDataDir / "default-instr-reg-write.snippet"));

            // Next PC process sensitivity list
            std::string sensitivityStringPCBypassed =
                "pc_in, pc_reg, increased_pc        ";
            std::string sensitivityStringPCNotBypassed = "";
            // Complete the default sensitivity list
            sensitivityStringPCBypassed += ",\n pc_load, pc_opcode";
            instantiator.replacePlaceholder(  // This is bypassed too
                "pc-sensitivity-list-pc-bypass", sensitivityStringPCBypassed);

            // Next PC signal assign condition
            std::string conditionString =
                "pc_load = '1' and ((unsigned(pc_opcode) = IFE_CALL or \n"
                "    unsigned(pc_opcode) = IFE_JUMP))\n";
            instantiator.replacePlaceholder(
                "not-bypassed-next-pc-condition", conditionString);

            if (generateDebugger()) {  // DEBUGTODO
                if (language != VHDL) {
                    std::string errorMsg =
                        "Language not set to VHDL when HW debugger is in "
                        "use.";
                    throw Exception(__FILE__, __LINE__, __func__, errorMsg);
                }

                generator.instructionFetch().setParameter(
                    "debug_logic_g", "boolean", "true");
                if (parameterValue(GENERATE_DEBUGGER_PARAM) ==
                    GENERATE_DEBUGGER_PARAM_MINIMAL) {
                  instantiator.replacePlaceholderFromFile(
                    "db-port-declarations",
                    Path(proGeDataDir/"debug_minimal_port_declaration.snippet"));
                  instantiator.replacePlaceholderFromFile(
                    "db-signal-declarations",
                    Path(proGeDataDir/"debug_minimal_signal_declaration.snippet"));
                } else {
                  instantiator.replacePlaceholderFromFile(
                          "db-port-declarations",
                          Path(proGeDataDir/"debug_port_declaration.snippet"));
                  instantiator.replacePlaceholderFromFile(
                          "db-signal-declarations",
                          Path(proGeDataDir/"debug_signal_declaration.snippet"));
                }
                copier.instantiateHDLTemplate(
                    templateDir + DS + "ifetch." +
                        (language == VHDL ? "vhdl" : "v") + ".tmpl",
                    dstDirectory,
                    std::string("ifetch.") +
                        (language == VHDL ? "vhdl" : "v"));
            } else {
                copier.instantiateHDLTemplate(
                    templateDir + DS + "ifetch." +
                        (language == VHDL ? "vhdl" : "v") + ".tmpl",
                    dstDirectory,
                    std::string("ifetch.") +
                        (language == VHDL ? "vhdl" : "v"));
            }
        } catch (const Exception& e) {
            abortWithError(e.errorMessage());
        }

        if (bypassInstructionRegister()) {
            if (language != VHDL) {
                std::string errorMsg =
                    "Instruction register bypass is not supported for "
                    "given HDL.";
                throw Exception(__FILE__, __LINE__, __func__, errorMsg);
            }

            addInstructioRegisterBypass(language, generator);
        }

        // generate the IC
        icGenerator_->SetHDL(language);
        icGenerator_->generateInterconnectionNetwork(dstDirectory);
        // generate the decoder
        decoderGenerator_->SetHDL(language);
        decoderGenerator_->generateInstructionDecoder(generator, dstDirectory);

        if (generateDebugger()) {
            if (language != VHDL) {
                std::string errorMsg =
                    "Language not set to VHDL when HW debugger is in use.";
                throw Exception(__FILE__, __LINE__, __func__, errorMsg);
            } else {
                generateDebuggerCode(generator);
            }
        }
        if (ttamachine_.isRISCVMachine()) {
            copier.instantiateHDLTemplate(
                templateDir + DS + "rv32_ifetch.vhdl.tmpl", dstDirectory,
                "ifetch.vhdl");
        } else {
            copier.instantiateHDLTemplate(
                templateDir + DS + "ifetch.vhdl.tmpl", dstDirectory,
                "ifetch.vhdl");
        }
    }

    /**
     * Returns the required latency of the hardware implementation of the
     * given immediate unit.
     *
     * @param iu The immediate unit.
     */
    virtual std::set<int>
    requiredRFLatencies(const TTAMachine::ImmediateUnit& iu) const {
        return decoderGenerator_->requiredRFLatencies(iu);
    }

    /**
     * Verifies that the plugin is compatible with the machine.
     *
     * @exception InvalidData If the plugin is not compatible with the
     *                        machine.
     */
    virtual void
    verifyCompatibility() const {
        int requiredDelaySlots = calculateSupportedDelaySlots();
        int specifiedDelaySlots = ttamachine_.controlUnit()->delaySlots();

        if (specifiedDelaySlots != requiredDelaySlots) {
            throw InvalidData(
                __FILE__, __LINE__, __func__,
                TCEString("Decoder generator supports ") +
                    Conversion::toString(requiredDelaySlots + 1) +
                    "-stage transport pipeline of GCU with given options. "
                    "Given machine has " +
                    Conversion::toString(specifiedDelaySlots + 1) +
                    " stages");
        }

        icGenerator_->verifyCompatibility();
        decoderGenerator_->verifyCompatibility();
    }

private:
    /**
     * Returns global package definitions in the form of a stream specifically
     * for the variable length instruction architecture.
     *
     * @param pkgStream The destination stream
     */
    virtual void
    writeGlobalDefinitions(HDL language, std::ostream& pkgStream) const {
        if (language == ProGe::VHDL) {
            pkgStream << "  -- instruction width" << endl
                      << "  constant INSTRUCTIONWIDTH : positive := "
                      << bem().width() << ";" << endl;
        } else if (language == ProGe::Verilog) {
            pkgStream << "// instruction width" << endl
                      << "parameter INSTRUCTIONWIDTH = " << bem().width()
                      << endl;
        }
    }

    /**
     * Tells whether IC generator should generate debug interface code.
     *
     * @return True if IC generator should generate the code.
     */
    bool generateDebugger(bool minimal = true) const {
        if (!hasParameterSet(GENERATE_DEBUGGER_PARAM)) {
            return false;
        } else {
            string paramValue = parameterValue(GENERATE_DEBUGGER_PARAM);
            if (paramValue == GENERATE_DEBUGGER_PARAM_YES ||
                paramValue == GENERATE_DEBUGGER_PARAM_INTERNAL ||
                paramValue == GENERATE_DEBUGGER_PARAM_EXTERNAL ||
                (paramValue == GENERATE_DEBUGGER_PARAM_MINIMAL && minimal)) {
                return true;
            } else {
                return false;
            }
        }
    }

    /**
     * Tells whether IC generator should generate bus tracing code.
     *
     * @return True if IC generator should generate the code.
     */
    bool generateBusTrace() const {
        if (!hasParameterSet(GENERATE_BUS_TRACE_PARAM)) {
            return false;
        } else {
            string paramValue = parameterValue(GENERATE_BUS_TRACE_PARAM);
            if (paramValue == GENERATE_BUS_TRACE_PARAM_YES) {
                return true;
            } else {
                return false;
            }
        }
    }

    bool
    hasSynchronousReset() const {
        return hasParameterSet(SYNC_RESET) &&
               parameterValue(SYNC_RESET) == ENABLE_FEATURE;
    }

    /**
     * Tells whether IC generator should generate global lock tracing code.
     *
     * @return True if IC generator should generate the code.
     */
    bool generateLockTrace() const {
        if (!hasParameterSet(GENERATE_LOCK_TRACE_PARAM)) {
            return false;
        } else {
            string paramValue = parameterValue(GENERATE_LOCK_TRACE_PARAM);
            if (paramValue == GENERATE_LOCK_TRACE_PARAM_YES) {
                return true;
            } else {
                return false;
            }
        }
    }

    /**
     * Tells whether instruction register should be left generated (true).
     */
    bool
    bypassInstructionRegister() const {
        if (!hasParameterSet(BYPASS_FETCHBLOCK_REG_PARAM)) {
            return false;
        } else {
            string paramValue = parameterValue(BYPASS_FETCHBLOCK_REG_PARAM);
            if (paramValue == BYPASS_FETCHBLOCK_REG_PARAM_YES) {
                return true;
            } else {
                return false;
            }
        }
    }

    /**
     * Returns the starting cycle to be written to the bus trace given as
     * parameter.
     *
     * @return The starting cycle.
     */
    int busTraceStartingCycle() const {
        if (!hasParameterSet(BUS_TRACE_STARTING_CYCLE)) {
            return 0;
        } else {
            string paramValue = parameterValue(BUS_TRACE_STARTING_CYCLE);
            try {
                unsigned int cycle = Conversion::toUnsignedInt(paramValue);
                return cycle;
            } catch (const Exception&) {
                return 0;
            }
        }
    }

    /**
     * Returns the starting cycle to be written to the global lock trace
     * given as parameter.
     *
     * If parameter is not defined use value from busTraceStartingCycle()
     * instead.
     *
     * @return The starting cycle.
     */
    int lockTraceStartingCycle() const {
        if (!hasParameterSet(LOCK_TRACE_STARTING_CYCLE)) {
            return busTraceStartingCycle();
        } else {
            string paramValue = parameterValue(LOCK_TRACE_STARTING_CYCLE);

            if (paramValue == BUS_TRACE_STARTING_CYCLE) {
               return busTraceStartingCycle();
            }

            try {
                unsigned int cycle = Conversion::toUnsignedInt(paramValue);
                return cycle;
            } catch (const Exception&) {
                return 0;
            }
        }
    }

    bool
    generateNoSelfLockingFUs() const {
        if (!hasParameterSet(NO_SELF_LOCKING_PARAM)) {
            return false;
        } else {
            string paramValue = parameterValue(NO_SELF_LOCKING_PARAM);
            return paramValue == NO_SELF_LOCKING_PARAM_YES;
        }
    }

    /**
     * Converts the given direction to a string.
     *
     * @param direction The direction.
     * @return The direction as string in VHDL.
     */
    static std::string
    vhdlDirection(ProGe::Direction direction) {
        switch (direction) {
            case ProGe::IN:
                return "in";
            case ProGe::OUT:
                return "out";
            case ProGe::BIDIR:
                return "inout";
        }
        assert(false);
        return std::string();
    }

    /**
     * Calculates the data port width of the given socket.
     *
     * @param socket The socket.
     * @return The width of the data port.
     */
    static int dataPortWidth(const TTAMachine::Socket& socket) {
        int ports = socket.portCount();
        int width(0);
        for (int i = 0; i < ports; i++) {
            Port* port = socket.port(i);
            if (port->width() > width) {
                width = port->width();
            }
        }
        return width;
    }

    /**
     * Enables instruction register bypass feature in the instruction fetch
     * block.
     */
    void
    addInstructioRegisterBypass(
        HDL language, const NetlistGenerator& generator) {
        assert(
            ttamachine_.isRISCVMachine() &&
            "Instruction register by pass only implemented for RISC-V");

        // Todo: remove when this feature is added to debugger-fetch.
        assert(
            !generateDebugger() &&
            "addInstructionRegisterBypass(): "
            "debugger does not support Instruction Register Bypass "
            "feature yet.");

        NetlistBlock* ifetchBlock = &generator.instructionFetch();

        if (language == ProGe::VHDL) {
            ifetchBlock->setParameter(
                "bypass_fetchblock_register", "boolean", "true");
        } else if (language == ProGe::Verilog) {
            // Todo
            assert(
                false &&
                "Verilog version of instruction register bypass "
                "not implemented.");
        } else {
            assert(false && "Unknown HDL choice.");
        }
    }

    /**
     * Return number of required delay slots. By default the number is three
     * but can be reduced or increased by options.
     */
    int
    calculateSupportedDelaySlots() const {
        int delaySlots = 3;
        if (bypassInstructionRegister()) {
            delaySlots -= 1;
        }
        return delaySlots;
    }

    /**
     * Reads parameters and configures the IC and decoder generators.
     */
    void
    readParameters() {
        icGenerator_->setExportBustrace(generateDebugger());
        decoderGenerator_->setGenerateDebugger(generateDebugger());
        // IC is combinatorial, no need for reset
        decoderGenerator_->setSyncReset(hasSynchronousReset());

        if (generateBusTrace()) {
            icGenerator_->setGenerateBusTrace(true);
            icGenerator_->setBusTraceStartingCycle(busTraceStartingCycle());
        } else {
            icGenerator_->setGenerateBusTrace(false);
            decoderGenerator_->setGenerateBusEnable(false);
        }

        if (generateLockTrace()) {
            decoderGenerator_->setGenerateLockTrace(true);
            decoderGenerator_->setLockTraceStartingCycle(
                lockTraceStartingCycle());
        } else {
            decoderGenerator_->setGenerateLockTrace(false);
        }
    }

    NetlistBlock* dbsmBlock;

    DefaultICGenerator* icGenerator_;
    DefaultDecoderGenerator* decoderGenerator_;
    const TTAMachine::Machine& ttamachine_;
    const BinaryEncoding& bem_;

};  // class DefaultICDecoderGenerator

EXPORT_ICDEC_GENERATOR(DefaultICDecoder)
EXPORT_ICDEC_ESTIMATOR(DefaultICDecoder)
