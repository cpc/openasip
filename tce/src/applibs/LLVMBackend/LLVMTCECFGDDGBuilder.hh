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
 * @file LLVMTCECFGDDGBuilder.hh
 *
 * This builder builds a CFG and DDG from the new LLVM TTA backend format.
 *
 * @author Heikki Kultala 2011
 * @note reting: red
 */

#ifndef LLVM_TCE_CFG_DDG_BUILDER_H
#define LLVM_TCE_CFG_DDG_BUILDER_H

#include <llvm/CodeGen/MachineFunctionPass.h>

#include "LLVMTCEBuilder.hh"
#include "DataDependenceGraphBuilder.hh"

class InterPassData;
namespace TTAMachine {
    class Machine;
}

namespace llvm {

//    extern "C" MachineFunctionPass* createLLVMTCECFGDDGBuilderPass();
    FunctionPass* createLLVMTCECFGBuilderPass(
        TCETargetMachine& tm, TTAMachine::Machine* mach, InterPassData& ipd);

    class LLVMTCECFGDDGBuilder : public LLVMTCEBuilder {
    public:
        static char ID;

        LLVMTCECFGDDGBuilder(
            const TargetMachine& tm, TTAMachine::Machine* mach, 
            InterPassData& ipd, bool functionAtATime=false);

        bool writeMachineFunction(MachineFunction& mf);

        virtual ~LLVMTCECFGDDGBuilder() {}

        virtual bool doFinalization(Module& m );

        virtual void emitSPInitialization() {}

    protected:

        // TODO: copy-paste from LLVMPomBuilder.hh
        virtual unsigned spDRegNum() const { 
            return dynamic_cast<const TCETargetMachine&>(
                targetMachine()).spDRegNum(); 
        }

        // the return address register's llvm reg number
        virtual unsigned raPortDRegNum() const { 
            return dynamic_cast<const TCETargetMachine&>(
                targetMachine()).raPortDRegNum(); 
        }

        // the ADF register file name of the llvm reg number
        virtual TCEString registerFileName(unsigned llvmRegNum) const { 
            return dynamic_cast<const TCETargetMachine&>(
                targetMachine()).rfName(llvmRegNum); 
        }

        // the ADF register index of the llvm reg number
        virtual int registerIndex(unsigned llvmRegNum) const {
            return dynamic_cast<const TCETargetMachine&>(
                targetMachine()).registerIndex(llvmRegNum); 
        }

        // operation name from a LLVM MachineInstr or OSAL Operation
        virtual TCEString operationName(const MachineInstr& mi) const; 

        virtual TTAProgram::Terminal* createFUTerminal(
            const MachineOperand&) const {
            // no FU terminals in the RISC-style backend, always through GPRs
            return NULL; 
        }

        virtual TTAProgram::Terminal* createMBBReference(
            const MachineOperand& mo);

        virtual TTAProgram::Terminal* createSymbolReference(
            const TCEString& symbolName);

    private:

        bool isRealInstruction(const MachineInstr& instr);
        bool hasRealInstructions(
            MachineBasicBlock::const_iterator i, 
            const MachineBasicBlock& mbb);

        InterPassData* ipData_;
        // TODO: how to get these?
        std::set<TCEString> allParamRegs_;

        // todo: information about sp and param regs, how to give?
        DataDependenceGraphBuilder ddgBuilder_;

        std::map<const MachineBasicBlock*,BasicBlockNode*> bbMapping_;

        // set to true in case the builder is used to schedule one
        // function at a time (the default processes the whole module)
        bool functionAtATime_;

        std::map<const MachineBasicBlock*, BasicBlockNode*> skippedBBs_;
    };
}

#endif
