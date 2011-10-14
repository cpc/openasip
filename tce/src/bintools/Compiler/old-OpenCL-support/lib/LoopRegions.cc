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
 * @file LoopRegions.cc
 *
 * LLVM pass to create a loop for each region. 
 * 
 * The loop count times the warp width will be the total ammount of 
 * times that original region is executed.
 *
 * @author Carlos Sanchez de La Lama 2009 (carlos.delalama@urjc.es)
 */

#include <cstdio>
#include "llvm/Function.h"
#include "llvm/Pass.h"
#include "llvm/PassAnalysisSupport.h"
#include "llvm/Support/CommandLine.h"
#include "AddContext.hh"
#include "KernelFunctions.hh"
#include "Rematerialize.hh"
#include "SplitRegions.hh"
#include "LoopRegions.hh"
#include "config.h"

using namespace llvm;
using namespace ocl;

cl::list<int>
LoopCount("loop-count",
          cl::desc("Loop count for region loops (x y z)"),
          cl::multi_val(3));

namespace {

    RegisterPass<LoopRegions> X("loop-regions",
                                "Create a loop for each region");
}

char LoopRegions::ID = 0;

bool LoopRegions::runOnFunction(Function &F) {

    builder = new IRBuilder<>(F.getContext());
    getAnalysis<Rematerialize>();

    if (LoopCount.size() != 3) {
        LoopCount.resize(3);
        LoopCount[0] = 1;
        LoopCount[1] = 1;
        LoopCount[2] = 1;
    }

    if (KernelFunctions.size() != 0) {
        if (find(KernelFunctions.begin(), KernelFunctions.end(),
                 F.getName()) ==
            KernelFunctions.end())
            return false;
    }

    SplitRegions &sr = getAnalysis<SplitRegions>();
    int n = sr.numberOfRegions(&F);

    /* find values used outside its defining region */
    InstructionMultimap im;

    for (int i = 0; i < n; ++i)
        findUsesOutsideRegion(im, sr.regionBody(&F, i));

    /* create loops in each region and replicate inter-region values */
    BasicBlock *allocas_bb = BasicBlock::Create(F.getContext(),
                                                F.getName() + ".loop_allocas",
                                                &F,
                                                sr.regionPrologue(&F, 0));
    loopAllocasBasicBlock_[&F] = allocas_bb;

    for (int i = 0; i < n; ++i) {
        LoopIndex li;
        BasicBlockVector &bbv = sr.regionBody(&F, i);

        li.z = generateLoops(bbv, LoopCount[2]);
        li.y = generateLoops(bbv, LoopCount[1]);
        li.x = generateLoops(bbv, LoopCount[0]);

        loopIndices_[&F].push_back(li);
    }

    replicateLoopValues(im, &F, allocas_bb);

    builder->SetInsertPoint(allocas_bb);
    builder->CreateBr(sr.regionPrologue(&F, 0));

    delete builder;
    return true;
}

void LoopRegions::getAnalysisUsage(AnalysisUsage &AU) const {
    AU.addPreserved<AddContext>();
    
    AU.addPreserved<Rematerialize>();
    AU.addRequired<Rematerialize>();

    AU.addPreserved<SplitRegions>();
    AU.addRequired<SplitRegions>();
}

LoopIndex LoopRegions::loopIndex(const Function *F, unsigned region_index) {
    assert(region_index < loopIndices_[F].size());
    
    return loopIndices_[F][region_index];
}

BasicBlock *LoopRegions::loopAllocasBasicBlock(const Function *F) {
    return loopAllocasBasicBlock_[F];
}

