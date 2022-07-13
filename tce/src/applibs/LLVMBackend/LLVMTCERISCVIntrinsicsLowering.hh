/*
 Copyright (C) 2022 Tampere University.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 */
/**
 * @file LLVMTCERISCVIntrinsicsLowering.hh
 *
 *
 * Pass for lowering RISC-V intrinsics
 *
 * @author Kari Hepola 2022
 * @note rating: red
 */

#ifndef LLVM_TCE_SCHEDULER_H
#define LLVM_TCE_SCHEDULER_H

#include <llvm/CodeGen/MachineFunctionPass.h>

#include "Machine.hh"
#include "BinaryEncoding.hh"
#include "InstructionFormat.hh"
#include "InterPassData.hh"

namespace llvm {

    extern "C" FunctionPass* createRISCVIntrinsicsPass(const char* target);

    class LLVMTCERISCVIntrinsicsLowering : public MachineFunctionPass {
    public:
        static char ID;
        LLVMTCERISCVIntrinsicsLowering();
        virtual ~LLVMTCERISCVIntrinsicsLowering() {}
        virtual bool runOnMachineFunction(MachineFunction &MF);
    private:
        InstructionFormat* findRFormat();
        std::string findRegs(const std::string& s) const;
        std::string findOperationName(const std::string& s) const;
        
        std::vector<int> findRegIndexes(
            const MachineBasicBlock::iterator& it) const;

        int constructEncoding(
            const std::string& opName, const std::vector<int>& regIdxs) const;

        virtual bool doInitialization(Module& m);
        TTAMachine::Machine* mach_;
        BinaryEncoding* bem_;
        InstructionFormat* rFormat_;

    };
}

#endif
