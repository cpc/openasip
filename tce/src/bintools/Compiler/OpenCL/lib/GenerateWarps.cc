/*
    Copyright (c) 2002-2010 Tampere University of Technology.

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
 * @file GenerateWarps.cc
 *
 * LLVM pass to generate warps in each region. A warp is a n-width
 * replication region code where internal control flow structure
 * is maintained.
 *
 * @author Carlos Sanchez de La Lama 2009-2010 (carlos.delalama@urjc.es)
 */

#define DEBUG_TYPE "generate-warps"

#include <cstdio>
#include <algorithm>
#include <map>
#include "llvm/ADT/Statistic.h"
#include "llvm/BasicBlock.h"
#include "llvm/Function.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Module.h"
#include "llvm/Pass.h"
#include "llvm/PassAnalysisSupport.h"
#include "llvm/Support/CommandLine.h"
#include "OpenCL.hh"
#include "AddContext.hh"
#include "KernelFunctions.hh"
#include "LoopRegions.hh"
#include "SplitRegions.hh"
#include "ThreadContext.hh"
#include "GenerateWarps.hh"

STATISTIC(WarpsCreated, "Total number of warps created");
STATISTIC(BasicBlocksCreated, "Total number of basic blocks created");

using namespace llvm;
using namespace ocl;

cl::list<int>
WarpWidth("warp-width",
          cl::desc("Number of times each region code is to be replicated (x y z)"),
          cl::multi_val(3));

namespace {

    RegisterPass<GenerateWarps> X("generate-warps",
                                  "Replicate region code");
}

char GenerateWarps::ID = 0;

//#define ZIP_WARPS

#ifdef ZIP_WARPS
static bool
zipWarps(Function &F, SplitRegions &sr) {
    bool r = false;

    for (int i = 0; i < sr.numberOfRegions(&F); ++i) {
	if (sr.regionBody(&F, i).size() == 1) {
	    // single BB region found
	    r = true;
	    
	    BasicBlockVector bbv;
	    BasicBlock *bb = sr.regionBody(&F, i)[0];

	    if (bb->getTerminator()->getNumSuccessors() != 1)
		break;
	    
	    while ((bb->getTerminator()->getNumSuccessors() == 1) &&
		   (bb != sr.regionEpilogue(&F, i))) {
		bbv.push_back(bb);
		bb = bb->getTerminator()->getSuccessor(0);
	    }

	    bb = BasicBlock::Create(sr.regionEpilogue(&F, i)->getContext(),
				    "",
				    &F,
				    sr.regionEpilogue(&F, i));

	    bb->getInstList().push_back(bbv.back()->
					getTerminator()->
					clone());

	    bool empty = false;
	    do  {
		for (BasicBlockVector::iterator i = bbv.begin(), e = bbv.end();
		     i != e; ++i) {
		    empty = true;
		    if (! (*i)->empty()) {
			empty = false;

			if (! (*i)->front().isTerminator())
			    (*i)->front().moveBefore(bb->getTerminator());
			else
			    (*i)->front().eraseFromParent();
		    }
		}
	    } while (!empty);

	    bbv[0]->getSinglePredecessor()->
	      getTerminator()->replaceUsesOfWith(bbv[0],
						 bb);
	    
	    for (BasicBlockVector::iterator i = bbv.begin(), e = bbv.end();
		 i != e; ++i)
		(*i)->eraseFromParent();
	}
    }
    
    return r;
}
#endif

static void 
replaceReferences(BasicBlockVector &bbv,
                  ValueMap &vm) {
    for (BasicBlockVector::iterator i = bbv.begin(), e = bbv.end();
         i != e; ++i) {
        for (BasicBlock::iterator i2 = (*i)->begin(), e2 = (*i)->end();
             i2 != e2; ++i2) {
            for (ValueMap::const_iterator vi = vm.begin(), ve = vm.end();
                 vi != ve; vi++) {
                assert(vi->first != NULL);
                assert(vi->second != NULL);
                i2->replaceUsesOfWith(vi->first, vi->second);
            }
        }
    }
}

