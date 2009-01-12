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
 * @file ExplorerCmdLineOptions.cc
 *
 * Declaration of ExplorerCmdLineOptions.
 *
 * @author Jari M�ntyneva 2007 (jari.mantyneva-no.spam-tut.fi)
 * @note rating: red
 */

#include <iostream>

//#include "ExplorerConstants.hh"
#include "CmdLineOptions.hh"
#include "ExplorerCmdLineOptions.hh"
#include "tce_config.h"

/// Long switch string for giving an explorer plugin to be used.
const std::string SWL_PLUGIN_NAME = "explorer_plugin";
/// Short switch string for giving an explorer plugin to be used.
const std::string SWS_PLUGIN_NAME = "e";
/// Long switch string for giving an explorer plugin info.
const std::string SWL_PLUGIN_INFO = "plugin_info";
/// Short switch string for giving an explorer plugin info.
const std::string SWS_PLUGIN_INFO = "p";
/// Long switch string for giving an explorer plugin to be used.
const std::string SWL_PLUGIN_PARAM = "plugin_param";
/// Short switch string for giving an explorer plugin to be used.
const std::string SWS_PLUGIN_PARAM = "u";
/// Design space explorer title
const std::string DSCLI_TITLE = "Design Space Explorer for TTA.";
/// Long switch string for adding a application directory.
const std::string SWL_TEST_DIR_ADD = "add_app_dir";
/// Short switch string for adding a application directory.
const std::string SWS_TEST_DIR_ADD = "d";
/// Long switch string for giving a start configuration id.
const std::string SWL_START_ID = "start";
/// Short switch string for giving a start configuration id.
const std::string SWS_START_ID = "s";
/// Long switch for verbose level.
const std::string SWL_VERBOSE_LEVEL = "verbose";
/// Short switch for verbose level.
const std::string SWS_VERBOSE_LEVEL = "v";
/// Long switch string for number of configurations flag.
const std::string SWL_CONFIGURATION_SUMMARY = "conf_summary";
/// Short switch string for number of configurations flag.
const std::string SWS_CONFIGURATION_SUMMARY = "c";
/// Long switch string for number of configurations flag.
const std::string SWL_CONFIGURATION_COUNT = "conf_count";
/// Short switch string for number of configurations flag.
const std::string SWS_CONFIGURATION_COUNT = "n";
/// Long switch string for writing out configuration files.
const std::string SWL_CONFIGURATION_WRITE = "write_conf";
/// Short switch string for writing out configuration files.
const std::string SWS_CONFIGURATION_WRITE = "w";
/// Long switch string for removing application paths from dsdb.
const std::string SWL_TEST_DIR_RM = "rm_app";
/// Short switch string for removing application paths from dsdb.
const std::string SWS_TEST_DIR_RM = "r";
/// Long switch string for listing application paths in the dsdb.
const std::string SWL_LIST_APPS = "list_apps";
/// Short switch string for listing loadable plugins.
const std::string SWS_LIST_APPS = "l";
/// Long switch string for listing loadable plugins.
const std::string SWL_LIST_PLUGINS = "list_plugins";
/// Short switch string for listing application paths in the dsdb.
const std::string SWS_LIST_PLUGINS = "g";
/// Short switch string for hdb file.
const std::string SWS_HDB_FILE = "b";
/// Long switch string for hdb file.
const std::string SWL_HDB_FILE = "hdb";
/// Short switch string for adf file. 
const std::string SWS_ADF_FILE = "a";
/// Long switch string for adf file. 
const std::string SWL_ADF_FILE = "adf";
/// Short switch string for idf file. 
const std::string SWS_IDF_FILE = "i";
/// Long switch string for idf file. 
const std::string SWL_IDF_FILE = "idf";

/**
 * Constructor.
 *
 * @todo Use textgenerator in the help texts.
 */
