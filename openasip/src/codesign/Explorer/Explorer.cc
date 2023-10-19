/*
    Copyright (c) 2002-2017 Tampere University.

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
 * @file DesignSpaceExlorer.cc
 *
 * Implementation of explorer.
 *
 * The command line version of the Design Space Explorer.
 *
 * @author Jari Mäntyneva 2007 (jari.mantyneva-no.spam-tut.fi)
 * @author Esa Määttä 2008 (esa.maatta-no.spam-tut.fi)
 * @author Pekka Jääskeläinen 2011, 2017
 * @note rating: red
 */

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

#include "StringTools.hh"
#include "Application.hh"
#include "Exception.hh"
#include "FileSystem.hh"
#include "DesignSpaceExplorer.hh"
#include "DesignSpaceExplorerPlugin.hh"
#include "ExplorerCmdLineOptions.hh"
#include "DSDBManager.hh"
#include "HDBRegistry.hh"
#include "MachineImplementation.hh"
#include "Machine.hh"
#include "ExplorerPluginParameter.hh"
#include "Environment.hh"
#include "Conversion.hh"

using std::vector;
using std::cout;
using std::cerr;
using std::endl;
using std::string;

const string EXPLORER_DEFAULT_HDB = "asic_130nm_1.5V.hdb";

/**
 * Loads the output Design Space Database file and creates a DSDB from it.
 * 
 * If the given filename doesn't exists creates a new DSDB file with the given
 * name.
 * 
 * @param dsdbFile The DSDB file name.
 * @return DSDBManager of the DSDB file.
 */
DSDBManager*
loadDSDB(const std::string& dsdbFile) {
    if (FileSystem::fileExists(dsdbFile)) {
        return new DSDBManager(dsdbFile);
    } else {
        return DSDBManager::createNew(dsdbFile);
    }
}

/**
 * Parses the given parameter which has form 'paramname=paramvalue" to
 * different strings.
 *
 * @param param The parameter.
 * @param paramName Parameter name is stored here.
 * @param paramValue Parameter value is stored here.
 * @exception InvalidData If the given parameter is not in the correct form.
 */
void
parseParameter(
    const std::string& param, std::string& paramName, std::string& paramValue) {
    string::size_type separatorPos = param.find("=", 0);
    if (separatorPos == string::npos) {
        string errorMsg = 
            "Explorer plugin parameters must be in form "
            "'parametername=parametervalue'.";
        throw InvalidData(__FILE__, __LINE__, __func__, errorMsg);
    }

    paramName = param.substr(0, separatorPos);
    paramValue = param.substr(separatorPos+1, param.length());
}

/**
 * Determines the widest column so that column widths can be tuned.
 *
 * Lengths of the data values are returned through the parameters.
 *
 * @param confs Configuration costs which lengths are checked.
 * @configurationID Length of the configuration ID string.
 * @applicationPath Length of the path string of an application.
 * @cycleCount Length of the program cycle count string.
 * @energyEstimate Length of the programs energy estimate string.
 */
void
determineLongest(
    const vector<DSDBManager::ConfigurationCosts>& confs, 
    int& configurationID,
    int& applicationPath, 
    int& cycleCount, 
    int& energyEstimate, 
    int& longestPathDelay, 
    int& area) {

    for (unsigned int i = 0; i < confs.size(); i++) {
        int size = Conversion::toString(confs[i].configurationID).size();
        if (size > configurationID) {
            configurationID = size;
        }
        size = confs[i].application.size();
        if (size > applicationPath) {
            applicationPath = size;
        }
        size = Conversion::toString(confs[i].cycleCount).size();
        if (size > cycleCount) {
            cycleCount = size;
        }
        size = Conversion::toString(confs[i].energyEstimate).size();
        if (size > energyEstimate) {
            energyEstimate = size;
        }
        size = Conversion::toString(confs[i].longestPathDelay).size();
        if (size > longestPathDelay) {          
            longestPathDelay = size;
        }

        size = Conversion::toString(confs[i].area).size(); 
        if (size > area) {
            area = size;                                                                                                                                                                          
        }
    }
}

/**
 * Outputs given number of empty characters to cout.
 *
 * @param numberOfSpaces The number of empty characters to output.
 */
void
printSpaces(unsigned int numberOfSpaces) {
    while (numberOfSpaces != 0) {
        cout << " ";
        numberOfSpaces--;
    }
}

/**
 * Returns the ordering of data indicated by the string.
 *
 * @order Option string given by user that tells the ordering.
 * @return The ordering of the data.
 */