static bool 
isReplicable(const Instruction& /*i*/) {
    return true;
}

bool 
GenerateWarps::runOnFunction(Function& F) {

    builder = new IRBuilder<>(F.getContext());

    if (WarpWidth.size() != 3)
        return false;

    if (KernelFunctions.size() != 0) {
      if (find(KernelFunctions.begin(), KernelFunctions.end(),
               F.getName()) ==
          KernelFunctions.end())
        return false;
    }

    LoopRegions &lr = getAnalysis<LoopRegions>();
    SplitRegions &sr = getAnalysis<SplitRegions>();

    int warp_width_x = WarpWidth[0];
    int warp_width_y = WarpWidth[1];
    int warp_width_z = WarpWidth[2];

    // replicate code for each workitem
    for (int z = warp_width_z - 1; z >= 0; --z) {
        for (int y = warp_width_y - 1; y >= 0; --y) {
            for (int x = warp_width_x - 1; x >= 0; --x) {

                if (x == 0 && y == 0 && z == 0)
                    continue;

                ValueMap vm;

//                 unsigned id = ((x & 0x0FF) |
//                                ((z & 0x0FF) << 8) |
//                                ((y & 0x0FF) << 16));
                
                // replicate loop allocas
                BasicBlockVector loop_allocas_bbv;
                loop_allocas_bbv.push_back(lr.loopAllocasBasicBlock(&F));
                BasicBlockVector bbv;
                char s[64];
                snprintf(s, 64, "__wi_%03d_%03d_%03d_la", x, y, z);
                replicateBasicBlocks(bbv, loop_allocas_bbv, vm, s);
                replaceReferences(bbv, vm);
                
                // replicate each region
                for (int i = 0; i < sr.numberOfRegions(&F); ++i) {
                    BasicBlockVector bbv;
                    char s[64];
                    snprintf(s, 64, "__wi_%03d_%03d_%03d_r%d", x, y, z, i);
                    replicateBasicBlocks(bbv, sr.regionBody(&F, i), vm, s);
                    replaceReferences(bbv, vm);
                    generateThreadPrologue(bbv,
                                           x, y, z,
                                           i);

                }
                ++WarpsCreated;
            }
        }
    }

    // process original code
    BasicBlock *la_bb = lr.loopAllocasBasicBlock(&F);
    for (BasicBlock::iterator i = la_bb->begin(), e = la_bb->end();
         i != e; ++i)
        addNamePrefix(i, "__wi_000_000_000_la", i->getName());

    for (int j = 0; j < sr.numberOfRegions(&F); ++j) {
        char s[64];
        snprintf(s, 64, "__wi_000_000_000_r%d", j);
        BasicBlockVector &bbv = sr.regionBody(&F, j);
        for (BasicBlockVector::iterator i = bbv.begin(), e = bbv.end();
             i != e; ++i) {
            for (BasicBlock::iterator i2 = (*i)->begin(), e2 = (*i)->end();
                 i2 != e2; ++i2) {
		addNamePrefix(i2, s, i2->getName());
	    }
        }
	generateThreadPrologue(sr.regionBody(&F, j), 0, 0, 0, j);
    }

    delete builder;

#ifdef ZIP_WARPS
    zipWarps(F, sr);
#endif

    return true;
}

void GenerateWarps::getAnalysisUsage(AnalysisUsage &AU) const {
    AU.addPreserved<AddContext>();

    AU.addPreserved<LoopRegions>();
    AU.addRequired<LoopRegions>();

    AU.addPreserved<SplitRegions>();
    AU.addRequired<SplitRegions>();
}

