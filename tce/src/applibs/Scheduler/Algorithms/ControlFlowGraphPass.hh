/**
 * @file ControlFlowGraphPass.hh
 *
 * Declaration of ControlFlowGraphPass interface.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_CFG_PASS_HH
#define TTA_CFG_PASS_HH

#include "Exception.hh"
#include "SchedulerPass.hh"

class BasicBlockPass;
class ControlFlowGraph;
namespace TTAMachine {
    class Machine;
}

/**
 * Interface for scheduler passes that handle control flow graphs.
 */
class ControlFlowGraphPass : public SchedulerPass {
public:
    ControlFlowGraphPass(InterPassData& data);
    virtual ~ControlFlowGraphPass();

    virtual void handleControlFlowGraph(
        ControlFlowGraph& cfg,
        const TTAMachine::Machine& targetMachine)
        throw (Exception) = 0;

    static void executeBasicBlockPass(
        ControlFlowGraph& cfg,
        const TTAMachine::Machine& targetMachine,
        BasicBlockPass& bbPass)
        throw (Exception);
};
#endif
