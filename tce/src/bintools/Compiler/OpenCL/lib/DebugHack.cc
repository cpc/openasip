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
 * @file DebugHack.cc
 *
 * This LLVM pass creates types and global variables for later
 * passes to mark instructions belonging to different work-items with
 * different source location debug information. This is sort of hack
 * until intruction-level metadata is implemented in LLVM.
 *
 * @author Carlos Sanchez de La Lama 2009 (carlos.delalama@urjc.es)
 */

#include <vector>
#include "llvm/Constants.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Function.h"
#include "llvm/Instructions.h"
#include "llvm/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/Dwarf.h"

using namespace llvm;

namespace {
    class CompileUnitType {
    public:
        static const StructType *get(LLVMContext &Context) {

            return StructType::get(
                Context,
                IntegerType::get(Context, 32),
                PointerType::get(StructType::get(Context), 0),
                IntegerType::get(Context, 32),
                PointerType::get(IntegerType::get(Context, 8), 0),
                PointerType::get(IntegerType::get(Context, 8), 0),
                PointerType::get(IntegerType::get(Context, 8), 0),
                IntegerType::get(Context, 1),
                IntegerType::get(Context, 1),
                PointerType::get(IntegerType::get(Context, 8), 0),
                IntegerType::get(Context, 32),
                NULL);
        }

        enum Fields {
            FIELD_TAG,
            FIELD_ANCHOR,
            FIELD_LANGUAGE_IDENTIFIER,
            FIELD_FILE_NAME,
            FIELD_FILE_DIRECTORY,
            FIELD_PRODUCER,
            FIELD_MAIN,
            FIELD_OPTIMIZED,
            FIELD_FLAGS,
            FIELD_VERSION
        };
    };
                
    struct DebugHack : public ModulePass {

        static char ID;
        DebugHack() : ModulePass(&ID) {}

        virtual bool runOnModule(Module &M);

    };

    char DebugHack::ID = 0;
    RegisterPass<DebugHack> X("debughack",
                              "Create work-item debug mark values");
    
}

bool DebugHack::runOnModule(Module &M) {

    const StructType *t = CompileUnitType::get(M.getContext());

    if (M.addTypeName("llvm.dbg.compile_unit.type", t))
        return false;

    Constant *filename = ConstantArray::get(M.getContext(),
                                            "workitem");
    Constant *directory = ConstantArray::get(M.getContext(),
                                             "/");
    Constant *producer = ConstantArray::get(M.getContext(),
                                            "llvmopencl");
    GlobalVariable *filename_str =
        new GlobalVariable(M,
                           filename->getType(),
                           true,
                           GlobalValue::InternalLinkage,
                           filename,
                           "filename.str");

    GlobalVariable *directory_str =
        new GlobalVariable(M,
                           directory->getType(),
                           true,
                           GlobalValue::InternalLinkage,
                           directory,
                           "directory.str");
    
    GlobalVariable *producer_str =
        new GlobalVariable(M,
                           producer->getType(),
                           true,
                           GlobalValue::InternalLinkage,
                           producer,
                           "producer.str");
    
    Constant *idx[2];
    idx[0] = ConstantInt::get(IntegerType::get(M.getContext(), 32), 0);
    idx[1] = ConstantInt::get(IntegerType::get(M.getContext(), 32), 0);

    std::vector<Constant *> v(t->getNumElements());
    v[CompileUnitType::FIELD_TAG] =
        ConstantInt::get(IntegerType::get(M.getContext(), 32),
                         LLVMDebugVersion + dwarf::DW_TAG_compile_unit);
    v[CompileUnitType::FIELD_ANCHOR] =
        ConstantPointerNull::get(
            PointerType::get(StructType::get(M.getContext()), 0));
    v[CompileUnitType::FIELD_LANGUAGE_IDENTIFIER] =
        ConstantInt::get(IntegerType::get(M.getContext(), 32),
                         dwarf::DW_LANG_C_plus_plus);
    v[CompileUnitType::FIELD_FILE_NAME] =
        ConstantExpr::getInBoundsGetElementPtr(filename_str, idx, 2);
    v[CompileUnitType::FIELD_FILE_DIRECTORY] =
        ConstantExpr::getInBoundsGetElementPtr(directory_str, idx, 2);
    v[CompileUnitType::FIELD_PRODUCER] =
        ConstantExpr::getInBoundsGetElementPtr(producer_str, idx, 2);
    v[CompileUnitType::FIELD_MAIN] = ConstantInt::getTrue(M.getContext());
    v[CompileUnitType::FIELD_OPTIMIZED] = ConstantInt::getFalse(M.getContext());
    v[CompileUnitType::FIELD_FLAGS] =
        ConstantPointerNull::get(
            PointerType::get(IntegerType::get(M.getContext(), 8), 0));
    v[CompileUnitType::FIELD_VERSION] =  ConstantInt::get(
        IntegerType::get(M.getContext(), 32),
        0);
    
    new GlobalVariable(M,
                       t,
                       true,
                       GlobalValue::InternalLinkage,
		       ConstantStruct::get(M.getContext(), v, false),
//                       ConstantStruct::get(M.getContext(), v),
                       "llvm.dbg.compile_unit");

    std::vector<const Type *> args_type(3);
    args_type[0] = IntegerType::get(M.getContext(), 32);
    args_type[1] = IntegerType::get(M.getContext(), 32);
    args_type[2] = PointerType::get(StructType::get(M.getContext()), 0);
    FunctionType *ft = FunctionType::get(Type::getVoidTy(M.getContext()),
                                         args_type,
                                         false);
    Function::Create(ft,
                     Function::ExternalLinkage, 
                     "llvm.dbg.stoppoint",
                     &M);

    return true;
}
