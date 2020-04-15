/*
    Copyright (c) 2011 Tampere University.

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
 * @note rating: red
 */

#include "CompilerWarnings.hh"

IGNORE_COMPILER_WARNING("-Wunused-parameter")

#include <llvm/CodeGen/MachineFunction.h>
#include "tce_config.h"
#include <llvm/IR/Function.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Analysis/AliasAnalysis.h>

#include <iostream>

#include "LLVMTCEScheduler.hh"
#include "Application.hh"
#include "InterPassData.hh"
#include "LLVMTCECmdLineOptions.hh"
#include "OperationPool.hh"

POP_COMPILER_DIAGS

namespace llvm {

static cl::opt<std::string>
ADFLocation(
    "adf",
    cl::desc("The TCE architecture definition file."),
    cl::init(""), cl::Hidden);
static cl::opt<bool>
DisableLLVMAA(
    "disable-llvmaa",
    cl::desc("Disables use of LLVM Alias Analysis information."));    
static cl::opt<bool>
DumpDDG(
    "dump-ddgs",
    cl::desc("Equivalent to --dump-ddgs-dot --dump-ddgs-xml."));
static cl::opt<bool>
DumpDDGDot(
    "dump-ddgs-dot",
    cl::desc("Write out Data Dependence Graph of processed procedures in dot format."));
static cl::opt<bool>
DumpDDGXML(
    "dump-ddgs-xml",
    cl::desc("Write out Data Dependence Graph of processed procedures in XML format."));
    
char LLVMTCEScheduler::ID = 0;

LLVMTCEScheduler::LLVMTCEScheduler() : 
    MachineFunctionPass(ID), tceIRBuilder_(NULL), interPassData_(NULL) {
    try {
        tceMachine_ = TTAMachine::Machine::loadFromADF(ADFLocation);
    } catch (const Exception& e) {
        Application::logStream()
            << "TCE: unable to load the ADF:" << std::endl
            << e.errorMessage() << std::endl;
    }
    // By default Application::cmdLineOptions returns NULL.
    // If we want to pass dump-ddg options we need to create new one.
    // Add also -O3 flag, otherwise the -O0 is used and Sequential Scheduler
    // called.
    LLVMTCECmdLineOptions* options = new LLVMTCECmdLineOptions; 
    std::vector<std::string> args;
    args.push_back("llc");
    args.push_back("-O3");    
    if (DumpDDG) {
        args.push_back("--dump-ddgs-dot");
        args.push_back("--dump-ddgs-xml");        
    } else if (DumpDDGDot) {
        args.push_back("--dump-ddgs-dot");    
    } else if (DumpDDGXML) {
        args.push_back("--dump-ddgs-xml");        
    }       
    if (DisableLLVMAA) {
        args.push_back("--disable-llvmaa");
    }
    try {
        options->parse(args);
        Application::setCmdLineOptions(options);        
    } catch (const IllegalCommandLine& e) {
        std::cerr << e.errorMessageStack() << std::endl;
    }
}

bool
LLVMTCEScheduler::doInitialization(Module& m) {
    mod_ = &m;
    return false;
}
bool
LLVMTCEScheduler::runOnMachineFunction(MachineFunction &MF) {
    OperationPool::setLLVMTargetInstrInfo(
        MF.getTarget().getSubtargetImpl(*MF.getFunction())->getInstrInfo());
	
    AliasAnalysis* AA = getAnalysisIfAvailable<AliasAnalysis>();
    if (tceIRBuilder_ == NULL) {
        interPassData_ = new InterPassData();
        tceIRBuilder_ =  new LLVMTCEIRBuilder(
            MF.getTarget(), tceMachine_, *interPassData_, AA, true, true);
        tceIRBuilder_->doInitialization(*mod_);
    }
    tceIRBuilder_->writeMachineFunction(MF);
    return false;
}

FunctionPass*
createTCESchedulerPass(const char* /*target*/) {
    if (ADFLocation == "") {
        Application::logStream()
            << "TCE: you need to provide the ADF location via llc -adf switch!"
            << std::endl;
        return NULL;
    }
    return new LLVMTCEScheduler();
}

}
