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
 * @file InlineAsmParser.cc
 *
 * Implementation of inline assembly parser.
 *
 * @author Henry Linjamäki (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#include "InlineAsmParser.hh"

#include <memory>
#include <regex>
#include <algorithm>

#include "AssemblerParser.hh"

#include "Machine.hh"
#include "Binary.hh"
#include "BasicBlock.hh"
#include "Program.hh"
#include "InstructionReferenceManager.hh"
#include "InstructionReference.hh"
#include "Procedure.hh"
#include "Instruction.hh"
#include "Move.hh"
#include "Terminal.hh"
#include "TerminalImmediate.hh"
#include "TerminalInstructionReference.hh"
#include "Immediate.hh"
#include "TPEFProgramFactory.hh"
#include "LiveRangeData.hh"

#include "TCETargetMachine.hh"
#include "LLVMUtilities.hh"

#include <llvm/CodeGen/MachineOperand.h>
#include <llvm/CodeGen/MachineInstr.h>
#include "llvm/IR/InlineAsm.h"
#include "llvm/IR/Mangler.h"
#include <llvm/ADT/SmallString.h>

#if LLVM_MAJOR_VERSION > 20
#define Kind_RegUse Kind::RegUse
#define Kind_RegDefEarlyClobber Kind::RegDefEarlyClobber
#define Kind_RegDef Kind::RegDef
#define Kind_Clobber Kind::Clobber
#define Kind_Mem Kind::Mem
#define Kind_Imm Kind::Imm
#endif

using namespace llvm;

InlineAsmParser::InlineAsmParser(
    const llvm::TCETargetMachine& tm,
    const llvm::Mangler& mangler) :
    tm_(tm), mangler_(mangler) {
    }

/**
 * Parses the given inline assembly instruction to POM basic block.
 *
 * After call parser diagnostic object is populated with encountered warning
 * and error reports if any. The object is received via diagnostics()
 * function. The object is cleared next time this method is called.
 *
 * @param inlineAsmMI The Instruction holding an inline asm block.
 * @param bb The basic block to add parsed code into.
 * @param irm The manager in where instruction references are copied to.
 * @return False if any errors in parsing. Otherwise returns true;
 */
bool
InlineAsmParser::parse(
    const llvm::MachineInstr& inlineAsmMI,
    const std::map<std::string, unsigned>& symbolTable,
    TTAProgram::BasicBlock& bb,
    TTAProgram::InstructionReferenceManager& irm) {

#ifdef DEBUG_INLINE_ASM_PARSER
    std::cerr << "*** before position string substitution:" << std::endl;
    inlineAsmMI.print(llvm::dbgs());
#endif

    std::string asmString = substituteAsmString(
        inlineAsmMI, symbolTable, mangler_);

#ifdef DEBUG_INLINE_ASM_PARSER
    std::cerr << "*** before inline asm parsing:" << std::endl
              << asmString << std::endl; //DEBUG
#endif

    // Parse inline asm.
    parserDiagnostic_.reset(std::make_shared<std::string>(asmString));
    std::unique_ptr<TPEF::Binary> bin(new TPEF::Binary());
    const TTAMachine::Machine& mach = tm_.ttaMachine();
    AssemblerParser parser(
        *bin, mach, &parserDiagnostic_, /* codeLinesOnly = */ true);

    try {
        if (!parser.compile(asmString)) {
            reportError(
                parser.errorLine(), "Syntax error in inline assembly.");
            return false;
        }
        parser.finalize(mach.isLittleEndian());
    } catch (CompileError& e) {
        reportError(parser.errorLine(), e.errorMessage());
        return false;
    } catch (Exception& e) {
        reportError(parser.errorLine(), e.errorMessage());
        return false;
    }

    // Convert to POM.
    std::unique_ptr<TTAProgram::Program> prog;
    try {
        // TODO Add universal machine for sequential code.
        TTAProgram::TPEFProgramFactory tpefFactory(*bin, mach);
        prog.reset(tpefFactory.build());
    } catch (Exception& e) {
        reportError(e.errorMessage());
        return false;
    }

    assert(prog);

    copyInstructions(*prog, bb, irm);
    addDebugInfoToInlineAsmBB(inlineAsmMI, bb);
    addLiveRangeData(inlineAsmMI, bb);

    if (!sanityChecks(inlineAsmMI, bb)) return false;

#ifdef DEBUG_INLINE_ASM_PARSER
    std::cerr << "*** After inline asm parsing:" << std::endl
              << bb.toString() << std::endl;
#endif

    return true;
}

/**
 * Adds gebug info to moves of the parsed inline asm.
 */
void
InlineAsmParser::addDebugInfoToInlineAsmBB(
    const MachineInstr& mi,
    TTAProgram::BasicBlock& bb) {

    // TODO/FIXME For some reason the source locations for asm block are
    //            shifted, pointing to incorrect lines of the source files.
    //            This error(?) can be seen already in disll.

    std::string sourceFileName;
    size_t sourceLineNumber;
    std::tie(sourceFileName, sourceLineNumber) = getSourceLocationInfo(mi);

    if (sourceFileName.size() >
        TPEF::InstructionAnnotation::MAX_ANNOTATION_BYTES) {
        sourceFileName =
        sourceFileName.substr(
            sourceFileName.size() -
            TPEF::InstructionAnnotation::MAX_ANNOTATION_BYTES,
            TPEF::InstructionAnnotation::MAX_ANNOTATION_BYTES);
    }
    TTAProgram::ProgramAnnotation srcLineAnn(
        TTAProgram::ProgramAnnotation::ANN_DEBUG_SOURCE_CODE_LINE,
        sourceLineNumber);
    if (sourceFileName.empty()) sourceFileName = "???";
    TTAProgram::ProgramAnnotation srcFileAnn(
        TTAProgram::ProgramAnnotation::ANN_DEBUG_SOURCE_CODE_PATH,
        sourceFileName);

    for (int i = 0; i < bb.instructionCount(); i++) {
        TTAProgram::Instruction& instr = bb.instructionAtIndex(i);
        for (int m = 0; m < instr.moveCount(); m++) {
            TTAProgram::Move& move = instr.move(m);
            move.addAnnotation(srcLineAnn);
            move.addAnnotation(srcFileAnn);
        }
    }
}

/**
 * Fills live range data structure.
 *
 * This method only fills inlineAsmX_ fields in the structure.
 * Data dependency graph builder handles the fields later on and build the
 * proper live range data.
 */
void
InlineAsmParser::addLiveRangeData(
    const llvm::MachineInstr& mi,
    TTAProgram::BasicBlock& bb) {

    if (!bb.liveRangeData_) {
        bb.liveRangeData_ = new LiveRangeData();
    }

    auto& liveRangeData = *bb.liveRangeData_;
    AsmOperandMap asmOperandMap = getInlineAsmOperands(mi);
    for (auto& opds : asmOperandMap) {
#if LLVM_MAJOR_VERSION < 21
        auto asmOpdKind = std::get<0>(opds.second);
#else
        llvm::InlineAsm::Kind asmOpdKind = std::get<0>(opds.second).getKind();
#endif
        auto& asmOpdNodes = std::get<1>(opds.second);
        TCEString reg;
        switch (asmOpdKind) {
        default:
            break;
        case InlineAsm::Kind_RegUse:
            assert(asmOpdNodes.size() == 1);
            reg = registerName(*asmOpdNodes.at(0));
            liveRangeData.inlineAsmRegUses_.insert(reg);
            break;
        case InlineAsm::Kind_RegDefEarlyClobber:
        case InlineAsm::Kind_RegDef:
            assert(asmOpdNodes.size() == 1);
            reg = registerName(*asmOpdNodes.at(0));
            liveRangeData.inlineAsmRegDefs_.insert(reg);
            break;
        case InlineAsm::Kind_Clobber:
            assert(asmOpdNodes.size() == 1);
            reg = registerName(*asmOpdNodes.at(0));
            liveRangeData.inlineAsmClobbers_.insert(reg);
            break;
        }
    }
}

/**
 * Performs sanity checks of parsed inline asm snippet.
 *
 * The method reports issues to the diagnostic object.
 *
 * @returns True if no errors were encountered beside some warnings.
 *          Otherwise, returns false on any error.
 */
bool
InlineAsmParser::sanityChecks(
    const llvm::MachineInstr&,
    TTAProgram::BasicBlock&) const {

    // Check unintended input register overwrite. //
    // TODO

    // Checks no input operands are clobbered. //
    // TODO

    return true;
}

/**
 * Returns register name of the machine operand (of reg type).
 */
std::string
InlineAsmParser::registerName(const llvm::MachineOperand& mo) const {
    assert(mo.isReg());
    return tm_.registerName(mo.getReg());
}

/**
 * Returns substituted assembly string.
 *
 * @param mi The instruction holding the inline asm.
 * @param symbolTable Table to traslate mangled names to addresses.
 * @param mangler The name mangler.
 */
std::string
InlineAsmParser::substituteAsmString(
    const llvm::MachineInstr& mi,
    const std::map<std::string, unsigned>& symbolTable,
    const llvm::Mangler& mangler) {


    assert(mi.isInlineAsm() && "MI must hold inline asm.");
    std::string result = mi.getOperand(
        InlineAsm::MIOp_AsmString).getSymbolName();
    assert(!result.empty());

    // If TCE operation macro - the _TCE_<operation> kinds.
    if (!InlineAsmParser::isInlineAsm(mi)) return result;

    auto mangledNameFn = [&mangler](const GlobalValue* gv) -> std::string {
        SmallString<256> Buffer;
        mangler.getNameWithPrefix(Buffer, gv, false);
        std::string name(Buffer.c_str());

        return name;
    };

    // Maps template strings to the assigned operand. e.g "$0" => "RF.5".
    std::map<std::string, std::string> templateStringMap;

    const std::string tmplPrefix = "$";
    std::string unhandledKind;
    AsmOperandMap opdMap = getInlineAsmOperands(mi);
    for (auto& asmOpd : opdMap) {
        unsigned asmOpdPos = asmOpd.first;
#if LLVM_MAJOR_VERSION < 21
        auto asmOpdKind = std::get<0>(asmOpd.second);
#else
        llvm::InlineAsm::Kind asmOpdKind =
            std::get<0>(asmOpd.second).getKind();
#endif
        std::vector<const llvm::MachineOperand*>& flagOpds =
            std::get<1>(asmOpd.second);
        unsigned numAsmOpds = flagOpds.size();
        switch (asmOpdKind) {
        case InlineAsm::Kind_Mem:
            if (unhandledKind.empty()) unhandledKind = "mem";
            // fall-through
        default:
            if (unhandledKind.empty()) unhandledKind = "??";
            if (Application::spamVerbose()) {
                std::cerr << "substituteAsmString():"
                          << " Ignoring asm operand kind: "
                          << unhandledKind << std::endl;
            }
            unhandledKind.clear();
            break;

        case InlineAsm::Kind_RegDefEarlyClobber:
        case InlineAsm::Kind_RegDef:
        case InlineAsm::Kind_RegUse:
            assert(numAsmOpds == 1);
            templateStringMap.insert(std::make_pair(
                tmplPrefix + std::to_string(asmOpdPos),
                tm_.registerName(flagOpds.at(0)->getReg())));
            break;

        case InlineAsm::Kind_Imm:
            assert(numAsmOpds == 1);
            if (flagOpds.at(0)->isImm()) {
                templateStringMap.insert(std::make_pair(
                    tmplPrefix + std::to_string(asmOpdPos),
                    std::to_string(flagOpds.at(0)->getImm())));
            } else if (flagOpds.at(0)->isGlobal()) {
                std::string name = mangledNameFn(flagOpds.at(0)->getGlobal());
                unsigned address = 0;
                if (symbolTable.find(name) != symbolTable.end()) {
                    address = symbolTable.at(name)
                        + flagOpds.at(0)->getOffset();
                // TODO: could be address to function. Can not handle that
                //       yet.
                } else {
                    THROW_EXCEPTION(
                        CompileError,
                        "Could not determine address of symbol '"
                        + name + "'. \nNote: Functions as inline asm "
                        "operands are not supported.");
                }

                templateStringMap.insert(std::make_pair(
                    tmplPrefix + std::to_string(asmOpdPos),
                    std::to_string(address)));
            } else {
                assert(false);
            }
            break;
        case InlineAsm::Kind_Clobber:
            break; // No need to handle.
        }
    }

    // Searches for "$<num>" string. Returns tuple of (position, length)
    // if found. Returns (string::npos, 0) if not found.
    auto findTemplateStrFn = [](const std::string& str, size_t pos)
        -> std::tuple<size_t, size_t> {
            if (pos > str.size())  {
                return std::make_tuple(std::string::npos, 0);
            }
            size_t len = 0;
            while ((pos = str.find("$", pos)) != std::string::npos) {
                size_t endPos = str.find_first_not_of("0123456789", pos+1);
                if (endPos == std::string::npos) {
                    // Check template string at end of string, e.g '$4<EOF>'
                    if (str.size()-pos > 1) {
                        return std::make_tuple(pos, str.size()-pos);
                    }
                    pos = std::string::npos;
                    break;
                }
                len = endPos-pos;
                if (len > 1) break; // Ignores "$$" and "${:uid}" strings.
                pos += len;
            }
            return std::make_tuple(pos, len);
        };

    size_t pos = 0;
    size_t len = 0;
    std::set<std::string> replacedTemplStrs;
    while (true) {
        std::tie(pos, len) = findTemplateStrFn(result, pos);
        if (pos == std::string::npos) break;
        const std::string& templStr = result.substr(pos, len);
        replacedTemplStrs.insert(templStr);
        result.replace(pos, len, templateStringMap.at(templStr));
        pos += len;
    }

    // Unreferenced template string, especially for output operands, may break
    // semantics of a program code.
    for (auto& tmplStrPair : templateStringMap) {
        const std::string& tmplStr = tmplStrPair.first;
        if (replacedTemplStrs.count(tmplStr)) continue;
        std::string msg;
        try {
            auto opdIdx = Conversion::toInt(tmplStr.substr(1))+1;
            msg = std::to_string(opdIdx) + ". operand is unreferenced";
        } catch (NumberFormatException&) {
            msg = "There is unreferenced operand";
        }
        std::string srcFile;
        size_t srcLine;
        std::tie(srcFile, srcLine) = getSourceLocationInfo(mi);
        std::string srcLoc;
        if (!srcFile.empty()) {
            srcLoc = srcFile + ":" + std::to_string(srcLine) + ": ";
        }
        std::cerr << srcLoc << "Warning: " << msg
                  << " in an inline asm block."
                  << std::endl;
    }

    // Replace "%=" template strings (redubbed  as "${:uid}" in LLVM).
    std::string uidTmplStr = "${:uid}";
    auto uid = std::to_string(asmId_++);
    pos = 0;
    while ((pos = result.find(uidTmplStr, pos)) != std::string::npos) {
        result.replace(pos, uidTmplStr.size(), uid);
        pos += uid.size();
    }
    assert(!result.empty());
    return result;
}

/**
 * Returns true if the instruction represents an inline asm block recognized
 * by this parser.
 */
bool
InlineAsmParser::isInlineAsm(const llvm::MachineInstr& mi) {
    if (mi.isInlineAsm()) {
        std::string amsStr(
            mi.getOperand(InlineAsm::MIOp_AsmString).getSymbolName());
        return amsStr.find_first_of("->;") != std::string::npos;
    }
    return false;
}

/*
 * Report an error at inline assembly string line.
 */
void
InlineAsmParser::reportError(size_t lineNum, const std::string& errorMsg) {
    parserDiagnostic_.addError(lineNum, errorMsg);
}

void
InlineAsmParser::reportError(const std::string& errorMsg) {
    parserDiagnostic_.addError(errorMsg);
}

std::vector<TTAProgram::TerminalInstructionReference*>
getInstructionReferenceTerminals(TTAProgram::Instruction& instr) {

    using TTAProgram::TerminalInstructionReference;

    std::vector<TTAProgram::TerminalInstructionReference*> result;

    for (int i = 0; i < instr.moveCount(); i++) {
        auto& move = instr.move(i);
        auto& srcTerml = move.source();
        if (srcTerml.isInstructionAddress()) {
            assert(dynamic_cast<TerminalInstructionReference*>(&srcTerml));
            result.push_back(static_cast<TerminalInstructionReference*>(
                &srcTerml));
        }
    }

    for (int i = 0; i < instr.immediateCount(); i++) {
        auto& valTerml = instr.immediate(i).value();
        if (valTerml.isInstructionAddress()) {
            assert(dynamic_cast<TerminalInstructionReference*>(&valTerml));
            result.push_back(static_cast<TerminalInstructionReference*>(
                &valTerml));
        }
    }

    return result;
}

void
InlineAsmParser::copyInstructions(
    TTAProgram::Program& from,
    TTAProgram::BasicBlock& targetBB,
    TTAProgram::InstructionReferenceManager& irm) {

    using namespace TTAProgram;

    auto procCount = from.procedureCount();
    // There should be only one default procedure from parsed inline asm.
    // However, file-level inline asm could have more: TODO when supported.
    assert(from.procedureCount() == 1
        && "Procedures are not allowed in non-file-level inline asm.");

    // A map for fixing instruction references. Old instr -> copied instr.
    std::map<const TTAProgram::Instruction*, TTAProgram::Instruction*>
        instrCopyMap;
    for (decltype(procCount) i = 0; i < procCount; i++) {
        const auto& proc = from.procedureAtIndex(i);
        auto instrCount = proc.instructionCount();
        for (decltype(instrCount) i = 0; i < instrCount; i++) {
            const auto& oldInstr = proc.instructionAtIndex(i);
            auto newInstr = oldInstr.copy();
            targetBB.add(newInstr);
            instrCopyMap.insert({&oldInstr, newInstr});
        }
    }

    for (auto& pair : instrCopyMap) {
        auto& newInstr = *pair.second;
        for (auto& refTerm : getInstructionReferenceTerminals(newInstr)) {
            auto& oldRefInstr = refTerm->instructionReference().instruction();
            assert(instrCopyMap.count(&oldRefInstr));
            auto newRefInstr = instrCopyMap.at(&oldRefInstr);
            auto newRef = irm.createReference(*newRefInstr);
            refTerm->setInstructionReference(newRef);
        }
    }
}


