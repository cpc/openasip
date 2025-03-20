/*
 Copyright (C) 2024 Tampere University.

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
 * @file RISCVTDGen.cc
 *
 * Implementation of RISCVTDGen class.
 *
 * @author Kari Hepola 2024 (kari.hepola@tuni.fi)
 * @note rating: red
 */

#include "RISCVTDGen.hh"

#include "BEMGenerator.hh"
#include "BinaryEncoding.hh"
#include "Exception.hh"
#include "InstructionFormat.hh"
#include "Machine.hh"
#include "MapTools.hh"
#include "Operation.hh"
#include "OperationPool.hh"
#include "OperationTriggeredEncoding.hh"
#include "OperationTriggeredField.hh"
#include "OperationTriggeredFormat.hh"
#include "RISCVFields.hh"
#include "RISCVTools.hh"
#include "TCEString.hh"
#include "Application.hh"
#include "tce_config.h"
#include <iostream>
#include <regex>
#include <sstream>
#include <iomanip>

#define DEBUG_RISCV_TDGEN 0

RISCVTDGen::RISCVTDGen(const TTAMachine::Machine& mach, bool roccEn)
    : TDGen(mach, false), bem_(NULL) {
    bem_ = BEMGenerator(mach, roccEn).generate();
    assert(bem_ != NULL);
    findCustomOps();
    initializeBackendContents();
}

std::string
RISCVTDGen::intToHexString(int num) const {
    std::stringstream ss;
    ss << "0x" << std::hex << std::uppercase << num;
    return ss.str();
}

std::string
RISCVTDGen::unsignedToHexString(unsigned num) const {
    std::stringstream ss;
    ss << "0x" << std::hex << std::uppercase << num;
    return ss.str();
}

// TODO: OpenASIP converts hex numbers to unsigned by default. This converted
// number might not fit into i32
/**
 * OpenASIP converts hex numbers to unsigned by default. The converted
 * number might not fit into i32. This function transforms
 * the given instruction pattern to use hex encoding.
 *
 * @param pattern The instruction pattern.
 * @return The instruction pattern in hex format.
 */
std::string
RISCVTDGen::decimalsToHex(const std::string& pattern) const {
    std::regex patternRegex(R"((XLenVT\s)(-?\d+))");
    std::smatch match;
    std::string modifiedPattern = pattern;

    auto start = modifiedPattern.cbegin();
    while (std::regex_search(
        start, modifiedPattern.cend(), match, patternRegex)) {
        try {
            std::string numStr = match[2].str();
            std::string hexStr;

            if (numStr[0] == '-') {
                int num = std::stoi(numStr);
                hexStr = TCEString::intToHexString(num);
            } else {
                unsigned long num = std::stoul(numStr);
                hexStr = TCEString::unsignedToHexString(num);
            }

            // Calculate the actual start position in the modified string
            size_t matchPos = match.position(2) + std::distance(
                modifiedPattern.cbegin(), start);
            modifiedPattern.replace(matchPos, match.length(2), hexStr);
            // Move start past the replaced part
            start = modifiedPattern.cbegin() + matchPos + hexStr.length();
        } catch (const std::invalid_argument& e) {
            std::cerr << "Invalid argument: " << e.what() << std::endl;
            break;
        } catch (const std::out_of_range& e) {
            std::cerr << "Out of range error: " << e.what() << std::endl;
            break;
        }
    }

    return modifiedPattern;
}


void
RISCVTDGen::findCustomOps() {
    customOps_.clear();
    const std::vector<std::string> formatsToSearch = {
        RISCVFields::RISCV_R_TYPE_NAME,
        RISCVFields::RISCV_R1R_TYPE_NAME,
        RISCVFields::RISCV_R1_TYPE_NAME,
        RISCVFields::RISCV_R3R_TYPE_NAME
    };
    for (const std::string& fName : formatsToSearch) {
        InstructionFormat* format = bem_->instructionFormat(fName);
        if (format == NULL) {
            continue;
        }
        for (int i = 0; i < format->operationCount(); i++) {
            const std::string op = format->operationAtIndex(i);
            if (!MapTools::containsKey(RISCVFields::RISCVRTypeOperations, op)) {
                customOps_.insert({op, format->encoding(op)});
            }
        }
    }
}

