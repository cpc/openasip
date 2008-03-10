/**
 * @file ProcedureTransferTracker.hh
 *
 * Declaration of ProcedureTransferTracker class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROCEDURE_TRANSFER_TRACKER_HH
#define TTA_PROCEDURE_TRANSFER_TRACKER_HH

#include "Listener.hh"

class SimulatorFrontend;
class ExecutionTrace;

namespace TTAProgram {
    class Instruction;
}

/**
 * Tracks procedure transfers in the simulated program.
 *
 * Stores data of the transfer in trace database.
 */
class ProcedureTransferTracker : public Listener {
public:
    ProcedureTransferTracker(
        SimulatorFrontend& subject, 
        ExecutionTrace& traceDB);
    virtual ~ProcedureTransferTracker();

    virtual void handleEvent();
    
private:
    /// the tracked SimulatorFrontend instance
    SimulatorFrontend& subject_;
    /// the trace database to store the trace to
    ExecutionTrace& traceDB_;
    /// the previously executed instruction
    const TTAProgram::Instruction* previousInstruction_;
};

#endif
