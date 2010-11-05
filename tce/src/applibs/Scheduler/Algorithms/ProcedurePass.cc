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
#include "ControlFlowGraphPass.hh"
#include "InstructionReference.hh"
#include "InstructionReferenceManager.hh"

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

    
    // collect the starting point instructions to keep as place holders
    std::map<const BasicBlockNode*, TTAProgram::Instruction*> placeHolders;
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
     
        TTAProgram::Instruction& placeHolder = *placeHolders[&bb];

        // Remove all original instructions (after the place holder first
        // instruction) from the procedure as we are dealing with basic blocks,
        // we can assume that only at most the first instruction is referred to.
        const int originalBBSize = 
            bb.originalEndAddress() - bb.originalStartAddress() + 1;
        int addr = placeHolder.address().location() +1;
        for (int i = 1; i < originalBBSize; ++i) {
            procedure.deleteInstructionAt(addr);
        }

        TTAProgram::Instruction* firstNewInstruction = NULL;
        TTAProgram::Instruction* lastNewInstruction = &placeHolder;
        TTAProgram::InstructionReferenceManager& irm = 
            procedure.parent().instructionReferenceManager();

        // Copy the instructions from the scheduled basic block back 
        // to the program.
        for (int i = bb.basicBlock().skippedFirstInstructions(); 
             i < bb.basicBlock().instructionCount(); ++i) {
            TTAProgram::Instruction& instrToCopy =
                bb.basicBlock().instructionAtIndex(i);
            TTAProgram::Instruction* newInstruction = instrToCopy.copy();
            procedure.insertAfter(*lastNewInstruction, newInstruction);
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
            procedure.parent().instructionReferenceManager().replace(
                placeHolder, *firstNewInstruction);
        } 
        // now we can delete also the place holder old instruction
        // @todo what if the instruction itself had a reference?
        procedure.deleteInstructionAt(placeHolder.address().location());

        // ...and we're done with this basic block
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
