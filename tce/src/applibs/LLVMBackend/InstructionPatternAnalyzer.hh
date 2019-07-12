/*
    Copyright (c) 2012 Tampere University.

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
 * @file InstructionPatternAnalyzer.hh
 *
 * Pass that analyzes the selection DAG going to be instruction selected.
 * Helps to find common instruction chains for conversion to custom 
 * operations.
 *
 * @author Pekka Jääskeläinen 2012,2019
 */

#ifndef TCE_INSTRUCTION_PATTERN_ANALYZER_HH
#define TCE_INSTRUCTION_PATTERN_ANALYZER_HH

#include "CompilerWarnings.hh"

IGNORE_COMPILER_WARNING("-Wunused-parameter")
IGNORE_COMPILER_WARNING("-Wcomment")

#include <llvm/CodeGen/MachineFunctionPass.h>

struct InstructionPatternAnalyzer : public llvm::MachineFunctionPass {
    static char ID;
    InstructionPatternAnalyzer() : MachineFunctionPass(ID) {}
    virtual ~InstructionPatternAnalyzer() {}

    virtual bool runOnMachineFunction(llvm::MachineFunction &F);
};

#endif
