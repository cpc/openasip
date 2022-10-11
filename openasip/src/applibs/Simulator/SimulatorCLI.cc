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
 * @file SimulatorCLI.cc
 *
 * Implementation of SimulatorCLI class
 *
 * @author Pekka Jääskeläinen 2012 (pjaaskel-no.spam-cs.tut.fi)
 *
 * @note rating: red
 */

#include "SimulatorCLI.hh"
#include "Application.hh"
#include "SimulatorFrontend.hh"
#include "SimulatorConstants.hh"
#include "Listener.hh"
#include "SimulationEventHandler.hh"
#include "SimulatorInterpreterContext.hh"
#include "LineReaderFactory.hh"
#include "LineReader.hh"
#include "SimulatorCmdLineOptions.hh"
#include "SimulatorInterpreter.hh"
#include "CompiledSimInterpreter.hh"
#include "TCEString.hh"
#include "FileSystem.hh"

/**
 * Class that catches simulated program runtime error events and prints
 * the error reports to stderr.
 */
class RuntimeErrorReporter : public Listener {
public:
    /**
     * Constructor.
     *
     * Registers itself to the SimulationEventHandler to listen to 
     * runtime error events.
     *
     * @param target The target SimulatorFrontend instance.
     */
    RuntimeErrorReporter(SimulatorFrontend& target) : 
        Listener(), target_(target) {
        target.eventHandler().registerListener(
            SimulationEventHandler::SE_RUNTIME_ERROR, this);
    }
    
    virtual ~RuntimeErrorReporter() {
        target_.eventHandler().unregisterListener(
            SimulationEventHandler::SE_RUNTIME_ERROR, this);
    }

    /**
     * Handles the runtime error event by printing and error report to
     * stderr and stopping simulation in case it's a fatal error.
     */
    virtual void handleEvent() {
        size_t minorErrors = target_.programErrorReportCount(
            SimulatorFrontend::RES_MINOR);
        size_t fatalErrors = target_.programErrorReportCount(
            SimulatorFrontend::RES_FATAL);
        InstructionAddress currentPC = target_.programCounter();
        InstructionAddress lastPC = target_.lastExecutedInstruction();
        ClockCycleCount cycleCount = target_.cycleCount();

        if (minorErrors > 0) {
            for (size_t i = 0; i < minorErrors; ++i) {
                std::cerr << "warning: runtime error: "
                          << target_.programErrorReport(
                              SimulatorFrontend::RES_MINOR, i)
                          << std::endl;
            }
        }

        if (fatalErrors > 0) {
            for (size_t i = 0; i < fatalErrors; ++i) {
                std::cerr << "error: runtime error: "
                          << target_.programErrorReport(
                              SimulatorFrontend::RES_FATAL, i) 
                          << std::endl;
            }
            target_.prepareToStop(SRE_RUNTIME_ERROR);
        }
        std::cerr 
            << "Current PC: " << currentPC << " last PC: " << lastPC
            << " cycle count: " << cycleCount << std::endl;
        target_.clearProgramErrorReports();
    }

private:
    /// Simulator frontend to use.
    SimulatorFrontend& target_;
};

/**
 *
 * @param simulatorInterpreter Use the given interpreter instead of default one.
 *                             Takes ownership of the object.
 */
SimulatorCLI::SimulatorCLI(SimulatorFrontend& frontend) :
    simFront_(frontend),
    context_(new SimulatorInterpreterContext(frontend)),
    reader_(LineReaderFactory::lineReader()) {

    assert(reader_ != NULL);

    reader_->initialize(SIM_COMMAND_PROMPT);
    reader_->setInputHistoryLog(SIM_DEFAULT_COMMAND_LOG);

    options_ = NULL;
    if (Application::cmdLineOptions() != NULL) {
        options_ = dynamic_cast<SimulatorCmdLineOptions*>(
            Application::cmdLineOptions());
    }

    if (frontend.isCompiledSimulation()) {
        interpreter_ =
            new CompiledSimInterpreter(
                Application::argc(), Application::argv(),
                *context_, *reader_);
    } else {
        interpreter_ =
            new SimulatorInterpreter(
                Application::argc(), Application::argv(),
                *context_, *reader_);
    }

    /// Catch runtime errors and print them out to the simulator console.
    errorReporter_ = new RuntimeErrorReporter(simFront_);
}

SimulatorCLI::~SimulatorCLI() {

    delete reader_;
    reader_ = NULL;

    delete interpreter_;
    interpreter_ = NULL;

    delete context_;
    context_ = NULL;

    delete errorReporter_;
    errorReporter_ = NULL;
}

/**
 * Executes the given script string in the script interpreter and
 * prints out possible results.
 *
 * @param interpreter Interpreter to use.
 * @param scriptString Script string to execute.
 */
void 
SimulatorCLI::interpreteAndPrintResults(const TCEString& scriptString) {
    interpreter_->interpret(scriptString);
    if (interpreter_->result().size() > 0)
        std::cout << interpreter_->result() << std::endl;
}

/**
 * Runs the interpreter loop.
 *
 * Inputs a line of script from the user, processes it and prints out the
 * results.
 */
void 
SimulatorCLI::run() {

    while (!interpreter_->isQuitCommandGiven()) {
        std::string command = "";
        try {
            command = reader_->readLine();
        } catch (const EndOfFile&) {
            // execute the actual interpreter quit command in case user
            // pressed ctrl-d or the input file ended, to make sure
            // all uninitialization routines are executed correctly
            interpreter_->interpret(SIM_INTERP_QUIT_COMMAND);
            std::cout << interpreter_->result() << std::endl;
            break;
        }
        command = StringTools::trim(command);
        if (command == "") {
            continue;
        }
        interpreter_->interpret(command);
        if (interpreter_->result().size() > 0)
            std::cout << interpreter_->result() << std::endl;
    }
}
