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
 * @file AddContext.cc
 *
 * LLVM pass to add special "workitem context" parameter to each function
 * in a module. This parameter is a pointer to a context structure
 * where special per-workitem information (such as id) is stored.
 *
 * @author Carlos Sanchez de La Lama 2009 (carlos.delalama@urjc.es)
 */

#define DEBUG_TYPE "add-context"

#include "llvm/ADT/Statistic.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Function.h"
#include "llvm/Instructions.h"
#include "llvm/Module.h"
#include "llvm/Pass.h"
#include "llvm/PassAnalysisSupport.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "SplitRegions.hh"
#include "ThreadContext.hh"
#include "AddContext.hh"
#include "config.h"

STATISTIC(NumberOfFunctions,
          "Number of functions the context parameter was added to");

using namespace llvm;
using namespace ocl;

static cl::list<std::string>
ExternalFunctions("external-functions",
                  cl::desc("External non-OpenCL functions used in kernel"),
                  cl::CommaSeparated);

namespace {
    
    RegisterPass<AddContext> X("add-context",
                               "Add per-WI context to kernel functions");
}

char AddContext::ID = 0;

bool AddContext::runOnModule(Module &M) {
        
    FunctionVector fv;
    FunctionMap fm;

    // construct a vector with functions to add the context to
    for (Module::iterator i = M.begin(), e = M.end();
         i != e; ++i) {
        if (find(ExternalFunctions.begin(), ExternalFunctions.end(),
                 i->getName()) !=
            ExternalFunctions.end())
            continue;
        if (i->isIntrinsic())
            continue;
#if defined(__APPLE__) && defined(__ppc__)
	std::string s = i->getName();
	int prefix_end = 2;
	int suffix_begin = s.find ("$LDBL", 0) - prefix_end;
	if (suffix_begin > 2) {
	  if (find(ExternalFunctions.begin(), ExternalFunctions.end(),
		   s.substr(prefix_end, suffix_begin)) !=
	      ExternalFunctions.end())
	    continue;
	}
#endif

        fv.push_back(i);
    }

#ifdef LLVM_2_9
    OpaqueType* t = OpaqueType::get(M.getContext());
#endif
    // create the new functions
    for (FunctionVector::iterator i = fv.begin(), e = fv.end();
         i != e; ++i) {
        Function *f = *i;
#ifdef LLVM_2_9
        std::vector<const Type*> parameters;
#else
        std::vector<Type*> parameters;
#endif
        for (Function::arg_iterator i = f->arg_begin(), e = f->arg_end();
             i != e; ++i)
            parameters.push_back(i->getType());

        parameters.push_back(IntegerType::get(M.getContext(), 32));
        parameters.push_back(IntegerType::get(M.getContext(), 32));
        parameters.push_back(IntegerType::get(M.getContext(), 32));

#ifdef LLVM_2_9
        parameters.push_back(PointerType::get(t, 0));
#else
        // TODO: this is untested.
        parameters.push_back(
            PointerType::get(StructType::create(M.getContext()), 0));
#endif
        
        bool vararg = f->getFunctionType()->isVarArg();
#ifdef LLVM_2_9
        const
#endif
        Type *return_type = f->getReturnType();

#ifdef LLVM_2_9
        FunctionType *new_function_type = 
            FunctionType::get(return_type, parameters, vararg);
#else
        FunctionType *new_function_type = 
            FunctionType::get(return_type, llvm::ArrayRef<Type*>(parameters.data(), parameters.size()), vararg);
#endif
        Function *nf = Function::Create(new_function_type,
                                        f->getLinkage(),
                                        "",
                                        f->getParent());
        nf->takeName(f);
//         nf->copyAttributesFrom(f);
        for (unsigned int i = 1; i <= f->getFunctionType()->getNumParams(); ++i)
            nf->addAttribute(i, f->getAttributes().getParamAttributes(i));
        nf->addAttribute(nf->getFunctionType()->getNumParams(),
                         Attribute::NoAlias);

        // move all basic blocks from old function into new one
        nf->getBasicBlockList().splice(nf->begin(),
                                       f->getBasicBlockList());

        // replace usage of old function parameters with new ones
        Function::arg_iterator j = nf->arg_begin();
        for (Function::arg_iterator i = f->arg_begin(), e = f->arg_end();
             i != e; ++i) {
            i->replaceAllUsesWith(j);
            j->takeName(i);
            ++j;
        }
        j->setName("_x");
        ++j;
        j->setName("_y");
        ++j;
        j->setName("_z");
        ++j;
        j->setName("_thread_context");

        fm.insert(std::make_pair(f, nf));

        ++NumberOfFunctions;
    }

    // replace calls to old function with calls to new ones
    for (FunctionVector::iterator i = fv.begin(), e = fv.end();
         i != e; ++i) {
        Function *f = *i;

        while (!f->use_empty()) {
            CallInst *call = dyn_cast<CallInst> (f->use_back());
            assert(call != NULL);

            // create argument list adding _thread_context at the beginning
            std::vector<Value *> arguments;
            for (unsigned i = 0; i < call->getNumArgOperands(); ++i)
                arguments.push_back(call->getArgOperand(i));
            arguments.resize(arguments.size() + 4);
            Function::ArgumentListType::reverse_iterator a = 
                call->getParent()->getParent()->getArgumentList().rbegin();
            for (int i = 1; i <= 4; ++i) {
                arguments[arguments.size() - i] = &(*a);
                ++a;
            }

#ifdef LLVM_2_9
            ReplaceInstWithInst(call,
                                CallInst::Create(
                                    fm[f], arguments.begin(),
                                    arguments.end()));
#else
            ReplaceInstWithInst(call,
                                CallInst::Create(
                                    fm[f], llvm::ArrayRef<llvm::Value*>(
                                        arguments.data(), 
                                        arguments.size())));
#endif
        }
        
        f->eraseFromParent();
    }

#ifdef LLVM_2_9
    // add the named type for cleaner code
    // I hope this is only for clarity as it's not available in LLVM 3.0.
    // TODO: find the corresponding way.
    M.addTypeName("struct._thread_context", ThreadContext::get(M.getContext()));
#endif

    return true;
}

void AddContext::getAnalysisUsage(AnalysisUsage &AU) const {
    AU.addPreserved<SplitRegions>();
}
