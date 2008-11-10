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
 * @file DesignSpaceExplorer.cc
 *
 * Implementation of DesignSpaceExplorer class
 *
 * @author Jari M‰ntyneva 2006 (jari.mantyneva-no.spam-tut.fi)
 * @author Esa M‰‰tt‰ 2008 (esa.maatta-no.spam-tut.fi)
 * @note rating: red
 */

#include <boost/timer.hpp>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <set>
#include <vector>
#include <string>

#include "DesignSpaceExplorer.hh"
#include "LLVMBackend.hh"
#include "DesignSpaceExplorerPlugin.hh"
#include "CostEstimates.hh"
#include "ExecutionTrace.hh"
#include "DSDBManager.hh"
#include "Machine.hh"
#include "Program.hh"
#include "MachineImplementation.hh"
#include "PluginTools.hh"
#include "CostEstimatorTypes.hh"
#include "UniversalMachine.hh"
#include "StringTools.hh"
#include "OperationBehavior.hh"
#include "SimulatorToolbox.hh"
#include "SimulatorFrontend.hh"
#include "SimulatorInterpreter.hh"
#include "OperationGlobals.hh"
#include "Application.hh"
#include "ComponentImplementationSelector.hh"

using std::set;
using std::vector;
using std::string;
using namespace CostEstimator;

PluginTools
DesignSpaceExplorer::pluginTool_;


/**
 * The constructor.
 */
DesignSpaceExplorer::DesignSpaceExplorer() {
    
    //schedulingPlan_ = 
    //    SchedulingPlan::loadFromFile(Environment::oldGccSchedulerConf());
    oStream_ = new std::ostringstream;
    OperationGlobals::setOutputStream(*oStream_);
}

/**
 * The destructor.
 */
DesignSpaceExplorer::~DesignSpaceExplorer() {

    //delete schedulingPlan_;
    //schedulingPlan_ = NULL;
    delete oStream_;
    oStream_ = NULL;
}

/**
 * Sets new design space database.
 *
 * @param dsdb Design space database to be used with the explorer.
 */
void
DesignSpaceExplorer::setDSDB(DSDBManager& dsdb) {

    dsdb_ = &dsdb;
}


/**
 * Evaluates one processor configuration (architecture+implementation pair).
 *
 * Evaluates the total area and the longest path delay of the current 
 * processor configuration (architecture+implementation pair) and also the
 * energy consumed in each program in the DSDB with this processor
 * configuration. Also configurations without implementation can be evaluated
 * but no cost estimations are performed so the CostEstimate object won't
 * include area, energy and longest path delay estimations. Estimation is not
 * included either if the estimate flag is set to false.
 *
 * @param configuration Machine configuration (architecture, implementation).
 * @param result CostEstimates object where the configuration cost
 * estimates are stored if the evaluation succeeds. 
 * @param estimate Flag indicating that the evaluate will also estimate the
 *                 given configuration.
 * @return Returns true if the evaluation succeeds false otherwise.
 */
