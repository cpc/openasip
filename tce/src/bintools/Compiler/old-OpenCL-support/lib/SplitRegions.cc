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
 * @file SplitRegions.cc
 *
 * LLVM pass split OpenCL kernel functions into smaller "regions".
 * A region is the minimal piece of code that will be replicated
 * to generate the different workitems later on. Regions are guaranteed
 * to have unique "head" and "tail" BBs (first of last of region
 * ilist).
 *
 * @todo Move stuff from the namespace to the class (?)
 * @todo Add systemtests.
 * @todo Make separate regions for top-level loops (with LoopInfo).
 * @todo Add support for barriers in conditional paths.
 * @todo Add sanity checks:
 * @todo   1) Allocas should only be on first BB on entry
 * @todo   2) Inter-region branches should ony go throught region frontiers.
 * @todo   3) Only forward inter-region branches should exist.
 * @todo      (generalization: regions have to be unconditional)
 *
 * @author Carlos Sanchez de La Lama 2009 (carlos.delalama@urjc.es)
 */

#define DEBUG_TYPE "split-regions"

#include <cstdio>
#include <vector>
#include <algorithm>
#include "llvm/ADT/Statistic.h"
#include "llvm/Function.h"
#include "llvm/Instructions.h"
#include "llvm/Pass.h"
#include "llvm/PassAnalysisSupport.h"
#include "llvm/Transforms/Utils/UnifyFunctionExitNodes.h"
#include "AddContext.hh"
#include "SplitRegions.hh"

STATISTIC(NumberOfBarriers, "Number of barriers found");
STATISTIC(NumberOfRegions, "Number of regions the kernel was split into");

using namespace llvm;
using namespace ocl;

typedef std::vector<BasicBlock*> BasicBlockVector;

namespace {
    
    RegisterPass<SplitRegions> X("split-regions",
                                 "Split OpenCL kernel into regions");
}

char SplitRegions::ID = 0;

static AllocaInst *findLastAlloca(BasicBlock *bb) {
    AllocaInst *last = NULL;
    for (BasicBlock::iterator i = bb->begin(), e = bb->end();
         i != e; ++i) {
        AllocaInst *alloca;
        if ((alloca = dyn_cast<AllocaInst>(i)) != NULL)
            last = alloca;
    }
    
    return last;
}

static CallInst *findBarrier(BasicBlock *bb) {
    for (BasicBlock::iterator i = bb->begin(), e = bb->end();
         i != e; ++i) {
        CallInst *call;
        if ((call = dyn_cast<CallInst> (i)) != NULL) {
            Function *f = call->getCalledFunction();
            if (f != NULL) {
                if (f->getNameStr() == "barrier")
                    return call;
            }
        }
    }
    
    return NULL;
}

