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
 * @file AddContext.hh
 *
 * Declarations for AddContext LLVM pass.
 *
 * @author Carlos Sanchez de La Lama 2009 (carlos.delalama@urjc.es)
 */

#include <map>
#include <vector>
#include "llvm/Type.h"

#include "config.h"

typedef std::vector<llvm::Function*> FunctionVector;
typedef std::map<llvm::Function*, llvm::Function*> FunctionMap;

namespace ocl {
    struct AddContext : public llvm::ModulePass {
        
        static char ID;
#ifdef LLVM_2_7
        AddContext() : ModulePass(&ID) {}
#else
        AddContext() : ModulePass(ID) {}
#endif
        virtual bool runOnModule(llvm::Module &M);

        virtual void getAnalysisUsage(llvm::AnalysisUsage &AU) const;
        
        const llvm::Type *threadContextType(void);        
    };
    
}
