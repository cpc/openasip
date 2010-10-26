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
 * @file LLVMTCEDDGBuilder.cc
 *
 * Implementation of LLVMTCEDDGBuilder class.
 *
 * @author Pekka Jääskeläinen 2010
 * @note reting: red
 */
#include "LLVMTCEDDGBuilder.hh"

#include <iostream>

namespace llvm {

char LLVMTCEDDGBuilder::ID = 0;

bool
LLVMTCEDDGBuilder::runOnMachineFunction(MachineFunction& tm) {
}

extern "C" MachineFunctionPass* createLLVMTCEDDGBuilderPass(
    TargetMachine& tm) {
    return new llvm::LLVMTCEDDGBuilder();
}

}
