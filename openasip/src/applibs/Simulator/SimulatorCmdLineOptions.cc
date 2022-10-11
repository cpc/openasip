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
 * @file SimulatorCmdLineOptions.cc
 *
 * Declaration of SimulatorCmdLineOptions.
 *
 * @author Pekka Jääskeläinen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @author Viljami Korhonen 2007 (viljami.korhonen-no.spam-tut.fi)
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

/// Long switch string for the custom remote debugger target
const std::string SWL_CUSTOM_DBG = "custom"; 
/// Short switch string for the custom remote debugger target
const std::string SWS_CUSTOM_DBG = "c"; 

/// Long switch string for the TCE builtin remote debugger target
const std::string SWL_REMOTE_DBG = "remote"; 
/// Short switch string for the TCE builtin remote debugger target
const std::string SWS_REMOTE_DBG = "r"; 

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

     addOption(
        new BoolCmdLineOptionParser(
            SWL_REMOTE_DBG, "connect to a remote debugging interface on an FPGA or ASIC.",
            SWS_REMOTE_DBG));

     addOption(
        new BoolCmdLineOptionParser(
            SWL_CUSTOM_DBG, "connect to a custom remote debugger (if implemented).",
            SWS_CUSTOM_DBG));
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
    std::cout << SIM_CLI_TITLE << " " << Application::TCEVersionString() 
              << std::endl;
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
 * Check what sort of simulation user asked for on the command line.
 *
 * If no value is given in the parsed command line the "normal" simulation, 
 * i.e. the interpreted, non-compiled, version is returned.
 *
 * @return type of TTA backend user wants
 */
SimulatorFrontend::SimulationType
SimulatorCmdLineOptions::backendType() {

    bool wantCompiled = false;
    bool wantRemote = false;
    bool wantCustom = false;

    wantCompiled |= optionGiven(SWL_FAST_SIM);
    wantCompiled &= findOption(SWL_FAST_SIM)->isFlagOn();

    wantRemote |= optionGiven(SWL_REMOTE_DBG);
    wantRemote &= findOption(SWL_REMOTE_DBG)->isFlagOn();

    wantCustom |= optionGiven(SWL_CUSTOM_DBG);
    wantCustom &= findOption(SWL_CUSTOM_DBG)->isFlagOn();

    // TODO: no check for if user requests simultaneously several 
    // versions of TTA backend. Start with the most picky one, 
    // user probably notices it erroring out.
    if (wantCustom) return SimulatorFrontend::SIM_CUSTOM;
    if (wantRemote) return SimulatorFrontend::SIM_REMOTE;
    if (wantCompiled) return SimulatorFrontend::SIM_COMPILED;
    return SimulatorFrontend::SIM_NORMAL;
}