// TODO: make this mapping better so that it works for any numIns

std::string
RISCVTDGen::getFormatType(const std::string& opName) const {
    OperationPool opPool;
    Operation& op = opPool.operation(opName.c_str());
    const unsigned numIns = op.numberOfInputs();
    const unsigned numOuts = op.numberOfOutputs();
    if (numIns == 3 && numOuts == 1) {
        return "OARVR3R";
    } else if (numIns == 2 && numOuts == 1) {
        return "OARVR2R";
    } else if (numIns == 1 && numOuts == 1) {
        return "OARVR1R";
    } else if (numIns == 1 && numOuts == 0) {
        return "OARVR1";
    } else {
        std::cerr << "Error: cannot find format type for operation "
                << op.name() << " with numIns=" << numIns << " numOuts="
                << numOuts << std::endl;
        std::cerr << "Legal configurations: " << std::endl;
        std::cerr << "  numIns=3, numOuts=1"  << std::endl;
        std::cerr << "  numIns=2, numOuts=1"  << std::endl;
        std::cerr << "  numIns=1, numOuts=1"  << std::endl;
        std::cerr << "  numIns=1, numOuts=0"  << std::endl;
        assert(false);
    }
    return "";
}

void
RISCVTDGen::writeInstructionDeclaration(std::ostream& o,
                                        const std::string& name,
                                        const int encoding) const {
    const TCEString opName = TCEString(name);
    const std::string f3 = RISCVTools::getFunc3Str(encoding);
    std::string f7 = RISCVTools::getFunc7Str(encoding);
    const std::string fType = getFormatType(name);
    assert(fType != "");
    std::string opc = "OPC_CUSTOM_0";
    if (fType == "OARVR3R") {
        std::string opc = "OPC_CUSTOM_1";
        // rs3 takes 5 bits from f7
        f7 = RISCVTools::getFunc2Str(encoding);
    } else {
        f7 = RISCVTools::getFunc7Str(encoding);
    }
    o << "def " << "OA_" + opName.upper() << " : " << fType << "<" << f7
      <<", " << f3 << ", " << opc << ", \"" << "oa_" + name << "\">;";
    o << "\n";
}

void
RISCVTDGen::writeInstructionDeclarations(std::ostream& o) const {
    dumpClassDefinitions(o);
    for (auto op : customOps_) {
        writeInstructionDeclaration(o, op.first, op.second);
    }
}

/**
 * OpenASIP uses different reg declarations and pattern structure than the
 * upstream RISC-V BE. This method transforms the TDGen generated patterns
 * to the RISC-V one.
 *
 * @param pattern The pattern generated by TDGen
 * @return The RISC-V target pattern
 */
std::string
RISCVTDGen::transformTCEPattern(std::string pattern,
                                const unsigned numIns) const {
    assert(numIns == 3 || numIns == 2 || numIns == 1);
    // Remove the output specifier
    size_t pos = pattern.find(',');
    if (pos != std::string::npos) {
        pattern = pattern.substr(pos + 2);
    }
    // Replace register specifiers
    TCEString patTCEStr = TCEString(pattern);
    patTCEStr.replaceString("R32IRegs:$op1", "(XLenVT GPR:$rs1)");
    patTCEStr.replaceString("i32 ", "XLenVT ");
    if (numIns == 3) {
        patTCEStr.replaceString("R32IRegs:$op2", "(XLenVT GPR:$rs2)");
        patTCEStr.replaceString("R32IRegs:$op3", "(XLenVT GPR:$rs3)");
        patTCEStr.replaceString("R32IRegs:$op4", "(XLenVT GPR:$rd)");
    } else if (numIns == 2) {
        patTCEStr.replaceString("R32IRegs:$op2", "(XLenVT GPR:$rs2)");
        patTCEStr.replaceString("R32IRegs:$op3", "(XLenVT GPR:$rd)");
    } else {
        patTCEStr.replaceString("R32IRegs:$op2", "(XLenVT GPR:$rd)");
    }
    patTCEStr.replaceString("\n", "");
    patTCEStr = decimalsToHex(patTCEStr);
    return patTCEStr;
}

