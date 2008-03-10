/**
 * @file SimulatorInterpreterContext.cc
 *
 * Implementation of SimulatorInterpreterContext.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */
#include "SimulatorInterpreterContext.hh"
#include "SimulatorFrontend.hh"
#include "Application.hh"

/**
 * Constructor.
 *
 * Initializes the SimulatorFrontend instance.
 *
 */
SimulatorInterpreterContext::SimulatorInterpreterContext(
    SimulatorFrontend& simFrontend) : 
    InterpreterContext(), simulatorFrontend_(simFrontend) {
}

/**
 * Destructor.
 *
 * Does nothing.
 */
SimulatorInterpreterContext::~SimulatorInterpreterContext() {
}

/**
 * Returns the SimulatorFrontend instance contained in the context.
 *
 * @return SimulatorFrontend instance.
 */
SimulatorFrontend&
SimulatorInterpreterContext::simulatorFrontend() {
    return simulatorFrontend_;
}
