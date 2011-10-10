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
 * @file LoopRegions.hh
 *
 * Declarations for LoopRegions LLVM pass.
 *
 * @author Carlos Sanchez de La Lama 2009 (carlos.delalama@urjc.es)
 */

#ifndef LOOPREGIONS_HH
#define LOOPREGIONS_HH

#include "llvm/Support/IRBuilder.h"

#include "SplitRegions.hh"

#include "config.h"

typedef std::multimap<llvm::Instruction*,
                      llvm::Instruction*> InstructionMultimap;
struct LoopIndex {
    llvm::Value *x, *y, *z;
};

extern llvm::cl::list<int> LoopCount;

namespace ocl {

    struct LoopRegions : public llvm::FunctionPass {
     
        static char ID;
#ifdef LLVM_2_7
        LoopRegions() : FunctionPass(&ID) {}
#else
	LoopRegions() : FunctionPass(ID) {}
#endif

        virtual bool runOnFunction(llvm::Function &F);

        virtual void getAnalysisUsage(llvm::AnalysisUsage &AU) const;

        LoopIndex loopIndex(const llvm::Function *F, unsigned region_index);

        llvm::BasicBlock *loopAllocasBasicBlock(const llvm::Function *F);

    private:

        void findUsesOutsideRegion(InstructionMultimap &im,
                                   const BasicBlockVector &bbv);

        llvm::Value* generateLoops(BasicBlockVector &bbv, int count);

        void replicateLoopValues(const InstructionMultimap &im,
                                 const llvm::Function *F,
                                 llvm::BasicBlock *allocas_bb);

        std::map<const llvm::Function *,
                 std::vector<LoopIndex> > loopIndices_;

        std::map<const llvm::Function *,
                 llvm::BasicBlock *> loopAllocasBasicBlock_;

        llvm::IRBuilder<>* builder;


    };

}

#endif // LOOPREGIONS_HH