void
RISCVTDGen::writePatternDefinition(std::ostream& o, Operation& op) {
    const unsigned numIns = op.numberOfInputs();
    const unsigned numOuts = op.numberOfOutputs();
    if (!operationCanBeMatched(op) || numIns > 3 || numIns < 1 ||
        numOuts != 1) {
        std::cout << "Skipping pattern for " << op.name() << std::endl;
        return;
    }
    std::string pattern;
    const std::string operandTypes =
        mach_.is64bit() ? std::string(numIns + numOuts, OT_REG_LONG)
                        : std::string(numIns + numOuts, OT_REG_INT);

    std::vector<OperationDAG*> dags;
    OperationDAG* trivialDag = NULL;
    const char operandChar = mach_.is64bit() ? OT_REG_LONG : OT_REG_INT;
    
    if (TDGen::llvmOperationPattern(op, operandChar) != "" ||
        op.dagCount() == 0) {
        trivialDag = createTrivialDAG(op);
        dags.push_back(trivialDag);
    } else {
        const std::vector<OperationDAG*> matchableDAGs =
            getMatchableOperationDAGs(op);
        dags.insert(dags.end(), matchableDAGs.begin(), matchableDAGs.end());
    }
    for (const OperationDAG* dag : dags) {
        pattern = operationPattern(op, *dag, operandTypes);
        o << "def : Pat<(XLenVT ";
        o << transformTCEPattern(pattern, numIns);
        o << ", (" << "OA_" + op.name().upper();
        if (numIns == 3) {
            o << " GPR:$rs1, GPR:$rs2, GPR:$rs3)>;\n";
        } else if (numIns == 2) {
            o << " GPR:$rs1, GPR:$rs2)>;\n";
        } else {
            o << " GPR:$rs1)>;\n";
        }
    }
    if (trivialDag != NULL) {
        delete trivialDag;
    }
}

void
RISCVTDGen::writePatternDefinitions(std::ostream& o) {
    OperationPool opPool;
    // o << "// ---- Instruction pattern defs start ----\n\n";
    for (auto customOp : customOps_) {
        const std::string opName = customOp.first;
        Operation& op = opPool.operation(opName.c_str());
        writePatternDefinition(o, op);
    }
    // o << "// ---- Instruction pattern defs end   ----\n\n";
}

void
RISCVTDGen::initializeBackendContents() {
    std::ostringstream tdDeclarationBuffer;
    std::ostringstream tdPatternBuffer;
    writeInstructionDeclarations(tdDeclarationBuffer);
    declarationStr_ = tdDeclarationBuffer.str();
    writePatternDefinitions(tdPatternBuffer);
    patternStr_ = tdPatternBuffer.str();

#ifdef DEBUG_RISCV_TDGEN
    std::cout << declarationStr_;
    std::cout << patternStr_;
#endif
}

void
RISCVTDGen::generateBackend(const std::string& path) const {
    std::ofstream customInstrInfoTD;
    customInstrInfoTD.open((path + "/RISCVInstrInfoOpenASIP.td").c_str());
    customInstrInfoTD << declarationStr_;
    customInstrInfoTD << patternStr_;
    customInstrInfoTD.close();
}

std::string
RISCVTDGen::generateBackend() const {
    return declarationStr_;
}