void LoopRegions::findUsesOutsideRegion(InstructionMultimap &im,
                                        const BasicBlockVector &bbv) {
    SplitRegions &sr = getAnalysis<SplitRegions>();
    
    int this_region_index = sr.regionIndex(bbv.front());
    
    for (BasicBlockVector::const_iterator i = bbv.begin(), e = bbv.end();
         i != e; ++i) {
        for (BasicBlock::iterator i2 = (*i)->begin(), e2 = (*i)->end();
             i2 != e2; ++i2) {
            for (Instruction::use_iterator ui = i2->use_begin(),
                     ue = i2->use_end();
                 ui != ue; ++ui) {
                Instruction *user;
#ifdef LLVM_2_7
                if ((user = dyn_cast<Instruction> (ui)) != NULL) {
#else
                if ((user = dyn_cast<Instruction> (*ui)) != NULL) {
#endif
                    if (sr.regionIndex(user->getParent()) != this_region_index)
                        im.insert(std::make_pair(i2, user));
                }
            }
        }
    }
}

Value *LoopRegions::generateLoops(BasicBlockVector &bbv, int count) {
    assert (count > 0);
    
    if (count == 1)
        return NULL;

    SplitRegions &sr = getAnalysis<SplitRegions>();

    BasicBlock *first_bb = bbv.front();
    BasicBlock *previous_bb = first_bb->getSinglePredecessor();
    assert(previous_bb);

    BasicBlock *last_bb = bbv.back();
    assert (last_bb->getTerminator()->getNumSuccessors() == 1);
    BasicBlock *next_bb = last_bb->getTerminator()->getSuccessor(0);

    int region_index = sr.regionIndex(first_bb);

    char s[16];
    snprintf(s, 16, "%d", region_index);

    /* loop entry BB created before region body */
    BasicBlock *entry = BasicBlock::Create(first_bb->getContext(),
                                           first_bb->getParent()->getName() +
                                           ".region_" + s + "_loop_entry",
                                           first_bb->getParent(),
                                           first_bb);

    /* loop exit BB created between region body and epilogue */
    BasicBlock *exit = BasicBlock::Create(first_bb->getContext(),
                                          first_bb->getParent()->getName() +
                                          ".region_" + s + "_loop_exit",
                                          next_bb->getParent(),
                                          next_bb);

    /* chain loop entry from previous BB */
    previous_bb->getTerminator()->replaceUsesOfWith(first_bb, entry);

    /* create code for loop entry block */
    builder->SetInsertPoint(entry);
#if (defined(LLVM_2_7) || defined(LLVM_2_8) || defined(LLVM_2_9))
    PHINode *value = 
        builder->CreatePHI(IntegerType::get(first_bb->getContext(), 32),
                          (std::string("r") + s + "_loop_count").c_str());;
#else
    // TODO: is this 5 correct value for the number of operands here?
    PHINode *value = 
        builder->CreatePHI(IntegerType::get(first_bb->getContext(), 32), 5,
                          (std::string("r") + s + "_loop_count").c_str());;
#endif
    Value *next_value = 
        builder->CreateAdd(value,
                           ConstantInt::get(IntegerType::get(first_bb->getContext(), 32), 1),
                          (std::string("r") + s + "_loop_count_next").c_str());
    value->addIncoming(ConstantInt::get(IntegerType::get(first_bb->getContext(), 32), 0),
                       previous_bb);
    value->addIncoming(next_value, exit);
    builder->CreateBr(first_bb);

    /* chain region body and loop exit */
    last_bb->getTerminator()->replaceUsesOfWith(next_bb, exit);

    /* create code for exit */
    builder->SetInsertPoint(exit);
    Value *end_condition =
        builder->CreateICmpULT(next_value,
                              ConstantInt::get(                                  
                                  IntegerType::get(first_bb->getContext(), 32),
                                  count),
                              (std::string("r") + s + "_loop_condition").c_str());
    builder->CreateCondBr(end_condition, entry, next_bb);

    return value;
}

void LoopRegions::replicateLoopValues(const InstructionMultimap &im,
                                      const Function *F,
                                      BasicBlock *allocas_bb) {
    SplitRegions &sr = getAnalysis<SplitRegions>();

    /* go thought all instructions used outside its defining region */
    for (InstructionMultimap::const_iterator i = im.begin(), e = im.end();
         i != e;) {

        /* create alloca big enough to store value for each iteration */
#ifdef LLVM_2_9
        const
#endif
        Type *t = i->first->getType();
        for (int j = 0; j <= 2; ++j) {
            if (LoopCount[j] > 1)
                t = ArrayType::get(t, LoopCount[j]);
        }
        builder->SetInsertPoint(allocas_bb);
        Value *alloca = builder->CreateAlloca(t,
                                             NULL,
                                             (i->first->getName() +
                                              "_loop_array").str());

        /* create store after definition */
        BasicBlock::iterator definition = i->first;
        int region_index = sr.regionIndex(definition->getParent());
        LoopIndex li = (loopIndices_[F])[region_index];
        if ((li.x != NULL) || (li.y != NULL) || (li.z != NULL)) {
          std::vector<Value *> gep_arguments;
          gep_arguments.push_back(ConstantInt::get(IntegerType::get(F->getContext(), 32), 0));
          if (li.z)
            gep_arguments.push_back(li.z);
          if (li.y)
            gep_arguments.push_back(li.y);
          if (li.x)
            gep_arguments.push_back(li.x);

          builder->SetInsertPoint(definition->getParent(), ++definition);
#ifdef LLVM_2_9
          Value *gep = builder->CreateGEP(alloca, gep_arguments.begin(),
                                         gep_arguments.end());
#else
          Value *gep = builder->CreateGEP(
              alloca, 
              llvm::ArrayRef<llvm::Value*>(
                  gep_arguments.data(),
                  gep_arguments.size()));
#endif
          builder->CreateStore(i->first, gep);
        }

        /* create load before each use */
        int n = 0;
        for (InstructionMultimap::const_iterator
                 i2 = im.equal_range(i->first).first,
                 e2 = im.equal_range(i->first).second;
             i2 != e2; ++i2) {
            Instruction *use = i2->second;
            int region_index = sr.regionIndex(use->getParent());
            LoopIndex li = (loopIndices_[F])[region_index];
            if ((li.x != NULL) || (li.y != NULL) || (li.z != NULL)) {
                std::vector<Value *> gep_arguments;
                gep_arguments.push_back(ConstantInt::get(IntegerType::get(F->getContext(), 32), 0));
                if (li.z)
                    gep_arguments.push_back(li.z);
                if (li.y)
                    gep_arguments.push_back(li.y);
                if (li.x)
                    gep_arguments.push_back(li.x);

                builder->SetInsertPoint(use->getParent(), use);
#ifdef LLVM_2_9
                Value *gep = builder->CreateGEP(alloca, gep_arguments.begin(),
                                               gep_arguments.end());
#else
                Value *gep = builder->CreateGEP(alloca, 
                                                llvm::ArrayRef<llvm::Value*>(
                                                    gep_arguments.data(),
                                                    gep_arguments.size()));

#endif
                Value *v = builder->CreateLoad(gep);
                use->replaceUsesOfWith(i2->first, v);
            }
            ++n;
        }

        /* jump over all the already processed uses of i->first */
        for (int j = 0; j < n; ++j)
            ++i;
    }
}

    
