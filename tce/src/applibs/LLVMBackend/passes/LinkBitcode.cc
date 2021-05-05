/*
    Copyright (c) 2002-2020 Tampere University.

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
 * @file LinkBitcode.cc
 *
 * Links other bitcode module to module.
 *
 * @author Mikael Lepistö 2008 (mikael.lepisto@tut.fi)
 */

#define DEBUG_TYPE "linkbitcode"

#include "CompilerWarnings.hh"
IGNORE_COMPILER_WARNING("-Wunused-parameter")

#include "llvm/Support/Compiler.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include "tce_config.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Linker/Linker.h"


POP_COMPILER_DIAGS

using namespace llvm;

#include <iostream>
#include <memory>
#include <map>

namespace {
    class LinkBitcode : public FunctionPass {
    public:
        static char ID; // Pass ID, replacement for typeid       
        LinkBitcode(Module& input);
        virtual ~LinkBitcode();        

        // from llvm::Pass:
        bool doInitialization(Module &M);
        bool doFinalization (Module &M);

        // to suppress Clang warnings
        using llvm::FunctionPass::doInitialization;
        using llvm::FunctionPass::doFinalization;
        bool runOnFunction(Function &F);

    private:
        Module& inputModule_;
    };

    char LinkBitcode::ID = 0;
// cannot be used as plugin pass right now..
//    RegisterPass<LinkBitcode>
//    X("linkbitcode", "Links in other bitcode module to currently handled module.");    
}


/**
 * Constructor
 */
LinkBitcode::LinkBitcode(Module& input) :
    FunctionPass(ID),
    inputModule_(input) {
}

/**
 * Destructor
 */
LinkBitcode::~LinkBitcode() {}


// Publically exposed interface to pass.
// const PassInfo* LinkBitcodeID = X.getPassInfo();

Pass* createLinkBitcodePass(Module& input) {
    return new LinkBitcode(input);
}

bool
LinkBitcode::doFinalization(Module& /*M*/) {
    return true;
}

bool
LinkBitcode::doInitialization(Module& M) {
    if (Linker::linkModules(M, std::unique_ptr<Module>(&inputModule_))) {
        errs() << "Error during linking in LinkBitcodePass: " << "\n";
    }
    return true;
}

bool
LinkBitcode::runOnFunction(Function&) {
    return true;
}
