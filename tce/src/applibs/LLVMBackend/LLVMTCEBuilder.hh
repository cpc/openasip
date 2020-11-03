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
 * @file LLVMTCEBuilder.hh
 *
 * Declaration of LLVMTCEBuilder class.
 *
 * @author Pekka Jääskeläinen 2010-2015
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

#include "tce_config.h"
#include <llvm/IR/Mangler.h>
#include <llvm/IR/Constant.h>
#include <llvm/Transforms/IPO.h>

// tce_config.h defines these. this undef to avoid warning.
// TODO: how to do this in tce_config.h???
#ifdef LLVM_LIBDIR
#undef LLVM_LIBDIR
#endif

#include "Exception.hh"
#include "BaseType.hh"
#include "TCEString.hh"
#include "passes/MachineDCE.hh"
#include "TCETargetMachine.hh"
#include "ProgramOperation.hh"
#include "InlineAsmParser.hh"

namespace TTAProgram {
    class Program;
    class Procedure;
    class CodeSnippet;
    class BasicBlock;
    class Terminal;
    class TerminalRegister;
    class TerminalInstructionAddress;
    class InstructionReferenceManager;
    class TerminalProgramOperation;
    class Instruction;
    class DataMemory;
    class DataDefinition;
    class Move;
    class MoveGuard;
}

class PRegionMarkerAnalyzer;
class UniversalMachine;
class Operation;
class LLVMTCECmdLineOptions;

namespace TTAMachine {
    class Machine;
    class AddressSpace;
    class Bus;
    class HWOperation;
}
namespace llvm {

    class ConstantInt;
    class ConstantFP;
    class ConstantExpr;
    class ConstantDataSequential;
    class TCETargetMachine;

    /**
     * Base class for the various LLVM to TCE builder implementations.
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

        TTAProgram::Program* result();

        TTAProgram::Instruction* firstInstructionOfBasicBlock(
            const llvm::BasicBlock* bb) {
            return bbIndex_[bb];
        }
        bool isProgramUsingRestrictedPointers() const { return noAliasFound_; }

        virtual void getAnalysisUsage(AnalysisUsage &AU) const {
#ifdef LLVM_OLDER_THAN_3_8
            AU.addRequired<AliasAnalysis>();
#else
            AU.addRequired<AAResultsWrapperPass>();
#endif
            AU.addRequired<MachineDCE>();
            AU.addPreserved<MachineDCE>();
            MachineFunctionPass::getAnalysisUsage(AU);
        }

        virtual bool isTTATarget() const {
            return (dynamic_cast<const TCETargetMachine*>(
                        &targetMachine()) != NULL);
        }

        void deleteDeadProcedures();

        void setInitialStackPointerValue(unsigned value);

    protected:

        bool doInitialization(Module &M);
        bool runOnMachineFunction(MachineFunction &MF);
        bool doFinalization(Module &M);

        bool hasAmbiguousASpaceRefs(
            const TTAProgram::Instruction& instr) const;

        virtual bool writeMachineFunction(MachineFunction &MF);
        void initDataSections();

        std::shared_ptr<TTAProgram::Move> createMove(
            const MachineOperand& src, const MachineOperand& dst,
            TTAProgram::MoveGuard* guard);

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
        TCEString registerName(unsigned llvmRegNum) const;
        virtual TCEString operationName(const MachineInstr& mi) const = 0;
        virtual TTAProgram::Terminal* createFUTerminal(
            const MachineOperand&) const {
            return NULL; 
        }

        /* Helper methods */
        std::string mbbName(const MachineBasicBlock& mbb);

        const TTAMachine::HWOperation& getHWOperation(std::string opName);

        TTAProgram::TerminalRegister* createTerminalRegister(
            const std::string& rfName, int index);

        TTAProgram::Terminal* createTerminal(
            const MachineOperand& mo, int bitLimit = 0);

        std::shared_ptr<TTAProgram::Move> createMove(
            TTAProgram::Terminal* src,
            TTAProgram::Terminal* dst,
            const TTAMachine::Bus &bus,
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
            const MachineInstr* mi, TTAProgram::CodeSnippet* proc,
            bool conditional=false, bool trueGuard=true);

        TTAProgram::Instruction* emitInlineAsm(
            const MachineFunction& mf,
            const MachineInstr* mi,
            TTAProgram::BasicBlock* bb,
            TTAProgram::InstructionReferenceManager& irm);

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

        static bool isInlineAsm(const MachineInstr& instr);
 
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

        PRegionMarkerAnalyzer* pregions_;

        int spillMoveCount_;

        unsigned initialStackPointerValue_;

        MachineFrameInfo* curFrameInfo_;

        /// The compiler options.
        LLVMTCECmdLineOptions* options_ = nullptr;

    private:

        /// Data definition structure for global values.
        struct DataDef {
            std::string name;
            //llvm::Constant* initializer;
            unsigned address;
            unsigned addressSpaceId;
            unsigned alignment;
            unsigned size;
            bool initialize;
        };

        /// Data definition structure for constant pool values
        struct ConstantDataDef {
            unsigned address = 0;
            unsigned alignment = 0;
            unsigned size = 0;
            const llvm::Constant* value = nullptr;

            ConstantDataDef(
                unsigned addr,
                unsigned align,
                unsigned size,
                const llvm::Constant* value)
            : address(addr), alignment(align), size(size), value(value) {}
        };

        typedef std::map<TTAMachine::AddressSpace*, TTAProgram::DataMemory*>
        DataMemIndex;

        void initMembers();

        void emitDataDef(const DataDef& def);
        void emitDataDef(const ConstantDataDef& def);

