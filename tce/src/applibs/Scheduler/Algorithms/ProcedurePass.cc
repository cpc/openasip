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
 * @file ProcedurePass.cc
 *
 * Definition of ProcedurePass class.
 *
 * @author Pekka J��skel�inen 2007 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "ProcedurePass.hh"
#include "Application.hh"
#include "Procedure.hh"
#include "Machine.hh"
#include "BasicBlockPass.hh"
#include "ControlFlowGraph.hh"
#include "ControlUnit.hh"
#include "TerminalInstructionAddress.hh"
#include "ControlFlowGraphPass.hh"
#include "InstructionReference.hh"
#include "InstructionReferenceManager.hh"
#include "TerminalFUPort.hh"
#include "Program.hh"
#include "Instruction.hh"
#include "BasicBlock.hh"

/**
 * Constructor.
 */
ProcedurePass::ProcedurePass(InterPassData& data) : 
    SchedulerPass(data) {
}

/**
 * Destructor.
 */
ProcedurePass::~ProcedurePass() {
}

/**
 * Handles a single procedure.
 *
 * @param proecude The procedure to handle.
 * @param machine The target machine if any. (NullMachine::instance() if
 * target machine is irrelevant).
 * @exception In case handling is unsuccesful for any reason (procedure might 
 * still get modified).
 */
void 
ProcedurePass::handleProcedure(
    TTAProgram::Procedure& procedure,
    const TTAMachine::Machine& targetMachine)
    throw (Exception) {
    
    ControlFlowGraphPass* cfgPass = dynamic_cast<ControlFlowGraphPass*>(this);
    if (cfgPass != NULL) {
        executeControlFlowGraphPass(procedure, targetMachine, *cfgPass);
    } else {
        abortWithError("Procedure pass is not also a cfg pass so you "
                       "must overload handleProcedure method!");
    }
}

