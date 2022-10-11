/*
    Copyright (c) 2002-2011 Tampere University.

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
 * @file LLVMAliasAnalyzer.hh
 *
 * Declaration of LLVMAliasAnalyzer class.
 * 
 * This class uses LLVM AliasAnalysis and MachineInstruction references
 * in ProgramOperation to query for aliases.
 *
 * @author Vladimír Guzma 2011 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_LLVM_ALIAS_ANALYZER
#define TTA_LLVM_ALIAS_ANALYZER

#include "MemoryAliasAnalyzer.hh"

#include "tce_config.h"

#include "CompilerWarnings.hh"
IGNORE_COMPILER_WARNING("-Wunused-parameter")

#include "llvm/Analysis/AliasAnalysis.h"

POP_COMPILER_DIAGS

typedef llvm::MemoryLocation Location;
typedef llvm::AliasResult AliasResult;

class LLVMAliasAnalyzer : public MemoryAliasAnalyzer {
public:
    LLVMAliasAnalyzer();
    virtual bool isAddressTraceable(
        DataDependenceGraph& ddg, const ProgramOperation& pop);
    virtual AliasingResult analyze(
        DataDependenceGraph& ddg, const ProgramOperation& pop1, 
        const ProgramOperation& pop2, MoveNodeUse::BBRelation bbInfo);
    virtual void setLLVMAA(llvm::AliasAnalysis* AA);
    ~LLVMAliasAnalyzer();

private:
    // Alias analyzer of LLVM will be set from DDGBuilder, since it is known
    // only when DDGBuilder's method build(...) is called.
    llvm::AliasAnalysis* AA_;
};

#endif
