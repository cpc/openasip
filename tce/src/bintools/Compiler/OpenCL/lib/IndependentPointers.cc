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
 * @file IndependentPointers.cc
 *
 * This LLVM pass makes all global buffer access to happen through
 * a copy of the global pointer parameter to the kernel function. The
 * copy of the pointer is done using a special pseudo-asm to avoid
 * if being removed by LLVM optimization passes.
 * This allows later passes to replicate this pointer, ensuring access
 * to global buffers is done through different pointers per each
 * work-item.
 * This pass has to be manually run, it is not fired from OpenCL
 * main pass (as it would prevent native execution).
 *
 * @author Carlos Sanchez de La Lama 2009 (carlos.delalama@urjc.es)
 */

#include "llvm/Attributes.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Function.h"
#include "llvm/InlineAsm.h"
#include "llvm/Pass.h"
#include "llvm/Support/IRBuilder.h"
#include "OpenCL.hh"
#include "config.h"

#define PSEUDOASM ".pointer_category"
#define CONSTRAINTS "=r,r"

using namespace llvm;

namespace {
    struct IndependentPointers : public FunctionPass {

        static char ID;
#ifdef LLVM_2_7
        IndependentPointers() : FunctionPass(&ID) {}
#else
        IndependentPointers() : FunctionPass(ID) {}
#endif
        virtual bool runOnFunction(Function &F);

    };

    char IndependentPointers::ID = 0;
    RegisterPass<IndependentPointers> X("independent-pointers",
                            "Separate pointers for global buffers access");

}

bool IndependentPointers::runOnFunction(Function &F) {
    if (!functionIsKernel(&F))
        return false;

    IRBuilder<> *builder = new IRBuilder<>(F.getContext());

    for (Function::arg_iterator i = F.arg_begin(), e = F.arg_end();
         i != e; ++i) {

#ifdef LLVM_2_9
        const
#endif
        PointerType *t = dyn_cast<PointerType> (i->getType());
        if (t == NULL)
            continue;
#ifdef LLVM_2_9
        std::vector<const Type*> parameters;
#else
        std::vector<Type*> parameters;
#endif
        parameters.push_back(t);
#ifdef LLVM_2_9
        FunctionType *ft = FunctionType::get(t, parameters, false);
#else
        FunctionType *ft = FunctionType::get(
            t, 
            llvm::ArrayRef<llvm::Type*>(parameters.data(), parameters.size()), 
            false);
#endif

        InlineAsm *ia = InlineAsm::get(ft, PSEUDOASM, CONSTRAINTS, false);

        builder->SetInsertPoint(&F.getEntryBlock(), F.getEntryBlock().begin());
        CallInst *c = builder->CreateCall(ia, i, i->getName() + ".independent");

        i->replaceAllUsesWith(c);
        c->replaceUsesOfWith(c, i);
    }

    return true;
}

        