void
RISCVTDGen::dumpClassDefinitions(std::ostream& o) const {
    o << "/*\n";
    o << " * Generated by OpenASIP\n";
    /* These are not needed since the target contents would be different
    between versions if necessary. */
    o << " * OpenASIP version: " << Application::TCEVersionString() << "\n";
    o << " * LLVM version: " << LLVM_VERSION << "\n";
    o << " */\n";
    o << "\n";
    o << "class OARVInstR1<bits<7> funct7, bits<3> funct3, RISCVOpcode opcode,\n";
    o << "    dag outs, dag ins, string opcodestr, string argstr>\n";
    o << "    : RVInst<outs, ins, opcodestr, argstr, [], InstFormatOther> {\n";
    o << "  bits<5> rs1;\n";
    o << "\n";
    o << "  let Inst{31-25} = funct7;\n";
    o << "  let Inst{24-20} = 0;\n";
    o << "  let Inst{19-15} = rs1;\n";
    o << "  let Inst{14-12} = funct3;\n";
    o << "  let Inst{11-7} = 0;\n";
    o << "  let Inst{6-0} = opcode.Value;\n";
    o << "}\n";
    o << "\n";
    o << "class OARVInstR1R<bits<7> funct7, bits<3> funct3, RISCVOpcode opcode,\n";
    o << "    dag outs, dag ins, string opcodestr, string argstr>\n";
    o << "    : RVInst<outs, ins, opcodestr, argstr, [], InstFormatOther> {\n";
    o << "  bits<5> rs1;\n";
    o << "  bits<5> rd;\n";
    o << "\n";
    o << "  let Inst{31-25} = funct7;\n";
    o << "  let Inst{24-20} = 0;\n";
    o << "  let Inst{19-15} = rs1;\n";
    o << "  let Inst{14-12} = funct3;\n";
    o << "  let Inst{11-7} = rd;\n";
    o << "  let Inst{6-0} = opcode.Value;\n";
    o << "}\n";
    o << "\n";
    o << "let hasSideEffects = 0, mayLoad = 0, mayStore = 0 in\n";
    o << "class OARVR2R<bits<7> funct7, bits<3> funct3, RISCVOpcode opcode, string\n";
    o << "             opcodestr, bit Commutable = 0>\n";
    o << "    : RVInstR<funct7, funct3, opcode, (outs GPR:$rd), (ins GPR:$rs1,\n";
    o << "              GPR:$rs2), opcodestr, \"$rd, $rs1, $rs2\"> {\n";
    o << "  let isCommutable = Commutable;\n";
    o << "}\n";
    o << "\n";
    o << "let hasSideEffects = 0, mayLoad = 0, mayStore = 0 in\n";
    o << "class OARVR3R<bits<2> funct2, bits<3> funct3, RISCVOpcode opcode, string\n";
    o << "             opcodestr, bit Commutable = 0>\n";
    o << "    : RVInstR4<funct2, funct3, opcode, (outs GPR:$rd), (ins GPR:$rs1,\n";
    o << "              GPR:$rs2, GPR:$rs3), opcodestr, \"$rd, $rs1, $rs2, $rs3\"> {\n";
    o << "  let isCommutable = Commutable;\n";
    o << "}\n";
    o << "\n";
    o << "let hasSideEffects = 0, mayLoad = 0, mayStore = 0 in\n";
    o << "class OARVR1R<bits<7> funct7, bits<3> funct3, RISCVOpcode opcode, string\n";
    o << "           opcodestr>\n";
    o << "    : OARVInstR1R<funct7, funct3, opcode, (outs GPR:$rd), (ins GPR:$rs1),\n";
    o << "              opcodestr, \"$rd, $rs1\"> {\n";
    o << "}\n";
    o << "\n";
    o << "let hasSideEffects = 1, mayLoad = 0, mayStore = 0 in\n";
    o << "class OARVR1<bits<7> funct7, bits<3> funct3, RISCVOpcode opcode, string\n";
    o << "           opcodestr>\n";
    o << "    : OARVInstR1<funct7, funct3, opcode, (outs), (ins GPR:$rs1),\n";
    o << "              opcodestr, \"$rs1\"> {\n";
    o << "}\n";
}
