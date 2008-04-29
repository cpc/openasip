/**
 * @file SchedulerCmdLineOptions.cc
 *
 * Implementation of SchedulerCmdLineOptions class.
 *
 * @author Ari Mets�halme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#include <iostream>
#include "SchedulerCmdLineOptions.hh"
#include "config.h"

using std::cout;
using std::endl;

const std::string SchedulerCmdLineOptions::ADF_PARAM_NAME = "adf";
const std::string SchedulerCmdLineOptions::CONF_PARAM_NAME = "config";
const std::string SchedulerCmdLineOptions::OUTPUT_PARAM_NAME = "output";
const std::string SchedulerCmdLineOptions::VERBOSE_SWITCH = "verbose";
const std::string SchedulerCmdLineOptions::USAGE =
    "Usage: schedule [OPTION]... SOURCE\n"
    "Schedule SOURCE binary with the given options\n"
    "Example: schedule -a mach.adf hello.tpef";

/**
 * Constructor.
 */
SchedulerCmdLineOptions::SchedulerCmdLineOptions(): CmdLineOptions(USAGE) {
    StringCmdLineOptionParser* adfFile = new StringCmdLineOptionParser(
        ADF_PARAM_NAME, "The ADF file of the target architecture", "a");
    addOption(adfFile);
    StringCmdLineOptionParser* confFile = new StringCmdLineOptionParser(
        CONF_PARAM_NAME, "The configuration file", "c");
    addOption(confFile);
    BoolCmdLineOptionParser* verboseSwitch = new BoolCmdLineOptionParser(
        VERBOSE_SWITCH, "The verbose switch", "v");
    addOption(verboseSwitch);
    StringCmdLineOptionParser* outputFile = new StringCmdLineOptionParser(
        OUTPUT_PARAM_NAME, "The output file", "o");
    addOption(outputFile);
}

/**
 * Destructor.
 */
SchedulerCmdLineOptions::~SchedulerCmdLineOptions() {
}

/**
 * Return true if the target ADF was defined in the command line.
 *
 * @return True if the target ADF was defined in the command line.
 */
bool
SchedulerCmdLineOptions::isTargetADFDefined() const {
    return findOption(ADF_PARAM_NAME)->isDefined();
}

/**
 * Return the name of the target ADF file.
 *
 * @return The name of the target ADF file.
 * @exception NotAvailable if the target ADF was not defined.
 */
std::string
SchedulerCmdLineOptions::targetADF() const {
    return findOption(ADF_PARAM_NAME)->String();
}

/**
 * Return true if the configuration file was defined in the command line.
 *
 * @return True if the configuration file was defined in the command line.
 */
bool
SchedulerCmdLineOptions::isConfigurationFileDefined() const {
    return findOption(CONF_PARAM_NAME)->isDefined();
}

/**
 * Return true if the verbose switch was defined in the command line.
 *
 * @return True if the verbose switch was defined in the command line.
 */
bool
SchedulerCmdLineOptions::isVerboseSwitchDefined() const {
    return findOption(VERBOSE_SWITCH)->isDefined();
}

/**
 * Return the name of the configuration file.
 *
 * @return The name of the configuration file.
 * @exception NotAvailable if the configuration file was not defined.
 */
std::string
SchedulerCmdLineOptions::configurationFile() const {
    return findOption(CONF_PARAM_NAME)->String();
}

/**
 * Return true if the output file was defined in the command line.
 *
 * @return True if the output file was defined in the command line.
 */
bool
SchedulerCmdLineOptions::isOutputFileDefined() const {
    return findOption(OUTPUT_PARAM_NAME)->isDefined();
}

/**
 * Return the name of the output file.
 *
 * @return The name of the output file.
 * @exception NotAvailable if the output file was not defined.
 */
std::string
SchedulerCmdLineOptions::outputFile() const {
    return findOption(OUTPUT_PARAM_NAME)->String();
}

/**
 * Print the version of the user interface.
 */
void
SchedulerCmdLineOptions::printVersion() const {
    cout << "schedule - TCE Scheduler command line interface "
         << VERSION << endl;
}