DSDBManager::Order orderingOfData(const string& order) {
    if (order == "I") {
        return DSDBManager::ORDER_BY_CONFIGURATION;
    } else if (order == "P") {
        return DSDBManager::ORDER_BY_APPLICATION;
    } else if (order == "C") {
        return DSDBManager::ORDER_BY_CYCLE_COUNT;
    } else if (order == "E") {
        return DSDBManager::ORDER_BY_ENERGY_ESTIMATE;
    } else {
        return DSDBManager::ORDER_BY_CONFIGURATION;
    }
}

/**
 * Parses parameters given from command line and passes them to the plugin.
 *
 * @param plugin Explorer plugin that receives the parameters.
 * @param options Explorer command line options where parameters are read.
 */
bool
loadPluginParameters(
    DesignSpaceExplorerPlugin* plugin, 
    const ExplorerCmdLineOptions& options) {

    // Check the parameters to be passed to the explorer plugin.
    for (int i = 0; i < options.explorerPluginParameterCount(); i++) {
        string param = options.explorerPluginParameter(i);
        string paramName;
        string paramValue;
        try {
            parseParameter(param, paramName, paramValue);
            plugin->giveParameter(paramName, paramValue);
        } catch (const Exception& e) {
            std::cerr << e.errorMessage() << std::endl;
            return false;
        }
    }
    return true;
}


/**
 * Loads explorer plugin.
 *
 * @param plugin Explorer plugin name to be loaded.
 * @param dsdb DSDB that plugin is associated with.
 * @return explorer plugin as a pointer.
 */
DesignSpaceExplorerPlugin*
loadExplorerPlugin(const std::string& plugin, DSDBManager* dsdb) {
    
    // Try to load the explorer plugin.
    DesignSpaceExplorerPlugin* explorer = NULL;
    try {
        explorer = DesignSpaceExplorer::loadExplorerPlugin(plugin, dsdb);
    } catch (const FileNotFound& e) {
        std::cerr << "No explorer plugin file named '" << plugin
                  << ".so' found."  << std::endl;
        delete dsdb;
        return NULL;
    } catch (const Exception& e) {
        std::string msg = "Error while trying to load the explorer plugin "
            "named '" + plugin + ".so'.";
        verboseLog(msg)
        msg = "With reason: " + e.errorMessage();
        verboseLogC(msg, 1)
        delete dsdb;
        delete explorer;
        return NULL;
    }
    return explorer;
}


/**
 * Prints explorer plugins and their descriptions.
 */
void
printPlugins() {
    vector<string> found_plugins;
    vector<string> stripped_plugins;
    vector<string> searchPaths = Environment::explorerPluginPaths();
    for (vector<string>::const_iterator iter = searchPaths.begin();
            iter != searchPaths.end(); iter++) {
        if (FileSystem::fileExists(*iter)) {
            // now list all files with postfix ".so"
            verboseLogC("Fetching plugins from directory: " + *iter, 1)
            FileSystem::findFromDirectory(".*\\.so$", *iter, found_plugins);
        }
    }
    cout << "| Plugin name                 | Description " << endl
         << "--------------------------------------------" << endl;
    cout.flags(std::ios::left);
    // Eliminate duplicates such as ComponentAdded.0.so ComponentAdded.so
    for (unsigned int i = 0; i < found_plugins.size(); ++i) {
        std::string finalPluginName = found_plugins[i];        
        std::string pluginName = FileSystem::fileNameBody(found_plugins[i]); 
        // Repeat "basename" until all .0.0. are removed so symbol can be found
        // correctly
        while (pluginName != finalPluginName) {
            finalPluginName = pluginName;
            pluginName = FileSystem::fileNameBody(pluginName); 
        }
        if(find(stripped_plugins.begin(), stripped_plugins.end(), pluginName)
            == stripped_plugins.end()) {
            stripped_plugins.push_back(pluginName);
        }
    }
    for (unsigned int i = 0; i < stripped_plugins.size(); ++i) {
        std::string pluginName = stripped_plugins[i];        
        DesignSpaceExplorerPlugin* plugin = loadExplorerPlugin(pluginName, NULL);
        if (!plugin) {
            return;
        }
        cout << std::setw(30) << pluginName << plugin->description() << endl;
    }
}


/**
 * Prints explorer plugin parameter info.
 *
 * @param plugin Explorer plugin which parameter info is to be printed.
 */