        unsigned createDataDefinition(
            int addressSpaceId, unsigned& addr,  const Constant* cv,
            bool forceInitialize=false,  unsigned forceAlignment=0);

        void createIntDataDefinition(
            int addressSpaceId, unsigned& addr, const llvm::ConstantInt* ci,
            bool isPointer = false);

        void createFPDataDefinition(
            int addressSpaceId, unsigned& addr, const llvm::ConstantFP* cfp);

        void createGlobalValueDataDefinition(
            int addressSpaceId, unsigned& addr, const GlobalValue* gv, 
            int offset = 0);

        void createExprDataDefinition(
            int addressSpaceId, unsigned& addr, const ConstantExpr* gv, 
            int offset = 0);

        void padToAlignment(
            int addressSpaceId, unsigned& addr, unsigned align);

        TTAProgram::Terminal* createAddrTerminal(
            const MachineOperand& base, const MachineOperand& offset);

        TTAProgram::Instruction* emitLoad(
            const MachineInstr* mi, TTAProgram::CodeSnippet* proc);
        
        TTAProgram::Instruction* emitStore(
            const MachineInstr* mi, TTAProgram::CodeSnippet* proc);

        TTAProgram::Instruction* emitReturn(
            const MachineInstr* mi, TTAProgram::CodeSnippet* proc);

        TTAProgram::Instruction* emitOperationMacro(
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

        TTAProgram::Instruction* emitWriteSP(
            const MachineInstr* mi, TTAProgram::CodeSnippet* proc);

        TTAProgram::Instruction* emitReturnTo(
            const MachineInstr* mi, TTAProgram::CodeSnippet* proc);

        TTAProgram::Instruction* handleMemoryCategoryInfo(
            const MachineInstr* mi, TTAProgram::CodeSnippet* proc);

        TTAProgram::Instruction* emitComparisonForBranch(
            TCEString firstOp, const MachineInstr* mi, TTAProgram::CodeSnippet* proc);

        TTAProgram::Instruction* emitRemaingingBrach(
            TCEString opName, const MachineInstr* mi, TTAProgram::CodeSnippet* proc);

        void createSPInitLoad(
            TTAProgram::CodeSnippet& target,
            TTAProgram::Terminal& src,
            TTAProgram::Terminal& dst);

        bool isInitialized(const Constant* cv);
        
        TTAProgram::MoveGuard* createGuard(
            const TTAProgram::Terminal* guardReg, bool trueOrFalse);

        void debugDataToAnnotations(
            const llvm::MachineInstr* mi, TTAProgram::Move& move);
        void addPointerAnnotations(
            const llvm::MachineInstr* mi, TTAProgram::Move& move);
        
        bool isBaseOffsetMemOperation(const Operation& operation) const;

        // Create MoveNodes before calling DDGBuilder.
        virtual void createMoveNode(
            ProgramOperationPtr&,
            std::shared_ptr<TTAProgram::Move> m,
            bool /*isDestination*/) {}

        unsigned addressSpaceId(TTAMachine::AddressSpace& aSpace) const;
        TTAMachine::AddressSpace& addressSpaceById(unsigned id);
        unsigned& dataEnd(TTAMachine::AddressSpace& aSpace);

        void addCandidateLSUAnnotations(
            unsigned asNum, TTAProgram::Move& move);

        TTAProgram::DataMemory&
        dataMemoryForAddressSpace(TTAMachine::AddressSpace& aSpace);

        void copyFUAnnotations(
            const std::vector<TTAProgram::Instruction*>& operandMoves, 
            TTAProgram::Move& move) const;

        std::string getAsmString(const MachineInstr& mi) const;

        /// Target architechture MAU size in bits.
        static unsigned MAU_BITS;

        /// Target architecture pointer size in maus.
        static unsigned POINTER_SIZE_32;
        static unsigned POINTER_SIZE_64;

        llvm::Module* mod_;

        TTAMachine::AddressSpace* instrAddressSpace_;

        /// The default data memory address space (address space 0).
        TTAMachine::AddressSpace* defaultDataAddressSpace_;

        /// Set to true in case this machine has more than one data
        /// address spaces.
        bool multiDataMemMachine_;

#if 0
        /// Data memory initializations.
        TTAProgram::DataMemory* dmem_;
#endif
        DataMemIndex dmemIndex_;
        
        /// Data definitions.
        std::vector<DataDef> data_;
        std::vector<DataDef> udata_;
        std::vector<ConstantDataDef> cpData_;

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
        std::vector<std::shared_ptr<TTAProgram::Move> > endReferences_;
        
        /// Global constant pool for all constants gathered from machine
        /// functions. Map key is unique constant and the value is address of
        /// the constant.
        std::map<const llvm::Constant*, unsigned> globalCP_;
        /// Constant pool for the current machine function. Map key is constant
        /// pool index and the value is address.
        std::map<unsigned, unsigned> currentFnCP_;

        /// The first position after the last data in the given address space.
        std::map<TTAMachine::AddressSpace*, unsigned> dataEnds_;

#if 0        
        unsigned end_;
#endif

        /// set to true in case at least one 'noalias' attribute (from
        /// the use of 'restricted' pointers) has been found
        bool noAliasFound_;
        /// set to true in case at least one non-default address space
        /// memory access has been found in the generated code
        bool multiAddrSpacesFound_;

        /// List of machine functions collected from runForMachineFunction.
        std::vector<MachineFunction*> functions_;

        bool dataInitialized_;

        std::set<TTAProgram::TerminalProgramOperation*> symbolicPORefs_;

        std::map<TCEString, ProgramOperationPtr > labeledPOs_;

        /// The data layout for the machine.
        const llvm::DataLayout* dl_;
    };
}
#endif
