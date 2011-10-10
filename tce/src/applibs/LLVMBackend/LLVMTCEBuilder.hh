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
 * @file LLVMTCEBuilder.hh
 *
 * Declaration of LLVMTCEBuilder class.
 *
 * @author Pekka Jääskeläinen 2010 
 * @note reting: red
 */

#ifndef LLVM_TCE_BUILDER_H
#define LLVM_TCE_BUILDER_H

#include <map>
#include <set>
#include <llvm/CodeGen/MachineFunctionPass.h>
#include <llvm/CodeGen/MachineConstantPool.h>
#include <llvm/CodeGen/MachineOperand.h>
#include <llvm/CodeGen/MachineInstr.h>
#include <llvm/CodeGen/MachineBasicBlock.h>
#include <llvm/Target/Mangler.h>
#include <llvm/Constant.h>

#include <llvm/Transforms/IPO.h>

#include "Exception.hh"
#include "BaseType.hh"
#include "TCEString.hh"
#include "passes/MachineDCE.hh"
#include "TCETargetMachine.hh"
#include "UniversalMachine.hh"
#include "ProgramOperation.hh"
#include "TerminalProgramOperation.hh"

namespace TTAProgram {
    class Program;
    class Procedure;
    class CodeSnippet;
    class Terminal;
    class TerminalRegister;
    class TerminalInstructionAddress;
    class Instruction;
    class DataMemory;
    class DataDefinition;
    class Move;
    class MoveGuard;
}

class UniversalMachine;
class Operation;

namespace TTAMachine {
    class Machine;
    class AddressSpace;
    class Bus;
}
namespace llvm {

    class ConstantInt;
    class ConstantFP;
    class ConstantExpr;
    class TCETargetMachine;

    /**
     * Base class for different LLVM to TCE builder implementations.
     */
    class LLVMTCEBuilder : public MachineFunctionPass {

    public:
        static char ID; // Pass identification, replacement for typeid

        LLVMTCEBuilder(char& ID);

        LLVMTCEBuilder(
            const TargetMachine& tm,
            TTAMachine::Machine* mach,
            char& ID,
            bool functionAtATime=false);

        virtual ~LLVMTCEBuilder();

        TTAProgram::Program* result() 
            throw (NotAvailable);

        TTAProgram::Instruction* firstInstructionOfBasicBlock(
            llvm::BasicBlock* bb) {
            return bbIndex_[bb];
        }
        bool isProgramUsingRestrictedPointers() const { return noAliasFound_; }
        bool isProgramUsingAddressSpaces() const {
            return  multiAddrSpacesFound_;
        }

        void getAnalysisUsage(AnalysisUsage &AU) const {
            AU.addRequired<AliasAnalysis>();
            AU.addRequired<MachineDCE>();
            AU.addPreserved<MachineDCE>();
            MachineFunctionPass::getAnalysisUsage(AU);
        }

        virtual bool isTTATarget() const {
            return (dynamic_cast<const TCETargetMachine*>(
                        &targetMachine()) != NULL);
        }

    protected:

        bool doInitialization(Module &M);
        bool runOnMachineFunction(MachineFunction &MF);
        bool doFinalization(Module &M);

        virtual bool writeMachineFunction(MachineFunction &MF);
        void initDataSections();

        TTAProgram::Move* createMove(
            const MachineOperand& src, const MachineOperand& dst);

        const TargetMachine& targetMachine() const { return *tm_; }

        /* Methods used for overriding TTA backend-specific behavior. */
        // the stack pointer register's llvm reg number
        virtual unsigned spDRegNum() const = 0;
        // the return address register's llvm reg number
        virtual unsigned raPortDRegNum() const = 0;
        // the ADF register file name of the llvm reg number
        virtual TCEString registerFileName(unsigned llvmRegNum) const = 0;
        // the ADF register index of the llvm reg number
        virtual int registerIndex(unsigned llvmRegNum) const = 0;
        // OSAL operation name from a LLVM MachineInstr
        virtual TCEString operationName(const MachineInstr& mi) const = 0;
        virtual TTAProgram::Terminal* createFUTerminal(
            const MachineOperand&) const {
            return NULL; 
        }

        /* Helper methods */
        std::string mbbName(const MachineBasicBlock& mbb);

        TTAProgram::Terminal* createTerminal(const MachineOperand& mo);

        TTAProgram::Move* createMove(
            TTAProgram::Terminal* src,
            TTAProgram::Terminal* dst,
            TTAMachine::Bus &bus,
            TTAProgram::MoveGuard *guard = NULL);

        void emitConstantPool(const llvm::MachineConstantPool& cp);

        virtual TTAProgram::Terminal* createMBBReference(
            const MachineOperand& mo);

        virtual TTAProgram::Terminal* createSymbolReference(
            const MachineOperand& mo);

        virtual TTAProgram::Terminal* createProgramOperationReference(
            const MachineOperand& mo);

        virtual TTAProgram::Terminal* createSymbolReference(
            const TCEString& symbolName);

        TTAProgram::Instruction* emitInstruction(
            const MachineInstr* mi, TTAProgram::CodeSnippet* proc);

        virtual TTAProgram::Instruction* emitMove(
            const MachineInstr* mi, TTAProgram::CodeSnippet* proc);

        void fixProgramOperationReferences();

        void addLabelForProgramOperation(
            TCEString label, ProgramOperationPtr po) {
            labeledPOs_[label] = po;
        }

        virtual void emitSPInitialization();
        
        void emitSPInitialization(TTAProgram::CodeSnippet& target);

        void clearFunctionBookkeeping() {
            labeledPOs_.clear();
            symbolicPORefs_.clear();            
        }

