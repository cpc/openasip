/*
    Copyright (c) 2002-2014 Tampere University.

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

#include "CallsToJumps.hh"
#include "ControlFlowGraph.hh"
#include "Program.hh"
#include "Procedure.hh"
#include "BasicBlock.hh"
#include "Instruction.hh"
#include "Move.hh"
#include "InstructionReference.hh"
#include "InstructionReferenceManager.hh"
#include "TerminalInstructionReference.hh"
#include "TerminalSymbolReference.hh"
#include "TerminalFUPort.hh"
#include "Terminal.hh"
#include "CodeGenerator.hh"
#include "UniversalMachine.hh"
#include "MoveGuard.hh"
#include "ControlUnit.hh"
#include "SpecialRegisterPort.hh"

using namespace TTAProgram;

//#define DEBUG_CALLS_TO_JUMPS

void
CallsToJumps::handleControlFlowGraph(
    ControlFlowGraph& cfg, const TTAMachine::Machine& targetMachine) {
    InstructionReferenceManager& irm = 
        cfg.instructionReferenceManager();
    int nodeCount = cfg.nodeCount();
    for (int bbIndex = 0; bbIndex < nodeCount; ++bbIndex) {
        BasicBlockNode& bbn = dynamic_cast<BasicBlockNode&>(cfg.node(bbIndex));
        if (!bbn.isNormalBB())
            continue;

        assert(!bbn.isScheduled());

        BasicBlock& bb = bbn.basicBlock();

        // scan through the instructions, looking for CALLs
        // replace each call with
        // 1) a move to RA with an instruction reference to the instruction
        // following the call (the next BB), and
        // 2) a JUMP to the original CALL destination
        // 3) some kind of annotation that marks that this JUMP should
        //    be treated like a function call, e.g., in the DDG builder
        for (int i = 0, count = bb.instructionCount(); i < count; ++i) {
            Instruction& instr = bb.instructionAtIndex(i);
            if (instr.hasCall()) {

                // If unscheduled it should have exactly one move, the call.
                assert(instr.moveCount() == 1);

                // In the TCE's CFG, a call splits the basic block, so this
                // should be the last instruction as there are no delay slots
                // in the intermediate representation.
                assert(i + 1 == count);


                Move& originalCall = instr.move(0);

                BasicBlockNode* nextBB = cfg.fallThruSuccessor(bbn);
                assert(nextBB != NULL);

#ifdef DEBUG_CALLS_TO_JUMPS
                Application::logStream()
                    << "### found call: " << originalCall.toString()
                    << std::endl;
                Application::logStream()
                    << "### BB: " << bb.toString() << std::endl;
                Application::logStream()
                    << "### FallThroughBB: " << nextBB->basicBlock().toString() 
                    << std::endl;
#endif

                // E.g. call to exit does not return and there is no sensible
                // place to return. Just do not add the RA write in that case.
                Instruction* returnLocation = 
                    (nextBB->basicBlock().instructionCount() > 0) ? 
                    &nextBB->basicBlock().firstInstruction() : NULL;

                CodeGenerator codeGen(targetMachine);

                UniversalMachine& uMach = UniversalMachine::instance();

                std::shared_ptr<Move> returnAddressMove = NULL;
                std::shared_ptr<Move> jumpToFunction;

                if (originalCall.isUnconditional()) {

                    if (returnLocation != NULL) {
                        InstructionReference returnRef = 
                            irm.createReference(*returnLocation);
                
                        returnAddressMove = std::make_shared<Move>(
                            new TerminalInstructionReference(returnRef),
                            new TTAProgram::TerminalFUPort(
                                *uMach.controlUnit()->returnAddressPort()),
                            uMach.universalBus());
                    }

                    jumpToFunction = std::make_shared<Move>(
                        originalCall.source().copy(), 
                        codeGen.createTerminalFUPort("jump", 1),
                        uMach.universalBus());
                } else {

                    if (returnLocation != NULL) {
                        InstructionReference returnRef = 
                            irm.createReference(*returnLocation);

                        returnAddressMove = std::make_shared<Move>(
                            new TerminalInstructionReference(returnRef),
                            new TTAProgram::TerminalFUPort(
                                *uMach.controlUnit()->returnAddressPort()),
                            uMach.universalBus(),
                            originalCall.guard().copy());
                    }

                    jumpToFunction = std::make_shared<Move>(
                        originalCall.source().copy(), 
                        codeGen.createTerminalFUPort("jump", 1),
                        uMach.universalBus(),
                        originalCall.guard().copy());
                }

                jumpToFunction->addAnnotation(
                    ProgramAnnotation(ProgramAnnotation::ANN_JUMP_FUNCTION_CALL));

                if (returnAddressMove != NULL) {
                    Instruction* raMoveInstr =
                        new Instruction(TTAMachine::NullInstructionTemplate::instance());
                    raMoveInstr->addMove(returnAddressMove);
                    bb.insertBefore(instr, raMoveInstr);
                    
                    if (irm.hasReference(instr))
                        irm.replace(instr, *raMoveInstr);
                }

                Instruction* jumpInstr =
                    new Instruction(TTAMachine::NullInstructionTemplate::instance());
                jumpInstr->addMove(jumpToFunction);

                bb.insertBefore(instr, jumpInstr);

                if (returnAddressMove == NULL && irm.hasReference(instr))
                    irm.replace(instr, *jumpInstr);

                // Keep the instruction references up to date as
                // the old call can be a jump target (so should be the
                // new RA-move) if it starts a basic block.

#ifdef DEBUG_CALLS_TO_JUMPS
                Application::logStream()
                    << "### removing " << instr.toString() << std::endl;
#endif
                assert(!irm.hasReference(instr));
                bb.remove(instr);
            }
        }
    }
}
