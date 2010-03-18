/*
    Copyright (c) 2002-2010 Tampere University of Technology.

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
 * @file TracedSimulatorFrontend.cc
 *
 * Implementation of TracedSimulatorFrontend class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2010
 * @note rating: red
 */

#include "TracedSimulatorFrontend.hh"


/**
 * The constructor.
 */
TracedSimulatorFrontend::TracedSimulatorFrontend() :
    SimulatorFrontend() {
}

/**
 * The Destrcutor.
 */
TracedSimulatorFrontend::~TracedSimulatorFrontend() {
}


/**
 * Informs listeners whenever a new machine is loaded to the simulator.
 *
 * @param machine Machine file name passed to the baseclass.
 */
void
TracedSimulatorFrontend::loadMachine(const std::string& machine)
    throw (IllegalMachine, SimulationStillRunning, IOException, FileNotFound) {

    handleEvent(SIMULATOR_RESET);
    handleEvent(SIMULATOR_LOADING_MACHINE);
    SimulatorFrontend::loadMachine(machine);
    handleEvent(SIMULATOR_MACHINE_LOADED);
}


/**
 * Informs listeners when a new program is loaded to the simulator.
 *
 * @param program Program file name passed to the baseclass.
 */
void
TracedSimulatorFrontend::loadProgram(const std::string& program) {

    handleEvent(SIMULATOR_RESET);
    handleEvent(SIMULATOR_LOADING_PROGRAM);
    SimulatorFrontend::loadProgram(program);
    handleEvent(SIMULATOR_PROGRAM_LOADED);
}


/**
 * Informs listeners when the simulation starts and stops.
 */
void
TracedSimulatorFrontend::run()
    throw (SimulationExecutionError) {

    handleEvent(SIMULATOR_START);
    handleEvent(SIMULATOR_RUN);
    SimulatorFrontend::run();
    handleEvent(SIMULATOR_STOP);
}

/**
 * Informs listeners when simulation is killed.
 */
void
TracedSimulatorFrontend::killSimulation() {
    SimulatorFrontend::killSimulation();
    handleEvent(SIMULATOR_STOP);
}

/**
 * Informs listeners when the simulation starts and stops.
 *
 * @param address Address passed to the baseclass.
 */
void
TracedSimulatorFrontend::runUntil(UIntWord address)
    throw (SimulationExecutionError) {

    handleEvent(SIMULATOR_START);
    handleEvent(SIMULATOR_RUN);
    SimulatorFrontend::runUntil(address);
    handleEvent(SIMULATOR_STOP);
}


/**
 * Informs listeners when the simulation starts and stops.
 *
 * @param count Number of nexts passed to the baseclass.
 */
void
TracedSimulatorFrontend::next(int count)
    throw (SimulationExecutionError) {

    handleEvent(SIMULATOR_START);
    handleEvent(SIMULATOR_RUN);
    SimulatorFrontend::next(count);
    handleEvent(SIMULATOR_STOP);
}

/**
 * Informs listeners when the simulation starts and stops.
 *
 * @param count Number of steps passed to the baseclass.
 */
void
TracedSimulatorFrontend::step(double count)
    throw (SimulationExecutionError) {

    if (!isSimulationRunning()) {
	handleEvent(SIMULATOR_START);
    }

    SimulatorFrontend::step(count);

    if (!isSimulationRunning()) {
	handleEvent(SIMULATOR_STOP);
    }
}
