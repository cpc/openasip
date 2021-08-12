/*
    Copyright (c) 2002-2020 Tampere University.

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
 * @file LLVMAliasAnalyzer.cc
 *
 * Implementation of LLVMAliasAnalyzer class.
 * 
 * This class uses LLVM AliasAnalysis and MachineInstruction references
 * in ProgramOperation to query for alises.
 *
 * @author Vladimír Guzma 2011 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#include "CompilerWarnings.hh"
IGNORE_COMPILER_WARNING("-Wunused-parameter")

#include "LLVMAliasAnalyzer.hh"

#include "CompilerWarnings.hh"
IGNORE_COMPILER_WARNING("-Wunused-parameter")
IGNORE_COMPILER_WARNING("-Wcomment")

#include <llvm/CodeGen/MachineInstr.h>
#include "tce_config.h"
#include <llvm/IR/Value.h>
#include <llvm/CodeGen/MachineMemOperand.h>

#include <llvm/Analysis/AliasAnalysis.h>


#include "MoveNode.hh"
#include "Move.hh"
#include "DataDependenceGraph.hh"
#include "Terminal.hh"

POP_COMPILER_DIAGS

using namespace TTAProgram;
using namespace TTAMachine;

#ifdef LLVM_OLDER_THAN_13
#define MayAlias llvm::MayAlias
#define PartialAlias llvm::PartialAlias
#define MustAlias llvm::MustAlias
#define NoAlias llvm::NoAlias
#else
#define MayAlias llvm::AliasResult::Kind::MayAlias
#define PartialAlias llvm::AliasResult::Kind::PartialAlias
#define MustAlias llvm::AliasResult::Kind::MustAlias
#define NoAlias llvm::AliasResult::Kind::NoAlias
#endif

LLVMAliasAnalyzer::LLVMAliasAnalyzer(){
    AA_ = NULL;
}

bool 
LLVMAliasAnalyzer::isAddressTraceable(
    DataDependenceGraph& /*ddg*/, 
    const ProgramOperation& po) {
    // If the ProgramOperation has access to it's originating LLVM MachineInstr
    // and MachineInstr have some memory operands we can try tracing memory
    // accessed. 
    const llvm::MachineInstr* instr = po.machineInstr();    
    if (instr && !instr->memoperands_empty()) {
        return true;
    } else {
        return false;
    }
}

/**
 * Given two program operation, aswer question if memory accesed by those
 * operations aliases.
 *
 * If both POs access same memory returns true, if is sure the memory
 * is not same, returns false. Returns unknown in case memory accesses
 * may alias, or if there is partial alias.
 */
MemoryAliasAnalyzer::AliasingResult
LLVMAliasAnalyzer::analyze(
    DataDependenceGraph& ddg, 
    const ProgramOperation& pop1, 
    const ProgramOperation& pop2,
    MoveNodeUse::BBRelation bbInfo) {

    if (bbInfo) {
        return ALIAS_UNKNOWN;
    }

    if (!isAddressTraceable(ddg, pop1) || 
        !isAddressTraceable(ddg, pop2) ||
        AA_ == NULL) {
        return ALIAS_UNKNOWN;
    }

    const llvm::MachineInstr* instr1 = pop1.machineInstr();
    const llvm::MachineInstr* instr2 = pop2.machineInstr();    

    llvm::MachineInstr::mmo_iterator begin1 =
        instr1->memoperands_begin();
    // Machine instruction could in theory have several memory operands.
    // In practice it is usually just one.
    MemoryAliasAnalyzer::AliasingResult result = ALIAS_UNKNOWN;
    while (begin1 != instr1->memoperands_end()) {
        const llvm::Value* val1 = (*begin1)->getValue();        
        uint64_t size1 = (*begin1)->getSize();
        llvm::MachineInstr::mmo_iterator begin2 =
            instr2->memoperands_begin();        
            
        while (begin2 != instr2->memoperands_end()) {
            const llvm::Value* val2 = (*begin2)->getValue();
            uint64_t size2 = (*begin2)->getSize();
            if (val1 && val2) {
            
                AliasResult res = 
                    AA_->alias(val1, size1, val2, size2);
                    
                if (res == MayAlias || res == PartialAlias) {
                    result = ALIAS_UNKNOWN;
                }
                if (res == MustAlias) {
                    result = ALIAS_TRUE;
                }
                if (res == NoAlias) {
                    result = ALIAS_FALSE;
                }
            } else {
                result = ALIAS_UNKNOWN;
            }
            begin2++;
        }
        begin1++;
    }
    return result;
}

/**
 * Sets active Alias Analyzer picked from LLVM.
 */
void
LLVMAliasAnalyzer::setLLVMAA(llvm::AliasAnalysis* AA) {
    AA_ = AA;
}

LLVMAliasAnalyzer::~LLVMAliasAnalyzer() {}