void
printPluginParamInfo(DesignSpaceExplorerPlugin& plugin) {
    using std::setw;
    DesignSpaceExplorerPlugin::ParameterMap pm = plugin.parameters();
    DesignSpaceExplorerPlugin::PMCIt it = pm.begin();
    cout.flags(std::ios::left);
    cout << setw(30) << "parameter name " << setw(15) << "type" << "default value" << endl;
    cout << "-----------------------------------------------------------------------------" << endl;
    while (it != pm.end()) {
        cout << setw(30) << it->first << setw(15);
        switch (it->second.type()) {
            case UINT:
                cout << "unsigned int"; 
                break;
            case INT:
                cout << "int"; 
                break;
            case STRING:
                cout << "string"; 
                break;
            case BOOL:
                cout << "boolean"; 
                break;
            default:
                cout << "unknown type"; 
        }
        if (!it->second.isCompulsory()) {
            if (it->second.type() == BOOL) {
                cout << 
                    (it->second.value() == "1" || it->second.value() == "true"
                     ? "true" : "false"); 
            } else {
                cout << it->second.value();
            }
        }
        cout << std::endl;
        if (it->second.description().size() > 0) {
            cout << std::endl;
            cout << it->second.description();
            cout << std::endl;
            cout << std::endl;
        }
        ++it; 
    }
}

/**
 * Prints the pareto set of configurations in the DSDB using the given
 * criteria set.
 */
void
printParetoSet(const DSDBManager& dsdb, TCEString criteriaSet) {
    if (criteriaSet != "C")
        return; // only connectivity,cycles criteria supported
     
    DSDBManager::ParetoSetConnectivityAndCycles paretoSet = 
        dsdb.paretoSetConnectivityAndCycles();

    const int CONF_COL_W = 6;
    const int CONNECTIVITY_COL_W = 6;
    const int CYCLES_COL_W = 8;

    std::cout << std::setw(CONF_COL_W) << std::right << "conf #" 
              << " | ";

    std::cout << std::setw(CONNECTIVITY_COL_W) << std::right << "conn" 
              << " | ";

    std::cout << std::setw(CYCLES_COL_W) << std::right << "cycles" 
              << std::endl;

    for (int i = 0; i < CONF_COL_W + CONNECTIVITY_COL_W + CYCLES_COL_W + 10; 
         ++i) {
        std::cout << "-";
    }
    std::cout << std::endl;

    for (DSDBManager::ParetoSetConnectivityAndCycles::const_iterator i = 
             paretoSet.begin(); i != paretoSet.end(); ++i) {
        DSDBManager::ParetoPointConnectivityAndCycles point = *i;
        std::cout << std::setw(CONF_COL_W) << std::right << point.get<0>()
                  << " | ";

        std::cout << std::setw(CONNECTIVITY_COL_W) << std::right << point.get<1>()
                  << " | ";

        std::cout << std::setw(CYCLES_COL_W) << std::right << point.get<2>()
                  << std::endl;
    }
}

bool
dumpConfiguration(
    DSDBManager* dsdb, const ExplorerCmdLineOptions& options, RowID confID) {
    DSDBManager::MachineConfiguration configuration =
        dsdb->configuration(confID);
    std::string adfFileName = options.adfOutFile()
                                  ? options.adfOutFileName()
                                  : Conversion::toString(confID) + ".adf";
    try {
        dsdb->writeArchitectureToFile(
            configuration.architectureID, adfFileName);
        if (Application::verboseLevel() > 0) {
            Application::logStream()
                << "Written ADF file of configuration " << confID << " to "
                << adfFileName << std::endl;
        }
    } catch (const Exception& e) {
        Application::errorStream()
            << "Error occured while writing the ADF." << std::endl;
        return false;
    }

    if (configuration.hasImplementation) {
        std::string idfFileName = Conversion::toString(confID) + ".idf";
        try {
            dsdb->writeImplementationToFile(
                configuration.implementationID, idfFileName);
            if (Application::verboseLevel() > 0) {
                Application::logStream()
                    << "Written IDF file of configuration " << confID
                    << " to " << idfFileName << std::endl;
            }
        } catch (const Exception& e) {
            Application::errorStream()
                << "Error occured while writing the IDF to " << idfFileName
                << ": " << e.errorMessage() << std::endl;
            return false;
        }
    }
    return true;
}

/**
 * Main function.
 *
 * Parses the command line and figures out what to do.
 *
 * @param argc The command line argument count.
 * @param argv The command line arguments (passed to the interpreter).
 * @return The return status.
 */
