/**
 * @file MinimizeMachine.cc
 *
 * Explorer plugin that removes resources until the real time requirements
 * of applications are not reached anymore.
 *
 * @author Jari Mäntyneva 2007 (jari.mantyneva@tut.fi)
 * @author Esa Määttä 2008 (esa.maatta@tut.fi)
 * @note rating: red
 */

#include <vector>
#include <set>
#include <string>

#include "DesignSpaceExplorerPlugin.hh"

#include "DSDBManager.hh"
#include "Machine.hh"

#include "MachineResourceModifier.hh"
#include "DesignSpaceExplorer.hh"
#include "CostEstimates.hh"
#include "SimulatorConstants.hh"

using namespace TTAProgram;
using namespace TTAMachine;
using namespace HDB;

/**
 * Explorer plugin that removes resources until the real time requirements
 * of applications are not reached anymore.
 * 
 * @TODO: maxRunTime parameter, which overrides app dir max runtime setting
 * @TODO: if no maxRunTime then no frequency is needed == no cycle count
 * limits
 * 
 * Supported parameters:
 *  - min_bus, boolean for do minimize busses minimization, default true.
 *  - min_fu, boolean for do minimize FUs minimization, default true.
 *  - min_rf, boolean for do minimize RFs minimization, default true.
 *  - frequency, running frequency for applications.
 */
class MinimizeMachine : public DesignSpaceExplorerPlugin {
public:
    DESCRIPTION("Removes resources until the real time "
        "requirements of applications are not reached anymore.");
    
    /**
     * Minimizes given configuration by not exceeding applications max
     * runtime which depends on given frequncy
     */
    virtual std::vector<RowID>
    explore(const RowID& configurationID, const unsigned int&) {
        std::vector<RowID> result;

        readParameters();

        // TODO: make better parameter error function
        if (frequency_ < 0) {
            result.push_back(configurationID);
            return result;
        }

        RowID min = minimizeMachine(configurationID);

        result.push_back(min);
        return result;
    }

private:
    /// minimize busses
    bool minBus_;
    /// minimize function units
    bool minFU_;
    /// minimize register files
    bool minRF_;
    /// running frequency in MHz for apps
    int frequency_;

    /**
     * Reads the parameters given to the plugin.
     */
    void readParameters() {
        const std::string minBus = "min_bus";
        const std::string minFU = "min_fu";
        const std::string minRF = "min_rf";
        const std::string frequency = "frequency";
        const int frequencyDefault = -1;

        if (hasParameter(minBus)) {
            try {
                minBus_ = booleanValue(parameterValue(minBus));
            } catch (const Exception& e) {
                parameterError(minBus, "Boolean");
                minBus_ = false;
            }
        } else {
            // set defaut value for minimize busses
            minBus_ = true;
        }

        if (hasParameter(minFU)) {
            try {
                minFU_ = booleanValue(parameterValue(minFU));
            } catch (const Exception& e) {
                parameterError(minFU, "Boolean");
                minFU_ = false;
            }
        } else {
            // set defaut value for minimize FUs
            minFU_ = true;
        }

        if (hasParameter(minRF)) {
            try {
                minRF_ = booleanValue(parameterValue(minRF));
            } catch (const Exception& e) {
                parameterError(minRF, "Boolean");
                minRF_ = false;
            }
        } else {
            // set defaut value for minimize RFs
            minRF_ = true;
        }

        if (hasParameter(frequency)) {
            try {
                frequency_ = Conversion::toInt(parameterValue(frequency));
            } catch (const Exception& e) {
                parameterError(frequency, "Integer");
                frequency_ = frequencyDefault;
            }
        } else {
            // frequency needed, added bogus default value
            parameterMissing(frequency, "Integer");
            frequency_ = frequencyDefault;
        }
    }
    
    /**
     * Print error message of invalid parameter to plugin error stream.
     *
     * @param param Name of the parameter that has invalid value.
     * @param type Type of the parameter ought to be.
     */
    void parameterError(const std::string& param, const std::string& type) {
        std::ostringstream msg(std::ostringstream::out);
        msg << "Invalid parameter value '" << parameterValue(param)
            << "' on parameter '" << param << "'. " << type 
            << " value expected." << std::endl;
        errorOuput(msg.str());
    }

