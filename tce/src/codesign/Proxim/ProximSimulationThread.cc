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
 * @file ProximSimulationThread.cc
 *
 * Implementation of ProximSimulationThread class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <sstream>
#include <string>
#include <iostream>
#include <wx/thread.h>
#include "ProximSimulationThread.hh"
#include "SimulatorInterpreterContext.hh"
#include "SimulatorInterpreter.hh"
#include "StringTools.hh"
#include "Application.hh"
#include "SimulatorEvent.hh"
#include "TracedSimulatorFrontend.hh"
#include "ProximRuntimeErrorHandler.hh"

using std::string;


/**
 * The Constructor.
 */
ProximSimulationThread::ProximSimulationThread():
    wxThread(wxTHREAD_JOINABLE),
    simulation_(NULL),
    lineReader_(new ProximLineReader),
    gui_(NULL),
    runtimeErrorHandler_(NULL) {

}




/**
 * The Destructor.
 */
ProximSimulationThread::~ProximSimulationThread() {
    delete lineReader_;
    delete interpreter_;

    if (simulation_ != NULL) {
        delete simulation_;
    }
    if (runtimeErrorHandler_ != NULL) {
        delete runtimeErrorHandler_;
    }
    if (interpreterContext_ != NULL) {
        delete interpreterContext_;
    }
}


/**
 * Initializes the thread.
 */
void
ProximSimulationThread::initialize(ProximMainFrame* gui) {

    gui_ = gui;

    lineReader_->initialize(">");
    simulation_ = new TracedSimulatorFrontend();

    interpreterContext_ = new SimulatorInterpreterContext(*simulation_);
    interpreter_ = new SimulatorInterpreter(
        0, NULL, *interpreterContext_, *lineReader_);

    simulation_->registerListener(
        TracedSimulatorFrontend::SIMULATOR_LOADING_PROGRAM, this);
    simulation_->registerListener(
        TracedSimulatorFrontend::SIMULATOR_PROGRAM_LOADED, this);
    simulation_->registerListener(
        TracedSimulatorFrontend::SIMULATOR_LOADING_MACHINE, this);
    simulation_->registerListener(
        TracedSimulatorFrontend::SIMULATOR_MACHINE_LOADED, this);
    simulation_->registerListener(
        TracedSimulatorFrontend::SIMULATOR_START, this);
    simulation_->registerListener(
        TracedSimulatorFrontend::SIMULATOR_STOP, this);
    simulation_->registerListener(
        TracedSimulatorFrontend::SIMULATOR_RUN, this);
    simulation_->registerListener(
        TracedSimulatorFrontend::SIMULATOR_RESET, this);

    runtimeErrorHandler_ = new ProximRuntimeErrorHandler(*simulation_, *gui_);
}


/**
 * Entry point for the simulation thread execution.
 *
 * The thread waits commands from the line reader, and executes
 * them using the simulator interpreter. Commands are executed
 * until the thread is terminated.
 */
void*
ProximSimulationThread::Entry() {

    assert(gui_ != NULL);

    while (!TestDestroy()) {
    
        std::string command = "";

        command = lineReader_->readLine();
        command = StringTools::trim(command);

        if (command == "") {
            continue;
        }

        if (command == "quit") {
            // Post an event which tells the gui thread to delete this thread.
            SimulatorEvent eventCmd(
                SimulatorEvent::EVT_SIMULATOR_TERMINATED);
            wxPostEvent(gui_, eventCmd);

            // Wait for the gui thread to call Delete() for this thread.
            while (!TestDestroy()) {
                Sleep(100);
            }
            return 0;
        }

        // Inform GUI about the command being executed.
        SimulatorEvent eventCmd(
            SimulatorEvent::EVT_SIMULATOR_COMMAND, command);
        wxPostEvent(gui_, eventCmd);

        interpreter_->interpret(command);

        // Inform GUI about the command execution being done.
        SimulatorEvent eventCmdDone(
            SimulatorEvent::EVT_SIMULATOR_COMMAND_DONE, command);
        wxPostEvent(gui_, eventCmdDone);

        // If the command resulted in error, inform the GUI about the error.
        if (interpreter_->error()) {
            SimulatorEvent eventError(
                SimulatorEvent::EVT_SIMULATOR_ERROR,
                interpreter_->result());
            wxPostEvent(gui_, eventError);
            continue;
        }

        // Inform GUI about the simulator interpreter result.
        if (interpreter_->result().size() > 0) {
            SimulatorEvent event(
                SimulatorEvent::EVT_SIMULATOR_OUTPUT,
                (interpreter_->result() + "\n"));
            wxPostEvent(gui_, event);
        }
    }
    return 0;
}


