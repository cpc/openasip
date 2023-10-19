/*
    Copyright (c) 2002-2011 Tampere University.

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
 * @file DesignSpaceExplorer.cc
 *
 * Implementation of DesignSpaceExplorer class
 *
 * @author Jari Mäntyneva 2006 (jari.mantyneva-no.spam-tut.fi)
 * @author Esa Määttä 2008 (esa.maatta-no.spam-tut.fi)
 * @author Pekka Jääskeläinen 2011
 * @note rating: red
 */

#include <sstream>
#include <fstream>
#include <algorithm>
#include <set>
#include <vector>
#include <string>

#include "DesignSpaceExplorer.hh"
#include "ADFSerializer.hh"
#include "ExplorerCmdLineOptions.hh"
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
#include "ExecutableInstruction.hh"
#include "OperationGlobals.hh"
#include "Application.hh"
#include "ComponentImplementationSelector.hh"
#include "Exception.hh"

using std::set;
using std::vector;
using std::string;
using namespace CostEstimator;

PluginTools
DesignSpaceExplorer::pluginTool_;

CostEstimates 
DesignSpaceExplorer::dummyEstimate_;


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
 * @exception InvalidData thrown in case there are flaws in the application 
 *            configuration which leads to evaluation failure.
 * @return Returns true if the evaluation succeeds false otherwise.
 */
