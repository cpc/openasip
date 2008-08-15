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
 * @author Veli-Pekka Jaaskelainen 2008 (vjaaskel@cs.tut.fi)
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

using namespace llvm;

#include <iostream>
#include <map>

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
       Module* currentModule_;
       std::map<unsigned, std::string> replace_;
    };

    char LowerIntrinsics::ID = 0;
    RegisterPass<LowerIntrinsics>
    X("lowerintrinsics", "Lower llvm intrinsics back to libcalls.");

}


/**
 * Constructor
 */
LowerIntrinsics::LowerIntrinsics() :
BasicBlockPass((intptr_t)&ID), currentModule_(NULL) {

    // Intrinsics to replace and corresponding function names.
    replace_[Intrinsic::memcpy_i32] = "memcpy";
    replace_[Intrinsic::memcpy_i64] = "memcpy";
    replace_[Intrinsic::memset_i32] = "memset";
    replace_[Intrinsic::memset_i64] = "memset";   
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
    currentModule_ = &M;
    return true;
}

bool
LowerIntrinsics::doFinalization(Module& M) {
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
               if (callee != NULL && callee->isIntrinsic()) {
                   if (callee->getIntrinsicID() == Intrinsic::flt_rounds) {
                       // Replace FLT_ROUNDS intrinsic with the actual
                       // constant value to avoid stupid  "if (1 == 0)"
                       // code even with full optimizations.
                       I->replaceAllUsesWith(
                           ConstantInt::get(Type::Int32Ty, 0, true));

                       I->eraseFromParent();
                       changed = true;
                       break;
                   } else if (replace_.find(callee->getIntrinsicID())
                              != replace_.end())  {
		  
                       // Call to LLVM Intrinsic function.
                       std::string funcName =
                           replace_[callee->getIntrinsicID()];

                       std::vector<Value*> args;
                       for (unsigned j = 1; j < I->getNumOperands(); j++)  {
                           args.push_back(I->getOperand(j));
                       }
                       // replace intrinsic with function call to memcpy
                       Constant* f = currentModule_->getOrInsertFunction(
                           funcName, callee->getFunctionType());

                       CallInst* call = CallInst::Create(
                           f, args.begin(), args.end(), "", I);

                       I->replaceAllUsesWith(call);
                       I->eraseFromParent();
                       changed = true;
                       break;
                   }
	    
               }
           }
       }
   }
   return true;
}

      
