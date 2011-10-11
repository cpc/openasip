/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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

#include "llvm/Support/Compiler.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Module.h"
#include "llvm/Pass.h"
#include "llvm/Linker.h"
#include "tce_config.h"

using namespace llvm;

#include <iostream>
#include <map>

namespace {
    class LinkBitcode : public BasicBlockPass {
    public:
        static char ID; // Pass ID, replacement for typeid       
        LinkBitcode(Module& input);
        virtual ~LinkBitcode();        
        bool doInitialization(Module &M);
        bool doFinalization (Module &M);
        bool runOnBasicBlock(BasicBlock &BB);

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
#ifdef LLVM_2_7
    BasicBlockPass((intptr_t)&ID), 
#else
    BasicBlockPass(ID), 
#endif
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
    std::string errors;
#ifndef LLVM_2_9
    if (Linker::LinkModules(&M, &inputModule_, Linker::DestroySource, &errors)) 
#else
    if (Linker::LinkModules(&M, &inputModule_, &errors)) 
#endif
    {

        errs() << "Error during linking in LinkBitcodePass: " << errors << "\n";
    } 
    return true;
}

bool
LinkBitcode::runOnBasicBlock(BasicBlock& /*BB*/) {
    return true;
}
