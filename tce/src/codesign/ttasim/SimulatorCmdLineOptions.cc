/**
 * @file SimulatorCmdLineOptions.cc
 *
 * Declaration of SimulatorCmdLineOptions.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @author Viljami Korhonen 2007 (viljami.korhonen@tut.fi)
 * @note rating: red
 */

#include <iostream>

#include "SimulatorToolbox.hh"
#include "SimulatorTextGenerator.hh"
#include "SimulatorConstants.hh"
#include "CmdLineOptions.hh"
#include "SimulatorCmdLineOptions.hh"

/// Long switch string for setting the debug mode.
const std::string SWL_DEBUG_MODE = "debugmode";
/// Short switch string for setting the debug mode.
const std::string SWS_DEBUG_MODE = "d";

/// Long switch string for giving a script to be executed.
const std::string SWL_EXECUTE_SCRIPT = "execute-script";
/// Short switch string for giving a script to be executed.
const std::string SWS_EXECUTE_SCRIPT= "e";

/// Long switch string for giving a machine file.
const std::string SWL_MACHINE_TO_LOAD = "adf";
/// Short switch string for giving a machine file.
const std::string SWS_MACHINE_TO_LOAD= "a";

/// Long switch string for giving the program file
const std::string SWL_PROGRAM_TO_LOAD = "program";
/// Short switch string for giving the program file
const std::string SWS_PROGRAM_TO_LOAD= "p";

/// Long switch string for the fast simulation
const std::string SWL_FAST_SIM = "quick";
/// Short switch string for the fast simulation
const std::string SWS_FAST_SIM= "q";


/**
 * Constructor.
 *
 * @todo Use textgenerator in the help texts.
 */
SimulatorCmdLineOptions::SimulatorCmdLineOptions() : CmdLineOptions("") {
    addOption(
        new BoolCmdLineOptionParser(
            SWL_DEBUG_MODE, "starts simulator in debugging mode (default), "
            "use --no-debugmode to disable",
            SWS_DEBUG_MODE));
    addOption(
        new StringCmdLineOptionParser(
            SWL_EXECUTE_SCRIPT, "executes the given string as a script in the "
            "simulator control language script interpreter, "
            "e.g. -e \"stepi 10000\" executes simulation for 10000 cycles",
            SWS_EXECUTE_SCRIPT));
    
    addOption(
        new StringCmdLineOptionParser(
            SWL_MACHINE_TO_LOAD, "sets the machine file (.adf) to be loaded.",            
            SWS_MACHINE_TO_LOAD));
    
    addOption(
        new StringCmdLineOptionParser(
            SWL_PROGRAM_TO_LOAD, "sets the program file to be loaded.",            
            SWS_PROGRAM_TO_LOAD));
            
     addOption(
        new BoolCmdLineOptionParser(
            SWL_FAST_SIM, "uses the fast simulation engine.",            
            SWS_FAST_SIM));
}

/**
 * Destructor.
 */
SimulatorCmdLineOptions::~SimulatorCmdLineOptions() {
}

/**
 * Prints the version of the program.
 */
void
SimulatorCmdLineOptions::printVersion() const {
    std::cout << SIM_CLI_TITLE << " " << VERSION << std::endl;
}

/**
 * Prints the help menu of the program.
 *
 * @todo Implement using SimulatorTextGenerator.
 */
void
SimulatorCmdLineOptions::printHelp() const {
    printVersion();
    std::cout << std::endl << SimulatorToolbox::textGenerator().text(
        Texts::TXT_CMD_LINE_HELP).str() << std::endl;
    CmdLineOptions::printHelp();
}

/**
 * Returns true if there is a value available for given option.
 *
 * @return True if the option is defined.
 */
bool
SimulatorCmdLineOptions::optionGiven(std::string key) {
    try {
        /// @todo: This returns always true if trying to find added
        /// CmdLineOption... 
        return findOption(key)->isDefined();
    } catch (const IllegalCommandLine&) {
        return false;
    }
    return true;
}

/**
 * Returns true if Simulator should be started in debugging mode.
 *
 * If no value is given in the parsed command line, default one is returned.
 *
 * @return True if Simulator should be started in debugging mode.
 */
bool 
SimulatorCmdLineOptions::debugMode() {
    if (!optionGiven(SWL_DEBUG_MODE)) {
        return true;
    }
    return findOption(SWL_DEBUG_MODE)->isFlagOn();
}

/**
 * Returns the script to be executed in the interpreter.
 *
 * Returns an empty string, if none defined.
 *
 * @return Script string.
 */
std::string
SimulatorCmdLineOptions::scriptString() {
    if (!optionGiven(SWL_EXECUTE_SCRIPT)) {
        return "";
    }
    return findOption(SWL_EXECUTE_SCRIPT)->String();
}

/**
 * Returns the filename of the given machine (.adf)
 *
 * @return the filename of the given machine (.adf)
 */
std::string 
SimulatorCmdLineOptions::machineFile() {
    return findOption(SWS_MACHINE_TO_LOAD)->String();
}
    
/**
 * Returns the filename of the given program
 *
 * @return the filename of the given program
 */
std::string
SimulatorCmdLineOptions::programFile() {
    return findOption(SWS_PROGRAM_TO_LOAD)->String();
}

/**
 * Returns true if Simulator should use the fast simulation engine
 *
 * If no value is given in the parsed command line, default one is returned.
 *
 * @return True if Simulator should use the fast simulation engine
 */
bool 
SimulatorCmdLineOptions::fastSimulationEngine() {
    if (!optionGiven(SWL_FAST_SIM)) {
        return false;
    }
    return findOption(SWL_FAST_SIM)->isFlagOn();
}