bool
DesignSpaceExplorer::evaluate(
    const DSDBManager::MachineConfiguration& configuration,
    CostEstimates& result, bool estimate) {

    TTAMachine::Machine* adf;
    IDF::MachineImplementation* idf = NULL;
    if (configuration.hasImplementation) {
        adf = dsdb_->architecture(configuration.architectureID);
        idf = dsdb_->implementation(configuration.implementationID);
    } else {
        adf = dsdb_->architecture(configuration.architectureID);
    }

    try {
        if (configuration.hasImplementation && estimate) {
            // estimate total area
            AreaInGates totalArea = estimator_.totalArea(*adf, *idf);
            dsdb_->setAreaEstimate(configuration.implementationID, totalArea);
            result.setArea(totalArea);
        
            // estimate longest path delay
            DelayInNanoSeconds longestPathDelay = 0;
            longestPathDelay = estimator_.longestPath(*adf, *idf);
            dsdb_->setLongestPathDelayEstimate(
                configuration.implementationID, longestPathDelay);
            result.setLongestPathDelay(longestPathDelay);
        }
       
        // get all programs from the dsdb
        set<RowID> applicationIDs = dsdb_->applicationIDs();
        for (set<RowID>::const_iterator i = applicationIDs.begin();
             i != applicationIDs.end(); i++) {

            string applicationPath = dsdb_->applicationPath(*i);
            TestApplication testApplication(applicationPath);
            
            std::string applicationFile = testApplication.applicationPath();

            // test that program is found
            if (applicationFile.length() < 1) {
                std::cerr << "No program found from application dir '" 
                          << applicationPath << std::endl;
                delete adf;
                adf = NULL;
                delete idf;
                idf = NULL;
                return false;
            }
            
            TTAProgram::Program* scheduledProgram =
                schedule(applicationFile, *adf);

            if (scheduledProgram == NULL) {
                verboseLogC("Evaluate failed: Scheduling program failed.", 1)
                delete adf;
                adf = NULL;
                delete idf;
                idf = NULL;
                return false;
            }

            // simulate the scheduled program
            ClockCycleCount runnedCycles;
            const ExecutionTrace* traceDB = NULL;
            if (configuration.hasImplementation && estimate) {
                traceDB = simulate(
                    *scheduledProgram, *adf, testApplication, 0, runnedCycles,
                    true);
            } else {
                simulate(
                    *scheduledProgram, *adf, testApplication, 0, runnedCycles,
                    false);
            }

            //std::cerr << "DEBUG: simulated" << std::endl;
            // verify the simulation
            if (testApplication.hasCorrectOutput()) {
                string correctResult = testApplication.correctOutput();
                string resultString = oStream_->str();
                if (resultString != correctResult) {
                    std::cerr << "Simulation FAILED, possible bug in scheduler!"
                              << std::endl;
                    std::cerr << "Architecture id in DSDB:" << std::endl;
                    std::cerr << configuration.architectureID << std::endl;
                    std::cerr << "use sqlite3 to find out which configuration "
                              << "has that id to get the machine written to "
                              << "ADF." << std::endl;
                    // @todo Do a method into DSDBManager to find out the
                    //       configuration ID.
                    std::cerr << "********** result found:" << std::endl;
                    std::cerr << resultString << std::endl;
                    std::cerr << "********** expected result:" << std::endl;
                    std::cerr << correctResult << std::endl;
                    std::cerr << "**********" << std::endl;
                    delete idf;
                    idf = NULL;
                    delete adf;
                    adf = NULL;
                    return false;
                }
                //std::cerr << "DEBUG: simulation OK" << std::endl;
                // reset the stream pointer in to the beginning and empty the
                // stream
                oStream_->str("");
                oStream_->seekp(0);

                // add simulated cycle count to dsdb
                dsdb_->addCycleCount(
                    (*i), configuration.architectureID,
                    runnedCycles);
                
                // add cycle count to result
                result.setCycleCount(*scheduledProgram, runnedCycles);
            }
            if (configuration.hasImplementation && estimate) {
                // energy estimate the simulated program
                EnergyInMilliJoules programEnergy =
                    estimator_.totalEnergy(
                        *adf, *idf, *scheduledProgram, *traceDB);
                dsdb_->addEnergyEstimate(
                    (*i), configuration.implementationID, programEnergy);
                result.setEnergy(*scheduledProgram, programEnergy);
            }
            delete traceDB;
            traceDB = NULL;
        }
    } catch (const Exception& e) {
        delete adf;
        adf = NULL;
        delete idf;
        idf = NULL;
        debugLog(e.errorMessageStack());
        return false;
    }
    delete idf;
    idf = NULL;
    delete adf;
    adf = NULL;
    return true;
}


/**
 * Returns the DSDBManager of the current exploration process.
 *
 * @return The DSDBManager of the current exploration process.
 */
DSDBManager&
DesignSpaceExplorer::db() {

    return *dsdb_;
}


/**
 * Compiles the given application bytecode file on the given target machine.
 *
 * @param applicationFile Bytecode filename with path.
 * @param machine The machine to compile the sequential program against.
 * @return Scheduled parallel program or NULL if scheduler produced exeption.
 */