    /**
     * Print error message of missing parameter to plugin error stream.
     *
     * @param param Name of the parameter that has missing value.
     * @param type Type of the parameter ought to be.
     */
    void parameterMissing(const std::string& param, const std::string& type) {
        std::ostringstream msg(std::ostringstream::out);
        msg << "Parameter '" << param << "' with expected type '" 
            << type << "' is missing, nothing done." << std::endl;
        errorOuput(msg.str());
    }
    
    /**
     * Minimizes the number of buses, function units and register files in the given
     * configuration in that order.
     *
     * This function minimizes the buses first, then function units and then 
     * register files.
     * Removes also the extra sockets after minimization. Won't handle
     * implementaions (IDF handling missing from fu and rf part).
     *
     * @param confToMinimize Configuration to be minimized.
     * @param maxCycles Maximum clock cycle count that won't be exceeded.
     * @return Minimized configuration id.
     */
    RowID minimizeMachine(RowID confToMinimize) {

        // calculate maxCycleCounts for all apps
        DSDBManager& dsdb = db();
        std::vector<ClockCycleCount> maxCycleCounts;
        std::set<RowID> applicationIDs = dsdb.applicationIDs();
        std::set<RowID>::const_iterator applicationIter;
        for (applicationIter = applicationIDs.begin();
                applicationIter != applicationIDs.end();
                applicationIter++) {

            TestApplication testApplication(
                    dsdb.applicationPath(*applicationIter));

            if (testApplication.maxRuntime() < 1) {
                maxCycleCounts.push_back(0);
            } else {
                maxCycleCounts.push_back(static_cast<ClockCycleCount>(
                            testApplication.maxRuntime() * 
                            frequency_ * 100000));
            }
        }
    
        RowID orgConfToMinimize = confToMinimize;
        if (minBus_) {
            confToMinimize = minimizeBuses(confToMinimize, maxCycleCounts);
        }

        if (minFU_) {
            confToMinimize = minimizeFunctionUnits(confToMinimize, 
                    maxCycleCounts);
        }

        if (minRF_) {
            confToMinimize = minimizeRegisterFiles(confToMinimize, 
                    maxCycleCounts);
        }

        if (orgConfToMinimize == confToMinimize) {
            debugLog(std::string("No new config could be generated by "
                        "MinimizeMachine plugin."));
        }
        return confToMinimize;
    }