/**
 * Requests the simulation backend to stop the simulation.
 */
void
ProximSimulationThread::requestStop() {
    simulation_->prepareToStop(SRE_USER_REQUESTED);
    if (simulation_->isSimulationStopped()) {
        SimulatorEvent simEvent(SimulatorEvent::EVT_SIMULATOR_STOP);
        wxPostEvent(gui_, simEvent);
    }
}


/**
 * Kills the current simulation.
 */
void
ProximSimulationThread::killSimulation() {
    if (simulation_->isSimulationRunning()) {
        simulation_->prepareToStop(SRE_USER_REQUESTED);
    }
    simulation_->killSimulation();
    SimulatorEvent simEvent(SimulatorEvent::EVT_SIMULATOR_STOP);
    wxPostEvent(gui_, simEvent);
}

/**
 * Finishes the current simulation.
 */
void
ProximSimulationThread::finishSimulation() {
    if (simulation_->isSimulationRunning()) {
        simulation_->prepareToStop(SRE_USER_REQUESTED);
    }
    simulation_->finishSimulation();
}


/**
 * Returns reference to the line reader which the thread uses to read
 * user input.
 *
 * @return LineReader used by the simulator.
 */
ProximLineReader&
ProximSimulationThread::lineReader() {
    return *lineReader_;
}


/**
 * Returns reference to the script interpreter of the simulator.
 *
 * @return Simulator script interpreter of the simulator.
 */
SimulatorInterpreter*
ProximSimulationThread::interpreter() {
    return interpreter_;

}


/**
 * Handles events from the simulator backend.
 *
 * The events are passed to the GUI as SimulatorEvents, which is a class
 * derived from the wxEvent class. This way responsibility of the event
 * handling can be passed to the GUI thread.
 */
void
ProximSimulationThread::handleEvent(int event) {

    switch (event) {
    case TracedSimulatorFrontend::SIMULATOR_LOADING_MACHINE: {
        SimulatorEvent simEvent(SimulatorEvent::EVT_SIMULATOR_LOADING_MACHINE);
        wxPostEvent(gui_, simEvent);
        return;
    }
    case TracedSimulatorFrontend::SIMULATOR_MACHINE_LOADED: {
        SimulatorEvent simEvent(SimulatorEvent::EVT_SIMULATOR_MACHINE_LOADED);
        wxPostEvent(gui_, simEvent);
        return;
    }
    case TracedSimulatorFrontend::SIMULATOR_LOADING_PROGRAM: {
        SimulatorEvent simEvent(SimulatorEvent::EVT_SIMULATOR_LOADING_PROGRAM);
        wxPostEvent(gui_, simEvent);
        return;
    }
    case TracedSimulatorFrontend::SIMULATOR_PROGRAM_LOADED: {
        SimulatorEvent simEvent(SimulatorEvent::EVT_SIMULATOR_PROGRAM_LOADED);
        wxPostEvent(gui_, simEvent);
        return;
    }
    case TracedSimulatorFrontend::SIMULATOR_START: {
        SimulatorEvent simEvent(SimulatorEvent::EVT_SIMULATOR_START);
        wxPostEvent(gui_, simEvent);
        return;
    }
    case TracedSimulatorFrontend::SIMULATOR_STOP: {
        SimulatorEvent simEvent(SimulatorEvent::EVT_SIMULATOR_STOP);
        wxPostEvent(gui_, simEvent);
        return;
    }
    case TracedSimulatorFrontend::SIMULATOR_RUN: {
        SimulatorEvent simEvent(SimulatorEvent::EVT_SIMULATOR_RUN);
        wxPostEvent(gui_, simEvent);
        return;
    }
    case TracedSimulatorFrontend::SIMULATOR_RESET: {
        gui_->reset();
        return;
    }
    };
}

/**
 * Returns pointer to the simulator.
 */
TracedSimulatorFrontend*
ProximSimulationThread::frontend() {
    return simulation_;
}


/**
 * Returns true if the thread is busy running the simulation.
 *
 * @return True, if the thread is currently running simulation.
 */
bool
ProximSimulationThread::isBusy() {
    if (simulation_->isSimulationRunning()) {
        return true;
    } else {
        return false;
    } 
}
