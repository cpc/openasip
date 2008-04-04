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

    bool changed = false;

    for (BasicBlock::iterator I = BB.begin(), E = BB.end(); I != E; ++I) {
        CallInst* ci = dynamic_cast<CallInst*>(&(*I));

        if (ci == NULL || ci->getNumOperands() == 0) {
            continue;
        }
        Function* callee = ci->getCalledFunction();
        if (callee == NULL) {
            continue;
        } else if (callee->isIntrinsic()) {
	    // LLVM Intrinsic!

            if (callee->getIntrinsicID() == Intrinsic::memcpy_i32 ||
                callee->getIntrinsicID() == Intrinsic::memcpy_i64) {
		
	        // memcpy intrinsic
	        std::vector<Value*> args;
	       
	        for (unsigned j = 1; j < I->getNumOperands(); j++)  {
		    args.push_back(I->getOperand(j));
	        }

	        // replace intrinsic with function call to memcpy
	        Constant* f = currentModule_->getOrInsertFunction("memcpy", callee->getFunctionType());
	        CallInst* call = new CallInst(f, args.begin(), args.end(), "", I);
	        I->replaceAllUsesWith(call);
	        I->eraseFromParent();
	        changed = true;
            }
        }
    }
    return changed;
}