void GenerateWarps::replicateBasicBlocks(BasicBlockVector &new_bbv,
                                         BasicBlockVector &old_bbv,
                                         ValueMap &vm,
                                         const std::string &prefix) {
    BasicBlock *next_of_old_bbv = ++Function::iterator(old_bbv.back());

    for (BasicBlockVector::const_iterator i = old_bbv.begin(),
             e = old_bbv.end();
         i != e; ++i) {

        BasicBlock *bb = BasicBlock::Create((*i)->getContext(),
                                            "",
                                            (*i)->getParent(),
                                            next_of_old_bbv);
        ++BasicBlocksCreated;

        new_bbv.push_back(bb);
        vm.insert(std::make_pair((*i), bb));

        // replicate all replicable instructions to new BB
        for (BasicBlock::iterator i2 = (*i)->begin(), e2 = (*i)->end();
             i2 != e2; ++i2) {
            if (isReplicable(*i2)) {
                Instruction *c = i2->clone();
                addNamePrefix(c, prefix, i2->getName());
                vm.insert(std::make_pair(i2, c));
                bb->getInstList().push_back(c);
            }
        }
    }
    
    // make original code jump to replicated one
    vm.erase(old_bbv.back()->getTerminator()); // this is not required
    old_bbv.back()->getTerminator()->eraseFromParent();
    builder->SetInsertPoint(old_bbv.back());
    builder->CreateBr(new_bbv.front());
}

void GenerateWarps::addNamePrefix(Value *v,
                                  const std::string &prefix,
                                  const std::string &name) {
    if (v->getType()->getTypeID() != Type::VoidTyID) {
        if (name.empty())
            v->setName(prefix);
        else
            v->setName(prefix + "." + name);
    }
}

void GenerateWarps::generateThreadPrologue(BasicBlockVector &bbv,
                                           int x,
                                           int y,
                                           int z,
                                           int region_index) {
    BasicBlock *bb = bbv.front();

    Instruction *i = bb->getFirstNonPHI();

    assert(i);

    builder->SetInsertPoint(bb, i);

    // get loop indices
    LoopRegions &lr = getAnalysis<LoopRegions>();
    LoopIndex index = lr.loopIndex(bb->getParent(), region_index);

    Value *local_x;
    Value *local_y;
    Value *local_z;

    if (index.x != NULL) {
        Value *v = builder->CreateMul(index.x,
                                      ConstantInt::get(IntegerType::get(bb->getContext(), 32),
                                                       WarpWidth[0]));
        local_x = builder->CreateAdd(v,
                                     ConstantInt::get(IntegerType::get(bb->getContext(), 32), 
                                                      x));
    }
    else
        local_x = ConstantInt::get(IntegerType::get(bb->getContext(), 32), x);

    if (index.y != NULL) {
        Value *v = builder->CreateMul(index.y,
                                      ConstantInt::get(IntegerType::get(bb->getContext(), 32),
                                                       WarpWidth[1]));
        local_y = builder->CreateAdd(v,
                                     ConstantInt::get(IntegerType::get(bb->getContext(), 32), y));
    }
    else
        local_y = ConstantInt::get(IntegerType::get(bb->getContext(), 32), y);

    if (index.z != NULL) {
        Value *v = builder->CreateMul(index.z,
                                      ConstantInt::get(IntegerType::get(bb->getContext(), 32),
                                                       WarpWidth[2]));
        local_z = builder->CreateAdd(v,
                                     ConstantInt::get(IntegerType::get(bb->getContext(), 32), z));
    }
    else
        local_z = ConstantInt::get(IntegerType::get(bb->getContext(), 32), z);

    Function::ArgumentListType::reverse_iterator j = bb->getParent()->getArgumentList().rbegin();

    Value *param_x = &(*(++j));
    Value *param_y = &(*(++j));
    Value *param_z = &(*(++j));

    for (BasicBlockVector::iterator i = bbv.begin(), e = bbv.end(); i != e; ++i) {
	for (BasicBlock::iterator i2 = (*i)->begin(), e2 = (*i)->end(); i2 != e2; ++i2) {
	    i2->replaceUsesOfWith(param_x, local_x);
	    i2->replaceUsesOfWith(param_y, local_y);
	    i2->replaceUsesOfWith(param_z, local_z);
	}
    }
}
