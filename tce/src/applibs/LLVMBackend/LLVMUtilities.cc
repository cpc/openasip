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
 * Implementations of LLVM utilities.
 *
 * @author Henry Linjamäki 2017 (henry.linjamaki-no.spam-tut.fi)
 * @note reting: red
 */

#include "LLVMUtilities.hh"

#include "TCEString.hh"
#include "tce_config.h"

#include "CompilerWarnings.hh"
IGNORE_COMPILER_WARNING("-Wunused-parameter")

#include <llvm/CodeGen/MachineInstr.h>
#include <llvm/CodeGen/MachineOperand.h>
#include <llvm/IR/DebugLoc.h>
#include <llvm/IR/DebugInfo.h>

POP_COMPILER_DIAGS

/**
 * Extracts source location info from the instruction.
 *
 * If the instruction does not have debug info returns ("", 0);
 */
std::tuple<std::string, size_t>
getSourceLocationInfo(const llvm::MachineInstr& mi) {
    using namespace llvm;
    DebugLoc dl = mi.getDebugLoc();
    if (!dl) return std::make_tuple("", 0);

    bool hasDebugInfo = false;
    hasDebugInfo = dl.getScope() != NULL;
    if (!hasDebugInfo) return std::make_tuple("", 0);

    size_t sourceLineNumber = 0;
    TCEString sourceFileName = "";

    // inspired from lib/codegen/MachineInstr.cpp
    sourceLineNumber = dl.getLine();
    sourceFileName = static_cast<TCEString>(
#ifdef LLVM_OLDER_THAN_11
        cast<DIScope>(dl.getScope())->getFilename());
#else
        cast<DIScope>(dl.getScope())->getFilename().str());
#endif
    return std::make_tuple(sourceFileName, sourceLineNumber);
}

/**
 * Returns source location as "<src-file>:<src-line>: " string if available.
 *
 * Otherwise return empty string.
 */
std::string getSourceLocationString(const llvm::MachineInstr& mi) {
    std::string srcFile;
    unsigned srcLine;
    std::tie(srcFile, srcLine) = getSourceLocationInfo(mi);
    if (!srcFile.empty()) {
        return srcFile + ":" + std::to_string(srcLine) + ": ";
    }
    return "";
}

/**
 * Decodes operands of INLINEASM instruction into more manageable struct.
 */
AsmOperandMap
getInlineAsmOperands(const llvm::MachineInstr& mi) {
    using namespace llvm;
    AsmOperandMap result;

    unsigned asmOpdPos = 0;
    unsigned endPos = mi.getNumOperands();
    unsigned i = InlineAsm::MIOp_FirstOperand;

    while (i < endPos) {
        const MachineOperand& mo = mi.getOperand(i);
        if (mo.isMetadata())  {
            i++;
            continue;
        }
        unsigned opdKind = InlineAsm::getKind(mo.getImm());
        unsigned numAsmOpds = InlineAsm::getNumOperandRegisters(mo.getImm());
        unsigned flagOpdBegin = i + 1;
        unsigned flagOpdEnd = flagOpdBegin + numAsmOpds;
        std::vector<const llvm::MachineOperand*> flagOps;
        for (unsigned opdIdx = flagOpdBegin; opdIdx < flagOpdEnd; opdIdx++) {
            flagOps.push_back(&mi.getOperand(opdIdx));
        }
        result.insert({asmOpdPos, std::make_tuple(opdKind, flagOps)});

        i += numAsmOpds + 1;
        asmOpdPos += 1;
    }

    return result;
}




