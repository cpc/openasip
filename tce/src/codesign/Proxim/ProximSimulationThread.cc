/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file ProximSimulationThread.cc
 *
 * Implementation of ProximSimulationThread class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
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

    runtimeErrorHandler_ =
	new ProximRuntimeErrorHandler(*simulation_, *gui_);
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
