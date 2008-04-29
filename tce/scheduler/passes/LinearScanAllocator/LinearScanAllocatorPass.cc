/** 
 * @file LinearScanAllocator.cc
 *
 * Implementation of LinearScanAllocator class
 *
 * Linear Scan register allocator.
 * This version Spills variables after allocation, not before.
 *
 * @author Heikki Kultala 2006 (heikki.kultala@tut.fi)
 * @note rating: red
 */

#include <iostream>
#include <set>
#include <list>

//#include <boost/graph/strong_components.hpp>

//#include "AssocTools.hh"

#include "LinearScanAllocatorCore.hh"
#include "LinearScanAllocatorPass.hh"

#include "Program.hh"

/*
#include "AddressSpaceMapper.hh"

#include "DataDependenceGraph.hh"
#include "DataDependenceEdge.hh"
#include "DataDependenceGraphBuilder.hh"
#include "ControlFlowGraph.hh"

#include "LinearScanAllocatorCore.hh"
#include "StackManager.hh"
#include "RegisterMap.hh"
*/

using namespace TTAProgram;
using namespace TTAMachine;
using std::string;
using std::set;
using std::cout;
using std::cerr;
using std::endl;

SCHEDULER_PASS(LinearScanAllocatorPass)

LinearScanAllocatorPass::LinearScanAllocatorPass() {}

LinearScanAllocatorPass::~LinearScanAllocatorPass() {}

void
LinearScanAllocatorPass::start() 
    throw (Exception) {

    for (int i = 0; i < program_->procedureCount(); i++) {
        Procedure& proc = program_->procedure(i);

        allocatorCore_.initialize(*target_,*program_,interPassData());
        allocatorCore_.allocateProcedure(proc);
    }
//    AddressSpaceMapper::mapAddressSpace(*program_,*target_);
}

/**
 * A short description of the module, usually the module name,
 * in this case "LinearScanAllocatorPass".
 *
 * @return The description as a string.
 */   
std::string
LinearScanAllocatorPass::shortDescription() const {
    return "Startable: LinearScanAllocatorPass";
}

/**
 * Optional longer description of the Module.
 *
 * This description can include usage instructions, details of choice of
 * helper modules, etc.
 *
 * @return The description as a string.
 */
std::string
LinearScanAllocatorPass::longDescription() const {
    std::string answer = "Startable: LinearScanAllocatorPass.";
    answer += " Implements Linear scan register allocator.";
    return answer;
}
