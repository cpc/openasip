/**
 * @file ControlFlowGraphPass.cc
 *
 * Definition of ControlFlowGraphPass class.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "ControlFlowGraphPass.hh"
#include "Application.hh"
#include "ControlFlowGraph.hh"
#include "Machine.hh"
#include "BasicBlockPass.hh"

/**
 * Constructor.
 */
ControlFlowGraphPass::ControlFlowGraphPass(InterPassData& data) : 
    SchedulerPass(data) {
}

/**
 * Destructor.
 */
ControlFlowGraphPass::~ControlFlowGraphPass() {
}

/**
 * Handles a single control flow graph.
 *
 * The pass should work with any kind of control flow graph, it should not
 * assume the CFG represents a whole procedure, for example.
 *
 * @param cfg The control flow graph to handle.
 * @param machine The target machine if any. (NullMachine::instance() if
 * target machine is irrelevant).
 * @exception In case handling is unsuccesful for any reason (cfg might 
 * still get modified).
 */
void 
ControlFlowGraphPass::handleControlFlowGraph(
    ControlFlowGraph& cfg,
    const TTAMachine::Machine& targetMachine)
    throw (Exception) {
    
    // just to avoid warnings -- need to keep the argument names for
    // Doxygen comments ;)
    cfg.nodeCount();
    targetMachine.machineTester();
    abortWithError("Should never call this.");
}

/**
 * Executes the given basic block pass on each basic block of the given
 * control flow graph in the original program order.
 *
 * A helper function for implementing most simplest types of CFG passes.
 *
 * @param cfg The control flow graph to handle.
 * @param targetMachine The target machine, if any. (NullMachine::instance() if
 * target machine is irrelevant).
 * @param bbPass The basic block pass to execute.
 * @exception In case handling is unsuccesful for any reason (cfg might 
 * still get modified).
 */
void
ControlFlowGraphPass::executeBasicBlockPass(
    ControlFlowGraph& cfg,
    const TTAMachine::Machine& targetMachine,
    BasicBlockPass& bbPass)
    throw (Exception) {

    const int nodeCount = cfg.nodeCount();
    for (int bbIndex = 0; bbIndex < nodeCount; ++bbIndex) {
        BasicBlockNode& bb = dynamic_cast<BasicBlockNode&>(cfg.node(bbIndex));
        if (!bb.isNormalBB())
            continue;
        bbPass.handleBasicBlock(bb.basicBlock(), targetMachine);
    }
}