    /**
     * Reduces the number of buses in the machine as much as possible to still
     * achieve the application run time requirements.
     *
     * This function can handle implementations. Uses binary search to minimize
     * the buses.
     *
     * @param confToMinimize ID of the machine configuration which buses
     * are reduced.
     * @param maxCycleCounts Max cycle counts per program.
     * @return ID of the new congiguration where buses are reduced.
    */
    RowID minimizeBuses(
            RowID confToMinimize, 
            std::vector<ClockCycleCount>& maxCycleCounts) {

        DSDBManager& dsdb = db();

        DSDBManager::MachineConfiguration configuration = 
            dsdb.configuration(confToMinimize);

        TTAMachine::Machine* mach = NULL;
        try {
            mach = dsdb.architecture(configuration.architectureID);
        } catch (const Exception& e) {
            debugLog(std::string("No machine architecture found in config id "
                        "by MimimizeMachine plugin. "));
            return confToMinimize;
        }

        TTAMachine::Machine::BusNavigator busNav = mach->busNavigator();

        // can't reduce buses from one
        int origBusCount = busNav.count();
        if (origBusCount < 2) {
            return confToMinimize;
        }

        DesignSpaceExplorer explorer;
        explorer.setDSDB(dsdb);

        CostEstimates estimates;
        if (!explorer.evaluate(configuration, estimates, false)) {
            delete mach;
            mach = NULL;
            return confToMinimize;
        }
         
        // goes through every apps new cycles
        for (int i = 0; i < estimates.cycleCounts(); i++) {
            // if no time constraints
            if (maxCycleCounts.at(i) < 1) {
                continue;
            }
            // if some apps maxCycles was exceeded
            if (maxCycleCounts.at(i) < estimates.cycleCount(i)) {
                // given configuration exceeds the given maximum of clock cycles
                delete mach;
                mach = NULL;
                return confToMinimize;
            }
        }

        MachineResourceModifier modifier;

        // variables for binary search    
        int busHigh = origBusCount;
        int busLow = 1;
        int busMid = (busLow + busHigh) /2;

        TTAMachine::Machine* newMach = NULL;
        RowID lastArchID = 0;
        RowID lastConfID = 0;
        RowID lastOKArchID = 0;
        RowID lastOKConfID = 0;

        std::list<std::string> removedBusNames;

        // use binary search to find out the bus count that can be removed
        // removes busses as long as each apps max cycle counts are not exceeded
        // if buses are not of equal value this doesn't really work.
        do {
            assert(newMach == NULL);
            bool newConfigOK = false;
            newMach = new TTAMachine::Machine(*mach);

            int busesToRemove = (origBusCount - busMid);

            if (!modifier.removeBuses(busesToRemove, *newMach, removedBusNames)) {
                // TODO: some good way to cope with non complete bus removal
            }

            DSDBManager::MachineConfiguration newConfiguration;
            try {
                lastArchID = dsdb.addArchitecture(*newMach);
                // machine stored to dsdb can be deleted
                delete newMach;
                newMach = NULL;
            } catch (const RelationalDBException& e) {
                // Error occurred while adding adf to the dsdb, adf
                // probably too big
                delete newMach;
                newMach = NULL;
                break;
            }

            newConfiguration.architectureID = lastArchID;
            newConfiguration.hasImplementation = false;
            try {
                lastConfID = dsdb.addConfiguration(newConfiguration);
            } catch (const KeyNotFound& e) {
                break;
            }
            CostEstimates newEstimates;

            if (explorer.evaluate(newConfiguration, newEstimates, false)) {
                newConfigOK = true;
                
                // goes through every apps new cycles
                for (int i = 0; i < newEstimates.cycleCounts(); i++) {
                    // if no time constraints
                    if (maxCycleCounts.at(i) < 1) {
                        continue;
                    }
                    // if some apps maxCycles was exceeded
                    if (maxCycleCounts.at(i) < newEstimates.cycleCount(i)) {
                        newConfigOK = false;
                        break;
                    }
                }
            } 
            
            if (newConfigOK) {
                busHigh = busMid - 1;
                busMid = (busLow + busMid) / 2;
                lastOKArchID = lastArchID;
                lastOKConfID = lastConfID;
            } else {
                busLow = busMid + 1;
                busMid = (busMid + busHigh) / 2;
            }
        } while (busLow <= busHigh && busLow <= busMid);

        // these aren't needed anymore
        lastArchID = 0;
        lastConfID = 0;

        // delete old machine
        delete mach;
        mach = NULL;
        
        // check if no new architecture/config could be created
        if (lastOKArchID == 0) {
            return confToMinimize;
        }
        
        // create new idf for the new machine if needed
        if (configuration.hasImplementation) {
            IDF::MachineImplementation* idf =
                dsdb.implementation(configuration.implementationID);

            // remove removed busses from orginal implementation file
            std::list<std::string>::const_iterator busNameIter = 
                removedBusNames.begin();
            while (busNameIter != removedBusNames.end()) {
                idf->removeBusImplementation(*busNameIter);
                busNameIter++;
            }

            DSDBManager::MachineConfiguration newConfiguration;
            newConfiguration.architectureID = lastOKArchID;

            CostEstimates newEstimates;
            if (explorer.evaluate(newConfiguration, newEstimates, false)) {
                newConfiguration.implementationID = 
                    dsdb.addImplementation(
                            *idf, newEstimates.longestPathDelay(),newEstimates.area());

                delete idf;
                idf = NULL;

                newConfiguration.hasImplementation = true;
                // add new configuration
                RowID confID = dsdb.addConfiguration(newConfiguration);
                return confID;
            } else {
                return confToMinimize;
            }
        } else {
            return lastOKConfID;
        }

        assert(false);
        return 9999; 
    }