int main(int argc, char* argv[]) {

    Application::initialize();    

    // boolean to check if done something useful
    bool doneUseful = false;

    // Parses the command line options.
    ExplorerCmdLineOptions* options = new ExplorerCmdLineOptions();
    try {
        options->parse(argv, argc);
        Application::setCmdLineOptions(options);
    } catch (ParserStopRequest) {
        return EXIT_SUCCESS;
    } catch (const IllegalCommandLine& i) {
        std::cerr << i.errorMessage() <<  std::endl;
        return EXIT_FAILURE;
    }

    if (options->writeOutConfiguration() &&
        options->writeOutBestConfiguration()) {
        Application::errorStream()
            << "Only one of '-w' or '--dump_best' can be "
            << "given at the time." << std::endl;
        return EXIT_FAILURE;
    }

    int verboseLevel = Application::verboseLevel();
    if (verboseLevel < 0) {
        Application::setVerboseLevel();
    } else {
        Application::setVerboseLevel(verboseLevel);
    }

    if (options->printPlugins()) {
        printPlugins();
        return EXIT_SUCCESS;
    }

    if (options->pluginInfo().length() != 0) {
        std::string plugin = options->pluginInfo();
        DesignSpaceExplorerPlugin* explorer = loadExplorerPlugin(plugin, NULL);
        if (!explorer) {
            return EXIT_FAILURE;
        }
        printPluginParamInfo(*explorer);
        delete explorer;
        explorer = NULL;
        return EXIT_SUCCESS;
    }

    // Only argument should be dsdb.
    if (options->numberOfArguments() != 1) {
        std::cerr << "Illegal number of arguments." << std::endl << std::endl;
        options->printHelp();
        return EXIT_FAILURE;
    }
    // Get the database file.
    std::string dsdbFile = "";
    dsdbFile = options->argument(1);

    // Loads the database.
    DSDBManager* dsdb = NULL;
    try {
        dsdb = loadDSDB(dsdbFile);
    } catch (const IOException& e) {
        std::cerr << e.errorMessage() << std::endl;
        delete dsdb;
        return EXIT_FAILURE;
    }

    RowID addedConfID = 0;
    // adds new configuration to the DSDB
    if (options->adfFile()) {
        TTAMachine::Machine* adf = NULL;
        IDF::MachineImplementation* idf = NULL;
        try {
            DSDBManager::MachineConfiguration conf;
            conf.hasImplementation = false;
            adf = TTAMachine::Machine::loadFromADF(options->adfFileName());
            conf.architectureID = dsdb->addArchitecture(*adf);
            if (options->idfFile()) {
                idf = 
                    IDF::MachineImplementation::loadFromIDF(
                        options->idfFileName());
                conf.implementationID = dsdb->addImplementation(*idf, 0, 0);
                conf.hasImplementation = true;
            }
            addedConfID = dsdb->addConfiguration(conf);
            if (Application::verboseLevel() > 0)
                Application::logStream()
                    << "Added configuration " << addedConfID
                    << " into the DSDB." << std::endl;
            delete adf;
            delete idf;
            doneUseful = true;
        } catch (const Exception& e) {
            std::cout << "Error occured reading ADF or IDF. " 
                      << e.errorMessage() << std::endl;
            delete dsdb;
            return EXIT_FAILURE;           
        }
    }

    // Check and add the test application directories.
    int testDirectories = options->testApplicationDirectoryCount();
    for (int i = 0; i < testDirectories; i++) {
        std::string testDir = options->testApplicationDirectory(i);
        if (FileSystem::fileExists(testDir) &&
            FileSystem::fileIsDirectory(testDir)) {
            TestApplication app(testDir);
            if (!app.isValid()) {
                std::cerr 
                    << "Application directory '" << testDir 
                    << "' is invalid. Ensure that at least program.bc with "
                    << "the input program exists. Also either " 
                    << "'correct_simulation_output' or 'verify.sh' is needed "
                    << "for verifying the program output."
                    << std::endl;
            } else if (!dsdb->hasApplication(testDir)) {
                dsdb->addApplication(testDir);
            } else {
                std::cerr 
                    << "application directory '" << testDir 
                    << "' has already been added" << std::endl;
            }
        } else {
            std::cerr << "Application directory '" << testDir
                      << "' does not exist." << std::endl;
        }
        doneUseful = true;
    }

    TCEString paretoSet = options->paretoSetValues();
    if (paretoSet != "") {
        // Prints the pareto sets
        if (paretoSet != "C") {
            std::cerr << "Unsupported pareto set value type." << std::endl;
            return EXIT_FAILURE;
        }
        printParetoSet(*dsdb, paretoSet);
        return EXIT_SUCCESS;
    }

    // Prints the summary of the configurations in the database.
    if (options->printSummary()) {
        DSDBManager::Order ordering = orderingOfData(options->summaryOrdering());
        cout << "Configurations in DSDB: " << endl;
        int idLength = 7;
        int pathLength = 16;
        int cycleLength = 11;
        int energyLength = 15;
        int lpdLength = 18;
        int areaLength = 4;
        cout << "| Conf ID | Application path | cycle count | energy estimate | longest path delay | area "
             << endl;
        cout << "------------------------------------------------------------------------------------------"
             << endl;
        vector<DSDBManager::ConfigurationCosts> confCosts =
            dsdb->applicationCostEstimatesByConf(ordering);
        // Checks the longes strings of all data values that will be printed.
        // Values are used to create a clean output of the results.
        determineLongest(
            confCosts, idLength, pathLength, cycleLength, 
            energyLength, lpdLength, areaLength);
        for (unsigned int i = 0; i < confCosts.size(); i++) {
            cout << "| ";
            cout << confCosts[i].configurationID;
            printSpaces(
                idLength - Conversion::toString(
                    confCosts[i].configurationID).size());
            cout << " | ";
            cout << confCosts[i].application;
            printSpaces(
                pathLength - confCosts[i].application.size());
            cout << " | ";
            cout << confCosts[i].cycleCount;
            printSpaces(
                cycleLength - Conversion::toString(
                    confCosts[i].cycleCount).size());
            cout << " | ";
            cout << confCosts[i].energyEstimate;
            printSpaces(
                energyLength - Conversion::toString(
                    confCosts[i].energyEstimate).size());
            cout << " | ";
            cout << confCosts[i].longestPathDelay;
            printSpaces(
                    lpdLength - Conversion::toString(
                        confCosts[i].longestPathDelay).size());
            cout << " | ";
            cout << confCosts[i].area;
            printSpaces(
                    areaLength - Conversion::toString(
                        confCosts[i].area).size());   
            cout << " |" << endl;
        }
        cout <<  "-----------------------------------------------------------------------------------------"
            << endl;
    }
    
    // Prints the total amount of configurations in the database.
    if (options->printSummary() || options->numberOfConfigurations()) {
        cout << "Total: " << dsdb->configurationIDs().size() 
             << " configurations "
             << "in the database." << endl;
        delete dsdb;
        return EXIT_SUCCESS;
    }

    // Write the configuration ADF and IDF to files.
    if (options->writeOutConfiguration()) {
        for (int i = 0; i < options->numberOfConfigurationsToWrite(); i++) {
            if (dsdb->hasConfiguration(options->configurationToWrite(i))) {
                if (!dumpConfiguration(
                        dsdb, *options, options->configurationToWrite(i))) {
                    delete dsdb;
                    return EXIT_FAILURE;
                }
            } else {
                // dsdb didn't have requested configuration
                std::cerr << "No configuration found with id: "
                          << options->configurationToWrite(i) << "."
                          << std::endl;
                delete dsdb;
                return EXIT_FAILURE;
            }
        }
        delete dsdb;
        return EXIT_SUCCESS;
    }
    
    // Remove applications from DSDB if requested
    if (options->applicationIDToRemoveCount() > 0) {
        for (int i = 0; i < options->applicationIDToRemoveCount(); i++) {
            RowID id = options->applicationIDToRemove(i);
            try {
                dsdb->removeApplication(id);
            } catch (const KeyNotFound&) {
                std::cerr << "No application with ID: " << id << " in DSDB."
                          << std::endl;
            }
        }
        delete dsdb;
        return EXIT_SUCCESS;
    }

    // If the list applications option is given.
    if (options->printApplications()) {
        std::cout << "Applications in the DSDB:" << std::endl;
        std::cout << " ID | Application" << std::endl;
        std::cout << "---------------------------------" << std::endl;
        std::set<RowID> appIDs = dsdb->applicationIDs();
        std::set<RowID>::const_iterator appIter = appIDs.begin();
        for (; appIter != appIDs.end(); appIter++) {
            std::cout << " " << (*appIter) << " | " 
                      << dsdb->applicationPath(*appIter)
                      << std::endl;
        }
        std::cout << "---------------------------------" << std::endl;
        std::cout << "Total: " << dsdb->applicationCount() 
                  << " applications in DSDB." << std::endl;
        delete dsdb;
        return EXIT_SUCCESS;
    }

    // Check the explorer plugin.
    std::string pluginToUse = "";
    pluginToUse = options->explorerPlugin();
    if (pluginToUse == "") {
        if (!doneUseful) {
            std::cerr << "No explorer plugin given." << std::endl;
            delete dsdb;
            return EXIT_FAILURE;
        } else {
            delete dsdb;
            return EXIT_SUCCESS;
        }
    }
    
    // Try to load the explorer plugin.
    DesignSpaceExplorerPlugin* explorer = loadExplorerPlugin(pluginToUse, dsdb);
    if (!explorer || !loadPluginParameters(explorer, *options)) {
        delete dsdb;
        return EXIT_FAILURE;
    }

    if (testDirectories < 1 && !dsdb->applicationCount() &&
        explorer->requiresApplication()) {
        // used plugin may not need a test application, or the app is
        // provided to the plugin with its own parameter
        std::cerr << "No test application paths given or found in dsdb."
                  << std::endl;
        return EXIT_FAILURE;
    }

    // Load the HDB files if given as option
    if (options->hdbFileNames()) {
        vector<string> hdbPaths;
        // put current working directory first in path find priority
        hdbPaths.push_back(FileSystem::currentWorkingDir());
        // concatenate search paths after current working dir path
        vector<string> srchPaths = Environment::hdbPaths();
        hdbPaths.insert(hdbPaths.end(), srchPaths.begin(), srchPaths.end());

        for (int i = 0; i < options->hdbFileNameCount(); i++) {
            string pathToHdb = options->hdbFileName(i);
            string hdbFile = options->hdbFileName(i);
            for (unsigned int p = 0; p < hdbPaths.size(); p++) {
                string tempPath = 
                    hdbPaths.at(p) + FileSystem::DIRECTORY_SEPARATOR + hdbFile;
                if (FileSystem::fileExists(tempPath)) {
                    pathToHdb = tempPath;
                    break;
                }
            }
            try {
                HDB::HDBRegistry::instance().hdb(pathToHdb);
            } catch (const FileNotFound& e) {
                std::cerr << "Could not find HDB file " 
                          << options->hdbFileName(i) << std::endl;
            }
        }
    } else {
        // if no hdb was given, use default hdb
        string pathToHdb = EXPLORER_DEFAULT_HDB;
        vector<string> hdbPaths = Environment::hdbPaths();
        for (unsigned int i = 0; i < hdbPaths.size(); i++) {
            string tempPath =
                hdbPaths.at(i) + FileSystem::DIRECTORY_SEPARATOR
                + EXPLORER_DEFAULT_HDB;
            if (FileSystem::fileExists(tempPath)) {
                pathToHdb = tempPath;
                break;
            }
        }
        try {
            HDB::HDBRegistry::instance().hdb(pathToHdb);
        } catch (const FileNotFound& e) {
            std::cerr << "Could not find HDB file " 
                      << pathToHdb << std::endl;
        }
    }

    try {
        RowID startPointConfigurationID = options->startConfiguration();

        if (startPointConfigurationID == 0 && options->adfFile()) {
            startPointConfigurationID = addedConfID;
        }

        if (startPointConfigurationID == 0 &&
            explorer->requiresStartingPointArchitecture()) {
            std::cerr << "No starting point configuration defined. "
                      << std::endl
                      << "Use -s or -a to define a configuration to "
                      << "start the exploration from." << std::endl;
            return EXIT_FAILURE;

        }
        vector<RowID> result =
            explorer->explore(startPointConfigurationID);
        if (result.empty()) {
            cout 
                << "No fitting processor configurations were created." << endl;
        } else {
            if (Application::verboseLevel() > 0) {
                Application::logStream()
                    << "Best result configurations:" << std::endl;
                for (unsigned int i = 0; i < result.size(); i++) {
                    Application::logStream() << " " << result[i] << endl;
                }
            }
            if (options->writeOutBestConfiguration() && result.size() > 0) {
                dumpConfiguration(dsdb, *options, result[result.size() - 1]);
            }
        }
    } catch (const Exception& e) {
        std::cerr << e.errorMessage()
                  << " " << e.fileName()
                  << " " << e.lineNum() << std::endl;
        delete dsdb;
        delete explorer;
        return EXIT_FAILURE;
    }

    delete dsdb;
    delete explorer;
    return EXIT_SUCCESS;
}
