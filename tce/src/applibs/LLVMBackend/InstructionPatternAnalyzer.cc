/*
    Copyright (c) 2012 Tampere University of Technology.

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
 * @file InstructionPatternAnalyzer.cc
 *
 * Declaration of InstructionPatternAnalyzer class.
 *
 * @author Pekka Jääskeläinen 2012
 */

#include "InstructionPatternAnalyzer.hh"
#include "MachineInstrDDG.hh"

#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFunctionAnalysis.h"
#include "llvm/Function.h"

using namespace llvm;

char InstructionPatternAnalyzer::ID = 0;

#include <iostream>

bool
InstructionPatternAnalyzer::runOnMachineFunction(llvm::MachineFunction &F) {
    MachineInstrDDG ddg(F);
    ddg.computeOptimalSchedule();
    ddg.writeToDotFile(F.getFunction()->getName().str() + ".pre_ra.dot");
    return false;
}

llvm::Pass* 
createInstructionPatternAnalyzer() {
    return new InstructionPatternAnalyzer();
}
