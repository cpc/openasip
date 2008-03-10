/**
 * @file SimulationEventHandler.hh
 *
 * Declaration of SimulationEventHandler class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_SIMULATION_EVENT_HANDLER_HH
#define TTA_SIMULATION_EVENT_HANDLER_HH

#include "Informer.hh"

/**
 * The informer of the simulation specific events.
 */
class SimulationEventHandler : public Informer {
public:
    SimulationEventHandler();
    virtual ~SimulationEventHandler();

    /// simulation specific event codes
    enum {
        SE_NEW_INSTRUCTION = 0,
        ///< Generated before executing a new instructon.
        SE_CYCLE_END,       ///< Generated before advancing the simulator 
                            ///< clock at the end of a simulation cycle.
        SE_RUNTIME_ERROR,   ///< Sent when a runtime error is detected in 
                            ///< the simulated program.
        SE_SIMULATION_STOPPED, ///< Generated after simulation has stopped,
                               ///< temporarily or permantently, and control
                               ///< is being returned to user interface.
        SE_MEMORY_ACCESS    ///< Genereated when memory read or write is
                            ///< initiated.
        
    } SimulationEvent;
private:
};

#endif