ExplorerCmdLineOptions::ExplorerCmdLineOptions() : CmdLineOptions("") {
    addOption(
        new StringCmdLineOptionParser(
            SWL_PLUGIN_NAME,
            "Design Space Explorer plugin to be used.",
            SWS_PLUGIN_NAME));
    addOption(
        new StringCmdLineOptionParser(
            SWL_PLUGIN_INFO,
            "Design Space Explorer plugin to print info about.",
            SWS_PLUGIN_INFO));
    addOption(
        new StringListCmdLineOptionParser(
            SWL_PLUGIN_PARAM,
            "Parameter to the explorer plugin in form 'name=value'.",
            SWS_PLUGIN_PARAM));
    addOption(
        new StringListCmdLineOptionParser(
            SWL_TEST_DIR_ADD,
            "Path(s) of the test application(s) to be added into the DSDB.",
            SWS_TEST_DIR_ADD));
    addOption(
        new IntegerListCmdLineOptionParser(
            SWL_TEST_DIR_RM,
            "ID(s) of the test program path(s) to be removed from the DSDB.",
            SWS_TEST_DIR_RM));
    addOption(
        new IntegerCmdLineOptionParser(
            SWL_START_ID,
            "Starting point configuration ID in the DSDB.",
            SWS_START_ID));
    addOption(
        new IntegerCmdLineOptionParser(
            SWL_VERBOSE_LEVEL,
            "The level of verbosity of errors and warnings.",
            SWS_VERBOSE_LEVEL));
    addOption(
        new StringCmdLineOptionParser(
            SWL_CONFIGURATION_SUMMARY,
            "Print the summary of machine configurations in the DSDB"
            "ordered by:\n"
            "  Ordering may be one of the following:\n"
            "    I  ordering by configuration Id,\n"
            "    P  ordering by application path,\n"
            "    C  ordering by cycle count,\n"
            "    E  ordering by energy estimate.",
            SWS_CONFIGURATION_SUMMARY));
    addOption(
        new BoolCmdLineOptionParser(
            SWL_CONFIGURATION_COUNT,
            "Print the number of machine configurations in the DSDB.",
            SWS_CONFIGURATION_COUNT));
    addOption(
        new IntegerListCmdLineOptionParser(
            SWL_CONFIGURATION_WRITE,
            "Write out the ADF and IDF files from the DSDB with given "
            "configuration id.",
            SWS_CONFIGURATION_WRITE));
    addOption(
        new BoolCmdLineOptionParser(
            SWL_LIST_APPS,
            "List the applications in the DSDB.",
            SWS_LIST_APPS));
    addOption(
        new BoolCmdLineOptionParser(
            SWL_LIST_PLUGINS,
            "List loadable plugins.",
            SWS_LIST_PLUGINS));
    addOption(
        new StringListCmdLineOptionParser(
            SWL_HDB_FILE,
            "HDB to use with exploration.",
            SWS_HDB_FILE));
    addOption(
        new StringListCmdLineOptionParser(
            SWL_ADF_FILE,
            "ADF to add into the DSDB.",
            SWS_ADF_FILE));
    addOption(
        new StringListCmdLineOptionParser(
            SWL_IDF_FILE,
            "IDF to add into the DSDB, needs also ADF.",
            SWS_IDF_FILE));
}

/**
 * Destructor.
 */
ExplorerCmdLineOptions::~ExplorerCmdLineOptions() {
}

/**
 * Prints the version of the program.
 */
void
ExplorerCmdLineOptions::printVersion() const {
    std::cout << DSCLI_TITLE << " version " 
              << Application::TCEVersionString() << std::endl;
}

/**
 * Prints the help menu of the program.
 *
 * @todo Implement using ExplorerTextGenerator.
 */
void
ExplorerCmdLineOptions::printHelp() const {
    printVersion();
    std::cout << std::endl 
              << "Usage: explore [options] [dsdb_file]"
              << std::endl
              << "Example: -e InitialMachineExplorer "
              << "-a data/newApplication "
              << "-u bus_count=5 -u build_idf=true test.dsdb"
              << std::endl
              << std::endl
              << "DSDB file is required always and new is created if not found."
              << std::endl
              << "Plugin name, DSDB and at least one application directory "
              << std::endl
              << "in the DSDB are required to run exploration, start "
              << std::endl
              << "configuration ID is optional."
              << std::endl
              << "Any number of parameters that are passed to the used plugin "
              << "can be given."
              << std::endl
              << "Queries can be carried out to the DSDB."
              << std::endl;
    CmdLineOptions::printHelp();
}

