/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
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
