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
 * @file SplitRegions.hh
 *
 * Declarations for SplitRegions LLVM pass.
 *
 * @author Carlos Sanchez de La Lama 2009 (carlos.delalama@urjc.es)
 */

#ifndef SPLITREGIONS_HH
#define SPLITREGIONS_HH

#include <map>

#include "config.h"

typedef std::vector<llvm::BasicBlock*> BasicBlockVector;

namespace ocl {
    
    struct SplitRegions : public llvm::FunctionPass {
        
        static char ID;
#ifdef LLVM_2_7
        SplitRegions() : FunctionPass(&ID) {}
#else
        SplitRegions() : FunctionPass(ID) {}
#endif
        virtual bool runOnFunction(llvm::Function &F);

        virtual void getAnalysisUsage(llvm::AnalysisUsage &AU) const;

        int numberOfRegions(const llvm::Function *F);
        
        BasicBlockVector &regionBody(const llvm::Function *F,
                                     unsigned index);
        llvm::BasicBlock *regionPrologue(const llvm::Function *F,
                                         unsigned index);
        llvm::BasicBlock *regionEpilogue(const llvm::Function *F,
                                         unsigned index);

        int regionIndex(const llvm::BasicBlock *bb);
        
    private:

        llvm::BasicBlock *createRegionPrologue(BasicBlockVector &bb,
                                               const std::string &name);
        llvm::BasicBlock *createRegionEpilogue(BasicBlockVector &bb,
                                               const std::string &name);
        
        std::map<const llvm::Function *,
                 std::vector<BasicBlockVector> > regionBodies_;
        std::map<const llvm::Function *, BasicBlockVector> regionPrologues_;
        std::map<const llvm::Function *, BasicBlockVector> regionEpilogues_;
    };

}

#endif // SPLITREGIONS_HH
