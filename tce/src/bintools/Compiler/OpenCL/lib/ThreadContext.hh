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
 * @file ThreadContext.hh
 *
 * Defined the _thread_context structure using LLVM API.
 * This structure stores per-workitem status information.
 *
 * @author Carlos Sanchez de La Lama 2009 (carlos.delalama@urjc.es)
 *
 * @note TypeBuider template seems to available on llvm-2.6 and later.
 */

#include "config.h"

#if 0
#include "llvm/Support/TypeBuilder.h"

namespace llvm {
    template<bool xcompile> class TypeBuilder<_thread_context, xcompile> {
    public:
        static const StructType *get(LLVMContext &Context) {
            return StructType::get(
                TypeBuilder<types::i<32>, xcompile>::get(Context),
                TypeBuilder<types::i<32>[3], xcompile>::get(Context),
                TypeBuilder<types::i<32>[3], xcompile>::get(Context),
                TypeBuilder<types::i<32>[3], xcompile>::get(Context),
                TypeBuilder<types::i<32>[3], xcompile>::get(Context),
                TypeBuilder<types::i<32>[3], xcompile>::get(Context),
                TypeBuilder<types::i<32>[3], xcompile>::get(Context),
                NULL);
        }

        enum Fields {
            FIELD_WORD_DIM,
            FIELD_GLOBAL_SIZE,
            FIELD_GLOBAL_ID,
            FIELD_LOCAL_SIZE,
            FIELD_LOCAL_ID,
            FIELD_NUM_GROUPS,
            FIELD_GROUP_ID
        };
    }
}
#else
#include "llvm/DerivedTypes.h"
#include "llvm/LLVMContext.h"

namespace ocl {
    class ThreadContext {
    public:
        static const llvm::StructType *get(llvm::LLVMContext &Context) {
#ifdef LLVM_2_9            
            return llvm::StructType::get(
                Context,
                llvm::IntegerType::get(Context, 32),
                llvm::ArrayType::get(llvm::IntegerType::get(Context, 32), 3),
                llvm::ArrayType::get(llvm::IntegerType::get(Context, 32), 3),
                llvm::ArrayType::get(llvm::IntegerType::get(Context, 32), 3),
                llvm::ArrayType::get(llvm::IntegerType::get(Context, 32), 3),
                llvm::ArrayType::get(llvm::IntegerType::get(Context, 32), 3),
                llvm::ArrayType::get(llvm::IntegerType::get(Context, 32), 3),
                NULL);
#else
            llvm::Type* structElements[7] = {
                llvm::IntegerType::get(Context, 32),
                llvm::ArrayType::get(llvm::IntegerType::get(Context, 32), 3),
                llvm::ArrayType::get(llvm::IntegerType::get(Context, 32), 3),
                llvm::ArrayType::get(llvm::IntegerType::get(Context, 32), 3),
                llvm::ArrayType::get(llvm::IntegerType::get(Context, 32), 3),
                llvm::ArrayType::get(llvm::IntegerType::get(Context, 32), 3),
                llvm::ArrayType::get(llvm::IntegerType::get(Context, 32), 3)
             };
            return llvm::StructType::get(Context, structElements, false);
#endif
        }

        enum Fields {
            FIELD_WORD_DIM,
            FIELD_GLOBAL_SIZE,
            FIELD_GLOBAL_ID,
            FIELD_LOCAL_SIZE,
            FIELD_LOCAL_ID,
            FIELD_NUM_GROUPS,
            FIELD_GROUP_ID
        };
    };
}
               
#endif