TTAProgram::Program*
DesignSpaceExplorer::schedule(
    const std::string applicationFile,
    TTAMachine::Machine& machine) {

    // optimization level
    const int optLevel = 2;

    // Run compiler and scheduler for current machine
    try {
        LLVMBackend compiler;
        return compiler.compileAndSchedule(applicationFile, machine, 
                optLevel, Application::verboseLevel());
    } catch (Exception& e) {
        std::cerr << "Error compiling and scheduling '" 
            << applicationFile << "':" << std::endl
            << e.errorMessageStack() << std::endl;
        return NULL;
    }
}


/**
 * Simulates the parallel program.
 *
 * Simulates the target machine as long as the program runs or the maximum
 * cycle count is reached. If maximum cycle count is reached an exception is
 * thrown.
 * 
 * @param program Sequential program.
 * @param machine Target machine.
 * @param testApplication Test application directory.
 * @param maxCycles Maximum amount of clock cycles that program is allowed to
 * run. Not used by this implementation.
 * @param runnedCycles Simulated cycle amount is stored here.
 * @param tracing Flag indicating is the tracing used.
 * @return Execution trace of the program.
 * @exception Exception All exceptions produced by simulator engine except
 * SimulationCycleLimitReached in case of max cycles are reached without
 * program finishing and SimulationTimeOut in case of simulation is killed
 * after simulating maximum time that is currently 10h.
 */
const ExecutionTrace*
DesignSpaceExplorer::simulate(
    const TTAProgram::Program& program, 
    const TTAMachine::Machine& machine,
    const TestApplication& testApplication, 
    const ClockCycleCount&,
    ClockCycleCount& runnedCycles, 
    const bool tracing, 
    const bool useCompiledSimulation)
    throw (Exception) {

    // initialize the simulator
    SimulatorFrontend simulator(useCompiledSimulation);
    
    // setting simulator timeout in seconds
    simulator.setTimeout(480);

    // use memory file in sqlite
    const string traceFile = ":memory:";
    simulator.setTraceDBFileName(traceFile);
    simulator.setRFAccessTracing(tracing);
    simulator.setUtilizationDataSaving(tracing);
    simulator.setExecutionTracing(tracing);
    simulator.loadMachine(machine);
    simulator.loadProgram(program);
    // run the 'setup.sh' in the test application directory
    testApplication.setupSimulation();
    // if there is 'simulate.ttasim' file in the application dir we use that
    if (testApplication.hasSimulateTTASim()) {
        std::string command = "";
        std::istream* input = testApplication.simulateTTASim();
        BaseLineReader reader(*input, *oStream_);
        reader.initialize();
        reader.setPromptPrinting(false);
        SimulatorInterpreterContext interpreterContext(simulator);
        SimulatorInterpreter interpreter(0, NULL, interpreterContext, reader);
        while (!interpreter.isQuitCommandGiven()) {
            try {
                command = reader.readLine();
            } catch (const EndOfFile&) {
                interpreter.interpret(SIM_INTERP_QUIT_COMMAND);
                if (interpreter.result().size() > 0) {
                    *oStream_ << interpreter.result() << std::endl;
                }
                break;
            }
            command = StringTools::trim(command);
            if (command == "") {
                continue;
            }
            interpreter.interpret(command);
            if (interpreter.result().size() > 0) {
                *oStream_ << interpreter.result() << std::endl;
            }
        }
        delete input;
        input = NULL;
    } else {
        // no 'simulate.ttasim' file
        BaseLineReader reader(std::cin, *oStream_);
        reader.initialize();
        SimulatorInterpreterContext interpreterContext(simulator);
        SimulatorInterpreter interpreter(0, NULL, interpreterContext, reader);
        simulator.run();
        *oStream_ << interpreter.result() << std::endl;
    }

    runnedCycles = simulator.cycleCount();

    // Flush data collected during simulation to the trace file.
    simulator.killSimulation();
    if (tracing) {
        return simulator.lastTraceDB();
    } else {
        return NULL;
    }
/*
    // start timer to stop too long simulations
    // Simulation is stopped after 10 hours of simulation, since it would most
    // propably never stop itself.
    boost::timer simulationTimer;
    int maxSimulationTimeInSeconds = 36000;

    // amount of simulated cycles so far
    ClockCycleCount simulatedCycles = 0;

    // amount of simulation steps to be advanced at a time
    ClockCycleCount steps = 100000;
    // run the simulation
    while ((simulatedCycles < maxCycles || maxCycles == 0)
           && !simulator_.hasSimulationEnded()) {
        simulator_.step(steps);
        simulatedCycles += steps;
        
        // In case the maximum simulation time is reched the simulation is
        // stopped and an exception is thrown.
        if (simulationTimer.elapsed() > maxSimulationTimeInSeconds) {
            simulator_.killSimulation();
            const std::string errorMessage = "Max simulation time reached";
            throw SimulationTimeOut(__FILE__, __LINE__, __func__, errorMessage);
        }        
    }

    // In case the maximum cycle amount is reached the
    // simulation is stopped and an exception is thrown.
    if (simulatedCycles < maxCycles && maxCycles != 0) {
        simulator_.killSimulation();
        const std::string errorMessage = "Max simulation cycles reached";
        throw SimulationCycleLimitReached(
            __FILE__, __LINE__, __func__, errorMessage);
    }

    // Simulation verification

    // Flush data collected during simulation to the trace file.
    simulator_.killSimulation();

    return &simulator_.getTraceDB();
*/
}

