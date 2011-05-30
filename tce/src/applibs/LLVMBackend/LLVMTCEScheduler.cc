/*
    Copyright (c) 2011 Tampere University of Technology.

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
 * @file LLVMTCEScheduler.cc
 *
 * Wrapper for TCE scheduler to be called from LLVM codegen.
 *
 * @author Pekka Jääskeläinen 2011
 * @note reting: red
 */
#include <llvm/CodeGen/MachineFunction.h>
#include <llvm/Function.h>
#include <llvm/Support/CommandLine.h>
#include <iostream>

#include "LLVMTCEScheduler.hh"
#include "Application.hh"

namespace llvm {

static cl::opt<std::string>
ADFLocation(
    "adf",
    cl::desc("The TCE architecture definition file."),
    cl::init(""), cl::Hidden);

char LLVMTCEScheduler::ID = 0;

bool
LLVMTCEScheduler::runOnMachineFunction(MachineFunction &MF) {
    Application::logStream()
        << "TCE: processing " << MF.getFunction()->getNameStr() 
        << std::endl;
    return false;
}
FunctionPass*
createTCESchedulerPass(const char* target) {
    Application::logStream()
        << "TCE: creating a scheduler for " << target << " with ADF "
        << ADFLocation << std::endl;

    if (ADFLocation == "") {
        Application::logStream()
            << "TCE: you need to provide the ADF location via llc -adf switch!"
            << std::endl;
        return NULL;
    }
    return new LLVMTCEScheduler();
}

}