bool
DesignSpaceExplorer::evaluate(
    const DSDBManager::MachineConfiguration& configuration,
    CostEstimates& result, bool estimate) {

    TTAMachine::Machine* adf = NULL;
    IDF::MachineImplementation* idf = NULL;
    if (configuration.hasImplementation) {
        adf = dsdb_->architecture(configuration.architectureID);
        idf = dsdb_->implementation(configuration.implementationID);
    } else {
        adf = dsdb_->architecture(configuration.architectureID);
    }

    try {
        // program independent estimations
        if (configuration.hasImplementation && estimate) {

            // estimate total area and longest path delay
            CostEstimator::AreaInGates totalArea = 0;
            CostEstimator::DelayInNanoSeconds longestPathDelay = 0;
            createEstimateData(*adf, *idf, totalArea, longestPathDelay);

            dsdb_->setAreaEstimate(configuration.implementationID, totalArea);
            result.setArea(totalArea);

            dsdb_->setLongestPathDelayEstimate(
                configuration.implementationID, longestPathDelay);
            result.setLongestPathDelay(longestPathDelay);
        }
       
        // get all programs from the dsdb
        set<RowID> applicationIDs = dsdb_->applicationIDs();
        for (set<RowID>::const_iterator i = applicationIDs.begin();
             i != applicationIDs.end(); i++) {

            if (dsdb_->isUnschedulable((*i), configuration.architectureID)) {
                return false;
            }

            if (!estimate && 
                dsdb_->hasCycleCount(*i, configuration.architectureID)) {
                // this configuration has been compiled+simulated previously,
                // the old cycle count can be reused for this app
                continue; 
            }

            string applicationPath = dsdb_->applicationPath(*i);
            TestApplication testApplication(applicationPath);
            
            std::string applicationFile = testApplication.applicationPath();

            // test that program is found
            if (applicationFile.length() < 1) {
                delete adf;
                adf = NULL;
                delete idf;
                idf = NULL;
                throw InvalidData(
                    __FILE__, __LINE__, __func__,
                    (boost::format(
                        "No program found from application dir '%s'") 
                     % applicationPath).str());
                return false;
            }
            
#if (!defined(HAVE_CXX11) && !defined(HAVE_CXX0X))
            std::auto_ptr<TTAProgram::Program> scheduledProgram(
                schedule(applicationFile, *adf));
#else
            std::unique_ptr<TTAProgram::Program> scheduledProgram(
                schedule(applicationFile, *adf));
#endif

            if (scheduledProgram.get() == NULL) {
                dsdb_->setUnschedulable((*i), configuration.architectureID);
                delete adf;
                adf = NULL;
                delete idf;
                idf = NULL;
                return false;
            }

            oStream_->str("");
            oStream_->seekp(0);
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
            }

            // add simulated cycle count to dsdb
            dsdb_->addCycleCount(
                    (*i), configuration.architectureID,
                    runnedCycles);

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
 * @param bytecodeFile Bytecode filename with path.
 * @param target The machine to compile the sequential program against.
 * @param paramOptions Compiler options (if cmdline options are not given)
 * @return Scheduled parallel program or NULL if scheduler produced exeption.
 */
TTAProgram::Program*
DesignSpaceExplorer::schedule(
    const std::string bytecodeFile,
    TTAMachine::Machine& target,
    TCEString paramOptions) {

    TCEString compilerOptions;
    
    ExplorerCmdLineOptions* options = 
        dynamic_cast<ExplorerCmdLineOptions*>(Application::cmdLineOptions());
    if (options != NULL) {
        if (options->compilerOptions()) {
            compilerOptions = options->compilerOptionsString();
        // use compiler options given by method parameters (-O3 by default)
        } else {
            compilerOptions = paramOptions;
        }
    }
    // If compiler options did not provide optimization, we use default.
    if (compilerOptions.find("-O") == std::string::npos) {
        compilerOptions += " -O3";        
    }
    static const std::string DS = FileSystem::DIRECTORY_SEPARATOR;
    
    // create temp directory for the target machine
    std::string tmpDir = FileSystem::createTempDirectory();

    // write machine to a file for tcecc
    std::string adf = tmpDir + DS + "mach.adf";
    std::string tpef = tmpDir + DS + "program.tpef";
    ADFSerializer serializer;
    serializer.setDestinationFile(adf);
    try {
        serializer.writeMachine(target);
    } catch (const SerializerException& exception) {
        FileSystem::removeFileOrDirectory(tmpDir);
        throw IOException(
            __FILE__, __LINE__, __func__, exception.errorMessage());
    }     
    // call tcecc to compile, link and schedule the program
    std::vector<std::string> tceccOutputLines;
    std::string tceccPath = Environment::tceCompiler();
    std::string tceccCommand = tceccPath + " "  
        + compilerOptions + " --no-link -a " + adf + " -o " 
        + tpef + " " + bytecodeFile + " --no-plugin-cache 2>&1";

    const bool debug = Application::verboseLevel() > 0;

    Application::runShellCommandAndGetOutput(tceccCommand, tceccOutputLines);

    if (debug && tceccOutputLines.size() > 0) {
        for (unsigned int i = 0; i < tceccOutputLines.size(); ++i) {
            std::cout << tceccOutputLines.at(i) << std::endl;
        }
    }
    
    // check if tcecc produced any tpef output
    if (!(FileSystem::fileExists(tpef) && FileSystem::fileIsReadable(tpef))) {
        if (debug) {
            std::cout << "failed command: " << tceccCommand << std::endl
                      << "temporary directory left for inspection at: " 
                      << tmpDir << std::endl;
        } else {
            FileSystem::removeFileOrDirectory(tmpDir);            
        }
        return NULL;    
    } 
    
    TTAProgram::Program* prog = NULL;
    try {
        prog = TTAProgram::Program::loadFromTPEF(tpef, target);
    } catch (const Exception& e) {
        FileSystem::removeFileOrDirectory(tmpDir);
        IOException error(__FILE__, __LINE__,__func__, e.errorMessage());
        error.setCause(e);
        throw error;
    }
    FileSystem::removeFileOrDirectory(tmpDir);    
    return prog;        
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
    const TTAProgram::Program& program, const TTAMachine::Machine& machine,
    const TestApplication& testApplication, const ClockCycleCount&,
    ClockCycleCount& runnedCycles, const bool tracing,
    const bool useCompiledSimulation,
    std::vector<ClockCycleCount>* executionCounts) {
    // initialize the simulator
    SimulatorFrontend simulator(
        useCompiledSimulation ? 
        SimulatorFrontend::SIM_COMPILED : 
        SimulatorFrontend::SIM_NORMAL);
    
    // setting simulator timeout in seconds
    simulator.setTimeout(480);

    // use memory file in sqlite
    // TODO: should use a tmp dir as now TraceDB produces multiple
    // text files, thus only the SQL part goes to memory
    const string traceFile = ":memory:";
    simulator.forceTraceDBFileName(traceFile);
    if (!useCompiledSimulation)
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
        if (interpreter.result().size() > 0) {
            *oStream_ << interpreter.result() << std::endl;
        }
    }

    runnedCycles = simulator.cycleCount();
    
    int instructionCount = program.instructionVector().size();
    if (executionCounts) {
        executionCounts->resize(instructionCount, 0);
        for (int i=0; i<instructionCount; ++i) {
            (*executionCounts)[i] = simulator.executableInstructionAt(i)
                .executionCount();
        }
    }

    // Flush data collected during simulation to the trace file.
    const ExecutionTrace* db = NULL;
    if (tracing) {
        db = simulator.lastTraceDB();
    }
    
    simulator.killSimulation();
    
    return db;
}

#pragma GCC diagnostic ignored "-Wstrict-aliasing"
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
    const std::string& pluginName, DSDBManager* dsdb) {
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

#pragma GCC diagnostic warning "-Wstrict-aliasing"

/**
 * 
 * Parses the plugin search directories and loads all available plugins
 */
std::vector<DesignSpaceExplorerPlugin*> DesignSpaceExplorer::getPlugins() {
    
    std::vector<DesignSpaceExplorerPlugin*> plugins;
    vector<string> found_plugins;
    vector<string> searchPaths = Environment::explorerPluginPaths();
    for (vector<string>::const_iterator iter = searchPaths.begin();
            iter != searchPaths.end(); iter++) {

        if (FileSystem::fileExists(*iter)) {
            FileSystem::findFromDirectory(".*\\.so$", *iter, found_plugins);
        }
    }
    for (unsigned int i = 0; i < found_plugins.size(); ++i) {
        std::string pluginName = FileSystem::fileNameBody(found_plugins[i]);
        DesignSpaceExplorerPlugin* plugin = loadExplorerPlugin(pluginName, NULL);
        if (!plugin) {
            continue;
        }
        
        plugins.push_back(plugin);
    }
    
    return plugins;
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

    delete mach;
    mach = NULL;

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
    const std::string& icDecHDB) {

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

    delete mach;
    mach = NULL;

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
    CostEstimator::DelayInNanoSeconds& longestPathDelay) {

    area = estimator_.totalArea(mach, idf);
    longestPathDelay = estimator_.longestPath(mach, idf);                
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
