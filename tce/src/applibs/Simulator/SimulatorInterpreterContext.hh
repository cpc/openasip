/**
 * @file SimulatorInterpreterContext.hh
 *
 * Declaration of SimulatorInterpreterContext class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_SIMULATOR_INTERPRETER_CONTEXT
#define TTA_SIMULATOR_INTERPRETER_CONTEXT

#include "InterpreterContext.hh"
#include "SimulatorFrontend.hh"

/**
 * Interpreter context for Simulator.
 *
 * Holds the SimulatorFrontend instance the interpreter commands use to
 * access simulator functionality.
 */
class SimulatorInterpreterContext : public InterpreterContext {
public:
    SimulatorInterpreterContext(SimulatorFrontend& simFrontend);
    virtual ~SimulatorInterpreterContext();

    SimulatorFrontend& simulatorFrontend();

private:
    SimulatorFrontend& simulatorFrontend_;
};
#endif
