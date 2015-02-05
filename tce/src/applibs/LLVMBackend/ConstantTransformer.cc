/*
    Copyright (c) 2002-2015 Tampere University of Technology.

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
 
#include "ConstantTransformer.hh"

#include "MachineInfo.hh"
#include "TCETargetMachine.hh"

#include <llvm/CodeGen/MachineFunction.h>
#include <llvm/CodeGen/MachineBasicBlock.h>
#include <llvm/MC/MCInstrInfo.h>
#include <llvm/Target/TargetInstrInfo.h>
#include <llvm/CodeGen/MachineInstrBuilder.h>

using llvm::MachineBasicBlock;
using llvm::MachineFunction;
using llvm::MachineOperand;
using llvm::TCETargetMachine;
using llvm::TCETargetMachinePlugin;

char ConstantTransformer::ID; 

bool
ConstantTransformer::doInitialization(llvm::Module&) {
    return false;
}

bool
ConstantTransformer::runOnMachineFunction(llvm::MachineFunction& mf) {

    bool changed = false;
    for (MachineFunction::iterator i = mf.begin(); i != mf.end(); i++) {
        MachineBasicBlock& mbb = *i;
        for (MachineBasicBlock::iterator j = mbb.begin();
             j != mbb.end(); j++) {
            const llvm::MachineInstr* mi = j;
            for (unsigned operandI = 0; operandI < mi->getNumOperands();
                 ++operandI) {
                const MachineOperand& mo = mi->getOperand(operandI);
                if (mo.isImm() && 
                    !MachineInfo::canEncodeImmediateInteger(mach_, mo.getImm())) {
                    
                    // check if it's possible to convert C to SUB(0, -C) such
                    // that it can be encoded for the target
                    if (MachineInfo::canEncodeImmediateInteger(mach_, 0) ||
                        MachineInfo::canEncodeImmediateInteger(mach_, -mo.getImm()) ||
                        MachineInfo::supportsOperation(mach_, "SUB")) {

#if (defined(LLVM_3_2) || defined(LLVM_3_3) || defined(LLVM_3_4) || defined(LLVM_3_5))
                        const llvm::MCInstrInfo* iinfo = mf.getTarget().getInstrInfo();
#else
                        const llvm::MCInstrInfo* iinfo = mf.getTarget().getSubtargetImpl()->getInstrInfo();
#endif
                        const TCETargetMachine& tm = 
                            dynamic_cast<const TCETargetMachine&>(mf.getTarget());
                        TCETargetMachinePlugin& plugin = tm.targetPlugin();
#if 0
                        Application::logStream() 
                            << "ConstantTransformer: converting constant in ";
                        j->dump();
                            
#endif
                        // RV_HIGH = SUB 0 -X
                        BuildMI(
                            mbb, j, j->getDebugLoc(), iinfo->get(plugin.opcode("SUB")), 
                            plugin.rvHighDRegNum()).addImm(0).addImm(-mo.getImm());
                        
                        // replace the original instruction's immediate operand
                        // with the RV_HIGH
                        llvm::MachineInstrBuilder mib = 
                            BuildMI(mbb, j, j->getDebugLoc(), j->getDesc());
                        for (unsigned opr = 0; opr < j->getNumOperands(); ++opr) {
                            MachineOperand& orig = j->getOperand(opr);
                            if (opr == operandI) {
                                mib.addOperand(
                                    MachineOperand::CreateReg(plugin.rvHighDRegNum(), false));
                                continue;
                            }
                            mib.addOperand(orig);
                            orig.clearParent();
                        }
                        
                        // the original instruction can be now deleted
                        j->eraseFromParent();
                        // start scanning the MBB from the beginning due to
                        // possibly invalidated iterators from adding the 
                        // new instructions
                        j = mbb.begin();
                        changed = true;
                    } else {
                        std::ostringstream errMsg;
                        errMsg << "Program uses constant '"
                               << mo.getImm() << "' that cannot be encoded "
                               << "for the machine by the current compiler.";
                        throw CompileError(
                            __FILE__, __LINE__, __func__, errMsg.str());
                    }
                }
            }
           
        }
    }
    return changed;
}

bool
ConstantTransformer::doFinalization(llvm::Module&) {
    return false;
}
