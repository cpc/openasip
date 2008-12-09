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
 * @file LowerIntrinsics.cc
 *
 * Converts llvm intrinsics to libcalls.
 *
 * @author Veli-Pekka Jaaskelainen 2008 (vjaaskel-no.spam-cs.tut.fi)
 */

#define DEBUG_TYPE "lowerintrinsics"

#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Utils/UnifyFunctionExitNodes.h"
#include "llvm/Module.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Instructions.h"
#include "llvm/Constants.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Pass.h"
#include "llvm/Intrinsics.h"
#include "llvm/CodeGen/IntrinsicLowering.h"
#include "llvm/Target/TargetData.h"

using namespace llvm;

#include <iostream>
#include <set>

namespace {
    class VISIBILITY_HIDDEN LowerIntrinsics : public BasicBlockPass {

        Module* dstModule_;
        
    public:
        static char ID; // Pass ID, replacement for typeid       
        LowerIntrinsics();
        virtual ~LowerIntrinsics();

        bool doInitialization(Module &M);
        bool doFinalization (Module &M);
        bool runOnBasicBlock(BasicBlock &BB);
       
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
BasicBlockPass((intptr_t)&ID), iLowering_(NULL), td_(NULL) {
}

/**
 * Destructor
 */
LowerIntrinsics::~LowerIntrinsics() {
}


// Publically exposed interface to pass.
const PassInfo* LowerIntrinsicsID = X.getPassInfo();
// - Interface to this file...
Pass* createLowerIntrinsicsPass() {
    return new LowerIntrinsics();
}


bool
LowerIntrinsics::doInitialization(Module &M) {
   
    // Initialize list of intrinsics to lower.
    replace_.insert(Intrinsic::flt_rounds);
    replace_.insert(Intrinsic::memcpy_i32);
    replace_.insert(Intrinsic::memcpy_i64);
    replace_.insert(Intrinsic::memset_i32);
    replace_.insert(Intrinsic::memset_i64);
    replace_.insert(Intrinsic::memmove_i32);
    replace_.insert(Intrinsic::memmove_i64);

    assert(iLowering_ == NULL && td_ == NULL);
    td_ = new TargetData(&M);
    iLowering_ = new IntrinsicLowering(*td_);
   
    iLowering_->AddPrototypes(M);

    return true;
}

bool
LowerIntrinsics::doFinalization(Module& M) {
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

           CallInst* ci = dynamic_cast<CallInst*>(&(*I));
           if (ci != NULL && ci->getNumOperands() != 0) {
               Function* callee = ci->getCalledFunction();
               if (callee != NULL && callee->isIntrinsic() &&
		   replace_.find(callee->getIntrinsicID()) != replace_.end()) {
                   if (callee->getIntrinsicID() == Intrinsic::flt_rounds) {
                       // Replace FLT_ROUNDS intrinsic with the actual
                       // constant value to avoid stupid  "if (1 == 0)"
                       // code even with full optimizations.
                       I->replaceAllUsesWith(
                           ConstantInt::get(Type::Int32Ty, 0, true));

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
