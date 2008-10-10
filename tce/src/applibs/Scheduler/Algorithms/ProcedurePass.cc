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
    
    // just to avoid warnings -- need to keep the argument names for
    // Doxygen comments ;)
    procedure.instructionCount();
    targetMachine.machineTester();
    abortWithError("Should never call this.");
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
