/*
 Copyright (C) 2022 Tampere University.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 */
/**
 * @file LLVMTCERISCVIntrinsicsLowering.cc
 *
 * Pass for lowering RISC-V intrinsics
 *
 * @author Kari Hepola 2022
 * @note rating: red
 */

#include "CompilerWarnings.hh"

IGNORE_COMPILER_WARNING("-Wunused-parameter")

#include <llvm/CodeGen/MachineFunction.h>
#include "tce_config.h"
#include <llvm/IR/Function.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Analysis/AliasAnalysis.h>

#include <iostream>

#include "LLVMTCERISCVIntrinsicsLowering.hh"
#include "Application.hh"
#include "InterPassData.hh"
#include "LLVMTCECmdLineOptions.hh"
#include "OperationPool.hh"
#include "BEMGenerator.hh"
#include "Exception.hh"

#include "tce_config.h"


POP_COMPILER_DIAGS

namespace llvm {

char LLVMTCERISCVIntrinsicsLowering::ID = 0;

LLVMTCERISCVIntrinsicsLowering::LLVMTCERISCVIntrinsicsLowering()
    : MachineFunctionPass(ID) {
    auto* option = static_cast<llvm::cl::opt<std::string>*>(
        llvm::cl::getRegisteredOptions().lookup("adf"));
    const std::string adf = option->getValue();
    try {
        mach_ = TTAMachine::Machine::loadFromADF(adf);
    } catch (const Exception& e) {
        Application::logStream()
            << "TCE: unable to load the ADF:" << std::endl
            << e.errorMessage() << std::endl
            << "Make sure you give the"
            << " correct adf with the -adf switch to llc";
    }
    bem_ = BEMGenerator(*mach_).generate();
    rFormat_ = findRFormat();
}

bool
LLVMTCERISCVIntrinsicsLowering::doInitialization(Module& /*m*/) {
    return false;
}

InstructionFormat*
LLVMTCERISCVIntrinsicsLowering::findRFormat() {
    InstructionFormat* format = NULL;
    for (int f = 0; f < bem_->instructionFormatCount(); f++) {
        if (bem_->instructionFormat(f).name() == "riscv_r_type") {
            format = &bem_->instructionFormat(f);
            break;
        }
    }
    if (format == NULL) {
        std::string msg = "BEM does not identify R format ";
        throw InvalidData(__FILE__, __LINE__, __func__, msg);
    }
    return format;
}

std::string
LLVMTCERISCVIntrinsicsLowering::findRegs(const std::string& s) const {
    std::vector<TCEString> asmSubStrings = StringTools::chopString(s, " ");
    std::string regs = "";
    int foundRegs = 0;
    for (TCEString i : asmSubStrings) {
        std::string ss = static_cast<std::string>(i);
        if (StringTools::containsChar(ss, '$')) {
            regs += " " + ss;
            foundRegs++;
        }
    }
    if (foundRegs != 3) {
        std::string msg =
            "Invalid amount of register operands, 3 required, found " +
            std::to_string(foundRegs);
        throw InvalidData(__FILE__, __LINE__, __func__, msg);
    }
    return regs;
}

std::string
LLVMTCERISCVIntrinsicsLowering::findOperationName(
    const std::string& s) const {
    std::vector<TCEString> asmSubStrings = StringTools::chopString(s, " ");
    std::string opName = "";
    for (TCEString i : asmSubStrings) {
        std::string ss = static_cast<std::string>(i);
        if (StringTools::containsChar(ss, '/')) {
            ss = StringTools::replaceAllOccurrences(ss, "//", "");
            ss = StringTools::stringToLower(ss);
            if (rFormat_->hasOperation(ss)) {
                opName = ss;
            } else {
                std::string msg = "Machine does not have operation: " + ss;
                throw InvalidData(__FILE__, __LINE__, __func__, msg);
            }
        }
    }
    if (opName == "") {
        const std::string msg = 
        "Warning: Failed to find operation name from: \"" + s + "\"";
        std::cerr << msg << std::endl;
        //std::string msg = "Failed to find operation name from: " + s;
        //throw InvalidData(__FILE__, __LINE__, __func__, msg);
    }
    return opName;
}

std::vector<int>
LLVMTCERISCVIntrinsicsLowering::findRegIndexes(
    const MachineBasicBlock::iterator& it) const {
    const unsigned OpIdx = 0;
    std::vector<int> regIdxs;
    const std::string asmString =
        std::string(it->getOperand(OpIdx).getSymbolName());
    for (unsigned o = 0; o < it->getNumOperands(); o++) {
        const MachineOperand& mo = it->getOperand(o);
        if (mo.isReg()) {
            // TODO: Fix this virtual to physical reg mapping
            #ifdef LLVM_OLDER_THAN_17
            const int magicNumber = 40;
            #else
            const int magicNumber = 41;
            #endif
            int idx = mo.getReg() - magicNumber;
            regIdxs.push_back(idx);
            if (idx < 0 && idx > 31) {
                std::string msg =
                    "Invalid register index: " + std::to_string(idx) +
                    " in instruction: " + asmString;
                throw InvalidData(__FILE__, __LINE__, __func__, msg);
            };
        }
    }
    if (regIdxs.size() != 3) {
        std::string msg = "3 reg operands required, found " +
                          std::to_string(regIdxs.size()) +
                          " in instruction: " + asmString;
        throw InvalidData(__FILE__, __LINE__, __func__, msg);
    }
    return regIdxs;
}

int
LLVMTCERISCVIntrinsicsLowering::constructEncoding(
    const std::string& opName, const std::vector<int>& regIdxs) const {
    const int fu3Mask = 0b1110000000;
    const int fu7Mask = 0b11111110000000000;
    const int opcodeMask = 0b1111111;
    int encoding = rFormat_->encoding(opName);
    const int fu3Enc = ((encoding & fu3Mask) >> 7);
    const int fu7Enc = ((encoding & fu7Mask) >> 10);
    const int opcodeEnc = encoding & opcodeMask;
    const int shiftedEnc = (fu7Enc << 25) + (fu3Enc << 12) + opcodeEnc;

    encoding = shiftedEnc + (regIdxs.at(0) << 7) + (regIdxs.at(1) << 15) +
               (regIdxs.at(2) << 20);

    return encoding;
}

bool
LLVMTCERISCVIntrinsicsLowering::runOnMachineFunction(MachineFunction& MF) {
    for (MachineFunction::iterator i = MF.begin(); i != MF.end(); i++) {
        for (MachineBasicBlock::iterator j = i->begin(); j != i->end(); j++) {
            if (j->isInlineAsm()) {
                const unsigned OpIdx = 0;
                std::string asmString =
                    std::string(j->getOperand(OpIdx).getSymbolName());
                const std::string opName = findOperationName(asmString);
                if (opName == "") {
                    continue;
                }
                const std::string regs = findRegs(asmString);
                const std::vector<int> regIdxs = findRegIndexes(j);

                int encoding = constructEncoding(opName, regIdxs);
                std::string* cName = new std::string(
                    ".long " + std::to_string(encoding) + "; #" + opName +
                    regs);
                j->getOperand(OpIdx).ChangeToES(
                    cName->c_str(), j->getOperand(OpIdx).getTargetFlags());
            }
        }
    }
    return false;
}

FunctionPass*
createRISCVIntrinsicsPass(const char* /*target*/) {
    return new LLVMTCERISCVIntrinsicsLowering();
}

}  // namespace llvm
