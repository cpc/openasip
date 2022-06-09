/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @author Pekka Jääskeläinen 2007 (pjaaskel-no.spam-cs.tut.fi)
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
#include "POMDisassembler.hh"
#include "InstructionReferenceManager.hh"
#include "BasicBlockScheduler.hh"
#include "Instruction.hh"
#include "FunctionUnit.hh"
#include "UniversalMachine.hh"

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
 * @todo: Why both BB and BBN as arguments? If BBN is needed then I think
 * BB argument can be replaced with a BBN one. We always should have it
 * anyways.
 *
 * @param basicBlock The basic block to handle.
 * @param machine The target machine if any. (NullMachine::instance() if
 * target machine is irrelevant).
 * @exception In case handling is unsuccesful for any reason (basicBlock
 * might still get modified).
 */
void
BasicBlockPass::handleBasicBlock(
    TTAProgram::BasicBlock& basicBlock,
    const TTAMachine::Machine& targetMachine,
    TTAProgram::InstructionReferenceManager& irm, BasicBlockNode* bbn) {
    if (basicBlock.instructionCount() == 0)
        return;

    DDGPass* ddgPass = dynamic_cast<DDGPass*>(this);
    std::vector<DDGPass*> ddgPasses;
    ddgPasses.push_back(ddgPass);
    if (ddgPass != NULL) {
        executeDDGPass(basicBlock, targetMachine, irm, ddgPasses, bbn);
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
    TTAProgram::BasicBlock& bb, const TTAMachine::Machine& targetMachine,
    TTAProgram::InstructionReferenceManager& irm,
    std::vector<DDGPass*> ddgPasses, BasicBlockNode*) {
    std::cerr << "Calling bbpass::executedgpass." << std::endl;

    DataDependenceGraph* ddg = createDDGFromBB(bb, targetMachine);

    // Used for live info dumping.
    static int bbNumber = 0;

#ifdef DDG_SNAPSHOTS
    std::string name = "scheduling";
    ddgSnapshot(ddg, name, false);
#endif
    
    SimpleResourceManager* rm = SimpleResourceManager::createRM(targetMachine);
    ddgPasses[0]->handleDDG(*ddg, *rm, targetMachine);

#ifdef DDG_SNAPSHOTS
    std::string name = "scheduling";
    ddgSnapshot(ddg, name, true);
#endif

    copyRMToBB(*rm, bb, targetMachine, irm);

    // Print the live range count for each cycle for the sched_yield emitting
    // paper.
    if (Application::verboseLevel() > 1 || 
        getenv("TCE_DUMP_LIVE_INFO") != NULL) {
        for (int index = 0; index < bb.instructionCount(); ++index) {
            if (bb.liveRangeData_ != NULL) {
                Application::logStream() 
                    << "liveinfo:" << bbNumber << ":" << index + rm->smallestCycle() << ":" 
                << bb.liveRangeData_->registersAlive(
                    rm->smallestCycle()+index, targetMachine.controlUnit()->delaySlots(), *ddg).
                size()
                << std::endl;
            }
        }
        bbNumber++;
    }

    delete ddg;
    SimpleResourceManager::disposeRM(rm);
}

/**
 * Creates a DDG from the given basic block and executes a Loop pass for that.
 */
bool
BasicBlockPass::executeLoopPass(
    TTAProgram::BasicBlock&, const TTAMachine::Machine&,
    TTAProgram::InstructionReferenceManager&, std::vector<DDGPass*>,
    BasicBlockNode*) {
    // not implemented
    assert(0 && "should not be here?");
    return false;
}

/** 
 * Prints DDG to a dot file before and after scheudling
 * 
 * @param ddg to print
 * @param name operation name for ddg
 * @param final specify if ddg is after scheduling
 * @param format format of the file
 * @return number of cycles/instructions copied.
 */

void
BasicBlockPass::ddgSnapshot(
    DataDependenceGraph* ddg, 
    std::string& name,
    DataDependenceGraph::DumpFileFormat format,
    bool final) {
    static int bbCounter = 0;

    if (final) {
	if (format == DataDependenceGraph::DUMP_DOT) {
	    ddg->writeToDotFile(
		(boost::format("bb_%.4d_1_after_%2%.dot") % bbCounter % name).str());
	} else {
	    ddg->writeToXMLFile(
		(boost::format("bb_%.4d_1_after_%2%.xml") % bbCounter % name).str());
	}
        ++bbCounter;
    } else {
	if (format == DataDependenceGraph::DUMP_DOT) {
	    ddg->writeToDotFile(
		(boost::format("bb_%.4d_0_before_%2%.dot") % bbCounter % name).str());
	} else {
	    ddg->writeToXMLFile(
		(boost::format("bb_%.4d_0_before_%2%.xml") % bbCounter % name).str());
	}
	Application::logStream() << "\nBB " << bbCounter << std::endl;
    }
}

/** 
 * Copies moves back from resourcemanager to basicblock,
 * putting them to correct instructions based in their cycle,
 * and adding the final delay slots.
 * 
 * @param rm the resourcemanager
 * @param bb the basicblock
 * @param targetMachine machine we are scheduling for.
 * @param irm IRM to keep book of the instruction references.
 * @param lastCycle the last instruction cycle of BB to copy.
 * @return number of cycles/instructions copied.
 */
void
BasicBlockPass::copyRMToBB(
    SimpleResourceManager& rm, TTAProgram::BasicBlock& bb, 
    const TTAMachine::Machine& targetMachine,
    TTAProgram::InstructionReferenceManager& irm, int lastCycle) {

    // find the largest cycle any of a pipelines is still used
    if (lastCycle == -1) {
        const int rmLastCycle = rm.largestCycle();
        lastCycle = rmLastCycle;
    }

    // only first ins of bb may have incoming references
    for (int i = 1; i < bb.instructionCount(); i++) {
        if (irm.hasReference(bb.instructionAtIndex(i))) {
            std::cerr << "non-first has ref:, index: " << i <<
                " bb size: " << bb.instructionCount() << std::endl;
        }
        assert (!irm.hasReference(bb.instructionAtIndex(i)));
    }

    TTAProgram::Instruction refHolder;
    if (bb.instructionCount() && irm.hasReference(bb.instructionAtIndex(0))) {
        irm.replace(bb.instructionAtIndex(0), refHolder);
    }

    int moveCount = 0;

    // update the BB with the new instructions
    bb.clear();

    int index = rm.smallestCycle();
    if (rm.initiationInterval() > 0 && rm.initiationInterval() != INT_MAX) {
        lastCycle = rm.initiationInterval() -1;
        index = 0;
    }

    for (; index <= lastCycle; ++index) {
        TTAProgram::Instruction* newInstruction = rm.instruction(index);
        if (newInstruction->hasControlFlowMove()) {
            // Add delay slots of the last control flow instruction
            // to loop end count. There can be multiple control
            // flow instructions in a basic block in case thread yield
            // emitter is enabled and it has inserted one or more
            // calls to thread_yield().

            // only this this when not modulo-scheduling.
            if (rm.initiationInterval() < 1 || 
                rm.initiationInterval() == INT_MAX) {
                lastCycle = 
                    std::max(
                        lastCycle, 
                        index + targetMachine.controlUnit()->delaySlots());
//                assert(lastCycle >= rmLastCycle);
            }
        }

        moveCount += newInstruction->moveCount();
        bb.add(newInstruction);
#if 0
        Application::logStream() 
            << newInstruction->instructionTemplate().name() << ": "
            << newInstruction->toString() << std::endl;
#endif
        rm.loseInstructionOwnership(index);
    }

    if (irm.hasReference(refHolder)) {
        irm.replace(refHolder, bb.firstInstruction());
    }

    const int instructionCount = lastCycle + 1 - rm.smallestCycle();
    if (Application::verboseLevel() > 1 && bb.isInInnerLoop()) {
        const int busCount = targetMachine.busNavigator().count();
        const int moveSlotCount = busCount * instructionCount;
        Application::logStream() 
            << "BB -- inner loop with trip count: " << bb.tripCount() 
            << std::endl
            << "BB -- instruction count: " << instructionCount << std::endl;
        Application::logStream()
            << "BB -- move slots used: " << moveCount << " of " 
            << moveSlotCount << " (" << (float)moveCount * 100 / moveSlotCount
            << "%)" << std::endl;
    }

    return;
}


DataDependenceGraph*
BasicBlockPass::createDDGFromBB(
    TTAProgram::BasicBlock& bb, const TTAMachine::Machine& mach) {
    return ddgBuilder().build(
        bb, DataDependenceGraph::INTRA_BB_ANTIDEPS, mach, "unknown_bb");
}
