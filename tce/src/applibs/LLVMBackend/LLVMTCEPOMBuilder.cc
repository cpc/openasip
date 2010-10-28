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

#include "Machine.hh"
#include "TCEString.hh"

namespace llvm {

char LLVMTCEPOMBuilder::ID = 0;

LLVMTCEPOMBuilder::LLVMTCEPOMBuilder() : LLVMPOMBuilder(ID) {
    std::cerr << "LLVMTCEPOMBuilder constructed" << std::endl;
    mach_ = TTAMachine::Machine::loadFromADF("tta/4bus_minimal.adf");
}


#if 0    
bool
LLVMTCEPOMBuilder::runOnMachineFunction(MachineFunction& mf) {

    for (llvm::MachineFunction::const_iterator bbi = mf.begin(); 
         bbi != mf.end(); ++bbi) {
        const llvm::MachineBasicBlock& bb = *bbi;
        for (llvm::MachineBasicBlock::const_iterator ii = bb.begin(); 
             ii != bb.end(); ++ii) {
            const llvm::MachineInstr& i = *ii;

            if (TCEString(i.getDesc().getName()) == "MOVE") {
                std::cerr << "got MOVE: ";
                for (unsigned oi = 0; oi < i.getNumOperands(); ++oi) {
                    const llvm::MachineOperand& operand = i.getOperand(oi);
                    if (operand.isReg()) {
                        TCEString regName(
                            mf.getTarget().getRegisterInfo()->getName(
                                operand.getReg()));
                        std::cerr << regName << " ";
                    } else if (operand.isImm()) {
                        std::cerr << operand.getImm() << " ";
                    } else {
                        std::cerr << __func__ << "unsupported operand type: ";
                        //operand.dump();
                        std::cerr << std::endl;
                        assert(false);
                    }
                }
                std::cerr << std::endl;
            } else {
                std::cerr 
                    << "unknown opcode: " << i.getDesc().getName() << std::endl;
            }

        }
    }
    return true;
}
#endif

extern "C" MachineFunctionPass* createLLVMTCEPOMBuilderPass() {
    return new llvm::LLVMTCEPOMBuilder();
}

}