void
ProcedurePass::copyCfgToProcedure(
    TTAProgram::Procedure& procedure,
    ControlFlowGraph& cfg) throw (Exception) {

    TTAProgram::InstructionReferenceManager& irm = 
        procedure.parent().instructionReferenceManager();
    
    int insCountDelta = 0;

    // collect the starting point instructions to keep as place holders
    typedef std::map<const BasicBlockNode*, TTAProgram::Instruction*,
        BasicBlockNode::Comparator> PlaceHolders;
    PlaceHolders placeHolders;

    // store pointers to al original instrs so we can remove them
    std::set<TTAProgram::Instruction*> originalInstr;
    for (int i = 0; i < procedure.instructionCount(); i++) {
        originalInstr.insert(&procedure.instructionAtIndex(i));
    }

    for (int bbIndex = 0; bbIndex < cfg.nodeCount(); ++bbIndex) {
        BasicBlockNode& bb = dynamic_cast<BasicBlockNode&>(cfg.node(bbIndex));
        if (!bb.isNormalBB())
            continue;
        if (!bb.hasOriginalAddress()) 
            throw Exception(
                __FILE__, __LINE__, __func__, 
                "Cannot replace a basic block without original address.");
        placeHolders[&bb] = &procedure.instructionAt(bb.originalStartAddress());
    }

    for (int bbIndex = 0; bbIndex < cfg.nodeCount(); ++bbIndex) {
        BasicBlockNode& bb = dynamic_cast<BasicBlockNode&>(cfg.node(bbIndex));

        if (!bb.isNormalBB())
            continue;
     
        PlaceHolders::iterator placeHolderIter = placeHolders.find(&bb);
        TTAProgram::Instruction& placeHolder = *placeHolderIter->second;
        placeHolders.erase(placeHolderIter);

        // Remove all original instructions (after the place holder first
        // instruction) from the procedure as we are dealing with basic blocks,
        // we can assume that only at most the first instruction is referred to.
        const int originalBBSize = 
            bb.originalEndAddress() - bb.originalStartAddress() + 1;
        int addr = placeHolder.address().location() + 1;
        for (int i = 1; i < originalBBSize; ++i) {
            TTAProgram::Instruction& ins = 
                procedure.instructionAt(addr);
            originalInstr.erase(&ins);
            procedure.CodeSnippet::remove(ins);
            delete &ins;
            insCountDelta--;
        }

        TTAProgram::Instruction* firstNewInstruction = NULL;
        TTAProgram::Instruction* lastNewInstruction = &placeHolder;

        // consistency check.
        for (int i = 0; i < bb.basicBlock().skippedFirstInstructions(); i++) {
            TTAProgram::Instruction& ins =
                bb.basicBlock().instructionAtIndex(i);
            if (irm.hasReference(ins)) {
                throw IllegalProgram(__FILE__,__LINE__, __func__, 
                                     "Skipped ins has a ref");
            }
        }

        // Copy the instructions from the scheduled basic block back 
        // to the program.
        for (int i = bb.basicBlock().skippedFirstInstructions(); 
             i < bb.basicBlock().instructionCount(); ++i) {
            TTAProgram::Instruction& instrToCopy =
                bb.basicBlock().instructionAtIndex(i);
            TTAProgram::Instruction* newInstruction = instrToCopy.copy();
            procedure.insertAfter(
                *lastNewInstruction, newInstruction);
            // update references..
            if (irm.hasReference(instrToCopy)) {
                irm.replace(instrToCopy,*newInstruction);
            }
            lastNewInstruction = newInstruction;
            if (firstNewInstruction == NULL)
                firstNewInstruction = newInstruction;
        }

        // replace all references to the placeholder instruction to point to
        // the new first instruction
        if (procedure.parent().instructionReferenceManager().hasReference(
                placeHolder)) {
            assert(firstNewInstruction != NULL);
            procedure.parent().instructionReferenceManager().replace(
                placeHolder, *firstNewInstruction);
        } 
        // now we can delete also the place holder old instruction
        // @todo what if the instruction itself had a reference?
        originalInstr.erase(&placeHolder);

        // might have code labels so do not optimize this
        procedure.remove(placeHolder);
        delete &placeHolder;
        // ...and we're done with this basic block
    }

    // refs to dead instructions are dead refs but there is no way to
    // remove insr refs. so replace them to refs to first instr.
//    TTAProgram::Instruction& firstIns = procedure.instructionAtIndex(0);

    // delete dead instructios (ones from the original procedure
    for(std::set<TTAProgram::Instruction*>::iterator i = 
            originalInstr.begin(); i != originalInstr.end(); i++) {
        TTAProgram::Instruction* ins = *i;
        assert(!irm.hasReference(*ins));
        procedure.CodeSnippet::remove(*ins);
        delete ins;
        insCountDelta--;
    }

    // fix the addresses of following procedures. 
    if (insCountDelta != 0) {
        if (procedure.isInProgram()) {
            if (!(&procedure == &procedure.parent().lastProcedure())) {
                procedure.parent().moveProcedure(
                    procedure.parent().nextProcedure(procedure), 
                    insCountDelta);
            }
        }
    }

    // scan for meaningless jumps and remove them.
    // currently does not work for jump addresses in long immediates,
    // just ignores them.
    // but this gets rid of unnecessary jumps after tce side if conversion.
    for (int i = 0; i < procedure.instructionCount()-1; i++) {
        TTAProgram::Instruction& ins = procedure.instructionAtIndex(i);
        for (int j = 0; j < ins.moveCount(); j++) {
            TTAProgram::Move& move = ins.move(j);
            // jump those target is easily known?
            if (move.isJump() && move.source().isInstructionAddress()) {
                TTAProgram::TerminalInstructionAddress& tia =
                    dynamic_cast<TTAProgram::TerminalInstructionAddress&>(
                        move.source());
                TTAProgram::Instruction& targetIns = 
                    tia.instructionReference().instruction();
                TTAProgram::TerminalFUPort& tfp = 
                    dynamic_cast<TTAProgram::TerminalFUPort&>(
                        move.destination());
                const TTAMachine::FunctionUnit* fu = &tfp.functionUnit();
                const TTAMachine::ControlUnit* gcu = 
                    dynamic_cast<const TTAMachine::ControlUnit*>(fu);

                // index of the instruction after delay slots
                int nextIndex = gcu == NULL ? 
                    i+1 : 
                    i+1+ gcu->delaySlots();
                
                // is index inside the procedure?
                if (nextIndex < procedure.instructionCount()) {
                    TTAProgram::Instruction& nextIns = 
                        procedure.instructionAtIndex(nextIndex);
                    // if jump to next ins, remove jump
                    if (&targetIns == &nextIns) {
                        ins.removeMove(move);
                        delete &move;
                    }
                }
            }
        }
    }
}

void 
ProcedurePass::executeControlFlowGraphPass(
        TTAProgram::Procedure& procedure,
        const TTAMachine::Machine& targetMachine,
        ControlFlowGraphPass& cfgp) throw (Exception) {

    ControlFlowGraph cfg(procedure);
    cfgp.handleControlFlowGraph(cfg, targetMachine);
    copyCfgToProcedure(procedure, cfg);
}
