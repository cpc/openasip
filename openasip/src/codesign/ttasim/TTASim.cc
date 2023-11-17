/*
    Copyright (c) 2002-2012 Tampere University.

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
 * The command line version of the TTA Simulator / Debugger
 *
 * @author Pekka Jääskeläinen 2005-2012 (pjaaskel-no.spam-cs.tut.fi)
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
 * A handler class for Ctrl-c signal.
 *
 * Stops the simulation (if it's running).
 */
class SigINTHandler : public Application::UnixSignalHandler {
public:
    /**
     * Constructor.
     *
     * @param target The target SimulatorFrontend instance.
     */
    SigINTHandler(SimulatorFrontend& target) : target_(target) {
    }

    /**
     * Stops the simulation.
     */
    virtual void execute(int /*data*/, siginfo_t* /*info*/) {
        target_.prepareToStop(SRE_USER_REQUESTED);
    }
private:
    /// Simulator frontend to use when stopping the simulation.
    SimulatorFrontend& target_;
};

/**
 * A handler class for SIGFPE signal
 *
 * Stops the simulation (if it's running). Used for catching
 * errors from the simulated program in the compiled simulation
 * engine.
 */
class SigFPEHandler : public Application::UnixSignalHandler {
public:
    /**
     * Constructor.
     *
     * @param target The target SimulatorFrontend instance.
     */
    SigFPEHandler(SimulatorFrontend& target) : target_(target) {
    }

    /**
     * Terminates the simulation.
     * 
     * @exception SimulationExecutionError thrown always
     */
    virtual void execute(int, siginfo_t *info) {
        std::string msg("Unknown floating point exception");
        
        if (info->si_code == FPE_INTDIV) {
            msg = "integer division by zero";
        } else if (info->si_code == FPE_FLTDIV) {
            msg = "floating-point division by zero";
        } else if (info->si_code == FPE_INTOVF) {
            msg = "integer overflow";
        } else if (info->si_code == FPE_FLTOVF) {
            msg = "floating-point overflow";
        } else if (info->si_code == FPE_FLTUND) {
            msg = "floating-point underflow";
        } else if (info->si_code == FPE_FLTRES) {
            msg = "floating-point inexact result";
        } else if (info->si_code == FPE_FLTINV) {
            msg = "invalid floating-point operation";
        } else if (info->si_code == FPE_FLTSUB) {
            msg = " Subscript out of range";
        }
    
        target_.prepareToStop(SRE_RUNTIME_ERROR);
        target_.reportSimulatedProgramError(
            SimulatorFrontend::RES_FATAL, msg);
       
        throw SimulationExecutionError(__FILE__, __LINE__, __FUNCTION__, msg);
    }
private:
    /// Simulator frontend to use when stopping the simulation.
    SimulatorFrontend& target_;
};

/**
 * A handler class for SIGSEGV signal
 *
 * Stops the simulation (if it's running). Used for catching
 * errors from the simulated program in the compiled simulation
 * engine.
 */
class SigSegvHandler : public Application::UnixSignalHandler {
public:
    /**
     * Constructor.
     *
     * @param target The target SimulatorFrontend instance.
     */
    SigSegvHandler(SimulatorFrontend& target) : target_(target) {
    }

    /**
     * Terminates the simulation.
     * 
     * @exception SimulationExecutionError thrown always
     */
    virtual void execute(int, siginfo_t*) {
        std::string msg("Invalid memory reference");
             
        target_.prepareToStop(SRE_RUNTIME_ERROR);
        target_.reportSimulatedProgramError(
            SimulatorFrontend::RES_FATAL, msg);
       
        throw SimulationExecutionError(__FILE__, __LINE__, __FUNCTION__, msg);
    }
private:
    /// Simulator frontend to use when stopping the simulation.
    SimulatorFrontend& target_;
};

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
    } catch (ParserStopRequest const&) {
        return EXIT_SUCCESS;
    } catch (const IllegalCommandLine& i) {
        std::cerr << i.errorMessage() << std::endl;
        return EXIT_FAILURE;
    }
    
    simFront.reset(new SimulatorFrontend(options->backendType()));
    
    SimulatorCLI* cli = new SimulatorCLI(*simFront);

    if (options->debugMode()) {
        // handler for catching ctrl-c from the user (stops simulation)
        SigINTHandler* ctrlcHandler = new SigINTHandler(*simFront);
        Application::setSignalHandler(SIGINT, *ctrlcHandler);

        if (simFront->isCompiledSimulation()) {

            /* Catch errors caused by the simulated program
               in compiled simulation these show up as normal
               signals as the simulation code is native code we are 
               running in the simulation process. */
            SigFPEHandler fpeHandler(*simFront);
            SigSegvHandler segvHandler(*simFront);
            Application::setSignalHandler(SIGFPE, fpeHandler);
            Application::setSignalHandler(SIGSEGV, segvHandler);
        }
    }

    // check if there is an initialization file in user's home dir and 
    // execute it
    const std::string personalInitScript =
        FileSystem::homeDirectory() + DIR_SEPARATOR + SIM_INIT_FILE_NAME;
    if (FileSystem::fileExists(personalInitScript)) {
        cli->interpreter().processScriptFile(personalInitScript);
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
        cli->interpreter().processScriptFile(currentDirInitScript);
    }

    if (machineToLoad != "") {
        cli->interpreteAndPrintResults(std::string("mach " ) + machineToLoad);
    }

    if (programToLoad != "") {
        cli->interpreteAndPrintResults(std::string("prog " ) + programToLoad);
        if (scriptString == "") {
            // by default, if program is given, start simulation immediately
            cli->interpreteAndPrintResults("run");
        }
    }

    if (scriptString != "") {
        cli->interpreteAndPrintResults(scriptString);
    }

    
    if (options->debugMode()) {        
        cli->run();   
        Application::restoreSignalHandler(SIGINT);
        if (simFront->isCompiledSimulation()) {
            Application::restoreSignalHandler(SIGFPE);
            Application::restoreSignalHandler(SIGSEGV);
        }        
    }
    delete cli;
    return EXIT_SUCCESS;
}
