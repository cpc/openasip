/*
    Copyright (c) 2002-2011 Tampere University.

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
 * @file LLVMTCEPOMBuilder.cc
 *
 * Implementation of LLVMTCEPOMBuilder class.
 *
 * @author Pekka J‰‰skel‰inen 2010-2012
 * @note rating: red
 */
#include "LLVMTCEPOMBuilder.hh"

#include <iostream>
#include <string>

#include "llvm/Support/CommandLine.h"

#include "POMDisassembler.hh"
#include "UniversalMachine.hh"
#include "TerminalFUPort.hh"
#include "Machine.hh"
#include "HWOperation.hh"
#include "Program.hh"
#include "Procedure.hh"
#include "Instruction.hh"
#include "TCEString.hh"
#include "Conversion.hh"
#include "ControlUnit.hh"
#include "SpecialRegisterPort.hh"
#include "SequentialScheduler.hh"
#include "InterPassData.hh"
#include "PreBypassBasicBlockScheduler.hh"
#include "LLVMTCEDataDependenceGraphBuilder.hh"
#include "FUPort.hh"

namespace llvm {

static cl::opt<bool>
ParallelizeMoves(
    "parallelize-moves",
    cl::desc("Parallelize the TTA moves as efficiently as possible."),
    cl::init(false), cl::Hidden);

char LLVMTCEPOMBuilder::ID = 0;

LLVMTCEPOMBuilder::LLVMTCEPOMBuilder() : 
    LLVMTCEBuilder(ID) {
}

unsigned
LLVMTCEPOMBuilder::spDRegNum() const {
    return 1000000; // ;)
}

unsigned
LLVMTCEPOMBuilder::raPortDRegNum() const {
    return 1000001; // ;D
}

TCEString
LLVMTCEPOMBuilder::registerFileName(unsigned llvmRegNum) const {

    if (llvmRegNum == 1000000) 
        return "RF"; /* temp hack, always assume SP is the RF.4 */
    abortWithError("Unimplemented.");
}

int
LLVMTCEPOMBuilder::registerIndex(unsigned llvmRegNum) const {

    if (llvmRegNum == 1000000) 
        return 4; /* temp hack, always assume SP is the RF.4 */
    abortWithError("Unimplemented.");
}

TTAProgram::Instruction*
LLVMTCEPOMBuilder::emitMove(
    const MachineInstr* mi, TTAProgram::CodeSnippet* proc,
    bool, bool) {
#if LLVM_OLDER_THAN_6_0
    TCEString opName(targetMachine().getSubtargetImpl(
                         *mi->getParent()->getParent()->getFunction())->
                     getInstrInfo()->getName(mi->getOpcode()));
#else
    TCEString opName(targetMachine().getSubtargetImpl(
                         mi->getParent()->getParent()->getFunction())->
                     getInstrInfo()->getName(mi->getOpcode()));

#endif
    /* Non-trigger move. */
    if (opName == "MOVE")
        return LLVMTCEBuilder::emitMove(mi, proc);

    /* A trigger move. The source is the 2nd last argument. */
    TTAProgram::Terminal* src = 
        createTerminal(mi->getOperand(mi->getNumOperands() - 2));
    TTAProgram::Terminal* dst = /* defined as implicit def */
        createTerminal(mi->getOperand(mi->getNumOperands() - 1));

    TTAMachine::Bus& bus = result()->universalMachine().universalBus();
    auto move = createMove(src, dst, bus);

    TTAProgram::Instruction* instr = new TTAProgram::Instruction();

    instr->addMove(move);
    proc->add(instr);
    return instr;
}


TCEString
LLVMTCEPOMBuilder::operationName(const MachineInstr&) const {
    return "MOVE";
}

TTAProgram::Terminal*
LLVMTCEPOMBuilder::createFUTerminal(const MachineOperand& mo) const {
#if LLVM_OLDER_THAN_6_0
    TCEString regName(
        targetMachine().getSubtargetImpl(
            *mo.getParent()->getParent()->getParent()->getFunction())->
        getRegisterInfo()->getName(mo.getReg()));
#else
    TCEString regName(
        targetMachine().getSubtargetImpl(
            mo.getParent()->getParent()->getParent()->getFunction())->
        getRegisterInfo()->getName(mo.getReg()));
#endif
    
    // test for _number which indicates a RF access
    std::vector<TCEString> pieces = regName.split("_");
    if (pieces.size() == 2) {
        TCEString indexStr = pieces.at(1);
        try {
            Conversion::toInt(indexStr);
            return NULL;
        } catch (Exception&) {
        }
    } 

    assert(pieces.size() > 0);

    TCEString fuName = pieces.at(0);
    TCEString portName = pieces.at(1);
    TCEString operationName = "";
    if (pieces.size() == 3) {
        // FU_triggerport_OP
        operationName = pieces.at(2);
    }

    // always assume it's the ALU of minimal.adf for now
    // should be parsed from the regName
    TTAMachine::FunctionUnit* fu = 
        mach_->functionUnitNavigator().item(fuName);

    assert(fu != NULL);
    
    TTAMachine::FUPort* fuPort = 
        dynamic_cast<TTAMachine::FUPort*>(fu->port(portName));
    assert(fuPort != NULL);
    if (operationName != "") {
        assert(fuPort->isTriggering());
        TTAMachine::HWOperation& hwOp = *fu->operation(operationName);
        return new TTAProgram::TerminalFUPort(hwOp, hwOp.io(*fuPort));
    } else {
        TTAProgram::TerminalFUPort* term = 
            new TTAProgram::TerminalFUPort(*fuPort);
        return term;
    }
}

extern "C" MachineFunctionPass* createLLVMTCEPOMBuilderPass() {
    return new llvm::LLVMTCEPOMBuilder();
}

bool
LLVMTCEPOMBuilder::doInitialization(Module &M) {
    mach_ = TTAMachine::Machine::loadFromADF("tta/4bus_minimal.adf");
    return LLVMTCEBuilder::doInitialization(M);
}

bool
LLVMTCEPOMBuilder::doFinalization(Module& m) {

    LLVMTCEBuilder::doFinalization(m);

    TTAProgram::Program& prog = *result();
    /* update the operation and operand info of non-trigger moves */
    for (int procIndex = 0; procIndex < prog.procedureCount(); ++procIndex) {

        std::map<std::string, const TTAMachine::HWOperation*> 
            lastTriggeredOperations;
        std::map<std::string, std::set<TTAProgram::TerminalFUPort*> > 
            pendingOperandTerminals;
        const TTAProgram::Procedure& proc = prog.procedureAtIndex(procIndex);
        for (int instrIndex = 0; instrIndex < proc.instructionCount(); 
             ++instrIndex) {
            TTAProgram::Instruction& instr = 
                proc.instructionAtIndex(instrIndex);     
            
            TTAProgram::TerminalFUPort* termFUportSrc = 
                dynamic_cast<TTAProgram::TerminalFUPort*>(
                    &instr.move(0).source());
            TTAProgram::TerminalFUPort* termFUportDst = 
                dynamic_cast<TTAProgram::TerminalFUPort*>(
                    &instr.move(0).destination());

            if (termFUportDst != NULL) {
                if (termFUportDst->isTriggering()) {
                    lastTriggeredOperations[
                        termFUportDst->functionUnit().name()] =
                        termFUportDst->hwOperation();

                    std::set<TTAProgram::TerminalFUPort*>& pending =
                        pendingOperandTerminals[
                            termFUportDst->functionUnit().name()];
                    std::set<TTAProgram::TerminalFUPort*>::const_iterator i = 
                        pending.begin();
                    const TTAMachine::HWOperation& hwOp = 
                        *termFUportDst->hwOperation();
                    for (; i != pending.end(); ++i) {
                        TTAProgram::TerminalFUPort& term = (**i);
                        term.setOperation(hwOp);
                        term.setOperationIndex(
                            hwOp.io(
                                dynamic_cast<const TTAMachine::FUPort&>(
                                    term.port())));
                    }
                    pendingOperandTerminals.clear();
                } else { 
                    // operand move 
                    pendingOperandTerminals[
                        termFUportDst->functionUnit().name()].insert(
                            termFUportDst);
                }
            }

            if (termFUportSrc != NULL && !termFUportSrc->isRA()) {
                // fix the output moves, assume the operation is the
                // previously triggered one in the FU
                const TTAMachine::HWOperation& hwOp = 
                    *lastTriggeredOperations[
                        termFUportSrc->functionUnit().name()];
                termFUportSrc->setOperation(hwOp);
                termFUportSrc->setOperationIndex(
                    hwOp.io(
                        dynamic_cast<const TTAMachine::FUPort&>(
                            termFUportSrc->port())));
            }
        }
    }    

    InterPassData ipData;
    if (ParallelizeMoves) {        
        LLVMTCEDataDependenceGraphBuilder ddgBuilder(ipData);
        PreBypassBasicBlockScheduler parScheduler(ipData, ddgBuilder);
        CATCH_ANY(parScheduler.handleProgram(prog, *mach_));
        TTAProgram::Program::writeToTPEF(prog, "parallel.tpef");
    } else {
        SequentialScheduler seqScheduler(ipData);
        seqScheduler.handleProgram(*result(), *mach_);
        TTAProgram::Program::writeToTPEF(prog, "sequential.tpef");
    }
    std::cout << POMDisassembler::disassemble(prog) << std::endl;
    return true;
}


}
