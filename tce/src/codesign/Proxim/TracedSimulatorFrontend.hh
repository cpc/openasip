/**
 * @file TracedSimulatorFrontend.hh
 *
 * Declaration of TracedSimulatorFrontend class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_TRACED_SIMULATOR_FRONTEND_HH
#define TTA_TRACED_SIMULATOR_FRONTEND_HH

#include "SimulatorFrontend.hh"
#include "Informer.hh"
#include "BaseType.hh"

/**
 * TracedSimulatorFrontend is a class which adds event passing to the
 * SimulatorFrontend baseclass.
 *
 * TracedSimulatorFrontend implements the Informer interface and passes
 * events whenever the simulator state changes. This allows a client to
 * track the simulator state by registering itself as a listener of the
 * simulator events.
 */
class TracedSimulatorFrontend : public SimulatorFrontend, public Informer {
public:
    TracedSimulatorFrontend();
    virtual ~TracedSimulatorFrontend();
      
    virtual void loadMachine(const std::string& fileName)
	throw (FileNotFound, IOException, SimulationStillRunning,
               IllegalMachine);

    virtual void loadProgram(const std::string& fileName)
	throw (FileNotFound, IOException, SimulationStillRunning,
	       IllegalProgram, IllegalMachine);

    virtual void run()
	throw (SimulationExecutionError);

    virtual void runUntil(UIntWord address)
	throw (SimulationExecutionError);

    virtual void step(double count = 1)
	throw (SimulationExecutionError);

    virtual void next(int count = 1)
	throw (SimulationExecutionError);

    virtual void killSimulation();

    /// IDs for the simulator events.
    enum event {
	SIMULATOR_MACHINE_LOADED = 0,
	SIMULATOR_PROGRAM_LOADED,
	SIMULATOR_START,
	SIMULATOR_STOP,
        SIMULATOR_RUN,
        SIMULATOR_LOADING_PROGRAM,
        SIMULATOR_LOADING_MACHINE,
        SIMULATOR_RESET
    };    
};
#endif