bool SplitRegions::runOnFunction(Function &F) {
    // we need kernels to have an unique exit BB
    //UnifyFunctionExitNodes& ufen = 
    getAnalysis<UnifyFunctionExitNodes>();

    // generate a list of incoming function BBs
    BasicBlockVector bbv;
    for (Function::iterator i = F.begin(), e = F.end();
         i != e; ++i)
        bbv.push_back(i);
    
    // region 0 is for allocas
    BasicBlock *first_bb = bbv.front();
    AllocaInst *alloca = findLastAlloca(first_bb);
    BasicBlock::iterator splitpoint;
    if (alloca) {
        splitpoint = alloca;
        ++splitpoint;
    } else
        splitpoint = first_bb->begin();
        
    regionBodies_[&F].push_back(BasicBlockVector());
    BasicBlockVector &allocas_region = regionBodies_[&F].back();
    allocas_region.push_back(first_bb);
    if (splitpoint->isTerminator())
        bbv.erase(bbv.begin());
    else
        bbv.front() = first_bb->splitBasicBlock(splitpoint);
        
    BasicBlock *prologue;
    prologue = createRegionPrologue(allocas_region,
                                    (F.getName() + ".region_0_prologue").str());
    regionPrologues_[&F].push_back(prologue);

    BasicBlock *epilogue;
    epilogue = createRegionEpilogue(allocas_region,
                                    (F.getName() + ".region_0_epilogue").str());
    regionEpilogues_[&F].push_back(epilogue);
        
    ++NumberOfRegions;
    
    // now split at barriers
    regionBodies_[&F].push_back(BasicBlockVector());
    ++NumberOfRegions;
    BasicBlockVector *current_region = &regionBodies_[&F].back();
    for (BasicBlockVector::iterator i = bbv.begin(), e = bbv.end();
         i != e; ++i) {
        current_region->push_back(*i);
        
        CallInst *barrier = findBarrier(*i);
        if (barrier) {
            ++NumberOfBarriers;
            *i = (*i)->splitBasicBlock(++BasicBlock::iterator(barrier));
            barrier->eraseFromParent();

            char s[16];
            snprintf(s, 16, "%ld", regionBodies_[&F].size() - 1L);
            BasicBlock *bb;
            bb = createRegionPrologue(*current_region,
                                      (F.getName() + ".region_" +
                                       s + "_prologue").str());
            regionPrologues_[&F].push_back(bb);
            bb = createRegionEpilogue(*current_region,
                                      (F.getName() + ".region_" +
                                       s + "_epilogue").str());
            regionEpilogues_[&F].push_back(bb);
            
            --i;
            regionBodies_[&F].push_back(BasicBlockVector());
            ++NumberOfRegions;
            current_region = &regionBodies_[&F].back();
        }
    }

    // create last region boundaries
    char s[16];
    snprintf(s, 16, "%ld", regionBodies_[&F].size() - 1L);
    BasicBlock *bb;
    bb = createRegionPrologue(*current_region,
                              (F.getName() + ".region_" +
                               s + "_prologue").str());
    regionPrologues_[&F].push_back(bb);
    bb = createRegionEpilogue(*current_region,
                              (F.getName() + ".region_" +
                               s + "_epilogue").str());
    regionEpilogues_[&F].push_back(bb);

    assert(regionPrologues_[&F].size() == regionBodies_[&F].size());
    assert(regionEpilogues_[&F].size() == regionBodies_[&F].size());
    
    return true;
}

void SplitRegions::getAnalysisUsage(AnalysisUsage &AU) const {
    AU.addRequired<UnifyFunctionExitNodes>();

    AU.addPreserved<AddContext>();
}

int SplitRegions::numberOfRegions(const Function *F) {
    return regionBodies_[F].size();
}

BasicBlockVector &SplitRegions::regionBody(const Function *F, unsigned index) {
    assert(index < regionBodies_[F].size());

    return regionBodies_[F][index];
}

BasicBlock *SplitRegions::regionPrologue(const Function *F, unsigned index) {
    assert(index < regionPrologues_[F].size());

    return regionPrologues_[F][index];
}

BasicBlock *SplitRegions::regionEpilogue(const Function *F, unsigned index) {
    assert(index < regionEpilogues_[F].size());

    return regionEpilogues_[F][index];
}

int SplitRegions::regionIndex(const BasicBlock *bb) {
    for (std::map<const llvm::Function *,
             std::vector<BasicBlockVector> >::iterator i = regionBodies_.begin(),
             e = regionBodies_.end();
         i != e; ++i) {
        for (unsigned j = 0; j < i->second.size(); ++j) {
            if (find(i->second[j].begin(), i->second[j].end(), bb) !=
                i->second[j].end())
                return j;
        }
    }

    return -1;
}
                 

BasicBlock *SplitRegions::createRegionPrologue(BasicBlockVector &region,
                                               const std::string &name) {
    BasicBlock *bb = region.front();
    
    region.front() = bb->splitBasicBlock(bb->begin());
    region.front()->takeName(bb);
    bb->setName(name);
    
    return bb;
}

BasicBlock *SplitRegions::createRegionEpilogue(BasicBlockVector &region,
                                               const std::string &name) {
    BasicBlock *bb = region.back();
    BasicBlock::iterator i(bb->getTerminator());
    
    return bb->splitBasicBlock (i, name);
}
