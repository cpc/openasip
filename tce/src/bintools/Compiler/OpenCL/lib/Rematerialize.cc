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
 * @file Rematerialize.cc
 *
 * This LLVM pass duplicates instructions whose results are used
 * on a region different from that they are defined on. The
 * goal is to reduce the number of "loop array"-allocated variables
 * in LoopRegions pass.
 *
 * @author Carlos Sanchez de La Lama 2009 (carlos.delalama@urjc.es)
 */

#include <list>
#include "llvm/Instructions.h"
#include "llvm/Function.h"
#include "llvm/Pass.h"
#include "llvm/PassAnalysisSupport.h"
#include "AddContext.hh"
#include "Rematerialize.hh"
#include "config.h"

using namespace llvm;
using namespace ocl;

typedef std::list<Instruction*> InstructionList;

namespace {

    RegisterPass<Rematerialize> X("rematerialize",
                                  "Rematerialize inter-region instructions");
}

char Rematerialize::ID = 0;

bool Rematerialize::runOnFunction(Function &F) {

    SplitRegions &sr = getAnalysis<SplitRegions>();
    int n = sr.numberOfRegions(&F);
    
    /* find values used outside its defining region */
    InstructionDeque id;
    
    for (int i = 0; i < n; ++i)
        findInstructionsToRematerialize(id, sr.regionBody(&F, i));

    while (!id.empty()) {
        Instruction *i = id.front();
        id.pop_front();

        if (isa<AllocaInst>(i))
            continue;

        if (isa<PHINode>(i))
            continue;

        if (LoadInst *load = dyn_cast<LoadInst>(i)) {
            if (!loadFromContext(load))
                continue;
        }

        /* rematerialize instruction before users on other regions */
        InstructionList il;
        for (Instruction::use_iterator ui = i->use_begin(), ue = i->use_end();
             ui != ue; ++ui) {
#ifdef LLVM_2_7
            if (Instruction *user = dyn_cast<Instruction>(ui))
#else
            if (Instruction *user = dyn_cast<Instruction>(*ui))
#endif
                il.push_back(user);
        }

        for (InstructionList::iterator i2 = il.begin(), e2 = il.end();
             i2 != e2; ++i2) {
            Instruction *user = *i2;
            if (sr.regionIndex(i->getParent()) !=
                sr.regionIndex(user->getParent())) {
                
                /* this use is out of defining region */
                Instruction *ni = i->clone();
                ni->insertBefore(user);
                user->replaceUsesOfWith(i, ni);
                
                /* now check new instruction operands for outside uses */
                for (Instruction::op_iterator oi = ni->op_begin(),
                         oe = ni->op_end();
                     oi != oe; ++oi) {
                    if (Instruction *o = dyn_cast<Instruction>(oi)) {
                        if (isToRematerialize(o))
                            id.push_back(o);
                    }
                }
            }
        }
    }
    
    return true;
}
   
void Rematerialize::getAnalysisUsage(AnalysisUsage &AU) const {
    AU.addPreserved<AddContext>();

    AU.addPreserved<SplitRegions>();
    AU.addRequired<SplitRegions>();
}

void
Rematerialize::findInstructionsToRematerialize(InstructionDeque &id,
                                               const BasicBlockVector &bbv) {
    for (BasicBlockVector::const_iterator i = bbv.begin(), e = bbv.end();
         i != e; ++i) {
        for (BasicBlock::iterator i2 = (*i)->begin(), e2 = (*i)->end();
             i2 != e2; ++i2) {
            if (isToRematerialize(i2))
                id.push_back(i2);
        }
    }
}

bool Rematerialize::isToRematerialize(const Instruction *i) {
    SplitRegions &sr = getAnalysis<SplitRegions>();
    
    int this_region_index = sr.regionIndex(i->getParent());

#ifdef LLVM_2_7
    for (Instruction::use_const_iterator ui = i->use_begin(),ue = i->use_end();
#else
    for (Instruction::const_use_iterator ui = i->use_begin(),ue = i->use_end();
#endif
         ui != ue; ui++) {
        const Instruction *user;
#ifdef LLVM_2_7
        if ((user = dyn_cast<Instruction>(ui)) != NULL) {
#else
        if ((user = dyn_cast<Instruction>(*ui)) != NULL) {
#endif
            if (sr.regionIndex(user->getParent()) != this_region_index)
                return true;
        }
    }

    return false;
}
        
bool Rematerialize::loadFromContext(const LoadInst *load) {

    const Value *v = load->getPointerOperand();
    while(const GetElementPtrInst *g = dyn_cast<GetElementPtrInst>(v))
        v = g->getPointerOperand();

    if (const Argument *a = dyn_cast<Argument>(v)) {
        if (a->getArgNo() == 0)
            return true;
    }

    return false;
}
