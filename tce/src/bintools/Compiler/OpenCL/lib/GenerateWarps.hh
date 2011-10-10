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
 * @file GenerteWarps.hh
 *
 * Declarations for GenerateWarps LLVM pass.
 *
 * @author Carlos Sanchez de La Lama 2009 (carlos.delalama@urjc.es)
 */

#ifndef GENERATEWARPS_HH
#define GENERATEWARPS_HH

#include "llvm/Support/CommandLine.h"
#include "llvm/Support/IRBuilder.h"
#include "SplitRegions.hh"
#include "config.h"

typedef std::map<llvm::Value*, llvm::Value*> ValueMap;

extern llvm::cl::list<int> WarpWidth;

namespace ocl {
    struct GenerateWarps : public llvm::FunctionPass {
     
        static char ID;
#ifdef LLVM_2_7
        GenerateWarps() : llvm::FunctionPass(&ID) {}
#else
	GenerateWarps() : llvm::FunctionPass(ID) {}
#endif
        virtual bool runOnFunction(llvm::Function &F);

        virtual void getAnalysisUsage(llvm::AnalysisUsage &AU) const;

    private:

        void replicateBasicBlocks(BasicBlockVector &new_bbv,
                                  BasicBlockVector &old_bbv,
                                  ValueMap &vm,
                                  const std::string &prefix);

        void addNamePrefix(llvm::Value *v,
                           const std::string &prefix,
                           const std::string &name);

	void generateThreadPrologue(BasicBlockVector &bbv,
				    int x,
                                    int y,
                                    int z,
                                    int region_index);

	void updateContextGEP(llvm::GetElementPtrInst *gep,
			      llvm::Function *f,
			      int x, int y, int z,
			      int region_index);

        llvm::IRBuilder<>* builder;
    };

}

#endif // GENERATEWARPS_HH
