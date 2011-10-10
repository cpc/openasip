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
 * @file OpenCL.cc
 *
 * Main LLVM OpenCL pass. It goes thought all the modules looking
 * for kernel functions, and calls other passes as appropiate.
 *
 * @author Carlos Sanchez de La Lama 2009 (carlos.delalama@urjc.es)
 */

#include <cstdio>
#include <cstring>
#include "llvm/Function.h"
#include "llvm/Module.h"
#include "llvm/Pass.h"
#include "llvm/PassAnalysisSupport.h"
#include "AddContext.hh"
#include "GenerateWarps.hh"
#include "LoopRegions.hh"
#include "OpenCL.hh"
#include "config.h"

#define KERNEL_PREFIX "__KERNEL"

using namespace llvm;
using namespace ocl;

static cl::opt<int>
MaximumWarpWidth("max-warp-width",
                 cl::desc("Number of parallel threads to create"),
                 cl::init(-1));

namespace{
  struct OpenCL : public ModulePass {

    static char ID;
#ifdef LLVM_2_7
    OpenCL() : ModulePass(&ID) {}
#else
    OpenCL() : ModulePass(ID) {}
#endif

    virtual bool runOnModule(Module &M);

    virtual void getAnalysisUsage(AnalysisUsage &AU) const;

  };

  char OpenCL::ID = 0;
  RegisterPass<OpenCL> X("opencl",
                         "Process OpenCL kernel code");
}

static int calculateWarpWidth(int maximum_width,
                              int total_x,
                              int total_y,
                              int total_z,
                              int *width_x,
                              int *width_y,
                              int *width_z) {
    int wx, wy, wz;
    int size_x, size_y, size_z;

    if (total_x <= maximum_width) {
        size_x = total_x;
    } else {
        wx = total_x / maximum_width + (total_x % maximum_width ? 1 : 0);
        size_x = total_x / wx;
    }

    if (size_x * total_y <= maximum_width) {
        size_y = total_y;
    } else {
        wy = (((size_x * total_y) / maximum_width) +
              ((size_x * total_y) % maximum_width ? 1 : 0));
        size_y = total_y / wy;
    }
    
    if (size_x * size_y * total_z < maximum_width) {
        size_z = total_z;
    } else {
        wz = (((size_x * size_y * total_z) / maximum_width) +
              ((size_x * size_y * total_z) % maximum_width ? 1 : 0));
        size_z = total_z / wz;
    }
    
    *width_x = size_x;
    *width_y = size_y;
    *width_z = size_z;

    assert (size_x * size_y * size_z <= maximum_width);
    
    return (size_x * size_y * size_z);
}

bool OpenCL::runOnModule(Module &M) {
    for (Module::iterator i = M.begin(), e = M.end();
         i != e; ++i) {
        if (functionIsKernel(i)) {
            /* compute warp width and loop count */
            KernelSize ks = kernelSize(i);

            if (WarpWidth.empty() && LoopCount.empty()) {
                if (MaximumWarpWidth == -1) {
                    WarpWidth.push_back(ks.x);
                    WarpWidth.push_back(ks.y);
                    WarpWidth.push_back(ks.z);
                } else {
                    int width_x, width_y, width_z;
                    calculateWarpWidth(MaximumWarpWidth,
                                       ks.x, ks.y, ks.z,
                                       &width_x, &width_y, &width_z);

                    LoopCount.push_back(ks.x / width_x);
                    LoopCount.push_back(ks.y / width_y);
                    LoopCount.push_back(ks.z / width_z);

                    WarpWidth.push_back(width_x);
                    WarpWidth.push_back(width_y);
                    WarpWidth.push_back(width_z);
                }
            }

            //GenerateWarps& gw = 
            getAnalysis<GenerateWarps>(*i);
        }
    }

    return true;
}

void OpenCL::getAnalysisUsage(AnalysisUsage &AU) const {
    AU.addPreserved<AddContext>();

    AU.addPreserved<GenerateWarps>();
    AU.addRequired<GenerateWarps>();
}

bool
functionIsKernel(const Function *f) {
    if (f->getNameStr().compare(0,
                                strlen(KERNEL_PREFIX),
                                KERNEL_PREFIX) == 0)
        return true;
    
    return false;
}

KernelSize
kernelSize(const Function *F) {
    KernelSize ks = {1, 1, 1};

    sscanf(F->getNameStr().c_str(), KERNEL_PREFIX "_%d_%d_%d", &ks.x, &ks.y, &ks.z);

    return ks;
}
