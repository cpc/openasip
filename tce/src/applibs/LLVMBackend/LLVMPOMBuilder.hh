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
 * @file LLVMPOMBuilder.hh
 *
 * Declaration of LLVMPOMBuilder class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel-no.spam-cs.tut.fi)
 * @note reting: red
 */

#ifndef LLVM_POM_BUILDER_H
#define LLVM_POM_BUILDER_H

#include <map>
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/Support/Mangler.h"
#include "llvm/Constant.h"
#include "Exception.hh"
#include "BaseType.hh"
#include "TCETargetMachine.hh"

namespace TTAProgram {
    class Program;
    class Procedure;
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

namespace TTAMachine {
    class Machine;
    class AddressSpace;
    class Bus;
}
namespace llvm {

    class ConstantInt;
    class ConstantFP;
    class ConstantExpr;

    FunctionPass* createLLVMPOMBuilderPass(
        TCETargetMachine& tm, TTAMachine::Machine* mach);

    class LLVMPOMBuilder : public MachineFunctionPass {

    public:
        LLVMPOMBuilder(
            TCETargetMachine& tm,
            TTAMachine::Machine* mach);

        virtual ~LLVMPOMBuilder();

        TTAProgram::Program* result() throw (NotAvailable);

    protected:

        bool doInitialization(Module &M);
        bool runOnMachineFunction(MachineFunction &MF);
        bool doFinalization(Module &M);

    private:

        struct DataDef {
            std::string name;
            //llvm::Constant* initializer;
            unsigned address;
            unsigned alignment;
            unsigned size;
            bool initialize;
        };

        void emitDataDef(const DataDef& def);

        void createDataDefinition(
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

        void emitConstantPool(const llvm::MachineConstantPool& cp);

        //TTAProgram::Terminal* createSrcTerminal(const MachineOperand& mo);
        //TTAProgram::Terminal* createDstTerminal(const MachineOperand& mo);
        TTAProgram::Terminal* createTerminal(const MachineOperand& mo);
        TTAProgram::Terminal* createAddrTerminal(
            const MachineOperand& base, const MachineOperand& offset);

        TTAProgram::Instruction* emitInstruction(
            const MachineInstr* mi, TTAProgram::Procedure* proc);

        TTAProgram::Instruction* emitMove(
            const MachineInstr* mi, TTAProgram::Procedure* proc);

        TTAProgram::Instruction* emitLoad(
            const MachineInstr* mi, TTAProgram::Procedure* proc);
        
        TTAProgram::Instruction* emitStore(
            const MachineInstr* mi, TTAProgram::Procedure* proc);

        TTAProgram::Instruction* emitReturn(
            const MachineInstr* mi, TTAProgram::Procedure* proc);

        TTAProgram::Instruction* emitInlineAsm(
            const MachineInstr* mi, TTAProgram::Procedure* proc);

        TTAProgram::Instruction* emitSelect(
            const MachineInstr* mi, TTAProgram::Procedure* proc);

        std::string mbbName(const MachineBasicBlock& mbb);
        bool isInitialized(const Constant* cv);

        void emitSPInitialization();
        
        TTAProgram::MoveGuard* createGuard(
            const TTAProgram::Terminal* guardReg, bool trueOrFalse);

        TTAProgram::Move* createMove(
            TTAProgram::Terminal* src,
            TTAProgram::Terminal* dst,
            TTAMachine::Bus &bus,
            TTAProgram::MoveGuard *guard = NULL);
        
        /// Target architechture MAU size in bits.
        static unsigned MAU_BITS;

        /// Target architecture pointer size in maus.
        static unsigned POINTER_SIZE;

        llvm::Module* mod_;

        /// Target machine description.
        llvm::TCETargetMachine& tm_;
        /// Machine for building the program.
        TTAMachine::Machine* mach_;
        /// Univeral machine for building the program.
        UniversalMachine* umach_;

        TTAMachine::AddressSpace* instrAddressSpace_;
        TTAMachine::AddressSpace* dataAddressSpace_;

        /// Current program being built.
        TTAProgram::Program* prog_;
        /// Mangler for mangling label strings.
        llvm::Mangler* mang_;
        /// Data memory initializations.
        TTAProgram::DataMemory* dmem_;

        /// Data definitions.
        std::vector<DataDef> data_;
        std::vector<DataDef> udata_;

        /// Basic block -> first instruction in the BB map.
        std::map<std::string, TTAProgram::Instruction*> mbbs_;

        /// Code labels.
        std::map<std::string, TTAProgram::Instruction*> codeLabels_;
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

        std::set<std::string> opset_;

        unsigned end_;

        bool programReady_;
    };
}
#endif