/**
 * Loads the given explorer plugin from the default search pathes of the
 * explorer plugins.
 *
 * Searches for file named the plugin with an extension ".so".
 * e.g. if the plugin is namen SimplePlugin is the file SimplePlugin.so
 * loaded. Plugins are searched from the default search pathes of explorer
 * plugins.
 * 
 * @param pluginName Name of the plugin to be loaded.
 * @param dsdb DSDBManager to be used by the plugin.
 * @return Returns the loaded DesignSpacsExplorerPlugin instance.
 * @exception FileNotFound If the given plugin is not found from the search
 * paths of explorer plugins.
 * @exception DynamicLibraryException If the dynamic library cannot be opened.
 */
DesignSpaceExplorerPlugin*
DesignSpaceExplorer::loadExplorerPlugin(
    const std::string& pluginName, DSDBManager* dsdb)
    throw (FileNotFound, DynamicLibraryException) {

    string pluginFileName = pluginName + ".so";
    vector<string> searchPaths = Environment::explorerPluginPaths();
    for (vector<string>::const_iterator iter = searchPaths.begin();
         iter != searchPaths.end(); iter++) {
        
        if (FileSystem::fileExists(*iter)) {
            pluginTool_.addSearchPath(*iter);
        }
    }
    pluginTool_.registerModule(pluginFileName);
        DesignSpaceExplorerPlugin* (*pluginCreator)();
        pluginTool_.importSymbol(
                "create_explorer_plugin_" + pluginName, pluginCreator, 
                pluginFileName);

    DesignSpaceExplorerPlugin* plugin = pluginCreator();
    if (dsdb) {
        plugin->setDSDB(*dsdb);
    }
    return plugin;
}


/**
 * Selects components for a machine and creates a new configuration.
 * 
 * Also stores the new configuration to the dsdb and returns it's rowID.
 *
 * @param conf MachineConfiguration of which architecture is used.
 * @param frequency The minimum frequency of the implementations.
 * @param maxArea Maximum area for implementations.
 * @param createEstimates Boolean for creating estimates.
 * @param icDec IC decoder to be used.
 * @param icDecHDB IC decoder HDB file.
 * @return RowID of the new machine configuration having adf and idf.
 * */
RowID 
DesignSpaceExplorer::createImplementationAndStore(
    const DSDBManager::MachineConfiguration& conf,
    const double& frequency,
    const double& maxArea,
    const bool& createEstimates,
    const std::string& icDec,
    const std::string& icDecHDB) {

    const TTAMachine::Machine* mach = dsdb_->architecture(conf.architectureID);
    IDF::MachineImplementation* idf = NULL;

    idf = selectComponents(*mach, frequency, maxArea, icDec, icDecHDB);
    if (!idf) {
        return 0;
    }
    
    CostEstimator::AreaInGates area = 0;
    CostEstimator::DelayInNanoSeconds longestPathDelay = 0;
    if (createEstimates) {
        createEstimateData(*mach, *idf, area, longestPathDelay);
    }

    DSDBManager::MachineConfiguration newConf;
    newConf.architectureID = conf.architectureID;

    newConf.implementationID = 
        dsdb_->addImplementation(*idf, longestPathDelay, area);
    newConf.hasImplementation = true;
    
    // idf written to the dsdb so it can be deleted
    delete idf;
    idf = NULL;

    return addConfToDSDB(newConf);
}


