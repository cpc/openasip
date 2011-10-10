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
 * @file Flatten.cc
 *
 * LLVM pass that adds "noinline" attribute to OpenCL kernels and
 * "alwaysinline" attribute to all non-kernel functions (preparing the
 * code for later kernel flattening).
 *
 * @author Carlos Sanchez de La Lama 2009 (carlos.delalama@urjc.es)
 */

#include "llvm/Attributes.h"
#include "llvm/Function.h"
#include "llvm/Pass.h"
#include "OpenCL.hh"

#include "config.h"

using namespace llvm;

namespace {
    struct Flatten : public FunctionPass {

        static char ID;
#ifdef LLVM_2_7
        Flatten() : FunctionPass(&ID) {}
#else
        Flatten() : FunctionPass(ID) {}
#endif
        virtual bool runOnFunction(Function &F);

    };

    char Flatten::ID = 0;
    RegisterPass<Flatten> X("flatten-kernels",
                            "Prapare inlining of kernel callees");

}

bool Flatten::runOnFunction(Function &F) {
    if (functionIsKernel(&F))
        F.addFnAttr(Attribute::NoInline);
    else
        F.addFnAttr(Attribute::AlwaysInline);

    return true;
}

        
