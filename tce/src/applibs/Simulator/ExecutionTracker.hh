/**
 * @file ExecutionTracker.hh
 *
 * Declaration of ExecutionTracker class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_EXECUTION_TRACKER_HH
#define TTA_EXECUTION_TRACKER_HH

#include "Listener.hh"

class TTASimulationController;
class ExecutionTrace;

/**
 * Tracks the simulation execution.
 *
 * Stores execution data in execution trace.
 */
class ExecutionTracker : public Listener {
public:
    ExecutionTracker(
        TTASimulationController& subject, 
        ExecutionTrace& traceDB);
    virtual ~ExecutionTracker();

    virtual void handleEvent();
    
private:
    /// the tracked SimulationController instance
    TTASimulationController& subject_;
    /// the trace database to store the trace to
    ExecutionTrace& traceDB_;
};

#endif