/**
 * Selects components for a machine and ands them to a given config.
 * 
 * @param conf MachineConfiguration of which architecture is used.
 * @param newConf MachineConfiguration where idf is to be added.
 * @param frequency The minimum frequency of the implementations.
 * @param maxArea Maximum area for implementations.
 * @param createEstimates Boolean for creating estimates.
 * @param icDec IC decoder to be used.
 * @param icDecHDB IC decoder HDB file.
 * @return RowID of the new machine configuration having adf and idf.
 * */
bool
DesignSpaceExplorer::createImplementation(
    const DSDBManager::MachineConfiguration& conf,
    DSDBManager::MachineConfiguration& newConf,
    const double& frequency,
    const double& maxArea,
    const bool& createEstimates,
    const std::string& icDec,
    const std::string& icDecHDB) const {

    const TTAMachine::Machine* mach = dsdb_->architecture(conf.architectureID);
    IDF::MachineImplementation* idf = NULL;

    idf = selectComponents(*mach, frequency, maxArea, icDec, icDecHDB);
    if (!idf) {
        return false;
    }

    CostEstimator::AreaInGates area = 0;
    CostEstimator::DelayInNanoSeconds longestPathDelay = 0;
    if (createEstimates) {
        createEstimateData(*mach, *idf, area, longestPathDelay);
    }

    newConf.architectureID = conf.architectureID;
    newConf.implementationID = dsdb_->addImplementation(*idf, longestPathDelay, area);
    newConf.hasImplementation = true;
    
    // idf written to the dsdb so it can be deleted
    delete idf;
    idf = NULL;
    return true;
}


/**
 * Selects components for a machine, creates a idf.
 * 
 * @param mach Target machine for which components are selected.
 * @param frequency The minimum frequency of the implementations.
 * @param maxArea Maximum area for implementations.
 * @param icDec IC decoder to be used.
 * @param icDecHDB IC decoder HDB file.
 * @return Machine Implementation pointer if ok, else NULL.
 * */
IDF::MachineImplementation* 
DesignSpaceExplorer::selectComponents(
    const TTAMachine::Machine& mach,
    const double& frequency,
    const double& maxArea,
    const std::string& icDec,
    const std::string& icDecHDB) const {

    ComponentImplementationSelector impSelector;
    IDF::MachineImplementation* idf = NULL;

    try {
        // TODO: check that idf is deleted when it has been written to the
        // dsdb, and not in use anymore (selectComponents reserves it with
        // new)
        idf = impSelector.selectComponents(&mach, icDec,
                icDecHDB, frequency, maxArea);
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        if (idf != NULL) {
            delete idf;
            idf = NULL;
        }
    }

    return idf;
}


/**
 * creates estimate data for machine and idf.
 * 
 * @param mach Machine mathcing given idf.
 * @param idf Implementation definition for given machine.
 * @param area Estimated area cost.
 * @param longestPathDelay Estimated longest path delay.
 * */
void
DesignSpaceExplorer::createEstimateData(
    const TTAMachine::Machine& mach,
    const IDF::MachineImplementation& idf,
    CostEstimator::AreaInGates& area,
    CostEstimator::DelayInNanoSeconds& longestPathDelay) const {

    CostEstimator::Estimator estimator;
    area = estimator.totalArea(mach, idf);
    longestPathDelay = estimator.longestPath(mach, idf);                
}


/**
 * Add given configuration to the database.
 *
 * @param conf Configuration to be added to the database.
 * @param dsdb Database where to add the configuration.
 * @return RowID Row ID of the config in the database. 0 if adding
 *         failed.
 */
RowID 
DesignSpaceExplorer::addConfToDSDB(
    const DSDBManager::MachineConfiguration& conf) {

    try {
        return dsdb_->addConfiguration(conf);
    } catch (const Exception& e) {
        debugLog(e.errorMessage());
        return 0;
    }
}
