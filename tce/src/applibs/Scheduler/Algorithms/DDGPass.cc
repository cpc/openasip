/**
 * 
 * @file DDGPass
 * 
 * Definition of a DDGPass class.
 *
  * @author Heikki Kultala 2007 (hkultala@cs.tut.fi)
 * @note rating:red
 */

#include "DDGPass.hh"
#include "DataDependenceGraph.hh"
#include "Machine.hh"
#include "SimpleResourceManager.hh"

/**
 * Constructor.
 */
DDGPass::DDGPass(InterPassData& data) :
    SchedulerPass(data) {
}

/**
 * Destructor.
 */
DDGPass::~DDGPass() {
}

/**
 * Handless a given DDG.
 *
 * @param ddg DDG to handle
 * @param rm Resource manager that is to be used.
 * @param machine The target machine if any. (NullMachine::instance() if
 * target machine is irrelevant).
 * @exception In case handling is unsuccesful for any reason (basicBlock
 * might still get modified).
 */
void
DDGPass::handleDDG(
    DataDependenceGraph& ddg,
    SimpleResourceManager& rm,
    const TTAMachine::Machine& targetMachine)
    throw (Exception) {

    // just to avoid warnings -- need to keep the argument names for
    // Doxygen comments ;)
    ddg.nodeCount();
    rm.largestCycle();
    targetMachine.machineTester();
    abortWithError("Should never call this.");
}