    /**
     * Reduces the number of register files in the machine as much as possible
     * to still achieve the application run time requirements.
     *
     * This function won't handle implementations.
     *
     * @param confToMinimize ID of the machine configuration which register
     *        files are reduced.
     * @param maxCycleCounts Max cycle counts per program.
     * @return ID of the new congiguration where register files are reduced or
     *         same that was given if nothing was done.
     */
    RowID minimizeRegisterFiles(
            RowID confToMinimize, 
            std::vector<ClockCycleCount>& maxCycleCounts) {

        DSDBManager& dsdb = db();
        RowID latestConfID = 0;

        MachineResourceModifier modifier;
        DesignSpaceExplorer explorer;
        explorer.setDSDB(dsdb);

        DSDBManager::MachineConfiguration configuration =
            dsdb.configuration(confToMinimize);

        const TTAMachine::Machine* origMach = NULL;
        try {
            origMach = dsdb.architecture(configuration.architectureID);
        } catch (const Exception& e) {
            debugLog(std::string("No machine architecture found in config id "
                        "by MimimizeMachine plugin. "));
            return confToMinimize;
        }

        MachineResourceModifier::RegisterMap origRegisterMap;
        modifier.analyzeRegisters(*origMach, origRegisterMap);

        CostEstimates estimates;
        // evaluates the desing with all dsdb apps
        if (!explorer.evaluate(configuration, estimates, false)) {
            // can't evaluate the given configuration
            delete origMach;
            origMach = NULL;
            return confToMinimize;
        }

        // goes through every apps new cycles
        for (int i = 0; i < estimates.cycleCounts(); i++) {
            // if no time constraints
            if (maxCycleCounts.at(i) < 1) {
                continue;
            }
            // if some apps maxCycles was exceeded
            if (maxCycleCounts.at(i) < estimates.cycleCount(i)) {
                // given configuration exceeds the given maximum of clock cycles
                delete origMach;
                origMach = NULL;
                return confToMinimize;
            }
        }

        MachineResourceModifier::RegisterMap::const_iterator registerMapIter =
            origRegisterMap.begin();

        ObjectState* currentState = origMach->saveState();
        // go through all different register file types
        for (; registerMapIter != origRegisterMap.end(); registerMapIter++) {
            TTAMachine::Machine mach;
            mach.loadState(currentState);
            TTAMachine::Machine::RegisterFileNavigator rfNav =
                mach.registerFileNavigator();

            int i = 0;
            rfNav = mach.registerFileNavigator();
            // go through every register file in the machine to find matching RF
            while (i < rfNav.count()) {
                if (((*registerMapIter).second)->isArchitectureEqual(
                            *rfNav.item(i))) {

                    mach.removeRegisterFile(*rfNav.item(i));
                    std::list<std::string> socketList;
                    modifier.removeNotConnectedSockets(mach, socketList);
                    DSDBManager::MachineConfiguration newConfiguration;
                    newConfiguration.architectureID = dsdb.addArchitecture(mach);
                    newConfiguration.hasImplementation = false;
                    RowID confID = dsdb.addConfiguration(newConfiguration);
                    CostEstimates newEstimates;

                    // if the evaluation fails the removed register is needed
                    // and the old machine state is loaded
                    if (!explorer.evaluate(
                                newConfiguration, newEstimates, false)) {

                        // continue with old machine state and
                        // try with next register file type
                        break;
                    }

                    // goes through every apps new cycles
                    bool belowMaxCycles = true;
                    for (int i = 0; i < newEstimates.cycleCounts(); i++) {
                        // if no time constraints
                        if (maxCycleCounts.at(i) < 1) {
                            continue;
                        }
                        // if some apps maxCycles was exceeded
                        if (maxCycleCounts.at(i) < newEstimates.cycleCount(i)) {
                            belowMaxCycles = false;
                            break;
                        }
                    }
                    if (!belowMaxCycles) {
                        // continue with old machine state and
                        // try with next register file type
                        break;
                    }
                    latestConfID = confID;

                    // save the new machine state
                    delete currentState;
                    currentState = mach.saveState();

                    // else continue removing same register types
                    // no need to advance the rf navigator, 
                    // because of the removal
                    continue;
                }
                // if the registers did not match, try next one in the machine
                i++;
            }
        }

        delete origMach;
        origMach = NULL;
        if (latestConfID != 0) {
            return latestConfID;
        } else {
            // no new config could be created
            return confToMinimize; 
        }
    }