        /// Code labels.
        std::map<std::string, TTAProgram::Instruction*> codeLabels_;

        /// Machine for building the program.
        TTAMachine::Machine* mach_;

        /// Target machine description.
        const llvm::TargetMachine* tm_;
        /// Mangler for mangling label strings.
        llvm::Mangler* mang_;

        /// Current program being built.
        TTAProgram::Program* prog_;

        /// The operations supported by the current target machine.
        std::set<std::string> opset_;

        // set to true in case the builder is used to schedule one
        // function at a time (the default processes the whole module)
        bool functionAtATime_;
	
    private:

        struct DataDef {
            std::string name;
            //llvm::Constant* initializer;
            unsigned address;
            unsigned alignment;
            unsigned size;
            bool initialize;
        };
        void initMembers();

        void emitDataDef(const DataDef& def);

        unsigned createDataDefinition(
            unsigned& addr,  const Constant* cv);

        void createIntDataDefinition(
            unsigned& addr, const llvm::ConstantInt* ci,
            bool isPointer = false);

        void createFPDataDefinition(
           unsigned& addr, const llvm::ConstantFP* cfp);

        void createGlobalValueDataDefinition(
            unsigned& addr, const GlobalValue* gv, int offset = 0);

        void createExprDataDefinition(
            unsigned& addr, const ConstantExpr* gv, int offset = 0);

        TTAProgram::Terminal* createAddrTerminal(
            const MachineOperand& base, const MachineOperand& offset);

        TTAProgram::Instruction* emitLoad(
            const MachineInstr* mi, TTAProgram::CodeSnippet* proc);
        
        TTAProgram::Instruction* emitStore(
            const MachineInstr* mi, TTAProgram::CodeSnippet* proc);

        TTAProgram::Instruction* emitReturn(
            const MachineInstr* mi, TTAProgram::CodeSnippet* proc);

        TTAProgram::Instruction* emitInlineAsm(
            const MachineInstr* mi, TTAProgram::CodeSnippet* proc);

        TTAProgram::Instruction* emitSpecialInlineAsm(
            const std::string op,
            const MachineInstr* mi,
            TTAProgram::CodeSnippet* proc);

        TTAProgram::Instruction* emitSetjmp(
            const MachineInstr* mi, TTAProgram::CodeSnippet* proc);

        TTAProgram::Instruction* emitLongjmp(
            const MachineInstr* mi, TTAProgram::CodeSnippet* proc);

        TTAProgram::Instruction* emitSelect(
            const MachineInstr* mi, TTAProgram::CodeSnippet* proc);
        
        TTAProgram::Instruction* emitGlobalXXtructorCalls(
            const MachineInstr* mi, TTAProgram::CodeSnippet* proc,
            bool constructors);

        TTAProgram::Instruction* emitReadSP(
            const MachineInstr* mi, TTAProgram::CodeSnippet* proc);

        TTAProgram::Instruction* handleMemoryCategoryInfo(
            const MachineInstr* mi, TTAProgram::CodeSnippet* proc);

        bool isInitialized(const Constant* cv);

        TTAProgram::MoveGuard* createGuard(
            const TTAProgram::Terminal* guardReg, bool trueOrFalse);

        void debugDataToAnnotations(
            const llvm::MachineInstr* mi, TTAProgram::Move* move);
        void addPointerAnnotations(
            const llvm::MachineInstr* mi, TTAProgram::Move* move);
        
        bool isBaseOffsetMemOperation(const Operation& operation) const;

         // Create MoveNodes before calling DDGBuilder.
         virtual void createMoveNode(
         	ProgramOperationPtr& po,
             TTAProgram::Move& m,
             bool isDestination) {}

        /// Target architechture MAU size in bits.
        static unsigned MAU_BITS;

        /// Target architecture pointer size in maus.
        static unsigned POINTER_SIZE;

        llvm::Module* mod_;

        TTAMachine::AddressSpace* instrAddressSpace_;
        TTAMachine::AddressSpace* dataAddressSpace_;

        /// Data memory initializations.
        TTAProgram::DataMemory* dmem_;

        /// Data definitions.
        std::vector<DataDef> data_;
        std::vector<DataDef> udata_;

        /// Machine basic block -> first instruction in the BB map.
        std::map<std::string, TTAProgram::Instruction*> mbbs_;

        /// Basic Block -> first instruction in the BB map
        std::map<const llvm::BasicBlock*, TTAProgram::Instruction*> bbIndex_;

        /// Data labels.
        std::map<std::string, unsigned> dataLabels_;

        /// Dummy code label references that have to be fixed after
        /// all instrutions have been built.
        std::map<TTAProgram::TerminalInstructionAddress*, std::string>
            codeLabelReferences_;

        /// Dummy basic block references that have to be fixed after
        /// all basic blocks have been built.
        std::map<TTAProgram::TerminalInstructionAddress*,
            std::string>
            mbbReferences_;

        /// Dummy references to the _end symbol.
        std::vector<TTAProgram::Move*> endReferences_;
        
        std::map<unsigned, unsigned> currentFnCP_;

        unsigned end_;

        /// set to true in case at least one 'noalias' attribute (from
        /// the use of 'restricted' pointers) has been found
        bool noAliasFound_;
        /// set to true in case at least one non-default address space
        /// memory access has been found
        bool multiAddrSpacesFound_;

        /// List of machine functions collected from runForMachineFunction.
        std::vector<MachineFunction*> functions_;

        int spillMoveCount_;

        bool dataInitialized_;

        std::set<TTAProgram::TerminalProgramOperation*> symbolicPORefs_;

        std::map<TCEString, ProgramOperationPtr >
        labeledPOs_;
    };
}
#endif