/**
 * Returns the explorer plugin name.
 *
 * @return The explorer plugin name.
 */
std::string
ExplorerCmdLineOptions::explorerPlugin() const {
    return findOption(SWL_PLUGIN_NAME)->String();
}

/**
 * Returns the the plugin name which info is to be printed.
 *
 * @return The explorer plugin name which info is to be printed.
 */
std::string
ExplorerCmdLineOptions::pluginInfo() const {
    return findOption(SWL_PLUGIN_INFO)->String();
}

/**
 * Returns the number of directories where test applications are located.
 *
 * @return The number of test directories.
 */
int
ExplorerCmdLineOptions::testApplicationDirectoryCount() const {
    return findOption(SWL_TEST_DIR_ADD)->listSize();
}

/**
 * By the given index, returns a test application directory name given as
 * command line parameter.
 *
 * @param index The index.
 * @return Test application directory path in the given index.
 * @exception OutOfRange If the given index is negative or greater than
 *                       the number of test directories given as parameter.
 */
std::string
ExplorerCmdLineOptions::testApplicationDirectory(int index) const
    throw (OutOfRange) {

    if (index < 0 || index >= testApplicationDirectoryCount()) {
        throw OutOfRange(__FILE__, __LINE__, __func__);
    }

    return findOption(SWL_TEST_DIR_ADD)->String(index + 1);
}

/**
 * Returns the number of parameters given to the explorer plugin.
 *
 * @return The number of parameters.
 */
int
ExplorerCmdLineOptions::explorerPluginParameterCount() const {
    return findOption(SWL_PLUGIN_PARAM)->listSize();
}

/**
 * By the given index, returns a parameter given to the explorer plugin.
 *
 * @param index The index.
 * @return Parameter in the given index.
 * @exception OutOfRange If the given index is negative or greater than
 *                       the number of parameters given to the explorer plugin.
 */
std::string 
ExplorerCmdLineOptions::explorerPluginParameter(int index) const
    throw (OutOfRange) {

    if (index < 0 || index >= explorerPluginParameterCount()) {
        throw OutOfRange(__FILE__, __LINE__, __func__);
    }

    return findOption(SWL_PLUGIN_PARAM)->String(index + 1);
}

/**
 * Returns true if the number of configurations options is set.
 *
 * @return True if the option is set.
 */
bool
ExplorerCmdLineOptions::numberOfConfigurations() const {
    return findOption(SWL_CONFIGURATION_COUNT)->isFlagOn();
}

/**
 * Returns true if the number of configurations options is set.
 *
 * @return True if the option is set.
 */
bool
ExplorerCmdLineOptions::printSummary() const {
    return findOption(SWL_CONFIGURATION_SUMMARY)->isDefined();
}

/**
 * Returns true if the number of configurations options is set.
 *
 * @return True if the option is set.
 */
std::string
ExplorerCmdLineOptions::summaryOrdering() const {
    return findOption(SWL_CONFIGURATION_SUMMARY)->String();
}

/**
 * Returns true if writing out configuration(s) option is used.
 *
 * @return True if the option is defined.
 */
bool
ExplorerCmdLineOptions::writeOutConfiguration() const {
    return findOption(SWL_CONFIGURATION_WRITE)->listSize();
}

/**
 * Returns the number of the write out configuration options given.
 *
 * @return Number of given write out options.
 */
int
ExplorerCmdLineOptions::numberOfConfigurationsToWrite() const {
    return findOption(SWL_CONFIGURATION_WRITE)->listSize();
}

/**
 * By the given index, returns the configuration id given as an option.
 *
 * @param index The index.
 * @return Configuration id in the given index.
 * @exception OutOfRange If the given index is negative or greater than
 *                       the number configurations.
 */
RowID
ExplorerCmdLineOptions::configurationToWrite(int index) const
    throw (OutOfRange) {

    if (index < 0 || index >= numberOfConfigurationsToWrite()) {
        throw OutOfRange(__FILE__, __LINE__, __func__);
    }

    return findOption(SWL_CONFIGURATION_WRITE)->integer(index + 1);
}

/**
 * Returns true if print applications option is used.
 *
 * @return True if the option is defined.
 */
bool
ExplorerCmdLineOptions::printApplications() const {
    return findOption(SWL_LIST_APPS)->isFlagOn();
}

