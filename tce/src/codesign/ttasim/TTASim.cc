/*
    Copyright (c) 2002-2012 Tampere University of Technology.

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
 * @file TTASim.cc
 *
 * Implementation of ttasim.
 *
 * The command line version of the TTA Simulator.
 *
 * @author Pekka J‰‰skel‰inen 2005-2012 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <string>
#include <iostream>
#include <boost/shared_ptr.hpp>

#include "SimulatorCmdLineOptions.hh"
#include "SimulatorFrontend.hh"
#include "FileSystem.hh"
#include "SimulatorInterpreter.hh"
#include "SimulatorCLI.hh"
#include "SimulatorToolbox.hh"

/**
 * Main function.
 *
 * Parses the command line and figures out whether to start the interactive
 * debugging mode or not.
 *
 * @param argc The command line argument count.
 * @param argv The command line arguments (passed to the interpreter).
 * @return The return status.
 */
int main(int argc, char* argv[]) {

    Application::initialize(argc, argv);
    
    boost::shared_ptr<SimulatorFrontend> simFront;
    /// @todo: read command line options and initialize the 
    /// simulator (frontend) using the given values.
    SimulatorCmdLineOptions* options = new SimulatorCmdLineOptions;
    try {
        options->parse(argv, argc);
        Application::setCmdLineOptions(options);
    } catch (ParserStopRequest) {
        return EXIT_SUCCESS;
    } catch (const IllegalCommandLine& i) {
        std::cerr << i.errorMessage() << std::endl;
        return EXIT_FAILURE;
    }
    
    simFront.reset(new SimulatorFrontend(options->fastSimulationEngine()));
    
    SimulatorCLI cli(*simFront);

    // check if there is an initialization file in user's home dir and 
    // execute it
    const std::string personalInitScript =
        FileSystem::homeDirectory() + DIR_SEPARATOR + SIM_INIT_FILE_NAME;
    if (FileSystem::fileExists(personalInitScript)) {
        cli.interpreter().processScriptFile(personalInitScript);
    }   
    
    std::string machineToLoad = options->machineFile();
    std::string programToLoad = options->programFile();
    const std::string scriptString = options->scriptString();
    
    if (options->numberOfArguments() != 0) {
        std::cerr << SimulatorToolbox::textGenerator().text(
            Texts::TXT_ILLEGAL_ARGUMENTS).str() << std::endl;
        return EXIT_FAILURE;
    }  

    // check if there is an initialization file in the current dir and
    // execute it
    const std::string currentDirInitScript =
        FileSystem::currentWorkingDir() + DIR_SEPARATOR + SIM_INIT_FILE_NAME;
    if (FileSystem::fileExists(currentDirInitScript)) {
        cli.interpreter().processScriptFile(currentDirInitScript);
    }

    if (machineToLoad != "") {
        cli.interpreteAndPrintResults(std::string("mach " ) + machineToLoad);
    }

    if (programToLoad != "") {
        cli.interpreteAndPrintResults(std::string("prog " ) + programToLoad);
        if (scriptString == "") {
            // by default, if program is given, start simulation immediately
            cli.interpreteAndPrintResults("run");
        }
    }

    if (scriptString != "") {
        cli.interpreteAndPrintResults(scriptString);
    }

    if (options->debugMode()) cli.run();   

    return EXIT_SUCCESS;
}
