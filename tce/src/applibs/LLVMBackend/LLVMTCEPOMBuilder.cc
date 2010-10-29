/*
    Copyright (c) 2002-2010 Tampere University of Technology.

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
 * @author Pekka Jääskeläinen 2010
 * @note reting: red
 */
#include "LLVMTCEPOMBuilder.hh"

#include <iostream>
#include <string>

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

namespace llvm {

char LLVMTCEPOMBuilder::ID = 0;

LLVMTCEPOMBuilder::LLVMTCEPOMBuilder() : LLVMPOMBuilder(ID) {
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

    TCEString regName(
        targetMachine().getRegisterInfo()->getName(llvmRegNum));
    return "RF";
}

int
LLVMTCEPOMBuilder::registerIndex(unsigned llvmRegNum) const {

    if (llvmRegNum == 1000000) 
        return 4; /* temp hack, always assume SP is the RF.4 */

    TCEString regName(
        targetMachine().getRegisterInfo()->getName(llvmRegNum));
    
    TCEString indexStr = regName.split("_").at(1);

    return Conversion::toInt(indexStr);
}

TTAProgram::Instruction*
LLVMTCEPOMBuilder::emitMove(
    const MachineInstr* mi, TTAProgram::Procedure* proc) {

    TCEString opName(mi->getDesc().getName());
    /* Non-trigger move. */
    if (opName == "MOVE")
        return LLVMPOMBuilder::emitMove(mi, proc);

    /* A trigger move. */

    // strip the 'r' or 'i' from the end of the operation name
    // e.g. ADDr
    opName = opName.substr(0, opName.size() - 1);

    TTAProgram::Terminal* src = 
        createTerminal(mi->getOperand(mi->getNumOperands() - 1));

    // always assume it's the ALU of minimal.adf for now
    // should be parsed from the regName
    TTAMachine::FunctionUnit* fu = mach_->functionUnitNavigator().item("ALU");

    // always assume the operation is ADD for now
    // should be parsed from the regName
    TCEString operationName = "ADD";

    int operand = 1; /* always assume trigger operand = 1 */

    TTAProgram::Terminal* dst = new TTAProgram::TerminalFUPort(
        *fu->operation(operationName), operand);

    TTAMachine::Bus& bus = builtProgram().universalMachine().universalBus();
    TTAProgram::Move* move = createMove(src, dst, bus);

    TTAProgram::Instruction* instr = new TTAProgram::Instruction();

    instr->addMove(move);
    proc->add(instr);
    return instr;
}


TCEString
LLVMTCEPOMBuilder::operationName(const MachineInstr& mi) const {
    return "MOVE";
}

TTAProgram::Terminal*
LLVMTCEPOMBuilder::createFUTerminal(const MachineOperand& mo) const {
    TCEString regName(
        targetMachine().getRegisterInfo()->getName(mo.getReg()));

    // todo fix:
    if (!regName.startsWith("FU"))
        return NULL;

    // always assume it's the ALU of minimal.adf for now
    // should be parsed from the regName
    TTAMachine::FunctionUnit* fu = mach_->functionUnitNavigator().item("ALU");

    // always assume the operation is ADD for now
    // should be parsed from the regName
    TCEString operationName = "ADD";

    int operand;
    if (regName.endsWith("i")) {
        // always assume the "input operand" is the operand 2 as in minimal.adf
        operand = 2;
    } else if (regName.endsWith("o")) {
        operand = 3;
    } else {
        operand = 1;
    }

    return new TTAProgram::TerminalFUPort(
        *fu->operation(operationName), operand);
}

extern "C" MachineFunctionPass* createLLVMTCEPOMBuilderPass() {
    return new llvm::LLVMTCEPOMBuilder();
}

bool
LLVMTCEPOMBuilder::doInitialization(Module &M) {
    mach_ = TTAMachine::Machine::loadFromADF("tta/4bus_minimal.adf");
    return LLVMPOMBuilder::doInitialization(M);
}

void
LLVMTCEPOMBuilder::assignBuses() {
    TTAProgram::Program& prog = builtProgram();
    TTAProgram::Program::InstructionVector ivec =
        prog.instructionVector();
    for (TTAProgram::Program::InstructionVector::const_iterator i = 
             ivec.begin(); i != ivec.end(); ++i) {
        TTAProgram::Instruction& instr = **i;
        for (int m = 0; m < instr.moveCount(); ++m) {
            TTAProgram::Move& move = instr.move(m);
            move.setBus(*mach_->busNavigator().item(0));
        }
    }
}

void
LLVMTCEPOMBuilder::addNOPs() {
    TTAProgram::Program& prog = builtProgram();
    TTAProgram::Program::InstructionVector ivec =
        prog.instructionVector();
    for (TTAProgram::Program::InstructionVector::const_iterator i = 
             ivec.begin(); i != ivec.end(); ++i) {
        TTAProgram::Instruction& instr = **i;
        for (int m = 0; m < instr.moveCount(); ++m) {
            TTAProgram::Move& move = instr.move(m);
            if (move.isTriggering()) {
                /* TODO: branch delay slots! */
                TTAProgram::TerminalFUPort& fuPort =
                    dynamic_cast<TTAProgram::TerminalFUPort&>(
                        move.destination());
                TTAMachine::HWOperation& operation = *fuPort.hwOperation();
                int nopsToAdd;
                if (move.isControlFlowMove()) {
                    nopsToAdd = mach_->controlUnit()->delaySlots() + 1;
                } else {
                    nopsToAdd = operation.latency();
                }
                
                while (--nopsToAdd > 0) {
                    instr.parent().insertAfter(
                        instr, 
                        new TTAProgram::Instruction(
                            *mach_->instructionTemplateNavigator().item(0)));
                }
            }
        }
    }
}

/**
 * Assigns the control unit to control flow operation targets.
 */
void
LLVMTCEPOMBuilder::assignControlUnit() {
    TTAProgram::Program& prog = builtProgram();
    TTAProgram::Program::InstructionVector ivec =
        prog.instructionVector();
    for (TTAProgram::Program::InstructionVector::const_iterator i = 
             ivec.begin(); i != ivec.end(); ++i) {
        TTAProgram::Instruction& instr = **i;
        for (int m = 0; m < instr.moveCount(); ++m) {
            TTAProgram::Move& move = instr.move(m);
            if (move.isControlFlowMove()) {
                TTAProgram::TerminalFUPort& fuPort =
                    dynamic_cast<TTAProgram::TerminalFUPort&>(
                        move.destination());
                TTAMachine::HWOperation& operation = *fuPort.hwOperation();

                move.setDestination(
                    new TTAProgram::TerminalFUPort(
                        *mach_->controlUnit()->operation(operation.name()),
                        fuPort.operationIndex()));

                if (move.source().isRA()) {
                    move.setSource(
                        new TTAProgram::TerminalFUPort(
                            *mach_->controlUnit()->returnAddressPort()));
                }
            }
        }
    }
}

bool
LLVMTCEPOMBuilder::doFinalization(Module& m) {
    LLVMPOMBuilder::doFinalization(m);

    assignBuses();
    assignControlUnit();
    addNOPs();

    std::cout << POMDisassembler::disassemble(builtProgram()) << std::endl;
    TTAProgram::Program::writeToTPEF(builtProgram(), "sequential.tpef");
}


}
