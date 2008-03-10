/**
 * @file TracedSimulatorFrontend.cc
 *
 * Implementation of TracedSimulatorFrontend class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
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
TracedSimulatorFrontend::loadProgram(const std::string& program)
    throw (FileNotFound, IOException, SimulationStillRunning,
           IllegalProgram, IllegalMachine) {

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
