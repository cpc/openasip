/*
    Copyright (c) 2002-2015 Tampere University.

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
 * @file TCEStubTargetTransformInfo.cc
 *
 * Declaration of TCEStubTargetTransformInfo class.
 *
 * @author Ville Korhonen 2015
 */

#include "CompilerWarnings.hh"
IGNORE_COMPILER_WARNING("-Wunused-parameter")

#include "tce_config.h"

#include "TCEStubTargetMachine.hh"
#include "TCEStubTargetTransformInfo.hh"
#include "MachineInfo.hh"
#include "CIStringSet.hh"
#include "Machine.hh"
#include "FunctionUnit.hh"
#include "Operation.hh"
#include "Operand.hh"
#include "OperationPool.hh"

#include <llvm/Analysis/TargetTransformInfo.h>
#include <llvm/Support/Debug.h>
#include <llvm/CodeGen/CostTable.h>
#include <llvm/CodeGen/TargetLowering.h>
#include <llvm/CodeGen/TargetRegisterInfo.h>

using namespace llvm;

#define DEBUG_TYPE "tcestubtti"

#if LLVM_MAJOR_VERSION < 21
unsigned
TCEStubTTIImpl::getNumberOfRegisters(bool vector) {
    // without adf information we have no clue about registers
    if (TM->ttaMach_ == NULL)
        return 0;

    // Widest operand tells directly the widest register
    unsigned widestOperand =
        MachineInfo::findWidestOperand(*(TM->ttaMach_), vector);

    return MachineInfo::numberOfRegisters(*(TM->ttaMach_), widestOperand);
}
#else
// This method is not in the LLVM 21's interface. TODO: check
// the required interfaces for vectorization info.
#endif

#if LLVM_MAJOR_VERSION < 21
unsigned
TCEStubTTIImpl::getRegisterBitWidth(bool vector) const {
    // without adf information we have no clue about registers
    if (TM->ttaMach_ == NULL)
        return 0;

    // Widest operand tells directly the widest register
    return MachineInfo::findWidestOperand(*(TM->ttaMach_), vector);
}

unsigned
TCEStubTTIImpl::getMaxInterleaveFactor(unsigned VF) {
    // This means maximum loop unroll factor
    // 2 because loopvectorizer requires >1
    // TODO: find some way adjust this.
    return 2;
}

#else

llvm::TypeSize
TCEStubTTIImpl::getRegisterBitWidth(
    TargetTransformInfo::RegisterKind K) const {
    bool vector = K != llvm::TargetTransformInfo::RegisterKind::RGK_Scalar;
    // without adf information we have no clue about registers
    if (TM->ttaMach_ == NULL) TypeSize::getZero();

    // Widest operand tells directly the widest register
    return TypeSize::getFixed(
        MachineInfo::findWidestOperand(*(TM->ttaMach_), vector));
}

unsigned
TCEStubTTIImpl::getMaxInterleaveFactor(llvm::ElementCount) const {
    // This means maximum loop unroll factor
    // 2 because loopvectorizer requires >1
    // TODO: find some way adjust this.
    return 2;
}

#endif

#if LLVM_MAJOR_VERSION < 21
unsigned
TCEStubTTIImpl::getCastInstrCost(
    unsigned Opcode, Type* Dst, Type* Src, const Instruction*)
#else
llvm::InstructionCost
TCEStubTTIImpl::getCastInstrCost(
    unsigned Opcode, Type* Dst, Type* Src,
    llvm::TargetTransformInfo::CastContextHint,
    llvm::TargetTransformInfo::TargetCostKind, const Instruction*) const
#endif
{
    // TODO: Maybe use ADF/osal information to figure out real cost
    // 1 selected at the moment because LLVM cost model assumed way too high
    // cost for trunc/zext instructions. Too low value here might lead to 
    // vectorization of loop that would be better left in scalar form
    return 1;
}
