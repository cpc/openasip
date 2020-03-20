/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file LowerIntrinsics.cc
 *
 * Converts llvm intrinsics to libcalls.
 *
 * @author Veli-Pekka Jaaskelainen 2008 (vjaaskel-no.spam-cs.tut.fi)
 * @author Mikael Lepistö 2009 (mikael.lepisto-no.spam-tut.fi)
 */

#define DEBUG_TYPE "lowerintrinsics"

#include "CompilerWarnings.hh"
IGNORE_COMPILER_WARNING("-Wunused-parameter")

#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Utils/UnifyFunctionExitNodes.h"
#include "tce_config.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Pass.h"
#include "llvm/CodeGen/IntrinsicLowering.h"
#include "tce_config.h"

POP_COMPILER_DIAGS

#include "llvm/IR/DataLayout.h"
typedef llvm::DataLayout TargetData;

using namespace llvm;

#include <iostream>
#include <set>

namespace {
#ifdef LLVM_OLDER_THAN_10
    class LowerIntrinsics : public BasicBlockPass {        
#else
    class LowerIntrinsics : public FunctionPass {
#endif
    public:
        static char ID; // Pass ID, replacement for typeid       
        LowerIntrinsics();
        virtual ~LowerIntrinsics();

        // from llvm::Pass:
        bool doInitialization(Module &M);
        bool doFinalization (Module &M);

        // to suppress Clang warnings
#ifdef LLVM_OLDER_THAN_10
        using llvm::BasicBlockPass::doInitialization;
        using llvm::BasicBlockPass::doFinalization;
#else
        using llvm::FunctionPass::doInitialization;
        using llvm::FunctionPass::doFinalization;
#endif

#ifdef LLVM_OLDER_THAN_10
        bool runOnBasicBlock(BasicBlock &BB) override;
#else
        bool runOnBasicBlock(BasicBlock &BB);
        bool runOnFunction(Function &F) override;
#endif
       
     private:
        /// List of intrinsics to replace.
        std::set<unsigned> replace_;
        IntrinsicLowering* iLowering_;
        TargetData* td_;
    };

    char LowerIntrinsics::ID = 0;
    RegisterPass<LowerIntrinsics>
    X("lowerintrinsics", "Lower llvm intrinsics back to libcalls.");
}


/**
 * Constructor
 */
LowerIntrinsics::LowerIntrinsics() :
#ifdef LLVM_OLDER_THAN_10
    BasicBlockPass(ID), 
#else
    FunctionPass(ID),
#endif
    iLowering_(NULL), td_(NULL) {
}

/**
 * Destructor
 */
LowerIntrinsics::~LowerIntrinsics() {
}


bool
LowerIntrinsics::doInitialization(Module &M) {
   
    // Initialize list of intrinsics to lower.
    replace_.insert(Intrinsic::flt_rounds);
    replace_.insert(Intrinsic::ceil);
    replace_.insert(Intrinsic::floor);
    replace_.insert(Intrinsic::round);
    replace_.insert(Intrinsic::exp2);
    replace_.insert(Intrinsic::memcpy);
    replace_.insert(Intrinsic::memset);
    replace_.insert(Intrinsic::memmove);
    
    assert(iLowering_ == NULL && td_ == NULL);
    td_ = new TargetData(&M);
    iLowering_ = new IntrinsicLowering(*td_);
#ifdef LLVM_OLDER_THAN_9
    iLowering_->AddPrototypes(M);
#endif
    return true;
}

bool
LowerIntrinsics::doFinalization(Module& /*M*/) {
    if (iLowering_ != NULL) { 
        delete iLowering_;
        iLowering_ = NULL;
    }
    if (td_ != NULL) {
        delete td_;
        td_ = NULL;
    }    
    return true;
}


bool
LowerIntrinsics::runOnBasicBlock(BasicBlock &BB) {
    
   bool changed = true;
   while (changed)  {
       changed = false;
       for (BasicBlock::iterator I = BB.begin(), E = BB.end(); I != E; ++I) {
           
           CallInst* ci = dyn_cast<CallInst>(&(*I));
           if (ci != NULL && ci->getNumArgOperands() != 0) {
               Function* callee = ci->getCalledFunction();
               if (callee != NULL && callee->isIntrinsic() &&
                   replace_.find(callee->getIntrinsicID()) != replace_.end()) {
                   if (callee->getIntrinsicID() == Intrinsic::flt_rounds) {
                       // Replace FLT_ROUNDS intrinsic with the actual
                       // constant value to avoid stupid  "if (1 == 0)"
                       // code even with full optimizations.
#ifdef LLVM_OLDER_THAN_3_9
                       I->replaceAllUsesWith(
                           ConstantInt::get(
                               Type::getInt32Ty(getGlobalContext()), 0, true));
#else
                       I->replaceAllUsesWith(
                           ConstantInt::get(
                               Type::getInt32Ty(BB.getContext()), 0, true));
#endif
                       I->eraseFromParent();
                       changed = true;
                       break;
                   } else {
                       iLowering_->LowerIntrinsicCall(ci);
                       changed = true;
                       break;
                   }
               }
           }
       }
   }
   return true;
}

#ifndef LLVM_OLDER_THAN_10
bool
LowerIntrinsics::runOnFunction(Function &F) {

    for (BasicBlock &BB : F) {
        runOnBasicBlock(BB);
    }

    return true;
}
#endif