/**
 * Returns true if print applications option is used.
 *
 * @return True if the option is defined.
 */
bool
ExplorerCmdLineOptions::printPlugins() const {
    return findOption(SWL_LIST_PLUGINS)->isFlagOn();
}

/**
 * Returns the number of test applications to be removed from the DSDB.
 *
 * @return The number of expendable applications.
 */
int
ExplorerCmdLineOptions::applicationIDToRemoveCount() const {
    return findOption(SWL_TEST_DIR_RM)->listSize();
}

/**
 * By the given index, returns the application id given as an option to
 * remove from DSDB.
 *
 * @param index The index.
 * @return Application id in the given index.
 * @exception OutOfRange If the given index is negative or greater than
 *                       the number of expendable applications.
 */
RowID
ExplorerCmdLineOptions::applicationIDToRemove(int index) const
    throw (OutOfRange) {

    if (index < 0 || index >= applicationIDToRemoveCount()) {
        throw OutOfRange(__FILE__, __LINE__, __func__);
    }

    return findOption(SWL_TEST_DIR_RM)->integer(index + 1);
}

/**
 * Returns true if at least one HDB file is given as an option.
 *
 * @return True if the option is defined.
 */
bool
ExplorerCmdLineOptions::hdbFileNames() const {
    return findOption(SWL_HDB_FILE)->listSize();
}

/**
 * Returns the number of the HDB file options given.
 *
 * @return Number of given HDB file options.
 */
int
ExplorerCmdLineOptions::hdbFileNameCount() const {
    return findOption(SWL_HDB_FILE)->listSize();
}

/**
 * By the given index, returns the HDB file given as an option.
 *
 * @param index The index.
 * @return HDB file string from the given index.
 * @exception OutOfRange If the given index is negative or greater than
 *                       the number given HDB files.
 */
std::string
ExplorerCmdLineOptions::hdbFileName(int index) const
    throw (OutOfRange) {

    if (index < 0 || index >= hdbFileNameCount()) {
        throw OutOfRange(__FILE__, __LINE__, __func__);
    }

    return findOption(SWL_HDB_FILE)->String(index + 1);
}

/**
 * Returns the ID of the start configuration given as option.
 *
 * If start configuration option was not given returns zero.
 *
 * @return The ID of given start configuration or zero if the option was not
 * used.
 */
RowID
ExplorerCmdLineOptions::startConfiguration() const {

    if (findOption(SWL_START_ID)->isDefined()) {
        return findOption(SWL_START_ID)->integer();
    } else {
        return 0;
    }
}

/**
 * Returns the ID of the start configuration given as option.
 *
 * If start configuration option was not given returns zero.
 *
 * @return The ID of given start configuration or zero if the option was not
 * used.
 */
int
ExplorerCmdLineOptions::verboseLevel() const {

    if (findOption(SWL_VERBOSE_LEVEL)->isDefined()) {
        return findOption(SWL_VERBOSE_LEVEL)->integer();
    } else {
        return -1;
    }
}

/**
 * Returns true if an adf file is given as an option.
 *
 * @return True if an adf file is given as an option.
 */
bool
ExplorerCmdLineOptions::adfFile() const {
    return findOption(SWL_ADF_FILE)->isDefined();
}

/**
 * Returns true if an idf file is given as an option.
 *
 * @return True if an idf file is given as an option.
 */
bool
ExplorerCmdLineOptions::idfFile() const {
    return findOption(SWL_IDF_FILE)->isDefined();
}

/**
 * Returns the name of the adf file given as an option.
 * 
 * Returns an empty string if no adf file was given.
 *
 * @return ADF file name.
 */
std::string
ExplorerCmdLineOptions::adfFileName() const {
    if (adfFile()) {
        return findOption(SWL_ADF_FILE)->String(1);
    } else {
        return "";
    }
}

/**
 * Returns the name of the idf file given as an option.
 *
 * Returns an empty string if no adf file was given.
 *
 * @return IDF file name.
 */
std::string
ExplorerCmdLineOptions::idfFileName() const {
    if (idfFile()) {
        return findOption(SWL_IDF_FILE)->String(1);
    } else {
        return "";
    }
}
