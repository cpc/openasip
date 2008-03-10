/**
 * @file ProcedurePass.hh
 *
 * Declaration of ProcedurePass interface.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROCEDURE_PASS_HH
#define TTA_PROCEDURE_PASS_HH

#include "Exception.hh"
#include "SchedulerPass.hh"

namespace TTAMachine {
    class Machine;
}

namespace TTAProgram {
    class Procedure;
}

class ControlFlowGraph;

/**
 * Interface for scheduler passes that handle procedures.
 */
class ProcedurePass : public SchedulerPass {
public:
    ProcedurePass(InterPassData& data);
    virtual ~ProcedurePass();

    virtual void handleProcedure(
        TTAProgram::Procedure& procedure,
        const TTAMachine::Machine& targetMachine)
        throw (Exception) = 0;

    static void copyCfgToProcedure(
        TTAProgram::Procedure& procedure, ControlFlowGraph& cfg)
        throw (Exception);
    
};
#endif
