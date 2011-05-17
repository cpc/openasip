/*
    Copyright (c) 2002-2009 Tampere University of Technology.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file BasicBlockPass.cc
 *
 * Definition of BasicBlockPass class.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pjaaskel-no.spam-cs.tut.fi)
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
    SchedulerPass(data), ddgBuilder_(data) {
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

    if (basicBlock.instructionCount() == 0)
        return;

    DDGPass* ddgPass = dynamic_cast<DDGPass*>(this);
    if (ddgPass != NULL) {
        executeDDGPass(basicBlock, targetMachine, *ddgPass);
    } else {
        abortWithError("basic block pass is not also a ddg pass so you "
                       "must overload handleBasicBlock method!");
    }
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
    
    SimpleResourceManager* rm = SimpleResourceManager::createRM(targetMachine);
    ddgPass.handleDDG(*ddg, *rm, targetMachine);

#ifdef DDG_SNAPSHOTS
    ddg->writeToDotFile(
        (boost::format("bb_%.4d_1_after_scheduling.dot") % bbCounter).str());
    ++bbCounter;
#endif

    copyRMToBB(*rm, bb, targetMachine);

    // deletes or stores rm for future use
    SimpleResourceManager::disposeRM(rm);
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
        rm.loseInstructionOwnership(cycle);

        if (firstNewInstruction == NULL) {
            firstNewInstruction = newInstruction;
        }
    }
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
    return ddgBuilder().build(bb, DataDependenceGraph::INTRA_BB_ANTIDEPS);
}