    /**
     * Reduces the number of function units in the machine as much as possible
     * to still achieve the application run time requirements.
     *
     * This function won't handle implementations.
     *
     * @param confToMinimize ID of the machine configuration which function units
     *                       are reduced.
     * @param maxCycleCounts Max cycle counts per program.
     * @return ID of the new congiguration where function units are reduced or
     *         same that was given if nothing was done.
     */
    RowID minimizeFunctionUnits(
            RowID confToMinimize,
            std::vector<ClockCycleCount>& maxCycleCounts) {

        DSDBManager& dsdb = db();
        RowID latestConfID = 0;

        MachineResourceModifier modifier;
        DesignSpaceExplorer explorer;
        explorer.setDSDB(dsdb);

        DSDBManager::MachineConfiguration configuration =
            dsdb.configuration(confToMinimize);

        const TTAMachine::Machine* origMach = NULL;
        try {
            origMach = dsdb.architecture(configuration.architectureID);
        } catch (const Exception& e) {
            debugLog(std::string("No machine architecture found in config id "
                        "by MimimizeMachine plugin. "));
            return confToMinimize;
        }

        MachineResourceModifier::FunctionUnitMap origFUMap;
        modifier.analyzeFunctionUnits(*origMach, origFUMap);

        CostEstimates estimates;
        if (!explorer.evaluate(configuration, estimates, false)) {
            // can't evaluate the given configuration
            delete origMach;
            origMach = NULL;
            return confToMinimize;
        }

        // goes through every apps new cycles
        for (int i = 0; i < estimates.cycleCounts(); i++) {
            // if no time constraints
            if (maxCycleCounts.at(i) < 1) {
                continue;
            }
            // if some apps maxCycles was exceeded
            if (maxCycleCounts.at(i) < estimates.cycleCount(i)) {
                // given configuration exceeds the given maximum of clock cycles
                delete origMach;
                origMach = NULL;
                return confToMinimize;
            }
        }
        
        MachineResourceModifier::FunctionUnitMap::const_iterator fuMapIter =
            origFUMap.begin();

        ObjectState* currentState = origMach->saveState();

        // go through all different function unit types
        for (; fuMapIter != origFUMap.end(); fuMapIter++) {
            TTAMachine::Machine mach;
            mach.loadState(currentState);
            TTAMachine::Machine::FunctionUnitNavigator fuNav =
                mach.functionUnitNavigator();
            int i = 0;
            // go through every function unit in the machine to find matching FU
            while (i < fuNav.count()) {
                if (((*fuMapIter).second)->isArchitectureEqual(
                            fuNav.item(i), true)) {

                    mach.removeFunctionUnit(*fuNav.item(i));
                    std::list<std::string> socketList;
                    modifier.removeNotConnectedSockets(mach, socketList);
                    DSDBManager::MachineConfiguration newConfiguration;
                    newConfiguration.architectureID = dsdb.addArchitecture(mach);
                    newConfiguration.hasImplementation = false;
                    RowID confID = dsdb.addConfiguration(newConfiguration);
                    CostEstimates newEstimates;

                    // if the evaluation fails the removed FU is needed
                    // and the old machine state is loaded
                    if (!explorer.evaluate(
                                newConfiguration, newEstimates, false)) {

                        // continue with old machine state and
                        // try with next FU type
                        break;
                    }

                    // goes through every apps new cycles
                    bool belowMaxCycles = true;
                    for (int i = 0; i < newEstimates.cycleCounts(); i++) {
                        // if no time constraints
                        if (maxCycleCounts.at(i) < 1) {
                            continue;
                        }
                        // if some apps maxCycles was exceeded
                        if (maxCycleCounts.at(i) < newEstimates.cycleCount(i)) {
                            belowMaxCycles = false;
                            break;
                        }
                    }
                    if (!belowMaxCycles) {
                        // continue with old machine state and
                        // try with next register file type
                        break;
                    }
                    latestConfID = confID;

                    // save the new machine state
                    delete currentState;
                    currentState = mach.saveState();

                    // continue removing same FU type
                    // no need to advance the FU navigator, 
                    // because of the removal
                    continue;
                }
                // if the units did not match, try next one in the machine
                i++;
            }
        }
        delete origMach;
        origMach = NULL;
        if (latestConfID != 0) {
            return latestConfID;
        } else {
            // no new config could be created
            return confToMinimize; 
        }
    }
};

EXPORT_DESIGN_SPACE_EXPLORER_PLUGIN(MinimizeMachine)
