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
#include "LLVMPOMBuilder.hh"

namespace llvm {

    extern "C" MachineFunctionPass* createLLVMTCEPOMBuilderPass();

    class LLVMTCEPOMBuilder : public LLVMPOMBuilder {
    public:
        static char ID;
        LLVMTCEPOMBuilder();
        virtual ~LLVMTCEPOMBuilder() {}

//        virtual bool runOnMachineFunction(MachineFunction& tm);
    };
}

#endif
