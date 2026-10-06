/*
 Copyright (C) 2022-2026 Tampere University.

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
