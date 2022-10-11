/*
 * Copyright (c) 2002-2017 Tampere University.
 *
 * This file is part of TTA-Based Codesign Environment (TCE).
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
/**
 * @file LLVMUtilities.cc
 *
 * Declarations of LLVM utilities.
 *
 * @author Henry Linjamäki 2017 (henry.linjamaki-no.spam-tut.fi)
 * @note reting: red
 */

#ifndef LLVMUTILITIES_HH
#define LLVMUTILITIES_HH

#include <string>
#include <tuple>
#include <map>
#include <vector>

#include "llvm/IR/InlineAsm.h"

namespace llvm {
    class MachineInstr;
    class MachineOperand;
}

std::tuple<std::string, size_t> getSourceLocationInfo(
    const llvm::MachineInstr& mi);

std::string getSourceLocationString(const llvm::MachineInstr& mi);

/// First = Inline asm kind defined in llvm/IR/InlineAsm.h
/// Second = The associated operands.
using AsmOperands =
    std::tuple< unsigned, std::vector<const llvm::MachineOperand*>>;
/// Inline assembly operand position. The numbers are matched to template
/// strings in inline asm texts - e.g. 2 => "$2".
using AsmPosition = unsigned;
using AsmOperandMap = std::map<AsmPosition, AsmOperands>;

AsmOperandMap getInlineAsmOperands(const llvm::MachineInstr& mi);

#endif // LLVMUTILITIES_HH
