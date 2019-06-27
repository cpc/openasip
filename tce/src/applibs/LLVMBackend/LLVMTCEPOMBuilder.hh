/*
    Copyright (c) 2002-2010 Tampere University.

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
 * @file LLVMTCEPOMBuilder.hh
 *
 * This builder builds a sequential POM from the new LLVM TTA backend format.
 *
 * @author Pekka Jääskeläinen 2010 
 * @note reting: red
 */

#ifndef LLVM_TCE_POM_BUILDER_H
#define LLVM_TCE_POM_BUILDER_H

#include <map>
#include <set>
#include <iostream>

#include "CompilerWarnings.hh"
IGNORE_COMPILER_WARNING("-Wunused-parameter")

#include <llvm/CodeGen/MachineFunctionPass.h>
#include <llvm/CodeGen/MachineConstantPool.h>
#include <llvm/CodeGen/MachineOperand.h>
#include <llvm/CodeGen/MachineInstr.h>
#include <llvm/CodeGen/MachineBasicBlock.h>

#include "tce_config.h"
#include <llvm/IR/Mangler.h>
#include <llvm/IR/Constant.h>
#include <llvm/Transforms/IPO.h>

#include "Exception.hh"

// tce_config.h defines these. this undef to avoid warning.
// TODO: how to do this in tce_config.h???
#ifdef LLVM_LIBDIR
#undef LLVM_LIBDIR
#endif
#include "BaseType.hh"
#include "LLVMPOMBuilder.hh"

POP_COMPILER_DIAGS

namespace TTAProgram {
    class TerminalFUPort;
}

namespace llvm {

    extern "C" MachineFunctionPass* createLLVMTCEPOMBuilderPass();

    /**
     * Implements building TCE POM from TTA-style (MOVE) sequential input 
     * from LLVM codegen.
     *
     * @note Experimental and still lots to do before can be considered for 
     * real use.
     */
    class LLVMTCEPOMBuilder : public LLVMTCEBuilder {
    public:
        static char ID;
        LLVMTCEPOMBuilder();
        virtual ~LLVMTCEPOMBuilder() {}

        virtual unsigned spDRegNum() const;
        virtual unsigned raPortDRegNum() const;
        virtual TCEString registerFileName(unsigned llvmRegNum) const;
        virtual int registerIndex(unsigned llvmRegNum) const;
        virtual TCEString operationName(const MachineInstr& mi) const;
        virtual TTAProgram::Terminal* createFUTerminal(
            const MachineOperand&) const;
        bool doFinalization(Module& m);
        bool doInitialization(Module &M);
        TTAProgram::Instruction* emitMove(
            const MachineInstr* mi, TTAProgram::CodeSnippet* proc,
            bool conditional=false, bool trueGuard=true);
    };
}

#endif
