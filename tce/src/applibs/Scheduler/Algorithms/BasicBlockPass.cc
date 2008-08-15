/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file BasicBlockPass.cc
 *
 * Definition of BasicBlockPass class.
 *
 * @author Pekka Jääskeläinen 2007 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "BasicBlockPass.hh"
#include "Application.hh"
#include "BasicBlock.hh"
#include "Machine.hh"
#include "DataDependenceGraphBuilder.hh"
#include "SimpleResourceManager.hh"
#include "ControlUnit.hh"
#include "DDGPass.hh"

/**
 * Constructor.
 */
BasicBlockPass::BasicBlockPass(InterPassData& data) : 
    SchedulerPass(data) {
}

/**
 * Destructor.
 */
BasicBlockPass::~BasicBlockPass() {
}

/**
 * Handles a single basic block.
 *
 * Modifies the given basic block, so if the old one is to be preserved,
 * client should copy the original. Does not restore the BB even though
 * handling was not successful.
 *
 * @param basicBlock The basic block to handle.
 * @param machine The target machine if any. (NullMachine::instance() if
 * target machine is irrelevant).
 * @exception In case handling is unsuccesful for any reason (basicBlock
 * might still get modified).
 */
void 
BasicBlockPass::handleBasicBlock(
    BasicBlock& basicBlock,
    const TTAMachine::Machine& targetMachine)
    throw (Exception) {

    // just to avoid warnings -- need to keep the argument names for
    // Doxygen comments ;)
    basicBlock.instructionCount();
    targetMachine.machineTester();
    abortWithError("Should never call this.");
}

/**
 * Creates a DDG from the given basic block and executes a DDG pass for that.
 */
void
BasicBlockPass::executeDDGPass(
    BasicBlock& bb,
    const TTAMachine::Machine& targetMachine, 
    DDGPass& ddgPass)
    throw (Exception) {

    DataDependenceGraph* ddg = createDDGFromBB(bb);

#ifdef DDG_SNAPSHOTS
    static int bbCounter = 0;
    ddg_->writeToDotFile(
        (boost::format("bb_%.4d_0_before_scheduling.dot") % bbCounter).str());
    Application::logStream() << "\nBB " << bbCounter << std::endl;
#endif
    
    SimpleResourceManager* rm = new SimpleResourceManager(targetMachine);
    ddgPass.handleDDG(*ddg, *rm, targetMachine);

#ifdef DDG_SNAPSHOTS
    ddg->writeToDotFile(
        (boost::format("bb_%.4d_1_after_scheduling.dot") % bbCounter).str());
    ++bbCounter;
#endif

    // Find largest cycle any move was scheduled in DDG
//    const int ddgLastCycle = ddg->largestCycle();

    copyRMToBB(*rm, bb, targetMachine);

    // deletes or stores rm for future use
    deleteRM(rm, bb);
    delete ddg;
}


/** 
 * Copies moves back from resourcemanager to basicblock,
 * putting them to correct instructions based in their cycle,
 * and addign the final delay slots.
 * 
 * @param rm the resourcemanager
 * @param bb the basicblock
 * @param targetMachine machine we are scheduling for.
 */
void BasicBlockPass::copyRMToBB(
    SimpleResourceManager& rm, BasicBlock& bb, 
    const TTAMachine::Machine& targetMachine) {
    // find the largest cycle any of a pipelines is still used
    const int rmLastCycle = rm.largestCycle();
//    assert(rmLastCycle >= ddgLastCycle);
    int lastCycle = rmLastCycle;

    // the location of the branch instruction in the BB
    int jumpCycle = -1;

    TTAProgram::Instruction* firstNewInstruction = NULL;
    TTAProgram::Instruction* placeHolder = &bb.instructionAtIndex(0);
    TTAProgram::Instruction* lastNewInstruction = placeHolder;

    // update the BB with the new instructions
    bb.clear();
    for (int cycle = 0; cycle <= lastCycle; ++cycle) {

        TTAProgram::Instruction* newInstruction = rm.instruction(cycle);

        if (newInstruction->hasControlFlowMove()) {
            assert(jumpCycle == -1 && "Multiple jumps in BB!");

            // add delay slots to loop end count
            lastCycle = cycle + targetMachine.controlUnit()->delaySlots();
            assert(lastCycle >= rmLastCycle);
        }

        bb.add(newInstruction);
        lastNewInstruction = newInstruction;

        if (firstNewInstruction == NULL) {
            firstNewInstruction = newInstruction;
        }
    }

    rm.loseInstructionOwnership();
}

/**
 * Helper function used to create DDG for BBPass.
 *
 * By overriding this in derived class a different way of creating DDG's
 * can be used ( like subgraphs of a big DDG )
 *
 * @param bb BasicBlock where DDG is to be created from
 */
DataDependenceGraph*
BasicBlockPass::createDDGFromBB(BasicBlock& bb) {
    DataDependenceGraphBuilder ddgb;
    return ddgb.build(bb);
}

/**
 * Helper function used to delete resourcemanager.
 *
 * By overriding this in derived class RM can be stored for a future use,
 * and releted later.
 *
 * @param rm ResourceManager to delete or store for future use.
 * @param bb BasicBlock which the RM relates to.
 */
void 
BasicBlockPass::deleteRM(SimpleResourceManager* rm, BasicBlock& bb) {
    delete rm;

    // to avoid warnings.
    bb.instructionCount();
}
