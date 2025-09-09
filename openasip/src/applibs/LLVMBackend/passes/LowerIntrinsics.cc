/*
    Copyright (c) 2002-2025 Tampere University.

    This file is part of OpenASIP.

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
 * @author Joonas Multanen 2023 (joonas.multanen-no.spam-tuni.fi)
 */

#include <CompilerWarnings.hh>
IGNORE_COMPILER_WARNING("-Wunused-parameter")

#include "tce_config.h"

#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Utils/UnifyFunctionExitNodes.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Intrinsics.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/Compiler.h>
#include <llvm/IR/Function.h>
#include <llvm/Pass.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/CodeGen/IntrinsicLowering.h>

POP_COMPILER_DIAGS

#include <llvm/IR/DataLayout.h>
typedef llvm::DataLayout TargetData;

using namespace llvm;

#include <iostream>
#include <set>


class LowerIntrinsics : public PassInfoMixin<LowerIntrinsics> {
public:
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);

    // to suppress Clang warnings
    //using llvm::FunctionPass::doInitialization;
    //using llvm::FunctionPass::doFinalization;

    bool runOnBasicBlock(BasicBlock &BB);

private:
    bool doInitialization(Module &M);
    bool doFinalization (Module &M);
    /// List of intrinsics to replace.
    std::set<unsigned> replace_;
    IntrinsicLowering* iLowering_;
};

PreservedAnalyses
LowerIntrinsics::run(Function& F, FunctionAnalysisManager&) {
    //errs() << F.getName() << "\n";
  Module *parentModule = F.getParent();
  doInitialization(*parentModule);
  for (BasicBlock &BB : F) {
      runOnBasicBlock(BB);
  }
  doFinalization(*parentModule);
  return PreservedAnalyses::all();
}

bool
LowerIntrinsics::doInitialization(Module &M) {

    // Initialize list of intrinsics to lower.
    replace_.insert(Intrinsic::get_rounding);
    replace_.insert(Intrinsic::ceil);
    replace_.insert(Intrinsic::floor);
    replace_.insert(Intrinsic::round);
    replace_.insert(Intrinsic::exp2);
    replace_.insert(Intrinsic::memcpy);
    replace_.insert(Intrinsic::memset);
    replace_.insert(Intrinsic::memmove);

    assert(iLowering_ == NULL);
#if LLVM_MAJOR_VERSION < 21
    DataLayout* DL = new DataLayout(&M);
    iLowering_ = new IntrinsicLowering(*DL);
    delete DL;
#else
    iLowering_ = new IntrinsicLowering(M.getDataLayout());
#endif
    return true;
}

bool
LowerIntrinsics::doFinalization(Module& /*M*/) {
    if (iLowering_ != NULL) {
        delete iLowering_;
        iLowering_ = NULL;
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
            if (ci != NULL && ci->arg_size() != 0) {
                Function* callee = ci->getCalledFunction();
                if (callee != NULL && callee->isIntrinsic() &&
                    replace_.find(callee->getIntrinsicID()) != replace_.end()) {
                    if (callee->getIntrinsicID() == Intrinsic::get_rounding) {
                        // Replace GET_ROUNDING intrinsic with the actual
                        // constant value to avoid stupid  "if (1 == 0)"
                        // code even with full optimizations.
                        I->replaceAllUsesWith(
                            ConstantInt::get(
                                Type::getInt32Ty(BB.getContext()), 0, true));
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


/* New PM Registration */
llvm::PassPluginLibraryInfo getLowerIntrinsicsPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "LowerIntrinsics", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerVectorizerStartEPCallback(
                [](llvm::FunctionPassManager &PM, OptimizationLevel Level) {
                  PM.addPass(LowerIntrinsics());
                });
            PB.registerPipelineParsingCallback(
                [](StringRef Name, llvm::FunctionPassManager &PM,
                   ArrayRef<llvm::PassBuilder::PipelineElement>) {
                  if (Name == "lowerintrinsic") {
                    PM.addPass(LowerIntrinsics());
                    return true;
                  }
                  return false;
                });
          }};
}

#ifndef LLVM_LOWERINTRINSICS_LINK_INTO_TOOLS
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getLowerIntrinsicsPluginInfo();
}
#endif
