/*
    Copyright (c) 2002-2015 Tampere University.

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
 * @file LLVMTCEIRBuilder.hh
 *
 * This builder builds a CFG and DDG from the new LLVM TTA backend format.
 *
 * @author Heikki Kultala 2011
 * @author Pekka Jääskeläinen 2011,2015
 * @note reting: red
 */

#ifndef LLVM_TCE_IR_BUILDER_H
#define LLVM_TCE_IR_BUILDER_H

#include <llvm/CodeGen/MachineFunctionPass.h>
#include <llvm/Analysis/AliasAnalysis.h>

#include "LLVMTCEBuilder.hh"
#include "DataDependenceGraphBuilder.hh"
#include "CopyingDelaySlotFiller.hh"

class InterPassData;
class LLVMTCECmdLineOptions;

namespace TTAMachine {
    class Machine;
}

namespace llvm {

    FunctionPass* createLLVMTCECFGBuilderPass(
        TCETargetMachine& tm, TTAMachine::Machine* mach, InterPassData& ipd);

    class LLVMTCEIRBuilder : public LLVMTCEBuilder {
    public:
        static char ID;

        LLVMTCEIRBuilder(
            const TargetMachine& tm, TTAMachine::Machine* mach, 
            InterPassData& ipd, AliasAnalysis* AA,
            bool functionAtATime=false, bool modifyMF=false);

        bool writeMachineFunction(MachineFunction& mf);

        virtual ~LLVMTCEIRBuilder();

        virtual bool doInitialization(Module& m );
        virtual bool doFinalization(Module& m );

        virtual void emitSPInitialization() {}

#if LLVM_OLDER_THAN_4_0
    virtual const char *getPassName() const {
#else
    virtual StringRef getPassName() const {
#endif
            return "TCE: build TCE scheduler IR from MachineFunctions";
        }

    protected:

        // TODO: copy-paste from LLVMPomBuilder.hh
        virtual unsigned spDRegNum() const { 
            return dynamic_cast<const TCETargetMachine&>(
                targetMachine()).spDRegNum(); 
        }

        // the return address register's llvm reg number
        virtual unsigned raPortDRegNum() const { 
            if (isTTATarget()) {
                return dynamic_cast<const TCETargetMachine&>(
                    targetMachine()).raPortDRegNum(); 
            } else {
                abortWithError("Unimplemented for non-TTA targets.");
            }
        }

        // the ADF register file name of the llvm reg number
        virtual TCEString registerFileName(unsigned llvmRegNum) const;

        // the ADF register index of the llvm reg number
        virtual int registerIndex(unsigned llvmRegNum) const;

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

        virtual void createMoveNode(
            ProgramOperationPtr& po,
            std::shared_ptr<TTAProgram::Move> m,
            bool isDestination) override;

    private:

        bool isRealInstruction(const MachineInstr& instr);
        bool hasRealInstructions(
            MachineBasicBlock::const_iterator i, 
            const MachineBasicBlock& mbb);

        void convertProcedureToMachineFunction(
            const TTAProgram::Procedure& proc,
            llvm::MachineFunction& mf);

        void compileFast(ControlFlowGraph& cfg);
        void compileOptimized(
            ControlFlowGraph& cfg, 
            llvm::AliasAnalysis* llvmAA);

        CopyingDelaySlotFiller& delaySlotFiller();

        ControlFlowGraph* buildTCECFG(llvm::MachineFunction& mf);
        void markJumpTableDestinations(
            llvm::MachineFunction& mf, 
            ControlFlowGraph& cfg);
        void fixJumpTableDestinations(
            llvm::MachineFunction& mf, 
            ControlFlowGraph& cfg);
        
        InterPassData* ipData_;
        // TODO: how to get these?
        std::set<TCEString> allParamRegs_;

        // todo: information about sp and param regs, how to give?
        DataDependenceGraphBuilder ddgBuilder_;

        std::map<const MachineBasicBlock*,BasicBlockNode*> bbMapping_;

        std::vector<std::vector<BasicBlockNode*> > jumpTableRecord_;
        
        // write back the scheduled instructions to the machine function?
        bool modifyMF_;

        AliasAnalysis* AA_;
        std::map<const MachineBasicBlock*, BasicBlockNode*> skippedBBs_;

        LLVMTCECmdLineOptions* options_;

        CopyingDelaySlotFiller* dsf_;
        bool delaySlotFilling_;
    };
}

#endif
