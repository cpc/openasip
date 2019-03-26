/*
    Copyright (c) 2002-2010 Tampere University.

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
 * @file LLVMTCEDataDependenceGraphBuilder.hh
 *
 * Declaration of LLVMTCE data dependence graph builder class.
 *
 * @author Pekka Jääskeläinen 2010 (pjaaskel)
 * @note rating: red
 */

#ifndef TTA_LLVMTCE_DDG_BUILDER_HH
#define TTA_LLVMTCE_DDG_BUILDER_HH

#include "DataDependenceGraphBuilder.hh"

namespace TTAProgram {
    class BasicBlock;
}

/**
 * LLVMTCEDataDependenceGraphBuilder class is repsonsible for building data 
 * dependence graphs with assistance from LLVM internal representation data.
 *
 * Currently used to produce DDGs from the prebypassed sequential code 
 * input from the TTA-like LLVM backend.
 */
class LLVMTCEDataDependenceGraphBuilder : 
    public DataDependenceGraphBuilder {
public:
    LLVMTCEDataDependenceGraphBuilder(InterPassData& ipd) : 
        DataDependenceGraphBuilder(ipd) {}
    virtual ~LLVMTCEDataDependenceGraphBuilder() {}

    virtual DataDependenceGraph* buildFromCFG(
        ControlFlowGraph& cGraph, const UniversalMachine* um = NULL);

    virtual DataDependenceGraph* buildFromBB(
        TTAProgram::BasicBlock& bb, const UniversalMachine* um = NULL) 
        throw (IllegalProgram);
private:
    void buildLocalDDG(TTAProgram::BasicBlock& bb);
};

#endif
