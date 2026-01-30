 /*
    Copyright (c) 2002-2025 Tampere University.

    This file is part of OpenASIP.

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
 * Architecture plugin generator for LLVM TCE backend.
 *
 * @author Veli-Pekka Jääskeläinen 2007 (vjaaskel-no.spam-cs.tut.fi)
 * @author Mikael Lepistö 2009 (mikael.lepisto-no.spam-tut.fi)
 * @author Pekka Jääskeläinen 2010-2011
 * @author Heikki Kultala 2012
 *
 * @note rating: red
 */

#include <fstream>
#include <algorithm>

#include "TDGen.hh"
#include "Machine.hh"
#include "ADFSerializer.hh"
#include "ControlUnit.hh"
#include "Operation.hh"
#include "HWOperation.hh"
#include "FUPort.hh"
#include "Conversion.hh"
#include "MachineConnectivityCheck.hh"
#include "ImmediateAnalyzer.hh"
#include "ImmInfo.hh"
#include "Bus.hh"
#include "Guard.hh"
#include "StringTools.hh"
#include "OperationPool.hh"
#include "OperationNode.hh"
#include "TerminalNode.hh"
#include "ConstantNode.hh"
#include "OperationDAG.hh"
#include "OperationDAGEdge.hh"
#include "OperationDAGSelector.hh"
#include "TCEString.hh"
#include "Operand.hh"
#include "Application.hh"
#include "LLVMBackend.hh" // llvmRequiredOps..
#include "MathTools.hh"
#include "tce_config.h"
#include "LLVMTCECmdLineOptions.hh"
#include "MachineInfo.hh"

// -----------
/// @todo These two lines can be removed after C++11 features can be used in
/// the source code, since it has native support for initializing std::map.
#include "boost/assign.hpp"
using namespace boost::assign;
// -----------

using std::endl;

using namespace TDGenerator;

// Vector value types that can be used in TTA machines.
const std::set<TCEString> ValueType::SUPPORTED_LLVM_VALUE_TYPES =
    list_of
    ("v2i1")   ("v4i1")   ("v8i1")   ("v16i1")             // i1 subword
    ("v2i8")   ("v4i8")   ("v8i8")   ("v16i8")  ("v32i8")  // i8 subword
    ("v2i16")  ("v4i16")  ("v8i16")  ("v16i16")            // i16 subword
    ("v1i32")  ("v2i32")  ("v4i32")  ("v8i32")  ("v16i32") // i32 subword
    ("v2f16")  ("v4f16")  ("v8f16")                        // f16 subword
    ("v2f32")  ("v4f32")  ("v8f32") ("v16f32")             // f32 subword
    ("v32i1")  ("v64i1")                                   // i1 subword
    ("v64i8")                                              // i8 subword
    ("v32i16")                                             // i16 subword

    ("v512i1") ("v1024i1") ("v128i8") ("v256i8")
    ("v64i16") ("v128i16") ("v32i32") ("v64i32")

    ("v128i1")
    ("v2048i1")
    ("v256i8")                                             // i8 subword

// New values that the custom vector extension patch unlocks.
#if defined(LLVM_HAS_CUSTOM_VECTOR_EXTENSION)
    ("v16f16") ("v32f16") ("v64f16")                       // f16 subword
    ("v32f32")                                             // f32 subword

    ("v256i1")
    ("v2048i1")
    ("v128i1")                                             // i1 subword

// new wider extension, for up to 4096 bits
#if LLVM_HAS_CUSTOM_VECTOR_EXTENSION == 2
    ("v4096i1") ("v512i8") ("v256i16") ("v256f16") ("v128i32") ("v128f32")
    ("v64f32") ("v128f16")

#endif
#endif
;

const int TDGen::FP_SUBW_WIDTH =
    Operand::defaultElementWidth(Operand::FLOAT_WORD);
const int TDGen::HFP_SUBW_WIDTH =
    Operand::defaultElementWidth(Operand::HALF_FLOAT_WORD);
const int TDGen::BOOL_SUBW_WIDTH =
    Operand::defaultElementWidth(Operand::BOOL);

const int TDGen::MAX_SCALAR_WIDTH = 64;

// Same as in generate_simd.py.in
const int TDGen::MAX_SUBW_COUNT = SIMD_WORD_WIDTH / BYTE_BITWIDTH;

const bool TDGen::EXPLOIT_BIGGER_REGISTERS = true;

// Operand type characters.
const char TDGen::OT_REG_BOOL = 'b';
const char TDGen::OT_REG_INT = 'r';
const char TDGen::OT_REG_FP = 'f';
const char TDGen::OT_REG_HFP = 'h';
const char TDGen::OT_REG_LONG = 's';
const char TDGen::OT_REG_DOUBLE = 'd';
const char TDGen::OT_IMM_BOOL = 'j';
const char TDGen::OT_IMM_INT = 'i';
const char TDGen::OT_IMM_FP = 'k';
const char TDGen::OT_IMM_HFP = 'l';
const char TDGen::OT_IMM_LONG = 'a';
const char TDGen::OT_VREG_BOOL = 'a';
const char TDGen::OT_VREG_INT8 = 'q';
const char TDGen::OT_VREG_INT16 = 't';
const char TDGen::OT_VREG_INT32 = 'u';
const char TDGen::OT_VREG_FP = 'e';
const char TDGen::OT_VREG_HFP = 'g';

// Operation base names (key) that have a pattern (value). Add new
// OSAL vector operations here, which have an LLVM pattern.
const std::map<TCEString, TCEString> TDGen::OPERATION_PATTERNS_ =
    map_list_of
    ("load","load %1%")
    ("store","store %2%, %1%")

    ("ld","load %1%")
    ("ld8","load %1%")
    ("ld16","load %1%")
    ("ld32","load %1%")

    ("st","store %2%, %1%")
    ("st8","store %2%, %1%")
    ("st16","store %2%, %1%")
    ("st32","store %2%, %1%")

    ("ldq","load %1%")
    ("ldh","load %1%")
    ("ldw","load %1%")
    ("stq","store %2%, %1%")
    ("sth","store %2%, %1%")
    ("stw","store %2%, %1%")

    ("not","not %1%")
    ("and","and %1%, %2%")
    ("ior","or %1%, %2%")
    ("xor","xor %1%, %2%")
    ("sxqh","sext %1%")
    ("sxqw","sext %1%")
    ("sxhw","sext %1%")
    ("sxbw","sext %1%")
    ("sxbh","sext %1%")
    ("sxbq","sext %1%")
    ("zxqh","zext %1%")
    ("zxqw","zext %1%")
    ("zxhw","zext %1%")
    ("truncwh","trunc %1%")
    ("truncwb","trunc %1%")
    ("trunchb","trunc %1%")

    ("shl","shl %1%, %2%")
    ("shr","sra %1%, %2%")
    ("shru","srl %1%, %2%")
    ("add","add %1%, %2%")
    ("sub","sub %1%, %2%")
    ("mul","mul %1%, %2%")
    ("div","sdiv %1%, %2%")
    ("divu","udiv %1%, %2%")
    ("eq","seteq %1%, %2%")
    ("ne","setne %1%, %2%")
    ("gt","setgt %1%, %2%")
    ("gtu","setugt %1%, %2%")
    ("ge","setge %1%, %2%")
    ("geu","setuge %1%, %2%")
    ("lt","setlt %1%, %2%")
    ("ltu","setult %1%, %2%")
    ("le","setle %1%, %2%")
    ("leu","setule %1%, %2%")
    ("mac","add %1%, (mul %2%, %3%)")

    ("addf","fadd %1%, %2%")
    ("subf","fsub %1%, %2%")
    ("mulf","fmul %1%, %2%")
    ("divf","fdiv %1%, %2%")
    ("absf","fabs %1%")
    ("negf","fneg %1%")
    ("eqf","setoeq %1%, %2%")
    ("equf","setueq %1%, %2%")
    ("nef","setone %1%, %2%")
    ("neuf","setune %1%, %2%")
    ("gtf","setogt %1%, %2%")
    ("gtuf","setugt %1%, %2%")
    ("gef","setoge %1%, %2%")
    ("geuf","setuge %1%, %2%")
    ("ltf","setolt %1%, %2%")
    ("ltuf","setult %1%, %2%")
    ("lef","setole %1%, %2%")
    ("leuf","setule %1%, %2%")
    ("macf","fadd %1%, (fmul %2%, %3%)")
    ("msuf","fsub %1%, (fmul %2%, %3%)")
    ("cif","sint_to_fp %1%")
    ("cfi","fp_to_sint %1%")
    ("cifu","uint_to_fp %1%")
    ("cfiu","fp_to_uint %1%")
    ("sqrtf","fsqrt %1%")
    ("chf","fpextend %1%")
    ("cfh","fpround %1%")
    ("csh","sint_to_fp %1%")
    ("cshu","uint_to_fp %1%")
    ("chs","fp_to_sint %1%")
    ("chsu","fp_to_uint %1%")

    ("addh","fadd %1%, %2%")
    ("subh","fsub %1%, %2%")
    ("mulh","fmul %1%, %2%")
    ("divh","fdiv %1%, %2%")
    ("absh","fabs %1%")
    ("negh","fneg %1%")
    ("eqh","setoeq %1%, %2%")
    ("neh","setone %1%, %2%")
    ("neuh","setune %1%, %2%")
    ("gth","setogt %1%, %2%")
    ("geh","setoge %1%, %2%")
    ("lth","setolt %1%, %2%")
    ("leh","setole %1%, %2%")
    ("mach","fadd %1%, (fmul %2%, %3%)")
    ("msuh","fsub %1%, (fmul %2%, %3%)")

    ("vselect","vselect %3%, %1%, %2%")
    ("insertelem","vector_insert %1%, %2%, %3%")
    ("extractelem", "vector_extract %1%, %2%")
    //("gather", "") /// @todo Once supported by LLVM
    //("scatter", "") /// @todo Once supported by LLVM
    // "pack" /// @note Has varying pattern, and thus, is generated separately.

    // Utilization of the following vector operations is defined in the
    // custom instruction selector since they don't have an LLVM pattern.
    // "andsame"
    // "iorsame"
    // "xorsame"
    // "shlsame"
    // "shrsame"
    // "shrusame"
    // "vbcast"
    // "vshuffle1"
    // "vshuffle2"

    // The following can be currently used only by invoking _TCE intrinsic.
    // "unpack"
    ;

/**
 * Custom comparator that orders operation names correctly number-wise.
 *
 * For instance, in an std::set operation names are ordered followingly:
 * ADD32X16, ADD32X4, ADD32X8, XOR1024, XOR512, XOR64
 *
 * This function corrects the operation name order to be the following:
 * ADD32X4, ADD32X8, ADD32X16, XOR64, XOR512, XOR1024
 */
bool
numbersToAscending(const TCEString& lhs, const TCEString& rhs) {
    size_t lhsI = lhs.length() - 1;
    size_t rhsI = rhs.length() - 1;

    // Reverse operation name to the position where the last number begins.
    while (lhsI > 0 && std::isdigit(lhs[lhsI])) --lhsI;
    while (rhsI > 0 && std::isdigit(rhs[rhsI])) --rhsI;

    // Make sure both strings contain a number.
    if (lhsI > 0 && lhsI != lhs.length() - 1 && rhsI > 0 &&
        rhsI != rhs.length() - 1) {
        // The string part before the number must match between both names.
        if (lhs.substr(0, lhsI + 1) == rhs.substr(0, rhsI + 1)) {
            // Make the smaller number precede the larger one.
            try {
                int lhsNumber = Conversion::toInt(lhs.substr(lhsI + 1));
                int rhsNumber = Conversion::toInt(rhs.substr(rhsI + 1));
                return lhsNumber < rhsNumber;
            } catch (const NumberFormatException& e) {
                abortWithError(e.errorMessage());
            }
        }
    }

    // Basic comparison for std::string.
    return lhs < rhs;
}

// SP, RES, KLUDGE, 2 GPRs?
//unsigned const TDGen::REQUIRED_I32_REGS = 5;

/* Leave this part (percent) of regs unallocated in each RF so
   the scheduler's reg renamer always has some registers.
   Rounds downwards.

   This probably should be a progressive percentage. More regs you 
   have, less likely is that you need them to avoid spills and more 
   likely it is that the reg renamer can use them to remove restricting 
   antideps.
*/
static const unsigned REG_RENAMER_PART = 0;
const std::string TDGen::guardRegTemplateName = "Guard";


/**
 * Constructor.
 *
 * @param mach Machine to generate plugin for.
 */
TDGen::TDGen(const TTAMachine::Machine& mach, bool initialize) :
    mach_(mach), immInfo_(NULL), dregNum_(0), maxVectorSize_(0),
    highestLaneInt_(-1), highestLaneBool_(-1),
    hasExBoolRegs_(false), hasExIntRegs_(false), hasSelect_(false),
    littleEndian_(mach.isLittleEndian()),
    argRegCount_(1),
    requiredI32Regs_(0), requiredI64Regs_(0), prebypassStackIndeces_(false),
    use64bitForFP_(false) {
    // These are TTA-specific, which we don't want to run for other targets
    if (initialize) {
        tempRegFiles_ = MachineConnectivityCheck::tempRegisterFiles(mach);

        prebypassStackIndeces_ = false;

        if (mach.is64bit()) {
            std::set<int> scalarWidths { 1, 32 };
            hasConditionalMoves_ =
                MachineConnectivityCheck::hasConditionalMoves(mach_, scalarWidths);

            // TODO: this does not make sense! requires more!
            requiredI64Regs_ = 0;
            requiredI64Regs_ += argRegCount_;

        } else {
            std::set<int> scalarWidths { 1, 32 };
            hasConditionalMoves_ =
                MachineConnectivityCheck::hasConditionalMoves(mach_, scalarWidths);

            // TODO: this does not make sense! requires more!
            requiredI32Regs_ = 0;
            requiredI32Regs_ += argRegCount_;
        }
        immInfo_ = ImmediateAnalyzer::analyze(mach_);
        maxScalarWidth_ = mach.is64bit() ? 64 : 32;
        initializeBackendContents();
    }

}

/**
 * Destructor.
 */
TDGen::~TDGen() {
    if (immInfo_ != NULL) {
        delete immInfo_;
    }
}
/*
 * Initializes backend components so that they are not generated
 * multiple times, as we need to use them both for hash generation
 * and the *.td files.
*/
void
TDGen::initializeBackendContents() {
    std::ostringstream registerInfoBuffer;
    std::ostringstream addressingModeDefsBuffer; 
    std::ostringstream operandDefsBuffer;
    std::ostringstream instrInfoBuffer;
    std::ostringstream instrFormatsBuffer;
    std::ostringstream callingConvBuffer;
    std::ostringstream argRegsArrayBuffer;
    std::ostringstream backendCodeBuffer;
    std::ostringstream topLevelTDBuffer;

    writeRegisterInfo(registerInfoBuffer);
    writeAddressingModeDefs(addressingModeDefsBuffer);
    writeOperandDefs(operandDefsBuffer);
    writeInstrInfo(instrInfoBuffer);
    writeInstrFormats(instrFormatsBuffer);
    writeCallingConv(callingConvBuffer);
    writeArgRegsArray(argRegsArrayBuffer);
    writeBackendCode(backendCodeBuffer);
    writeTopLevelTD(topLevelTDBuffer);

    registerInfo_ = registerInfoBuffer.str();
    addressingModeDefs_ = addressingModeDefsBuffer.str();
    operandDefs_ = operandDefsBuffer.str();
    instrInfo_ = instrInfoBuffer.str();
    instrFormats_ = instrFormatsBuffer.str();
    callingConv_ = callingConvBuffer.str();
    argRegsArray_ = argRegsArrayBuffer.str();
    backendCode_ = backendCodeBuffer.str();
    topLevelTD_ = topLevelTDBuffer.str();
}

/**
 * Generates all files required to build a tce backend plugin
 * (excluding static plugin code included from include/llvm/TCE/).
 */
void
TDGen::generateBackend(const std::string& path) const {
    std::ofstream regTD;
    regTD.open((path + "/GenRegisterInfo.td").c_str());
    regTD << registerInfo_;
    regTD.close();

    std::ofstream instrTD0;
    instrTD0.open((path + "/GenInstrInfo0.td").c_str());
    instrTD0 << addressingModeDefs_;
    instrTD0.close();

    std::ofstream operandTD;
    operandTD.open((path + "/GenOperandInfo.td").c_str());
    operandTD << operandDefs_;
    operandTD.close();

    std::ofstream instrTD;
    instrTD.open((path + "/GenInstrInfo.td").c_str());
    instrTD << instrInfo_;
#ifdef DEBUG_TDGEN
    writeInstrInfo(std::cerr);
#endif
    instrTD.close();

    std::ofstream formatTD;
    formatTD.open((path + "/GenTCEInstrFormats.td").c_str());
    formatTD << instrFormats_;
    formatTD.close();

    std::ofstream ccTD;
    ccTD.open((path + "/GenCallingConv.td").c_str());
    ccTD << callingConv_;
    ccTD.close();

    std::ofstream argArr;
    argArr.open((path + "/ArgRegs.hh").c_str());
    argArr << argRegsArray_;
    argArr.close();

    std::ofstream pluginInc;
    pluginInc.open((path + "/Backend.inc").c_str());
    pluginInc << backendCode_;
    pluginInc.close();

    std::ofstream topLevelTD;
    topLevelTD.open((path + "/TCE.td").c_str());
    topLevelTD << topLevelTD_;
    topLevelTD.close();
}

/**
 * Writes tce backend plugin code into a single string.
 * Used for hash generation.
 */
std::string
TDGen::generateBackend() const {
    const std::string buffer = registerInfo_ + addressingModeDefs_
        + operandDefs_ + instrInfo_ + instrFormats_ + callingConv_
        + argRegsArray_ + backendCode_ + topLevelTD_;
    return buffer;
}

/**
 * Writes .td definition of a single register to the output stream.
 *
 * @param o Output stream to write the definition to.
 * @param reg Information about the physical register.
 * @param regName Name for the register in the llvm framework.
 * @param regTemplate Base class for the register.
 * @param aliases Comma-separated list of aliases for this register.
 */
void
TDGen::writeRegisterDef(
    std::ostream& o,
    const RegInfo& reg,
    const std::string regName,
    const std::string regTemplate,
    const std::string aliases,
    RegType type) {

    std::string templ = regTemplate;

    o << "def " << regName << " : " << templ
      << "<\"" << reg.rf << "." << reg.idx
      << "\", [" << aliases << "]>, DwarfRegNum<"
      << "[" << dregNum_ << "]>;"
      << std::endl;


    if (type == GPR) {
        gprRegNames_.push_back(regName);
    } else if (type == ARGUMENT) {
        argRegNames_.push_back(regName);
    } else if (type == RESULT) {
        argRegNames_.push_back(regName);
        resRegNames_.push_back(regName);
    } else {
        assert(type == RESERVED);
    }

    regs_[regName] = reg;

    regsInClasses_[regTemplate].push_back(regName);

    if (guardedRegs_.find(reg) != guardedRegs_.end()) {
        llvmGuardRegs_.push_back(regName);
        regsInClasses_[guardRegTemplateName].push_back(regName);
    }

    dregNum_++;
}


/**
 * Writes .td definitions of all registers in the machine to an output stream.
 *
 * @param o Output stream for the .td definitions.
 * @return True, if the definitions were succesfully generated.
 */
bool
TDGen::writeRegisterInfo(std::ostream& o) {
    analyzeRegisters();

    if (!checkRequiredRegisters()) {
        return false;
    }

    assert(regs32bit_.size() >= requiredI32Regs_);

    // Write scalar register information without old vector backend registers.
    writeStartOfRegisterInfo(o);
    writeRegisterClasses(o);
    writeRARegisterInfo(o);
    write1bitRegisterInfo(o);
//    write16bitRegisterInfo(o); // currently not supported properly
    write32bitRegisterInfo(o);
    write64bitRegisterInfo(o);

    // Gather and separate all operations to vector and scalar containers.
    gatherAllMachineOperations();

    // Create register classes for vector operands.
    analyzeMachineVectorRegisterClasses();
    // Group registers by width.
    analyzeMachineRegisters();
    // Associate register groups properly with register classes.
    associateRegistersWithVectorRegisterClasses();

    // Write register related information to the .td file.
    writeVectorRegisterBaseClasses(o);
    writeVectorRegisterNames(o);
    writeVectorRegisterClasses(o);
    writeGuardRegisterClassInfo(o);

    return true;
}

/**
 * Writes static register info to the beginning of register info .td file.
 *
 * @param o Output stream to the file.
 */
void
TDGen::writeStartOfRegisterInfo(std::ostream& o) {
    o << "//" << endl;
    o << "// This file is generated automatically!" << endl;
    o << "// Do not edit." << endl;
    o << "//" << endl;
    o << endl;

    o << "class TCEReg<string n, list<Register> aliases> : "
      << "Register<n> {"
      << endl;

    o << "   let Namespace = \"TCE\";" << endl;
    o << "   let Aliases = aliases;" << endl;
    o << "}" << endl;
    o << "class TCEVectorReg<string n, list<Register> subregs> : "
      << "RegisterWithSubRegs<n, subregs> {"
      << endl
      << "    let Namespace = \"TCE\";" << endl
      << "}\n" << endl;
}

/**
 * Writes all short immediate definitions to the stream.
 *
 * @param o The output stream.
 */
void
TDGen::writeOperandDefs(std::ostream& o) {
    assert(immInfo_ != nullptr
        && "Short immediate analysis results are not available!");

    o << "// Immediate definitions for TCE instructions." << std::endl;

    writeIntegerImmediateDefs(o, *immInfo_);

    o << std::endl;
    o << "// Immediate definitions for immediate to register moves."
      << std::endl;
    writeMoveImmediateDefs(o);

    o << std::endl;
}

/**
 * Writes unique immediate operands (ImmLeafs) for all operations that can have
 * short immediates to transported to theirs operands.
 *
 * The names of the immediate operands are stored in i32immOperandDefs_ map.
 * Note: operations, that can not have short immediates to be trasported at
 * all, do not entry in the map.
 *
 * @param o The output stream.
 * @param iivs The immediate info.
 */
void
TDGen::writeIntegerImmediateDefs(
    std::ostream& o,
    const ImmInfo& iivs) {

    TCEString immDefBaseName = mach_.is64bit() ? "i64imm" : "i32imm";
    TCEString immDefType = mach_.is64bit() ? "i64" : "i32";
    int bits = mach_.is64bit() ? 64 : 32;
    using ImmBounds = std::pair<int64_t, int64_t>;

    OperationPool opPool;
    std::set<const Operation*> opset;
    for (auto& iiv : iivs) {
        const Operation& op = opPool.operation(iiv.first.first.c_str());
        if (&op == &NullOperation::instance())
            continue;
        opset.insert(&op);
    }

    std::map<ImmBounds, std::string> immediateClasses;
    int immId = 0;
    for (auto op : opset) {
        // Divide input operands into groups that can be swapped together.
        // Commutative instructions with immediate operands in the patterns
        // are defined having the immediate operands as last operands
        // (e.g. ADDrri). For these a combined ImmLeaf is created (union of
        // each of ImmLeaf of the inputs).
        std::vector<std::set<int>> inputGroups;
        for (int opdIdx = 1; opdIdx < op->numberOfInputs() + 1; opdIdx++) {
            const Operand& opd = op->operand(opdIdx);

            bool alreadyGrouped = false;
            for (auto& group : inputGroups) {
                if (group.count(opdIdx)) {
                    alreadyGrouped = true;
                    break;
                }
            }
            if (alreadyGrouped) continue;

            inputGroups.push_back(std::set<int>());
            inputGroups.back().insert(opdIdx);
            for (int otherOpdIdx : opd.swap()) {
                inputGroups.back().insert(otherOpdIdx);
            }
        }

        // For each input operand group assign an ImmLeaf operand definition.
        // The names of ImmLeaf definitions are stored in immOperandDefs_,
        // which is accessed using operation and operand as the key.
        for (auto group : inputGroups) {
            ImmBounds widestImmBound;
            for (auto idx : group) {
                // Determine max transportable immediate value range.
                if (iivs.count(*op, idx)) {
                    // perform union
                    ImmBounds immBound = iivs.immediateValueBounds(
                        *op, idx, bits);
                    widestImmBound.first = std::min(
                        widestImmBound.first, immBound.first);
                    widestImmBound.second = std::max(
                        widestImmBound.second, immBound.second);
                }
                // Can not transport immediates at all.
                if (widestImmBound == ImmBounds()) continue;

                // immediateClasses stores unique encountered immediate value
                // ranges.
                if (immediateClasses.count(widestImmBound) == 0) {
                    std::string immDefName = std::string("i32imm")
                        + Conversion::toString(immId);
                    if (!mach_.is64bit()) {
                        immediateClasses[widestImmBound] = immDefName;
                    }
                    writeImmediateDef(
                        o, immDefName, "i32", immediatePredicate(
                        widestImmBound.first, widestImmBound.second));

                    immDefName = std::string("i64imm")
                        + Conversion::toString(immId);

                    if (mach_.is64bit()) {
                        immediateClasses[widestImmBound] = immDefName;
                    }

                    writeImmediateDef(
                        o, immDefName, "i64", immediatePredicate(
                        widestImmBound.first, widestImmBound.second));

                    immId++;

                }
                immOperandDefs_[ImmInfo::key(*op, idx)] =
                    immediateClasses.at(widestImmBound);
            }
        }
    }
}

/**
 * Writes immediate operand definitions that are used for MOV instructions.
 *
 * Creates immediate operand definitions for immediate to register instructions
 * (MOVI), that only accept supported immediate values.
 *
 * @note Implemented currently only for i32 registers.
 */
void
TDGen::writeMoveImmediateDefs(std::ostream& o) {
    using MCC = MachineConnectivityCheck;

    TCEString immDefBaseName = mach_.is64bit() ? "i64imm" : "i32imm";
    TCEString immDefType = mach_.is64bit() ? "i64" : "i32";
    int bits = mach_.is64bit() ? 64 : 32;

    std::pair<int64_t, uint64_t> moveImm{ 0, 0 };
    for (auto& rf : mach_.registerFileNavigator()) {
        if (rf->width() != bits) continue;

        for (auto& bus : mach_.busNavigator()) {
            if (!MCC::busConnectedToRF(*bus, *rf)
                || bus->immediateWidth() == 0) {
                continue;
            }

            if (bus->immediateWidth() >= bits) {
                moveImm.first = -(1ll << (bits-1));
                moveImm.second = (1ll << bits)-1;
                break;
            } else {
                std::pair<int64_t, uint64_t> imm =
                    MathTools::bitsToIntegerRange<int64_t, uint64_t>(
                        bus->immediateWidth(),
                        bus->signExtends());

                moveImm.first = std::min(moveImm.first, imm.first);
                moveImm.second = std::max(moveImm.second, imm.second);
            }
        }
    }

    for (auto& iu : mach_.immediateUnitNavigator()) {
        for (auto& it : mach_.instructionTemplateNavigator()) {
            int supportedWidth = it->supportedWidth(*iu);
            if (supportedWidth >= bits) {
                moveImm.first = -(1ll << (bits-1));
                moveImm.second = (1ll << bits)-1;
                break;
            } else {
                std::pair<int64_t, uint64_t> imm =
                    MathTools::bitsToIntegerRange<int64_t, uint64_t>(
                        supportedWidth, iu->signExtends());

                moveImm.first = std::min(moveImm.first, imm.first);
                moveImm.second = std::max(moveImm.second, imm.second);
            }
        }
    }

    writeImmediateDef(o, "i32MoveImm", "i32", immediatePredicate(
                          moveImm.first, moveImm.second));

    writeImmediateDef(o, "i64MoveImm", "i64", immediatePredicate(
                          moveImm.first, moveImm.second));

}

void
TDGen::writeRegisterClasses(std::ostream& o) {

    o << "class R1<string n, list<Register> aliases> : TCEReg<n, aliases> {"
      << "}" << std::endl;

    o << "class R32<string n, list<Register> aliases> : TCEReg<n, aliases> {"
      << "}" << std::endl;

    o << "class R64<string n, list<Register> aliases> : TCEReg<n, aliases> {"
      << "}" << std::endl;


    o << "class R16<string n, list<Register> aliases> : TCEReg<n, aliases> {"
      << "}" << std::endl;
}


/**
 * Iterates through all registers in the machine and adds register information
 * to the register sets.
 */
void
TDGen::analyzeRegisters() {

    const TTAMachine::Machine::BusNavigator busNav =
        mach_.busNavigator();

    // Check which registers have guards and put their info in
    // guardedRegs_ set.
    for (int i = 0; i < busNav.count(); i++) {
        const TTAMachine::Bus& bus = *busNav.item(i);
        for (int g = 0; g < bus.guardCount(); g++) {
            const TTAMachine::RegisterGuard* rg =
                dynamic_cast<const TTAMachine::RegisterGuard*>(bus.guard(g));

            if (rg != NULL) {
                RegInfo reg = {
                    rg->registerFile()->name(),
                    (unsigned)rg->registerIndex() };

                guardedRegs_.insert(reg);
            }
        }
    }

    const TTAMachine::Machine::RegisterFileNavigator nav =
        mach_.registerFileNavigator();

    bool regsFound = true;
    int currentIdx = 0;
    while (regsFound) {
        regsFound = false;
        for (int i = 0; i < nav.count(); i++) {
            const TTAMachine::RegisterFile* rf = nav.item(i);

            // Skip definition for registers that are marked as 'reserved'
            if (rf->isReserved()) continue;

            // Skip the 0th index of a register file with zero register flag
            if (currentIdx == 0 && rf->zeroRegister()) {
                regsFound = true;
                continue;
            }

            // Check that the registerfile has both input and output ports.
            bool hasInput = false;
            bool hasOutput = false;
            for (int p = 0; p < rf->portCount(); p++) {
                if (rf->port(p)->isInput()) hasInput = true;
                if (rf->port(p)->isOutput()) hasOutput = true;
            }

            if (!hasInput) {
                if (Application::verboseLevel() >
                    Application::VERBOSE_LEVEL_DEFAULT) {

                    Application::logStream()
                        << "Warning: Skipping register file '"
                        << rf->name()
                        << "': no input ports."
                        << std::endl;
                }
                continue;
            }

            if (!hasOutput) {
                if (Application::verboseLevel() >
                    Application::VERBOSE_LEVEL_DEFAULT) {
                    
                    Application::logStream()
                        << "Warning Skipping register file '"
                        << rf->name()
                        << "': no output ports."
                        << std::endl;
                }
                continue;
            }

            unsigned width = rf->width();
            std::vector<RegInfo>* ri = NULL;
            if (width == 64) ri = &regs64bit_;
            else if (width == 32) ri = &regs32bit_;
            //else if (width == 16) ri = &regs16bit_;
            else if (width == 8) ri = &regs8bit_;
            else if (width == 1) ri = &regs1bit_;
            else {
                continue;
            }

            int lastIdx = rf->size();
            // todo: find a good solution to use just one big rf for this.
            
            bool isTempRegRf = AssocTools::containsKey(tempRegFiles_, rf);
            if (isTempRegRf) {
                // If the machine is not enough connected,
                // preserve last register
                // of all register files for routing values.
                lastIdx--;
            }

            // leave some 32bit regs unallocated for the post pass scheduler's
            // register renamer
            if (width == 32) {
                int renamerRegs = rf->size() * REG_RENAMER_PART / 100;
                lastIdx -= renamerRegs;
            }

            if (currentIdx < lastIdx) {
                RegInfo reg = {rf->name(), (unsigned)currentIdx};
                if (!(width == 32 &&
                    guardedRegs_.find(reg) != guardedRegs_.end())) {
                    ri->push_back(reg);
                }
                regsFound = true;
            }
        }
        currentIdx++;
    }
}


/**
 * Writes 1-bit register definitions to the output stream.
 */
void
TDGen::write1bitRegisterInfo(std::ostream& o) {
    if (regs1bit_.size() < 1) {
        RegInfo reg = {"dummy1", 0};
        std::string name = "I1DUMMY";
        writeRegisterDef(o, reg, name, "R1", "", RESERVED);
    } else {
        for (unsigned i = 0; i < regs1bit_.size(); i++) {
            std::string regName = "B" + Conversion::toString(i);
            writeRegisterDef(o, regs1bit_[i], regName, "R1", "", GPR);
        }
    }

    int stackSize = mach_.is64bit() ? 64 : 32;
    for (RegClassMap::iterator ri = regsInClasses_.begin(); 
         ri != regsInClasses_.end(); ri++) {
        // go through all 1-bit RF classes
        if (ri->first.find("R1") == 0) {
            o << std::endl
              << "def " << ri->first << "Regs : RegisterClass<\"TCE\", [i1]"
              << ", " << stackSize << ", (add ";
            o << ri->second[0];
            for (unsigned i = 1; i < ri->second.size(); i++) {
                o << " , " << ri->second[i];
            }
            o << ")> {" << std::endl
              << " let Size=" << stackSize << ";" << std::endl
              << "}" << std::endl;
        }
    }
}

/**
 * Gathers all machine op names and sorts them to vector and scalar
 * operations.
 */
void
TDGen::gatherAllMachineOperations() {
    // Get all operation names to one container.
    allOpNames_ = MachineInfo::getOpset(mach_);

    OperationPool opPool;
    OperationDAGSelector::OperationSet::const_iterator it;
    for (it = allOpNames_.begin(); it != allOpNames_.end(); ++it) {
        TCEString opName = *it;
        Operation* op = &opPool.operation(opName.c_str());

        if (op != &NullOperation::instance()) {
            if (op->isVectorOperation()) {
                vectorOps_.insert(std::make_pair(opName, op));
            } else {
                scalarOps_.insert(std::make_pair(opName, op));
            }
        }
    }
}

/*
 * Writes 32-bit register definitions to the output stream.
 */
void
TDGen::write32bitRegisterInfo(std::ostream& o) {

    std::string i32regs;
    size_t i = 0;

    if (!mach_.is64bit()) {
        // --- Hardcoded reserved registers. ---
        writeRegisterDef(o, regs32bit_[0], "SP", "R32", "", RESERVED);
        writeRegisterDef(o, regs32bit_[1], "IRES0", "R32", "", RESULT);
        writeRegisterDef(o, regs32bit_[2], "FP", "R32", "", RESERVED);

        if (prebypassStackIndeces_) {
            writeRegisterDef(
                o, regs32bit_[3], "KLUDGE_REGISTER", "R32", "", GPR);
        } else {
            writeRegisterDef(
                o, regs32bit_[3], "KLUDGE_REGISTER", "R32", "", RESERVED);
        }

        i32regs = "SP, IRES0, FP, KLUDGE_REGISTER";

        //after the loop, i will have the index of first non-special reg.
        for (i = 4;
             i < std::min((size_t)argRegCount_ + 3, regs32bit_.size());
             i++) {
            std::string regName = "A" + Conversion::toString(i);
            writeRegisterDef(o, regs32bit_[i], regName, "R32", "", ARGUMENT);
            i32regs += ", ";
            i32regs += regName;
        }
    }

    if (regs32bit_.size() < 1) {
        RegInfo reg = { "dummy32", 0 };
        writeRegisterDef(o, reg, "dummy32", "R64", "", RESERVED);
        o << "def R32Regs : RegisterClass<\"TCE\", [i32,f32,f16,i1], 32, (add dummy32)>;"
          << std::endl;
        o << "def R32IRegs : RegisterClass<\"TCE\", [i32], 32, (add dummy32)>;"
          << std::endl;
        o << "def R32FPRegs : RegisterClass<\"TCE\", [f32], 32, (add dummy32)>;"
          << std::endl;
        o << "def R32HFPRegs : RegisterClass<\"TCE\", [f16], 32, (add dummy32)>;"
          << std::endl;
    }

    // Generate data structure that contain registers grouped by their
    // physical RFs
    for (; i < regs32bit_.size(); i++) {
        std::string regName = "I" + Conversion::toString(i);
        writeRegisterDef(o, regs32bit_[i], regName, "R32", "", GPR);

        if (!regsInRFClasses_.count(regs32bit_[i].rf)) {
            regsInRFClasses_[regs32bit_[i].rf] = std::vector<std::string>();
        }
        i32regs += ", ";
        i32regs += regName;
        regsInRFClasses_[regs32bit_[i].rf].push_back(regName);
    }

    o << std::endl;

    if (!mach_.is64bit()) {
        // Bypass registers
        for (size_t j = 0; j < 256; ++j) {
            std::string regName = "BP" + Conversion::toString(j);

            o << "def " << regName << " : " 
              << "R32<\"ByPass_Regs" 
              << "\", []>, DwarfRegNum<"
              << "[" << dregNum_++ << "]>;"
              << std::endl;
        }
    
        // Bypass register class
        o << "def R32_ByPass_Regs : RegisterClass<\"TCE\", [i32,f32,f16], 32, (add ";
        for (size_t j = 0; j < 256; ++j) {
            std::string regName = "BP" + Conversion::toString(j);
            
            if (j != 0) {
                o << ", ";
            }
        
            o << regName;    
        }
        
        o << ")>;" << std::endl;
    }

    
    for (RegClassMap::iterator 
        it = regsInRFClasses_.begin(); it != regsInRFClasses_.end(); ++it) {
        o << "def R32_" 
          << it->first 
          << "_Regs : RegisterClass<\"TCE\", [i32,f32,f16], 32, (add ";
        
        for (std::vector<std::string>::iterator r = it->second.begin();
             r != it->second.end(); ++r) {
            if (r != it->second.begin()) {
                o << ", ";
            }

            o << *r;
        }

        o << ")>;" << std::endl;
    }

    o << std::endl;

    // Register classes for all 32-bit registers.
    // TODO: why are these needed? same as integer classes below?
    for (RegClassMap::iterator ri = regsInClasses_.begin(); 
         ri != regsInClasses_.end(); ri++) {
        // go through all 1-bit RF classes
        if (ri->first.find("R32") == 0) {

            o << "def " << ri->first << "Regs : RegisterClass<\"TCE\", [i32,f32,f16,i1], 32, (add ";
            o << ri->second[0];
            for (unsigned i = 1; i < ri->second.size(); i++) {
                o << " , " << ri->second[i];
            }
            o << ")>;" << std::endl;
        }
    }
    o << std::endl;
    
    // Integer register classes for 32-bit registers
    for (RegClassMap::iterator ri = regsInClasses_.begin(); 
         ri != regsInClasses_.end(); ri++) {
        // go through all 32-bit RF classes
        if (ri->first.find("R32") == 0) {
            o << "def " << ri->first << "IRegs : RegisterClass<\"TCE\", [i32], 32, (add ";
            o << ri->second[0];
            for (unsigned i = 1; i < ri->second.size(); i++) {
                o << " , " << ri->second[i];
            }
            o << ")>;" << std::endl;
        }
    }
    o << std::endl;

    // Floating point register classes for 32-bit registers
    for (RegClassMap::iterator ri = regsInClasses_.begin(); 
         ri != regsInClasses_.end(); ri++) {
        // go through all 32-bit RF classes
        if (ri->first.find("R32") == 0) {

            o << "def " << ri->first << "FPRegs : RegisterClass<\"TCE\", [f32], 32, (add ";
            o << ri->second[0];
            for (unsigned i = 1; i < ri->second.size(); i++) {
                o << " , " << ri->second[i];
            }
            o << ")>;" << std::endl;
        }
    }

    // Half-float register classes for 32-bit registers (TODO: 16-bit registers also?)
    for (RegClassMap::iterator ri = regsInClasses_.begin(); 
         ri != regsInClasses_.end(); ri++) {
        // go through all 32-bit RF classes
        if (ri->first.find("R32") == 0) {
            o << "def " << ri->first << "HFPRegs : RegisterClass<\"TCE\", [f16], 32, (add ";
            o << ri->second[0];
            for (unsigned i = 1; i < ri->second.size(); i++) {
                o << " , " << ri->second[i];
            }
            o << ")>;" << std::endl;
        }
    }

//        if (regs32bit_.size() <3) {
    if (mach_.is64bit()) { // for now. later try to use 32-b it regs for float.
        use64bitForFP_ = true;
    }

    if (!use64bitForFP_) {

        o  << "def FPRegs : RegisterClass<\"TCE\", [f32], 32, (add "
           << i32regs << ")>;" << std::endl;

        o  << "def HFPRegs : RegisterClass<\"TCE\", [f16], 32, (add "
           << i32regs << ")>;" << std::endl;
    }
    o << std::endl;
}

/**
 * Writes 16-bit register definitions to the output stream.
 */
void
TDGen::write16bitRegisterInfo(std::ostream& o) {
    // --- Hardcoded reserved registers. ---
    writeRegisterDef(o, regs16bit_[0], "HIRES0", "R16", "", RESULT);

    // -------------------------------------

    for (unsigned i = 1; i < regs16bit_.size(); i++) {
        std::string regName = "H" + Conversion::toString(i);
        writeRegisterDef(o, regs16bit_[i], regName, "R16", "", GPR);
    }

    o << std::endl;

    // All 16-bit regs.
    for (RegClassMap::iterator ri = regsInClasses_.begin();
         ri != regsInClasses_.end(); ri++) {
        // go through all 1-bit RF classes
        if (ri->first.find("R16") == 0) {
            o << "def " << ri->first
              << "Regs : RegisterClass<\"TCE\", [i16], 32, (add ";
            o << ri->second[0];
            for (unsigned i = 1; i < ri->second.size(); i++) {
                o << " , " << ri->second[i];
            }
            o << ")>;" << std::endl;
        }
    }
    o << std::endl;

    for (RegClassMap::iterator ri = regsInClasses_.begin();
         ri != regsInClasses_.end(); ri++) {
        // go through all 1-bit RF classes
        if (ri->first.find("R16") == 0) {
            o << "def " << ri->first
              << "IRegs : RegisterClass<\"TCE\", [i16], 32, (add ";
            o << ri->second[0];
            for (unsigned i = 1; i < ri->second.size(); i++) {
                o << " , " << ri->second[i];
            }
            o << ")>;" << std::endl;
        }
    }
    o << std::endl;

    // floating-point-versions of these

    for (RegClassMap::iterator ri = regsInClasses_.begin();
         ri != regsInClasses_.end(); ri++) {
        // go through all 1-bit RF classes
        if (ri->first.find("R16") == 0) {
            o << "def " << ri->first
              << "FPRegs : RegisterClass<\"TCE\", [f16], 32, (add ";
            o << ri->second[0];
            for (unsigned i = 1; i < ri->second.size(); i++) {
                o << " , " << ri->second[i];
            }
            o << ")>;" << std::endl;
        }
    }

    o << std::endl;
}

/**
 * Creates vector register classes required by the machine.
 *
 * The function goes through all operations and lists all different vector
 * operand value types, after which a register class is created for each
 * vector value type to support the vector value type.
 */
void
TDGen::analyzeMachineVectorRegisterClasses() {
    OperationPool opPool;

    // Has all vector value types (e.g. "v4i32", "v16i8", "v2f16", "v8f32")
    // that can be found among all the operands present in the machine.
    // Register classes will be defined based on these vector value types.
    std::set<TCEString> reqRegClassTypes;

    // List all existing vector value types in the machine.
    OperationDAGSelector::OperationSet::iterator opnIt;
    for (opnIt = allOpNames_.begin(); opnIt != allOpNames_.end(); ++opnIt) {
        Operation& op = opPool.operation((*opnIt).c_str());

        if (&op == &NullOperation::instance()) {
            continue;
        }

        // Go through every operand of the operation.
        for (int i = 1; i <= op.operandCount(); ++i) {
            Operand& operand = op.operand(i);

            // We are only interested in vector operands, skip otherwise.
            if (!operand.isVector()) {
                continue;
            }

            TCEString vtStr = ValueType::valueTypeStr(operand);
            if (vRegClasses_.find(vtStr) == vRegClasses_.end()) {
                reqRegClassTypes.insert(vtStr);
            }

            // If operand type is "raw" and vector subword width matches
            // float or half float width, list it also.
            if (operand.type() == Operand::RAW_DATA &&
                (operand.elementWidth() == FP_SUBW_WIDTH ||
                 operand.elementWidth() == HFP_SUBW_WIDTH)) {

                ValueType vt(operand);
                vt.isFloat_ = true;
                const TCEString floatVtStr = vt.valueTypeStr();

                if (vRegClasses_.find(floatVtStr) == vRegClasses_.end()) {
                    reqRegClassTypes.insert(floatVtStr);
                }
            }
        }
    }

    // Now we have listed all differend vector value types from the machine.
    // Create a register class for every LLVM supported vector value type.

    std::set<TCEString>::iterator it;
    for (it = reqRegClassTypes.begin(); it != reqRegClassTypes.end(); ++it) {
        const TCEString& vtStr = *it;
        const ValueType vt(vtStr);

        if (vt.isSupportedByLLVM()) {
            TCEString name = "V" + Conversion::toString(vt.subwCount_);

            if ((vt.isFloat_ && vt.subwWidth_ == FP_SUBW_WIDTH) ||
                (vt.isFloat_ && vt.subwWidth_ == HFP_SUBW_WIDTH)) {
                name += "F";
            } else {
                name += "I";
            }

            name += Conversion::toString(vt.subwWidth_) + "Regs";

            RegisterClass newRegClass(vt, name);
            vRegClasses_.insert(std::make_pair(vtStr, newRegClass));
        } else {
            verbose("Warning: RegisterClass not created for type: " + vtStr);
        }
    }
}


/**
 * Writes 64-bit register definitions to the output stream.
 */
void
TDGen::write64bitRegisterInfo(std::ostream& o) {

    // --- Hardcoded reserved registers. ---
    std::string i64regs;
    std::string f64regs;
    size_t i  = 0;

    if (mach_.is64bit()) {
        // --- Hardcoded reserved registers. ---
        writeRegisterDef(o, regs64bit_[0], "SP", "R64", "", RESERVED);
        writeRegisterDef(o, regs64bit_[1], "IRES0", "R64", "", RESULT);
        writeRegisterDef(o, regs64bit_[2], "FP", "R64", "", RESERVED);
        writeRegisterDef(
            o, regs64bit_[3], "KLUDGE_REGISTER", "R64", "", RESERVED);
        i64regs = "SP, IRES0, FP, KLUDGE_REGISTER";

        //after the loop, i will have the index of first non-special reg.
        for (i = 4;
             i < std::min(argRegCount_ + 3, (unsigned int)regs64bit_.size());
             i++) {
            std::string regName = "A" + Conversion::toString(i);
            writeRegisterDef(o, regs64bit_[i], regName, "R64", "", ARGUMENT);
            i64regs += ", ";
            i64regs += regName;
        }
    } else {

        if (regs64bit_.size() < 1) {
            RegInfo reg = { "DUMMY64", 0 };
            writeRegisterDef(o, reg, "DUMMY64", "R64", "", RESERVED);
            i64regs = "DUMMY64";
        }

    }

    // i already contains the correct starting index.
    for (; i < regs64bit_.size(); i++) {
        std::string intRegName = "L" + Conversion::toString(i);
        if (i != 0) 
            i64regs += ", ";
        i64regs += intRegName;
        writeRegisterDef(o, regs64bit_[i], intRegName, "R64", "", GPR);
    }

    if (mach_.is64bit()) {
        // Bypass registers
        for (size_t j = 0; j < 256; ++j) {
            std::string regName = "BP" + Conversion::toString(j);
            
            o << "def " << regName << " : " 
              << "R64<\"ByPass_Regs" 
              << "\", []>, DwarfRegNum<"
              << "[" << dregNum_++ << "]>;"
              << std::endl;
        }
    
        // Bypass register class
        o << "def R64_ByPass_Regs : RegisterClass<\"TCE\", [i64,i32,f64,f32,f16], 64, (add ";
        for (size_t j = 0; j < 256; ++j) {
            std::string regName = "BP" + Conversion::toString(j);
            
            if (j != 0) {
                o << ", ";
            }
        
            o << regName;    
        }
        
        o << ")>;" << std::endl;
    }

    TCEString dataTypes64 = use64bitForFP_ ?
        "[i64,f64,f32,f16]" :
        "[i64,f64]";

    o << std::endl
      << "def R64Regs : RegisterClass<\"TCE\", " << dataTypes64
      << ", 64, (add " << i64regs << ")> ;"
      << std::endl;

    o << std::endl
      << "def R64IRegs : RegisterClass<\"TCE\", [i64], 64, (add "
      << i64regs << ")> ;"
      << std::endl;

    o << std::endl
      << "def R64FPRegs : RegisterClass<\"TCE\", [f32], 64, (add "
      << i64regs << ")>;" << std::endl;

    o << std::endl
      << "def R64HFPRegs : RegisterClass<\"TCE\", [f16], 64, (add "
      << i64regs << ")>;" << std::endl;

    o << std::endl
      << "def R64DFPRegs : RegisterClass<\"TCE\", [f64], 64, (add "
      << i64regs << ")>;" << std::endl << std::endl;

    if (use64bitForFP_) {
        o  << "def FPRegs : RegisterClass<\"TCE\", [f32], 64, (add "
           << i64regs << ")>;" << std::endl << std::endl;

        o  << "def HFPRegs : RegisterClass<\"TCE\", [f16], 64, (add "
           << i64regs << ")>;" << std::endl << std::endl;
    }
}


/**
 * Groups all registers in the machine by register width.
 *
 * Goes through register files and sorts all real machine registers into
 * <Width, vector<Register>> groups. A base class for each Width is also
 * created.
 */
void
TDGen::analyzeMachineRegisters() {
    const TTAMachine::Machine::RegisterFileNavigator nav =
        mach_.registerFileNavigator();
    std::map<int, std::vector<RegisterInfo>>::iterator it;

    // Go through every register file in the machine.
    for (int i = 0; i < nav.count(); i++) {
        const TTAMachine::RegisterFile* rf = nav.item(i);
        assert(rf != NULL);

        // Check that the registerfile has both input and output ports.
        bool hasInput = false;
        bool hasOutput = false;
        for (int p = 0; p < rf->portCount(); p++) {
            if (rf->port(p)->isInput()) hasInput = true;
            if (rf->port(p)->isOutput()) hasOutput = true;
        }

        if (!hasInput) {
            if (Application::verboseLevel() >
                Application::VERBOSE_LEVEL_DEFAULT) {

                Application::logStream()
                    << "Warning: Skipping register file '"
                    << rf->name()
                    << "': no input ports."
                    << std::endl;
            }
            continue;
        }

        if (!hasOutput) {
            if (Application::verboseLevel() >
                Application::VERBOSE_LEVEL_DEFAULT) {
                Application::logStream()
                << "Warning Skipping register file '"
                << rf->name()
                << "': no output ports."
                << std::endl;
            }
            continue;
        }

        int rfWidth = rf->width();

        // If there is no entry for the width of the registers, create one.
        if (registers_.find(rfWidth) == registers_.end()) {
            registers_.insert(
                std::make_pair(rfWidth, std::vector<RegisterInfo>()));
        }

        it = registers_.find(rfWidth);
        std::vector<RegisterInfo>& registers = it->second;

        // Skip scalar size registers, they have been listed in TDGen.
        if (rfWidth > MAX_SCALAR_WIDTH) {
            // Kludge fix: Machines with >32 bit regs but without any vector
            // operations generates register info td file with 64 bit register
            // definition which are not associated to any register class.
            // This triggers assertion in LLVM side.
            if (vRegClasses_.empty()) continue;

            int lastIdx = rf->size();

            bool isTempRegRf = AssocTools::containsKey(tempRegFiles_, rf);
            if (isTempRegRf) {
                // If the machine is not enough connected,
                // preserve last register
                // of all register files for routing values.
                lastIdx--;
            }

            // Associate register file's registers with the width.
            for (int index = 0; index < lastIdx; ++index) {
                TCEString regName(
                    rf->name() + "_" + Conversion::toString(index));
                RegisterInfo reg(regName, rf->name(), index, rfWidth);
                registers.push_back(reg);
            }
        }
    }

    orderEqualWidthRegistersToRoundRobin();

    // TDGen has hardcoded register name definitions, some
    // inspection needs to be done to create correct register names.

    std::vector<RegisterInfo> regs1bit;
    for (size_t i = 0; i < regs1bit_.size(); ++i) {
        RegisterInfo reg(
            "B" + Conversion::toString(i),
            regs1bit_[i].rf,
            regs1bit_[i].idx,
            1);
        regs1bit.push_back(reg);
    }
    registers_[1] = regs1bit;

    /// @todo Apparently TDGen does not write information of  8 bit
    /// registers -> can't add them.

    std::vector<RegisterInfo> regs16bit;
    for (size_t i = 0; i < regs16bit_.size(); ++i) {
        RegisterInfo reg(
            "H" + Conversion::toString(i),
            regs16bit_[i].rf,
            regs16bit_[i].idx,
            16);

        if (i == 0) {
            reg.regName_ = "HIRES0";
        }

        regs16bit.push_back(reg);
    }
    registers_[16] = regs16bit;

    std::vector<RegisterInfo> regs32bit;
    for (size_t i = 0; i < regs32bit_.size(); ++i) {
        RegisterInfo reg(
            "I" + Conversion::toString(i),
            regs32bit_[i].rf,
            regs32bit_[i].idx,
            32);


        if (!mach_.is64bit()) {
            if (i == 0) {
                reg.regName_ = "SP";
            } else if (i == 1) {
                reg.regName_ = "IRES0";
            } else if (i == 2) {
                reg.regName_ = "FP";
            } else if (i == 3) {
                reg.regName_ = "KLUDGE_REGISTER";
            } else if (i < argRegCount_ + 3) {
                reg.regName_ = "A" + Conversion::toString(i);
            }
        }
        regs32bit.push_back(reg);
    }
    registers_[32] = regs32bit;

    std::vector<RegisterInfo> regs64bit;
    for (size_t i = 0; i < regs64bit_.size(); ++i) {
        RegisterInfo reg(
            "L" + Conversion::toString(i),
            regs64bit_[i].rf,
            regs64bit_[i].idx,
            64);

        if (mach_.is64bit()) {
            if (i == 0) {
                reg.regName_ = "SP";
            } else if (i == 1) {
                reg.regName_ = "IRES0";
            } else if (i == 2) {
                reg.regName_ = "FP";
            } else if (i == 3) {
                reg.regName_ = "KLUDGE_REGISTER";
            } else if (i < argRegCount_ + 3) {
                reg.regName_ = "A" + Conversion::toString(i);
            }
        }
        regs64bit.push_back(reg);
    }
    registers_[64] = regs64bit;

    // Create base classes for different register widths.
    for (it = registers_.begin(); it != registers_.end(); ++it) {
        int width = it->first;
        if (width > MAX_SCALAR_WIDTH) {
            baseClasses_.insert(
                std::make_pair(width, "VR" + Conversion::toString(width)));
        } else {
            baseClasses_.insert(
                std::make_pair(width, "R" + Conversion::toString(width)));
        }
    }
}

/**
 * Sets registers to use round robin order in register allocation.
 *
 * Starting from register index number 0, all same width registers having
 * the same index number are pushed to a vector. Then, index is increased
 * by one and same action is performed, index increased etc., until all
 * registers of same width are pushed to the vector. Then, the vector is
 * used to replace old register order.
 *
 * For example, two register files R64_1 (register indices 0, 1, 2 and 3)
 * and R64_2 (register indices 0, 1) are ordered as:
 * R64_1(0), R64_2(0), R64_1(1), R64_2(1), R64_1(2), R64_1(3), and LLVM
 * register allocator will use this order to allocate the registers.
 */
void
TDGen::orderEqualWidthRegistersToRoundRobin() {
    std::vector<RegisterInfo> roundRobinOrder;

    // Loop through every different group of register widths.
    std::map<int, std::vector<RegisterInfo>>::iterator it;
    for (it = registers_.begin(); it != registers_.end(); ++it) {
        roundRobinOrder.clear();
        std::vector<RegisterInfo>& registers = it->second;

        unsigned i = 0;
        unsigned idx = 0;

        // Loop until all registers of same width are reordered.
        while (roundRobinOrder.size() != registers.size()) {
            RegisterInfo& info = registers[i];
            if (info.regIndex_ == idx) {
                roundRobinOrder.push_back(info);
            }

            if (i == registers.size() - 1) {
                i = 0;
                ++idx;
            } else {
                ++i;
            }
        }

        registers = roundRobinOrder;
    }
}

/**
 * Attaches proper register groups to register files.
 */
void
TDGen::associateRegistersWithVectorRegisterClasses() {
    std::map<TCEString, RegisterClass>::iterator rcIt;
    for (rcIt = vRegClasses_.begin(); rcIt != vRegClasses_.end(); ++rcIt) {
        RegisterClass& regClass = rcIt->second;

        // Go through all register groups (they are in ascending order by
        // width -> starts from the smallest width always towards wider ones).
        std::map<int, std::vector<RegisterInfo>>::iterator regsIt =
            registers_.begin();

        bool found = false;
        while (!found && regsIt != registers_.end()) {
            int regsWidth = regsIt->first;
            size_t amountOfRegs = (regsIt->second).size();

            // If can find a group of registers which satisfies the
            // register class width requirement, choose those registers.
            if (regsWidth >= regClass.valueType().width() &&
                amountOfRegs > 0) {
                const std::vector<RegisterInfo>& regs = regsIt->second;
                regClass.addRegisters(regs);
                found = true;
            }

            ++regsIt;
        }

        // If bigger register exploitation is desired, append all the rest
        // of the registers, because there are only wider registers left.
        if (EXPLOIT_BIGGER_REGISTERS) {
            while (regsIt != registers_.end()) {
                const std::vector<RegisterInfo>& biggerRegs = regsIt->second;
                regClass.addRegisters(biggerRegs);
                ++regsIt;
            }
        }
    }
}

/**
 * Checks if operation has operands of raw type.
 *
 * @param op Operation to be checked.
 * @return True, if operation has at least one operand of raw type.
 */
bool
TDGen::hasRawOperands(const Operation& op) const {
    for (int i = 1; i <= op.operandCount(); ++i) {
        const Operand& operand = op.operand(i);
        if (operand.type() == Operand::RAW_DATA && operand.isVector()) {
            return true;
        }
    }
    return false;
}

/**
 * Returns subword width of vector operands of "raw" type.
 *
 * @note This function chooses the subword width based on subword width
 * of the WIDEST "raw" data operand that can be found from the operation,
 * since that approach works for current SIMD operation definitions.
 * If this approach doesn't work for your operation, modify the function
 * properly to fulfil your needs.
 *
 * @param op Operation that has raw operands.
 * @return Subword width of the raw vector operands.
 */
int
TDGen::subwordWidthOfRawData(const Operation& op) const {
    int widestWidth = -1;
    int widestRawOperandIndex = -1;
    for (int i = 1; i <= op.operandCount(); ++i) {
        const Operand& operand = op.operand(i);
        if (operand.type() == Operand::RAW_DATA && operand.isVector()) {
            if (operand.width() > widestWidth) {
                widestWidth = operand.width();
                widestRawOperandIndex = i;
            }
        }
    }

    if (widestRawOperandIndex >= 0) {
        return op.operand(widestRawOperandIndex).elementWidth();
    }

    assert(false && "Invalid input operation.");
}

/**
 * Checks if all operands of the operation have a supporting register class.
 *
 * @param op Operation whose operands will be checked.
 * @return True, if all operands are supported by register classes.
 */
bool
TDGen::hasRegisterClassSupport(const Operation& op) const {
    for (int i = 1; i <= op.operandCount(); ++i) {
        const Operand& operand = op.operand(i);

        if (operand.isVector()) {
            TCEString vtStr = ValueType::valueTypeStr(operand);
            if (vRegClasses_.find(vtStr) == vRegClasses_.end()) {
                return false;
            }
        }
    }

    return true;
}

/**
 * Checks if the ValueType has a supporting register class.
 *
 * @param vt ValueType to be checked.
 * @return True, if the value type is supported by a register class.
 */
bool
TDGen::hasRegisterClassSupport(const ValueType& vt) const {
    if (vRegClasses_.find(vt.valueTypeStr()) == vRegClasses_.end()) {
        return false;
    }

    return true;
}

/**
 * Returns the name of the vector class that supports given operand.
 *
 * @param operand Checked if any register class supports value type of this.
 * @return Name of the register class that supports operand's value type.
 */
TCEString
TDGen::associatedVectorRegisterClass(const Operand& operand) const {
    std::map<TCEString, RegisterClass>::const_iterator it =
        vRegClasses_.find(ValueType::valueTypeStr(operand));

    if (it == vRegClasses_.end()) {
        // Shouldn't get here because any operation whose operands are not
        // supported will not get created.
        assert(false);
    }

    const RegisterClass& regClass = it->second;
    return regClass.name();
}

/**
 * Prints debugging information if verbose switch is used.
 *
 * @param msg Debug message.
 */
void
TDGen::verbose(const TCEString& msg) const {
    if (Application::verboseLevel() > Application::VERBOSE_LEVEL_DEFAULT) {
        Application::logStream() << msg << endl;
    }
}

/**
 * Writes a pattern fragment definition.
 *
 * @param origPat Pattern that instruction selector should replace.
 * @param replacerPat Pattern that will replace the original pattern.
 */
void
TDGen::writePatternReplacement(
        std::ostream& o,
        const TCEString& origPat,
        const TCEString& replacerPat) const {
    o << "def : Pat<(" << origPat << "), (" << replacerPat << ")>;" << endl;
}

/**
 * Writes operation definition using given operand types.
 *
 * Changes the operands to match the provided ones, after which a definition
 * is written. Finally, original operands are restored.
 *
 * @param o Output stream.
 * @param op Operation used to create new operation definitions.
 * @param skipPattern If true, operation pattern writing is skipped.
 * @param inputs Input operand types for the new operation.
 * @param outputs Output operand types for the new operation.
 * @param instrSuffix Optional suffix that will be added to instruction name.
 */
void
TDGen::writeOperationDefUsingGivenOperandTypes(
    std::ostream& o,
    Operation& op,
    bool skipPattern,
    std::vector<ValueType> inputs,
    std::vector<ValueType> outputs,
    TCEString instrSuffix) {
    assert(
        inputs.size() == static_cast<size_t>(op.numberOfInputs()) &&
        "Given input operand count doesn't match operation's count.");
    assert(
        outputs.size() == static_cast<size_t>(op.numberOfOutputs()) &&
        "Given output operand count doesn't match operation's count.");

    std::vector<ValueType> oldInputs;
    std::vector<ValueType> oldOutputs;

    // Change operands to match the provided ones.
    for (int i = 0; i < op.numberOfInputs(); ++i) {
        Operand& input = op.input(i);
        oldInputs.push_back(ValueType(input));

        ValueType vt = inputs[i];
        input.setType(vt.operandType());
        input.setElementWidth(vt.subwWidth_);
        input.setElementCount(vt.subwCount_);
    }

    for (int i = 0; i < op.numberOfOutputs(); ++i) {
        Operand& output = op.output(i);
        oldOutputs.push_back(ValueType(output));

        ValueType vt = outputs[i];
        output.setType(vt.operandType());
        output.setElementWidth(vt.subwWidth_);
        output.setElementCount(vt.subwCount_);
    }

    // Write new operation definition.
    TCEString types = createDefaultOperandTypeString(op) + instrSuffix;
    writeVectorOperationDef(o, op, types, "", skipPattern);

    // Restore old operands.
    for (int i = 0; i < op.numberOfInputs(); ++i) {
        Operand& input = op.input(i);

        ValueType vt = oldInputs[i];
        input.setType(vt.operandType());
        input.setElementWidth(vt.subwWidth_);
        input.setElementCount(vt.subwCount_);
    }

    for (int i = 0; i < op.numberOfOutputs(); ++i) {
        Operand& output = op.output(i);

        ValueType vt = oldOutputs[i];
        output.setType(vt.operandType());
        output.setElementWidth(vt.subwWidth_);
        output.setElementCount(vt.subwCount_);
    }
}

/**
 * Checks if the given operation is a vector load memory operation.
 *
 * @param op Operation which is checked.
 * @return True, if operation is a vector load memory operation.
 */
bool
TDGen::isVectorLoadOperation(const Operation&) const {
    abortWithError("WiP");
    return false;
}

/**
 * Checks if the given operation is a vector store memory operation.
 *
 * @param op Operation which is checked.
 * @return True, if operation is a vector store memory operation.
 */
bool
TDGen::isVectorStoreOperation(const Operation&) const {
    abortWithError("WiP");
    return false;
}

bool
TDGen::isWrongEndianessVectorOp(const Operation&) const {
    abortWithError("WiP");
    return false;
}

/**
 * Checks if the given operation is a bitwise operation.
 *
 * @param op Operation which is checked.
 * @return True, if operation is a bitwise operation.
 */
bool
TDGen::isVectorBitwiseOperation(const Operation&) const {
    abortWithError("WiP");
    return false;
}


/**
 * Writes register class for registers usable as guard.
 *
 * Call this after all regular register definitions are written (after all
 * calls to writeRegisterDef());
 */
void
TDGen::writeGuardRegisterClassInfo(std::ostream& o) {

    int bits = mach_.is64bit() ? 64 : 32;
    o << std::endl
      << "// #################################" << std::endl
      << "// Register class definitions for guards" << std::endl;

    if (llvmGuardRegs_.empty()) {
        o << "// No guard registers in this machine" << std::endl;
        RegInfo reg = {"noGuardReg", 0};
        std::string name = "NOGUARD";
        writeRegisterDef(o, reg, name, "R1", "", RESERVED);
        o << "def " << guardRegTemplateName
          << "Regs : RegisterClass<\"TCE\", [i1], " << bits << ", (add "
          << name << ")>;" << std::endl;
        return;
    }

    o << "def " << guardRegTemplateName
      << "Regs : RegisterClass<\"TCE\", [i1], " << bits << ", (add ";
    bool firstItem = true;
    for (auto& regDef : llvmGuardRegs_) {
        if (firstItem) {
            o << regDef;
            firstItem = false;
            continue;
        }
        o << ", " << regDef;
    }
    o << ")> {" << std::endl;
    o << "    let Size=" << bits << ";" << std::endl;
    o << "}" << std::endl;
    o << "// #################################" << std::endl << std::endl;
}

/**
 * Writes base class definitions to the .td file.
 */
void
TDGen::writeVectorRegisterBaseClasses(std::ostream& o) const {
    o << endl
      << "// #################################" << endl
      << "// Vector register base class definitions" << endl;

    std::map<int, TCEString>::const_iterator bcIt;
    for (bcIt = baseClasses_.begin(); bcIt != baseClasses_.end(); ++bcIt) {
        int width = bcIt->first;

        if (width > MAX_SCALAR_WIDTH) {
            TCEString baseClassName = bcIt->second;
            o << "class " << baseClassName
              << "<string n, list<Register> aliases> : "
              << "TCEVectorReg<n, aliases> {}" << endl;
        }
    }
    o << "// #################################" << endl;
}

/**
 * Writes register definitions to the .td file.
 */
void
TDGen::writeVectorRegisterNames(std::ostream& o) {
    o << endl
      << "// #################################" << endl
      << "// Register name definitions for vector registers" << endl;

    std::set<TCEString> declaredRegNames;

    std::map<int, std::vector<RegisterInfo>>::const_iterator it;
    for (it = registers_.begin(); it != registers_.end(); ++it) {
        int regsWidth = it->first;

        const std::vector<RegisterInfo>& regs = it->second;
        TCEString baseClass = baseClasses_.find(regsWidth)->second;

        for (size_t i = 0; i < regs.size(); ++i) {
            // Don't declare <=32b registers again, it has been done in TDGen.
            if (regsWidth > MAX_SCALAR_WIDTH) {
                RegisterInfo reg = regs[i];
                RegInfo oldRegInfo = {reg.regFileName_, reg.regIndex_};
                writeRegisterDef(
                    o, oldRegInfo, reg.regName_, baseClass, "", RESERVED);
                declaredRegNames.insert(reg.regName_);
            }
        }
    }
    o << "// #################################" << endl;
}

/**
 * Writes register class definitions to the .td file.
 */
void
TDGen::writeVectorRegisterClasses(std::ostream& o) const {
    o << endl
      << "// #################################" << endl
      << "// Register class definitions for vector registers" << endl;

    std::map<TCEString, RegisterClass>::const_iterator it;
    for (it = vRegClasses_.begin(); it != vRegClasses_.end(); ++it) {
        const TCEString& vtStr = it->first;
        const RegisterClass& regClass = it->second;

        if (regClass.numberOfRegisters() == 0) {
            std::string msg =
                std::string("The machine is missing registers for ") +
                regClass.valueType().valueTypeStr() + " operands" +
                " required by some operation(s).";

            std::cerr << "Error: Illegal machine: " << msg << std::endl;
            THROW_EXCEPTION(IllegalMachine, msg);
        }

        o << "def " << regClass.name() << " : "
          << "RegisterClass<\"TCE\", "
          << "[" << vtStr << "], " << regClass.alignment() << ", (add ";

        // Add registers to the register class.
        for (size_t i = 0; i < regClass.numberOfRegisters(); ++i) {
            const RegisterInfo& reg = regClass.registerInfo(i);
            o << reg.regName_;

            if (i != regClass.numberOfRegisters() - 1) {
                o << ", ";
            }
        }
        o << ")> ";

        // Register classes below 8 bit of total width need to be set as
        // "8 bit of size" so that when they are spilled to stack, the
        // byte size of the register class is at least 1.
        if (regClass.valueType().width() < 8) {
            o << "{" << endl << "    let Size=8;" << endl << "}" << endl;
        } else {
            o << ";" << endl;
        }
    }

    o << "// #################################" << endl;
}

/**
 * Writes needed defitinion(s) of the vector operation to the .td file.
 *
 * Creates 4 differend instructions of the operation at most:
 * - Integer version (register)
 * - Integer version (immediate)
 * - Float/half float version (register)
 * - Float/half float version (immediate)
 *
 * @param o Output stream to the .td file.
 * @param op Vector operation of which the definitions will be created.
 * @param skipPattern True, if skip pattern creation.
 */
void
TDGen::writeVectorOperationDefs(std::ostream&, Operation&, bool) {
    abortWithError("WiP");
}

/**
 * Writes instruction of the vector operation to the .td file.
 *
 * Creates register version and possibly an immediate version of the
 * vector operation in case there are any scalar operands.
 *
 * @param o Output stream to the file.
 * @param op Operation to be written as an instruction to the .td file.
 * @param valueTypes Operand value type identifier characters.
 * @param attributes Operation attributes.
 * @param skipPattern True, if skip pattern creation.
 */
void
TDGen::writeVectorOperationDef(
    std::ostream& o, Operation& op, TCEString valueTypes,
    const TCEString& attributes, bool skipPattern) {
    // Make sure operation's operands are fully supported by
    // vector register classes.
    if (!hasRegisterClassSupport(op)) {
        verbose(
            "Warning: " + op.name() + valueTypes +
            " was not created due to unsupported operands.");
        return;
    }

    // Write register version of the operation.
    TDGen::writeOperationDef(o, op, valueTypes, attributes, skipPattern);

    // Information of some operations is needed later, save it if needed.
    saveAdditionalVectorOperationInfo(op, valueTypes, true);

    // Create the same instruction with immediate inputs if there are
    // any input operands that are scalars.
    bool scalarsFound = false;
    for (int i = op.numberOfOutputs(); i < op.operandCount(); ++i) {
        char& c = valueTypes[i];
        switch (c) {
            case OT_REG_BOOL:
                c = OT_IMM_BOOL;
                scalarsFound = true;
                break;
            case OT_REG_INT:
                c = OT_IMM_INT;
                scalarsFound = true;
                break;
            case OT_REG_FP:
                c = OT_IMM_FP;
                scalarsFound = true;
                break;
            case OT_REG_HFP:
                c = OT_IMM_HFP;
                scalarsFound = true;
                break;
            default:
                continue;
        }
    }

    if (scalarsFound) {
        // Write immediate version of the operation.
        TDGen::writeOperationDef(o, op, valueTypes, attributes, skipPattern);

        // Information of some immediate versions are needed also.
        saveAdditionalVectorOperationInfo(op, valueTypes, false);
    }
}

/**
 * Saves information of some operations.
 *
 * Additional information of some operations is needed later, and thus,
 * they need to be saved for later reference.
 *
 * @param op Vector operation.
 * @param valueTypes Operand value type identifier characters.
 * @param isRegisterOp If false, operation has immediate operands.
 */
void
TDGen::saveAdditionalVectorOperationInfo(
    const Operation&, const TCEString&, bool) {
    abortWithError("WiP");
}

/**
 * Writes instruction definitions for memory vector operations.
 *
 * Subword aligned LDQ/LDH/LDW/STQ/STH/STQ vector operations can be passed
 * directly to writing vector operation definitions, but vector aligned
 * LOAD and STORE operations can be used for several different vector types
 * of the same width, thus, all the different vector types need to be
 * covered and separate memory instruction have to be made for them.
 */
void
TDGen::writeVectorMemoryOperationDefs(
    std::ostream&, Operation&, bool) {
    abortWithError("WiP");
}

/**
 * Writes MOV instructions between vector register classes.
 */
void
TDGen::writeVectorRegisterMoveDefs(std::ostream& o) {
    o << endl
      << "// Vector register->register move definitions." << endl
      << "let isAsCheapAsAMove = 1 in {" << endl;

    std::map<TCEString, RegisterClass>::const_iterator it;
    for (it = vRegClasses_.begin(); it != vRegClasses_.end(); ++it) {
        const RegisterClass& regClass = it->second;

        const TCEString opcode = "MOV" + regClass.name();
        const TCEString vtStr = regClass.valueType().valueTypeStr();

        const TCEString outs = "(outs " + regClass.name() + ":$dst), ";
        const TCEString ins = "(ins " + regClass.name() + ":$src), ";
        const TCEString insPred =
            "(ins GuardRegs:$pred, " + regClass.name() + ":$src), ";
        const TCEString asmOp = "\"$src -> $dst;\", []";

        o << "def " << opcode << " : "
          << "InstTCE<" << outs << ins << asmOp << ">;" << endl;

        opNames_[opcode] = opcode;
        movOperations_.insert(opcode);

        if (hasConditionalMoves_) {
            o << "def PRED_TRUE_" << opcode << " : "
              << "InstTCE<" << outs << insPred << asmOp << ">;" << endl;

            o << "def PRED_FALSE_" << opcode << " : "
              << "InstTCE<" << outs << insPred << asmOp << ">;" << endl;

            opNames_["PRED_TRUE_" + opcode] = "?" + opcode;
            opNames_["PRED_FALSE_" + opcode] = "!" + opcode;
            truePredOps_[opcode] = "PRED_TRUE_" + opcode;
            falsePredOps_[opcode] = "PRED_TRUE_" + opcode;

            movOperations_.insert("PRED_TRUE_" + opcode);
            movOperations_.insert("PRED_FALSE_" + opcode);
        }
    }

    o << "}" << endl;
}

/**
 * Writes truncstore patterns for vector value types.
 */
void
TDGen::writeVectorTruncStoreDefs(std::ostream& o) const {
    o << endl << "// Vector truncstore definitions, needed below." << endl;

    // Create truncstore definitions for vector value types, there has to
    // exist store operations for the same value type that the TRUNC
    // operation has. For example, in order to create truncstore using
    // TRUNCWH16X4, there has to be store operations in the machine that
    // supports storing v4i16 value type.

    std::map<TCEString, RegisterClass>::const_iterator rcIt;
    for (rcIt = vRegClasses_.begin(); rcIt != vRegClasses_.end(); ++rcIt) {
        o << "def truncstore" << rcIt->first << " : "
          << "PatFrag<(ops node:$val, node:$ptr),"
          << "(truncstore node:$val, node:$ptr), "
          << "[{return cast<StoreSDNode>(N)->getMemoryVT() == "
          << "MVT::" << rcIt->first << ";}]>;" << endl;
    }
    // Create patterns which will replace "truncstore" pattern with separate
    // TRUNC and then STORE operations.

    o << endl << "// Pattern definitions for truncstores." << endl;
    std::vector<std::pair<const Operation*, TCEString>>::const_iterator it;
    for (it = truncOperations_.begin(); it != truncOperations_.end(); ++it) {
        const TCEString& truncInstr = it->second;
        const TCEString& vtFromStr =
            ValueType::valueTypeStr(it->first->input(0));
        const TCEString targetVtStr =
            ValueType::valueTypeStr(it->first->output(0));

        std::map<TCEString, InstructionInfo>::const_iterator regStoreIt =
            registerStores_.find(targetVtStr);

        // Use register version of store operations.
        if (regStoreIt != registerStores_.end()) {
            const TCEString& regStoreInstr = (regStoreIt->second).instrName_;
            const TCEString originalPattern = "truncstore" + targetVtStr +
                                              " " + vtFromStr +
                                              ":$op2, ADDRrr:$op1";
            const RegisterClass& regClass =
                vRegClasses_.find(vtFromStr)->second;
            const TCEString replacerPattern =
                regStoreInstr + " ADDRrr:$op1, (" + truncInstr + " " +
                regClass.name() + ":$op2)";

            writePatternReplacement(o, originalPattern, replacerPattern);
        }

        std::map<TCEString, InstructionInfo>::const_iterator immStoreIt =
            immediateStores_.find(targetVtStr);

        // Use immediate version of store operations.
        if (immStoreIt != immediateStores_.end()) {
            const TCEString& immStoreInstr = (immStoreIt->second).instrName_;
            const TCEString originalPattern = "truncstore" + targetVtStr +
                                              " " + vtFromStr +
                                              ":$op2, ADDRri:$op1";
            const RegisterClass& regClass =
                vRegClasses_.find(vtFromStr)->second;
            const TCEString replacerPattern =
                immStoreInstr + " ADDRri:$op1, (" + truncInstr + " " +
                regClass.name() + ":$op2)";
            writePatternReplacement(o, originalPattern, replacerPattern);
        }
    }
}

/**
 * Writes scalar_to_vector patterns for vector value types.
 */
void
TDGen::writeScalarToVectorDefs(std::ostream& o) const {
    o << endl << "// Scalar to vector definitions." << endl;
    std::map<TCEString, TCEString>::const_iterator it;
    for (it = vbcastOperations_.begin(); it != vbcastOperations_.end();
         ++it) {
        const TCEString& vtStr = it->first;
        const TCEString& vbcastInstr = it->second;

        ValueType scalarVt(vtStr);
        scalarVt.subwCount_ = 1;  // Change vector type to scalar.
        TCEString scalarVtStr = scalarVt.valueTypeStr();

        /// @todo Add i8 and i16 subword support once those types have
        /// RegisterClass support. For now, just skip generation for them.
        if (scalarVtStr == "f16" || scalarVtStr == "f32" ||
            scalarVtStr == "i32") {
            TCEString originalPattern =
                vtStr + " (scalar_to_vector " + scalarVtStr + ":$in)";
            TCEString replacerPattern =
                vbcastInstr + " " + scalarVtStr + ":$in";

            writePatternReplacement(o, originalPattern, replacerPattern);
        }
    }
}

/**
 * Writes bit conversion patterns.
 */
void
TDGen::writeVectorBitConversions(std::ostream& o) const {
    o << endl << "// Bit conversions between vector types." << endl;

    std::map<TCEString, RegisterClass>::const_iterator it1;
    for (it1 = vRegClasses_.begin(); it1 != vRegClasses_.end(); ++it1) {
        const TCEString& vtStr1 = it1->first;
        const RegisterClass& regClass1 = it1->second;

        std::map<TCEString, RegisterClass>::const_iterator it2;
        for (it2 = vRegClasses_.begin(); it2 != vRegClasses_.end(); ++it2) {
            const TCEString& vtStr2 = it2->first;
            const RegisterClass& regClass2 = it2->second;
            if (regClass1.valueType().width() !=
                    regClass2.valueType().width() ||
                vtStr1 == vtStr2) {
                continue;
            }
            const TCEString originalPattern = vtStr2 + " (bitconvert (" +
                                              vtStr1 + " " +
                                              regClass1.name() + ":$src))";
            const TCEString replacerPattern =
                vtStr2 + " " + regClass2.name() + ":$src";
            writePatternReplacement(o, originalPattern, replacerPattern);
        }
    }

    o << endl << "// Bit conversions to/from i32." << endl;

    // Write i32 -> same size vector type bit conversions.
    std::map<TCEString, RegisterClass>::const_iterator it;
    for (it = vRegClasses_.begin(); it != vRegClasses_.end(); ++it) {
        const TCEString& vtStr = it->first;
        const RegisterClass& regClass = it->second;

        if (regClass.valueType().width() == 32 && vtStr != "i32") {
            TCEString originalPattern = "i32 (bitconvert (" + vtStr + " " +
                                        regClass.name() + ":$src))";
            TCEString replacerPattern = "i32 R32IRegs:$src";
            writePatternReplacement(o, originalPattern, replacerPattern);

            originalPattern = vtStr + " (bitconvert (i32 R32IRegs:$src))";
            replacerPattern = vtStr + " " + regClass.name() + ":$src";
            writePatternReplacement(o, originalPattern, replacerPattern);
        }
    }
}

/**
 * This function writes scalar operation exploitations.
 *
 * Some scalar operations can be exploited to execute vector operations.
 * For instance, bitwise logical operations are same for any operand width.
 * A 32-bit scalar XOR operation can execute bitwise exclusive OR operation
 * for v32i1, v4i8 and v2i16 vector operands similarly as it does for i32
 * type.
 */
void
TDGen::writeScalarOperationExploitations(std::ostream& o) {
    o << endl
      << endl
      << "// Scalar operations exploited to execute small vector operations."
      << endl;

    // Use minimal architecture NOT/AND/IOR/XOR 32-bit operations to execute
    // bitwise vector operations with operands of 32 bits or lower width.
    OperationPool opPool;

    std::vector<Operation*> scalarBitwiseOps;
    scalarBitwiseOps.push_back(&opPool.operation("NOT"));
    scalarBitwiseOps.push_back(&opPool.operation("AND"));
    scalarBitwiseOps.push_back(&opPool.operation("IOR"));
    scalarBitwiseOps.push_back(&opPool.operation("XOR"));
    scalarBitwiseOps.push_back(&opPool.operation("NOT64"));
    scalarBitwiseOps.push_back(&opPool.operation("AND64"));
    scalarBitwiseOps.push_back(&opPool.operation("IOR64"));
    scalarBitwiseOps.push_back(&opPool.operation("XOR64"));


    for (size_t opI = 0; opI < scalarBitwiseOps.size(); ++opI) {
        Operation& op = *scalarBitwiseOps[opI];
        if (&op == &NullOperation::instance() ||
            !mach_.hasOperation(op.name())) {
            continue;
        }

        int width = 2;

        while (width <= op.output(0).width()) {
            // Get vector value types for the current width, e.g v4i8, v2i16
            // and v32i1 value types for width 32.
            std::vector<ValueType> intVecVts =
                ValueType::vectorTypesOfWidth(width, true);

            for (size_t i = 0; i < intVecVts.size(); ++i) {
                ValueType& vecVt = intVecVts[i];
                const TCEString vecVtStr = vecVt.valueTypeStr();

                if (hasRegisterClassSupport(vecVt)) {
                    std::vector<ValueType> inputs;
                    std::vector<ValueType> outputs;

                    for (int i = 0; i < op.numberOfInputs(); ++i) {
                        inputs.push_back(vecVt);
                    }
                    for (int i = 0; i < op.numberOfOutputs(); ++i) {
                        outputs.push_back(vecVt);
                    }

                    writeOperationDefUsingGivenOperandTypes(
                        o, op, false, inputs, outputs, vecVtStr);
                }
            }

            width *= 2;
        }
    }

    // Use LDQ/LDH/LDW and STQ/STH/STW operations to create memory access
    // operations for vector operands of 32-bit or lower width.
    int memDataOpndWidth = 2;

    const char* defaultType = mach_.is64bit() ? "i64" : "i32";

    while (memDataOpndWidth <= MAX_SCALAR_WIDTH) {
        std::vector<ValueType> vecVts =
            ValueType::vectorTypesOfWidth(memDataOpndWidth);

        for (size_t i = 0; i < vecVts.size(); ++i) {
            const ValueType& dataOpndVt = vecVts[i];
            const TCEString dataOpndVtStr = dataOpndVt.valueTypeStr();

            if (hasRegisterClassSupport(dataOpndVt)) {
                Operation* op = NULL;
                if (memDataOpndWidth <= 8) {
                    if (littleEndian_) {
                        op = &opPool.operation("LD8");
                    } else {
                        op = &opPool.operation("LDQ");
                    }
                } else if (memDataOpndWidth <= 16) {
                    if (littleEndian_) {
                        op = &opPool.operation("LD16");
                    } else {
                        op = &opPool.operation("LDH");
                    }
                } else if (memDataOpndWidth <= 32){
                    if (littleEndian_) {
                        op = &opPool.operation("LD32");
                    } else {
                        op = &opPool.operation("LDW");
                    }
                } else {
                    if (littleEndian_) {
                        op = &opPool.operation("LD64");
                    } else {
                        op = &opPool.operation("LDD");
                        std::cerr << "Warning: SIMD support with "
                                  << "big-endian adf is deprecated" << std::endl;
                    }
                }

                std::vector<ValueType> inputs;
                std::vector<ValueType> outputs;

                // Address operand 32 or 64 bits
                inputs.push_back(ValueType(defaultType));
                // Data operand.
                outputs.push_back(dataOpndVt);

                writeOperationDefUsingGivenOperandTypes(
                    o, *op, false, inputs, outputs, dataOpndVtStr);
            }
        }

        for (size_t i = 0; i< vecVts.size(); ++i) {
            const ValueType& dataOpndVt = vecVts[i];
            const TCEString dataOpndVtStr = dataOpndVt.valueTypeStr();

            if (hasRegisterClassSupport(dataOpndVt)) {
            Operation* op = NULL;
            if (memDataOpndWidth <= 8) {
                if (littleEndian_) {
                    op = &opPool.operation("ST8");
                } else {
                    op = &opPool.operation("STQ");
                }
            } else if (memDataOpndWidth <= 16) {
                if (littleEndian_) {
                    op = &opPool.operation("ST16");
                } else {
                    op = &opPool.operation("STH");
                }
            } else if (memDataOpndWidth <= 32) {
                if (littleEndian_) {
                    op = &opPool.operation("ST32");
                } else {
                    op = &opPool.operation("STW");
                }
            } else {
                if (littleEndian_) {
                    op = &opPool.operation("ST64");
                } else {
                    op = &opPool.operation("STD");
                    std::cerr << "Warning: SIMD support with "
                              << "big-endian adf is deprecated" << std::endl;
                }
            }

            std::vector<ValueType> inputs;
            std::vector<ValueType> outputs;

            // Address operand 32 or 64 bits.
            inputs.push_back(ValueType(defaultType));
            inputs.push_back(dataOpndVt);
            // There is no output operands for store.

            writeOperationDefUsingGivenOperandTypes(
                o, *op, false, inputs, outputs, dataOpndVtStr);

            }
        }

        memDataOpndWidth *= 2;
    }
}

/**
 * Writes bitwise NOT/AND/IOR/XOR operation definitions for vector operations.
 *
 * For 32-bit or less vector widths, 32-bit base operation is used. For
 * bitwise bool vector operations wider than 32 bits, corresponding bitwise
 * operation is searched from the machine, and used to create operation
 * definitions if it exists.
 *
 * @param o Output stream.
 * @param op Operation used to create bitwise operations.
 * @param skipPattern If true, operation pattern writing is skipped.
 */
void
TDGen::writeVectorBitwiseOperationDefs(
    std::ostream& o, Operation& op, bool skipPattern) {
    o << endl;

    if (op.numberOfOutputs() == 0) {
        assert(false && "Bitwise operation has 0 outputs.");
    }

    int opWidth = op.output(0).width();
    std::vector<ValueType> vts = ValueType::vectorTypesOfWidth(opWidth);

    for (size_t i = 0; i < vts.size(); ++i) {
        const ValueType& vecVt = vts[i];
        const TCEString vecVtStr = vecVt.valueTypeStr();

        // Only integer value types are used in bitwise operations.
        if (!vecVt.isFloat_ && hasRegisterClassSupport(vecVt)) {
            std::vector<ValueType> inputs;
            std::vector<ValueType> outputs;

            for (int i = 0; i < op.numberOfInputs(); ++i) {
                inputs.push_back(vecVt);
            }
            for (int i = 0; i < op.numberOfOutputs(); ++i) {
                outputs.push_back(vecVt);
            }

            writeOperationDefUsingGivenOperandTypes(
                o, op, skipPattern, inputs, outputs, vecVtStr);
        }
    }

    o << endl;
}

/**
 * Attaches load and store operations to vector types that don't have them.
 *
 * Checks first which vector value types don't have load and store operations
 * for memory accessing. After collecting the value types without load/store,
 * the function tries to search existing load/store instructions whose
 * data operand width matches the vector value type that doesn't have a
 * load/store operation. If the function can find matching memory
 * instructions, it creates new memory instructions for the vector value types
 * by exploiting existing matching memory instructions.
 */
void
TDGen::writeVectorLoadStoreOperationExploitations(
    std::ostream& o) {

    const char* defaultTypeStr = mach_.is64bit() ? "i64" : "i32";

    // Value type string of the register class.
    std::set<TCEString> withoutLoadOperation;
    std::set<TCEString> withoutStoreOperation;

    // Collect all vector value types that are without load or store
    // operation support for memory accessing.
    std::map<TCEString, RegisterClass>::iterator it;
    for (it = vRegClasses_.begin(); it != vRegClasses_.end(); ++it) {
        const TCEString vtStr = it->first;
        const ValueType vt(vtStr);

        // 32-bit and smaller vectors are covered by LDQ/LDH/LDW/STQ/STH/STW.
        // TODO: shoudl this have 64 also for 32-bit ADFs?
        if (vt.width() > maxScalarWidth_) {
            if (registerLoads_.find(vtStr) == registerLoads_.end()) {
                withoutLoadOperation.insert(vtStr);
            }

            if (registerStores_.find(vtStr) == registerStores_.end()) {
                withoutStoreOperation.insert(vtStr);
            }
        }
    }

    o << endl << "// Define memory operations for vector operands without"
      << endl << "// load or store operation definition by exploiting"
      << endl << "// existing memory operations of proper width."  << endl;

    OperationPool opPool;

    std::set<TCEString>::const_iterator wLoadIt;
    for (wLoadIt = withoutLoadOperation.begin();
         wLoadIt != withoutLoadOperation.end(); ++wLoadIt) {
        const TCEString& dataOpndVtStr = *wLoadIt;
        const ValueType vt(dataOpndVtStr);

        bool foundExploitable = false;
        std::map<TCEString, InstructionInfo>::const_iterator loadIt =
            registerLoads_.begin();

        // Try to find any machine load instruction that is able to load
        // equal amount of bytes from memory as the ValueType is wide.
        while (!foundExploitable && loadIt != registerLoads_.end()) {
            const ValueType loadInstrVt(loadIt->first);

            // If found a match, remove the last two characters of the
            // load instruction name to get the OSAL operation name.
            if (vt.width() == loadInstrVt.width()) {
                foundExploitable = true;
                const TCEString opName = (loadIt->second).osalOpName_;
                Operation& op = opPool.operation(opName.c_str());

                std::vector<ValueType> inputs;
                std::vector<ValueType> outputs;

                // Address operand always 32 or 64 bits.
                inputs.push_back(ValueType(defaultTypeStr));
                // Data operand.
                outputs.push_back(dataOpndVtStr);

                writeOperationDefUsingGivenOperandTypes(
                    o, op, false, inputs, outputs, dataOpndVtStr);
            }

            ++loadIt;
        }
    }

    std::set<TCEString>::const_iterator wStoreIt;
    for (wStoreIt = withoutStoreOperation.begin();
         wStoreIt != withoutStoreOperation.end(); ++wStoreIt) {
        const TCEString& dataOpndVtStr = *wStoreIt;
        const ValueType vt(dataOpndVtStr);

        bool foundExploitable = false;
        std::map<TCEString, InstructionInfo>::const_iterator storeIt =
            registerStores_.begin();

        // Try to find any machine store instruction that is able to store
        // equal amount of bytes to memory as the ValueType is wide.
        while (!foundExploitable && storeIt != registerStores_.end()) {
            const ValueType storeInstrVt(storeIt->first);

            // If found a match, remove the last two characters of the
            // store instruction name to get the OSAL operation name.
            if (vt.width() == storeInstrVt.width()) {
                foundExploitable = true;
                const TCEString opName = (storeIt->second).osalOpName_;
                Operation& op = opPool.operation(opName.c_str());

                std::vector<ValueType> inputs;
                std::vector<ValueType> outputs;

                // Address operand always i32 or 64 bits.
                inputs.push_back(ValueType(defaultTypeStr));
                inputs.push_back(dataOpndVtStr);
                // There is no output operands for store.

                writeOperationDefUsingGivenOperandTypes(
                    o, op, false, inputs, outputs, dataOpndVtStr);
            }

            ++storeIt;
        }
    }

    o << endl;
}

/**
 * Writes vector operation definitions that exploit bigger vector operations.
 *
 * Exploited vector operands are written in ascending order by their subword
 * count, meaning that e.g. ADD32X16, ADD32X32, and ADD32X4 operation
 * exploitations are listed so that ADD32X4 are written first, then
 * ADD32X16, and then ADD32X32. This way the instruction selection will
 * first use ADD32X4 to execute vector addition for v2i32 vector operand
 * types, and thus, is more optimal solution than executing the same
 * v2i32 addition with ADD32X16 (though ADD32X4 would be also available
 * in the machine).
 */
void
TDGen::writeWiderVectorOperationExploitations(std::ostream&) {
    abortWithError("WiP");
}

/**
 * Generates implementation of getStore() function.
 */
void
TDGen::genGeneratedTCEPlugin_getStore(std::ostream& o) const {
    TCEString prefix = "&"; // Address of -operator.
    TCEString rcpf = "RegsRegClass";
    TCEString rapf = "TCE::RARegRegClass";
    TCEString storeB = littleEndian_ ? "ST8Brb;" : "STQBrb;";
    TCEString storeW = littleEndian_ ? "ST32" : "STW";
    TCEString storeH = littleEndian_ ? "ST16" : "STH";
    TCEString storeL = "ST64";

    o << "#include <stdio.h>" << endl
      << "int GeneratedTCEPlugin::getStore(const TargetRegisterClass *rc)"
      << " const {" << endl;

    o << "\tif (rc == " << prefix << rapf
      << ") return TCE::STWRArr;"
      << endl;

    for (RegClassMap::const_iterator ri = regsInClasses_.begin();
         ri != regsInClasses_.end(); ri++) {

        if ((ri->first.find("R1") == 0 ||
             ri->first.find(TDGen::guardRegTemplateName) == 0) &&
            ri->first.find("R16") != 0) {
            o << "\tif (rc == " << prefix << "TCE::" << ri->first
              << rcpf << ") return TCE::" << storeB << endl;
        }
        if (ri->first.find("R32") == 0) {
            o << "\tif (rc == " << prefix << "TCE::" << ri->first
              << rcpf << ") return TCE::" << storeW << "rr;" << endl;

            o << "\tif (rc == " << prefix << "TCE::"  << ri->first
              << "I" << rcpf << ") return TCE::" << storeW << "rr;" << endl;

            o << "\tif (rc == " << prefix << "TCE::"  << ri->first
              << "FP" << rcpf << ") return TCE::" << storeW << "fr;" << endl;

            o << "\tif (rc == " << prefix << "TCE::"  << ri->first
              << "HFP" << rcpf << ") return TCE::" << storeH << "hr;" << endl;
        }
        if (mach_.is64bit()) {
            if (ri->first.find("R64") == 0) {
                o << "\tif (rc == " << prefix << "TCE::" << ri->first
                  << rcpf << ") return TCE::" << storeL << "ss;" << endl;

                o << "\tif (rc == " << prefix << "TCE::"  << ri->first
                  << "I" << rcpf << ") return TCE::" << storeL << "ss;"
                  << endl;

                o << "\tif (rc == " << prefix << "TCE::"  << ri->first
                  << "DFP" << rcpf << ") return TCE::" << storeL << "ds;"
                  << endl;
            }
                // TODO: double support here?
        }
    }

    if (use64bitForFP_) {
        o << "\tif (rc == &TCE::FPRegsRegClass) return TCE::ST64fs;"
          << std::endl;

        o << "\tif (rc == &TCE::HFPRegsRegClass) return TCE::ST64hs;"
          << std::endl;
    } else {
        o << "\tif (rc == &TCE::FPRegsRegClass) return TCE::"
          << storeW << "fr;" << std::endl;

        o << "\tif (rc == &TCE::HFPRegsRegClass) return TCE::"
          << storeW << "hr;" << std::endl;
    }

    std::map<TCEString, RegisterClass>::const_iterator it;
    for (it = vRegClasses_.begin(); it != vRegClasses_.end(); ++it) {
        const TCEString& vtStr = it->first;
        const RegisterClass& regClass = it->second;

        std::map<TCEString, InstructionInfo>::const_iterator storeIt =
            registerStores_.find(vtStr);

        if (storeIt != registerStores_.end()) {
            o << "\tif (rc == &TCE::" << regClass.name() << "RegClass) "
              << "return TCE::" << (storeIt->second).instrName_ << ";" << endl;
        } else {
            verbose("Warning: no store generated for RegisterClass " +
                    regClass.name() + " to GeneratedTCEPlugin::getStore");
        }
    }

    o << "\tstd::cerr << \"regclass id:\" <<rc->getID() << \"of size \" "
      << "<<rc->MC->RegsSize<< std::endl;" << std::endl;
    o  << "\tassert(0&&\"Storing given regclass to stack not supported. "
       << "Bug in backend?\");"
       << endl
       << "} " << endl
       << endl;
}

/**
 * Generates implementation of getLoad() function.
 */
void
TDGen::genGeneratedTCEPlugin_getLoad(std::ostream& o) const {
    TCEString prefix = "&"; // Address of -operator.
    TCEString rcpf = "RegsRegClass";
    TCEString rapf = "TCE::RARegRegClass";

    TCEString loadB = littleEndian_ ? "LD8" : "LDQ";
    if (!mach_.hasOperation(loadB)) {
        loadB = littleEndian_ ? "LDU8" : "LDQU";
        if (!mach_.hasOperation(loadB)) {
            loadB="";
        }
    }

    TCEString loadW = littleEndian_ ? "LD32" : "LDW";
    TCEString loadH = littleEndian_ ? "LD16" : "LDH";
    TCEString loadL = "LD64";
    if (!mach_.hasOperation(loadH)) {
        loadH = littleEndian_ ? "LDU16;" : "LDHU";
        if (!mach_.hasOperation(loadH)) {
            loadH="";
        }
    }

    o << "int GeneratedTCEPlugin::getLoad(const TargetRegisterClass *rc)"
      << " const {" << endl;

    o << "\tif (rc == " << prefix << rapf << ") return TCE::"
      << loadW << "RAr;" << endl;

    if (!mach_.hasOperation(loadW) && mach_.hasOperation("LDU32")) {
        loadW = "LDU32";
    }

    for (RegClassMap::const_iterator ri = regsInClasses_.begin();
         ri != regsInClasses_.end(); ri++) {
        if ((ri->first.find("R1") == 0 ||
             ri->first.find(TDGen::guardRegTemplateName) == 0) &&
            ri->first.find("R16") != 0 && loadB != "") {
            o << "\tif (rc == " << prefix << "TCE::" << ri->first
              << rcpf <<") return TCE::" << loadB << "Br;" << endl;
        }
        if (ri->first.find("R32") == 0) {
            o << "\tif (rc == " << prefix << "TCE::" << ri->first
              << rcpf << ") return TCE::" << loadW << "rr;" << endl;

            o << "\tif (rc == " << prefix << "TCE::" << ri->first
              << "I" << rcpf << ") return TCE::" << loadW << "rr;" << endl;

            o << "\tif (rc == " << prefix << "TCE::" << ri->first
              << "FP" << rcpf << ") return TCE::" << loadW << "fr;" << endl;
            if (loadH != "") {
                o << "\tif (rc == " << prefix << "TCE::" << ri->first
                  << "HFP" << rcpf << ") return TCE::" << loadH << "hr;" << endl;
            }
        }
        if (mach_.is64bit()) {
            if (ri->first.find("R64") == 0) {
                o << "\tif (rc == " << prefix << "TCE::" << ri->first
                  << rcpf << ") return TCE::" << loadL << "ss;" << endl;

                o << "\tif (rc == " << prefix << "TCE::" << ri->first
                  << "I" << rcpf << ") return TCE::" << loadL << "ss;" << endl;

                o << "\tif (rc == " << prefix << "TCE::" << ri->first
                  << "DFP" << rcpf << ") return TCE::" << loadL << "ds;" << endl;

            }
        }
    }

    if (use64bitForFP_) {
        o << "\tif (rc == &TCE::FPRegsRegClass) return TCE::LD64fs;"
          << std::endl;

        o << "\tif (rc == &TCE::HFPRegsRegClass) return TCE::LD64hs;"
           << std::endl;
    } else {
        o << "\tif (rc == &TCE::FPRegsRegClass) return TCE::"
          << loadW << "fr;" << std::endl;

        o << "\tif (rc == &TCE::HFPRegsRegClass) return TCE::"
          << loadW << "hr;" << std::endl;
    }

    std::map<TCEString, RegisterClass>::const_iterator it;
    for (it = vRegClasses_.begin(); it != vRegClasses_.end(); ++it) {
        const TCEString& vtStr = it->first;
        const RegisterClass& regClass = it->second;

        std::map<TCEString, InstructionInfo>::const_iterator loadIt =
            registerLoads_.find(vtStr);

        if (loadIt != registerLoads_.end()) {
            o << "\tif (rc == &TCE::" << regClass.name() << "RegClass) "
              << "return TCE::" << (loadIt->second).instrName_ << ";" << endl;
        } else {
            verbose("Warning: no load generated for RegisterClass " +
                    regClass.name() + " to GeneratedTCEPlugin::getLoad");
        }
    }

    o  << "\tprintf(\"regclass of size %d \\n\", rc->MC->RegsSize);" << std::endl
      << "\tassert(0&&\"loading from stack to given regclass not supported."
      << " Bug in backend?\");"
      << endl
      << "} " << endl
      << endl;
}


/**
 * Generates a function that adds existing register classes in lowering phase.
 */
void
TDGen::genTCETargetLoweringSIMD_addVectorRegisterClasses(
    std::ostream& o) const {
    o << endl << "// Adds all vector classes to backend" << endl
      << "void TCETargetLowering::addVectorRegisterClasses() {" << endl;
    std::map<TCEString, RegisterClass>::const_iterator it;
    for (it = vRegClasses_.begin(); it != vRegClasses_.end(); ++it) {
        const RegisterClass& regClass = it->second;

        o << "\taddRegisterClass("
          << "MVT::" << regClass.valueType().valueTypeStr() << ", "
          << "&TCE::" << regClass.name() << "RegClass);"
          << endl;
    }

    o << "}" << endl;

    o << endl << "// Sets build and extract lowering" << endl
      << "void TCETargetLowering::addVectorLowerings() {" << endl;

    for (it = vRegClasses_.begin(); it != vRegClasses_.end(); ++it) {
        const RegisterClass& regClass = it->second;

        o << "\tsetOperationAction(ISD::BUILD_VECTOR,"
          << "MVT::" << regClass.valueType().valueTypeStr() << ", "
          << "Custom);" << endl;

        o << "\tsetOperationAction(ISD::EXTRACT_SUBVECTOR,"
          << "MVT::" << regClass.valueType().valueTypeStr() << ", "
          << "Expand);" << endl;

        for (auto vt : ValueType::SUPPORTED_LLVM_VALUE_TYPES) {
            if (vRegClasses_.find(vt) == vRegClasses_.end()) {
                o << "\tsetTruncStoreAction(MVT::"
                  << regClass.valueType().valueTypeStr()
                  << ", MVT::" << vt << ", Expand);" << endl;
            }
        }
    }
    o << "}" << endl;
}

/**
 * Generates a function that returns correct reg class for a value type.
 */
void
TDGen::genTCETargetLoweringSIMD_associatedVectorRegClass(
    std::ostream& o) const {
    o << endl << "// Returns vector register class for given value type"
      << endl
      << "std::pair<unsigned, const TargetRegisterClass*> "
      << "TCETargetLowering::associatedVectorRegClass(const EVT& vt) "
      << "const {"
      << endl;

    std::map<TCEString, RegisterClass>::const_iterator it;
    for (it = vRegClasses_.begin(); it != vRegClasses_.end(); ++it) {
        const TCEString& vtStr = it->first;
        const RegisterClass& regClass = it->second;

        o << "\tif (vt == MVT::" << vtStr << ") "
          << "return std::make_pair(0U, &TCE::"
          << regClass.name() << "RegClass);" << endl;
    }

    o << "\treturn std::make_pair(0U, (TargetRegisterClass*)0);" << endl
      << "}" << endl;
}

/**
 * Generates a function that returns correct return value type for comparisons.
 *
 * In LLVM vector comparison operations result into boolean vectors,
 * which have the same subword count as input vectors.
 */
void
TDGen::genTCETargetLoweringSIMD_getSetCCResultVT(std::ostream& o) const {
    o << endl
      << "llvm::EVT TCETargetLowering::getSetCCResultVT(const EVT& vt) "
      << "const {"
      << endl;

    int subwCount = 2;
    while (subwCount <= MAX_SUBW_COUNT) {
        TCEString boolVecVtStr = "v" + Conversion::toString(subwCount) + "i1";
        ValueType boolVecVt(boolVecVtStr);

        if (!boolVecVt.isSupportedByLLVM()) {
            break;
        }

        o << "\tif (vt.getVectorElementCount().getKnownMinValue() == "
          << Conversion::toString(subwCount) << ") return llvm::MVT::"
          << boolVecVtStr << ";" << endl;
        subwCount *= 2;
    }

    o << "\treturn llvm::MVT::INVALID_SIMPLE_VALUE_TYPE;" << endl
      << "}" << endl;
}

/**
 * Writes return address register definition to the output stream.
 */
void
TDGen::writeRARegisterInfo(std::ostream& o) {
    o << "class Rra<string n> : TCEReg<n, []>;" << std::endl;
    o << "def RA : Rra<\"return-address\">, ";
    o << "DwarfRegNum<[" << dregNum_++ << "]>;";
    o << std::endl;
    if (mach_.is64bit()) {
        o << "def RAReg : RegisterClass<\"TCE\", [i64], 64, (add RA)>;" <<
            std::endl;
    } else {
        o << "def RAReg : RegisterClass<\"TCE\", [i32], 32, (add RA)>;" <<
            std::endl;
    }
}



#if 0 // TODO: where does this belong???

    o << "\treturn llvm::MVT::INVALID_SIMPLE_VALUE_TYPE;" << endl
      << "}" << endl;
}
#endif

/**
 * Generates implementation of copyPhysVectorReg().
 *
 * Only the previously created vector register classes are added
 * to the implementation to make LLVM compilation flow work.
 */
void
TDGen::genTCEInstrInfoSIMD_copyPhysVectorReg(std::ostream& o) const {
    o << endl
      << "#include <llvm/CodeGen/MachineInstrBuilder.h>" << endl
      << "// Copies a vector register to another" << endl

      << "bool TCEInstrInfo::copyPhysVectorReg(" << endl
      << "\tMachineBasicBlock& mbb," << endl
      << "\tMachineBasicBlock::iterator mbbi," << endl
      << "const DebugLoc& dl," << endl
      << "\tMCRegister destReg, MCRegister srcReg," << endl
      << "\tbool killSrc) const {" << endl
      << endl;

    std::map<TCEString, RegisterClass>::const_iterator it;
    for (it = vRegClasses_.begin(); it != vRegClasses_.end(); ++it) {
        const RegisterClass& regClass = it->second;

        o << "\tif (TCE::" << regClass.name()
          << "RegClass.contains(destReg, srcReg)) {" << endl
          << "\t\tBuildMI(mbb, mbbi, dl, get(TCE::"
          << "MOV" << regClass.name() << "), destReg)" << endl
          << "\t\t.addReg(srcReg, getKillRegState(killSrc));" << endl
          << "\t\treturn true;" << endl
          << "\t}" << endl;
    }
    o << "\treturn false;" << endl << "}" << endl;
}

void
TDGen::writeVectorImmediateWriteDefs(std::ostream& instrInfoTD) {
    for (auto it = vRegClasses_.begin(); it != vRegClasses_.end(); ++it) {
        const RegisterClass& regClass = it->second;
        TCEString regClassName = regClass.name();
        ValueType valType = regClass.valueType();
        TCEString valTypeName = valType.valueTypeStr();
        if (valType.width() <= 32) {
            TCEString opName = "MOVI" + valTypeName;
            instrInfoTD << "def " << opName << " : InstTCE<(outs "
                        << regClassName
                        << ":$dst), (ins i32imm:$val), \"\",[]>;"
                        << std::endl;

            opNames_[opName] = "MOVE";
        }
    }
}

void
TDGen::genGeneratedTCEPlugin_getVectorImmediateOpcode(
    std::ostream& backendCode) const {
    backendCode << "int GeneratedTCEPlugin::getVectorImmediateOpcode(const "
                   "EVT& vt) const {"
                << std::endl;
    for (auto it = vRegClasses_.begin(); it != vRegClasses_.end(); ++it) {
        const RegisterClass& regClass = it->second;
        TCEString regClassName = regClass.name();
        ValueType valType = regClass.valueType();
        TCEString valTypeName = valType.valueTypeStr();
        if (valType.width() <= 32) {
            TCEString opName = "MOVI" + valTypeName;
            backendCode << "\tif (vt == MVT::" << valTypeName
                        << ") return TCE::" << opName << ";" << std::endl;
        }
    }
    backendCode << "\treturn -1;\n}" << endl;
}

void
TDGen::genGeneratedTCEPlugin_getGatherOpcode(std::ostream& o) const {
    o << endl
      << "// Returns correct vector GATHER opcode" << endl
      << "int GeneratedTCEPlugin::getGatherOpcode("
      << "const EVT& vt) const {" << endl;

    for (auto it = gatherOperations_.begin(); it != gatherOperations_.end();
         ++it) {
        o << "\tif (vt == MVT::" << it->first
          << ") return TCE::" << it->second << ";" << endl;
    }

    o << "\treturn -1;" << endl << "}" << endl;
}

void
TDGen::genGeneratedTCEPlugin_getLoadOpcode(std::ostream& o) const {
    o << endl
      << "// Returns correct load opcode" << endl
      << "int GeneratedTCEPlugin::getLoadOpcode("
      << "const EVT& vt) const {" << endl;

    for (auto it = registerLoads_.begin(); it != registerLoads_.end(); ++it) {
        o << "\tif (vt == MVT::" << it->first
          << ") return TCE::" << it->second.instrName_ << ";" << endl;
    }
    o << "\treturn -1;" << endl << "}" << endl;
}

void
TDGen::genGeneratedTCEPlugin_getAddOpcode(std::ostream& o) const {
    o << endl
      << "// Returns correct vector ADD opcode" << endl
      << "int GeneratedTCEPlugin::getAddOpcode("
      << "const EVT& vt) const {" << endl;

    for (auto it = addOperations_.begin(); it != addOperations_.end(); ++it) {
        o << "\tif (vt == MVT::" << it->first
          << ") return TCE::" << it->second << ";" << endl;
    }

    o << "\treturn -1;" << endl << "}" << endl;
}

void
TDGen::genGeneratedTCEPlugin_getShlOpcode(std::ostream& o) const {
    o << endl
      << "// Returns correct vector SHL opcode" << endl
      << "int GeneratedTCEPlugin::getShlOpcode("
      << "const EVT& vt) const {" << endl;

    for (auto it = shlOperations_.begin(); it != shlOperations_.end(); ++it) {
        o << "\tif (vt == MVT::" << it->first
          << ") return TCE::" << it->second << ";" << endl;
    }

    o << "\treturn -1;" << endl << "}" << endl;
}

void
TDGen::genGeneratedTCEPlugin_getIorOpcode(std::ostream& o) const {
    o << endl
      << "// Returns correct vector SHL opcode" << endl
      << "int GeneratedTCEPlugin::getIorOpcode("
      << "const EVT& vt) const {" << endl;

    for (auto it = iorOperations_.begin(); it != iorOperations_.end(); ++it) {
        o << "\tif (vt == MVT::" << it->first
          << ") return TCE::" << it->second << ";" << endl;
    }

    o << "\treturn -1;" << endl << "}" << endl;
}

/**
 * Checks that the target machine has required registers to build a usable
 * plugin.
 *
 * @return True if required registers were found, false if not.
 */
bool
TDGen::checkRequiredRegisters() {
    if (!mach_.is64bit() && regs32bit_.size() < requiredI32Regs_) {
        std::string msg =
            (boost::format(
                "Architecture doesn't meet the minimal requirements.\n"
                "Only %d 32-bit general purpose registers found. At least %d\n"
                "needed.")
                % regs32bit_.size() % requiredI32Regs_)
            .str();

        if (tempRegFiles_.size() > 0) {
            msg += "Your machine is not fully connected, thus one register\n"
                "from each register file are reserved for temp moves and\n"
                "not used as general purpose registers.";
        }

        throw InvalidData(__FILE__, __LINE__, __func__, msg);
        return false;
    }

    if (regs64bit_.size() < requiredI64Regs_) {
        std::string msg =
            (boost::format(
                "Architecture doesn't meet the minimal requirements.\n"
                "Only %d 64-bit general purpose registers found. At least %d\n"
                "needed. ")
             % regs64bit_.size() % requiredI64Regs_)
            .str();

        if (tempRegFiles_.size() > 0) {
            msg += "Your machine is not fully connected, thus one register\n"
                "from each register file is reserved for temp moves and\n"
                "not used as general purpose registers.";
        }

        throw InvalidData(__FILE__, __LINE__, __func__, msg);
        return false;
    }

    return true;
}

/**
 * Writes all machine instructions to instruction info .td file.
 *
 * @param os Output stream to the file.
 */
void
TDGen::writeInstrInfo(std::ostream& os) {

    writeCallSeqStart(os);

    OperationDAGSelector::OperationSet opNames;
    OperationDAGSelector::OperationSet requiredOps =
        LLVMBackend::llvmRequiredOpset(true, littleEndian_, mach_.is64bit());

    const TTAMachine::Machine::FunctionUnitNavigator fuNav =
        mach_.functionUnitNavigator();

    OperationPool opPool;

    for (int i = 0; i < fuNav.count(); i++) {
        const TTAMachine::FunctionUnit* fu = fuNav.item(i);
        for (int o = 0; o < fu->operationCount(); o++) {
            const std::string opName = fu->operation(o)->name();
            opNames.insert(StringTools::stringToUpper(opName));
        }
    }

    if (littleEndian_) {
        if (mach_.is64bit()) {
            opNames_["ST64fa"] = "ST64";
            opNames_["ST64fs"] = "ST64";
            opNames_["ST64ha"] = "ST64";
            opNames_["ST64hs"] = "ST64";
            opNames_["ST64da"] = "ST64";
            opNames_["ST64ds"] = "ST64";

            opNames_["LD64fa"] = "LD64";
            opNames_["LD64fs"] = "LD64";
            opNames_["LD64ha"] = "LD64";
            opNames_["LD64hs"] = "LD64";
            opNames_["LD64da"] = "LD64";
            opNames_["LD64ds"] = "LD64";
        }

        if (mach_.hasOperation("LD32")) {
            opNames_["LD32fr"] = "LD32";
            opNames_["LD32fi"] = "LD32";

            opNames_["LD32hr"] = "LD32";
            opNames_["LD32hi"] = "LD32";

        } else if (mach_.hasOperation("LDU32")) {
            opNames_["LD32fr"] = "LDU32";
            opNames_["LD32fi"] = "LDU32";

            opNames_["LD32hr"] = "LDU32";
            opNames_["LD32hi"] = "LDU32";
        }
        opNames_["ST32fr"] = "ST32";
        opNames_["ST32fi"] = "ST32";

        opNames_["ST32hr"] = "ST32";
        opNames_["ST32hi"] = "ST32";
        
        opNames_["ST16hr"] = "ST16";
        opNames_["ST16hi"] = "ST16";
    } else {
        opNames_["LDWfr"] = "LDW";
        opNames_["LDWfi"] = "LDW";
        opNames_["LDWhr"] = "LDW";
        opNames_["LDWhi"] = "LDW";

        opNames_["STWfr"] = "STW";
        opNames_["STWfi"] = "STW";
        
        opNames_["STWhr"] = "STW";
        opNames_["STWhi"] = "STW";
        opNames_["STHhr"] = "STH";
        opNames_["STHhi"] = "STH";
    }

    // Some global/address immediate if conversion fails
    // so commented out.
    // TODO: ^ up-to-date comment? ^
    if (hasConditionalMoves_) {

        if (mach_.is64bit()) {
            truePredOps_["MOV64ss"] = "PRED_TRUE_MOV64ss";
            falsePredOps_["MOV64ss"] = "PRED_FALSE_MOV64ss";
        }

        truePredOps_["MOVI32rr"] = "PRED_TRUE_MOVI32rr";
        falsePredOps_["MOVI32rr"] = "PRED_FALSE_MOVI32rr";
        truePredOps_["MOVI1rr"] = "PRED_TRUE_MOVI1rr";
        falsePredOps_["MOVI1rr"] = "PRED_FALSE_MOVI1rr";
    }

    OperationDAGSelector::OperationSet::const_iterator iter = opNames.begin();
    for (; iter != opNames.end(); iter++) {
        OperationDAGSelector::OperationSet::iterator r = 
            requiredOps.find(*iter);
        if (r != requiredOps.end()) {
            requiredOps.erase(r);
        }
        Operation& op = opPool.operation((*iter).c_str());
        bool skipPattern = false;

        if (&op == &NullOperation::instance()) {
            continue;
        }

        // TODO: Allow multioutput (remove last or)
        if (!operationCanBeMatched(op)) {
            // TODO: write opeation def without graphs
            if (&op != &NullOperation::instance()) {
                skipPattern = true;
                if (Application::verboseLevel() > 0) {
                    Application::logStream()
                        << "Skipped writing operation pattern for "
                        << op.name() << endl;
                }
            } else {
                // NULL op - ignore
                continue;
            }
        }

        // TODO: remove this. For now MIMO operation patterns are not
        // supported by tablegen.
        if (op.numberOfOutputs() > 1) {
            skipPattern = true;
            continue;
        }

        if (op.isVectorOperation()) {
            if (isVectorLoadOperation(op) || isVectorStoreOperation(op)) {
                writeVectorMemoryOperationDefs(os, op, skipPattern);
            } else if (isVectorBitwiseOperation(op)) {
                writeVectorBitwiseOperationDefs(os, op, skipPattern);
            } else {
                writeVectorOperationDefs(os, op, skipPattern);
            }
        } else {
            writeOperationDefs(os, op, skipPattern);
        }
    }

    writeVectorRegisterMoveDefs(os);
    writeScalarToVectorDefs(os);
    writeVectorBitConversions(os);
    writeScalarOperationExploitations(os);
    writeVectorLoadStoreOperationExploitations(os);
    writeVectorTruncStoreDefs(os);
    writeVectorImmediateWriteDefs(os);

    // Call operations.
    writeControlFlowInstrDefs(os);

    // Hardware loop instructions
    writeHWLoopDef(os);

    // Emulated operations.
    for (iter = requiredOps.begin(); iter != requiredOps.end(); iter++) {
        const Operation& op = opPool.operation((*iter).c_str());       

        if (&op == &NullOperation::instance()) {
            std::string msg = "Required OP '" + *iter + "' not found.";
            throw InvalidData(__FILE__, __LINE__, __func__, msg);
        }

        const OperationDAGSelector::OperationDAGList emulationDAGs =
            OperationDAGSelector::findDags(op.name(), allOpNames_, immInfo_);

        if (emulationDAGs.empty()) {
            if (Application::verboseLevel() >
                Application::VERBOSE_LEVEL_DEFAULT) {
                Application::logStream() << "Warning: Operation '" << *iter
                                         << "' not supported." << endl;
            }
        } else {
            /// @note This todo marking is from TDGenSIMD
            // TODO: write all dags of operation (first as normal, the rest
            // as pattern)
            writeEmulationPattern(os, op, emulationDAGs.smallestNodeCount());
        }
    }

    createByteExtLoadPatterns(os);
    createShortExtLoadPatterns(os);
    if (mach_.is64bit()) {
        create32BitExtLoadPatterns(os);
    }

    writeBooleanStorePatterns(os);

    writeMiscPatterns(os);
    createSelectPatterns(os);
    createConstantMaterializationPatterns(os);
    createConstShiftPatterns(os);
    createBoolAndHalfLoadPatterns(os);

    os << std::endl;
#if LLVM_MAJOR_VERSION > 21
    if (mach_.is64bit())
        os << "defm : RemapAllTargetPseudoPointerOperands<R64IRegs>;"
           << std::endl;
    else
        os << "defm : RemapAllTargetPseudoPointerOperands<R32IRegs>;"
           << std::endl;
#endif
}

void TDGen::writeCallDefRegs(std::ostream& o) {

    for (unsigned i = 0; i < resRegNames_.size(); i++) {
        if (i > 0) o << ", ";
        o << resRegNames_[i];
    }
    // GPRs, argument registers, and vector registers are clobbered by calls
    // but this is now handled by the RegMask operand added to CALL instructions
}


/**
 * Writes control flow instructions definitions and patterns
 */
void
TDGen::writeControlFlowInstrDefs(std::ostream& os) {

    writeCondBranchDefs(os);

    writeCallDef(os);
}

/**
 * Writes hwloop instructions and patterns.
 */
void
TDGen::writeHWLoopDef(std::ostream& os) {
    os << std::endl << "// Hardware loop instructions" << std::endl;
    if (mach_.controlUnit()->hasOperation("hwloop")) {
        os << "let isTerminator=1 in {" << std::endl
           << "  def HWLOOPii : InstTCE<(outs), (ins i32imm0:$rIter, "
              "i32imm0:$rInstr), \"\", []>;"
           << std::endl
           << "  def HWLOOPri : InstTCE<(outs), (ins i32imm0:$rIter, "
              "i32imm0:$rInstr), \"\", []>;"
           << std::endl
           << "}" << std::endl
           << "def : Pat<(int_set_loop_iterations i32imm0:$rIter), "
              "(HWLOOPii i32imm0:$rIter, 0)>;"
           << std::endl
           << "def : Pat<(int_set_loop_iterations R32IRegs:$rIter), "
              "(HWLOOPri R32IRegs:$rIter, 0)>;"
           << std::endl;
        opNames_["HWLOOPii"] = "hwloop";
        opNames_["HWLOOPri"] = "hwloop";
    }

    // Write loop jump pseudo
    os << "let isTerminator = 1 in {" << std::endl
       << "  def LJUMP : InstTCE<(outs), (ins brtarget:$dst), \"\", []>;"
       << std::endl
       << "}" << std::endl
       << std::endl;
    opNames_["LJUMP"] = "PSEUDO";
}

/**
 * Writes instructions definitions and patterns for conditional branches.
 */
void
TDGen::writeCondBranchDefs(std::ostream& os) {

    auto cuOpset = MachineInfo::getOpset(*mach_.controlUnit());

    if (MachineInfo::supportsBoolRegisterGuardedJumps(mach_)) {

        os << std::endl << "let isTerminator = 1, isBranch = 1 in {" << std::endl;

        if (cuOpset.count("JUMP") && opNames_.count("TCEBRCOND") == 0) {
            writeInstrDef(os, "TCEBRCOND", "",
                          "GuardRegs:$gr, brtarget:$dst",
                          "? $gr $dst -> jump.1;",
                          "(brcond GuardRegs:$gr, bb:$dst)");
            os << std::endl;
            opNames_["TCEBRCOND"] = "?jump";
        }

        if (cuOpset.count("JUMP") && opNames_.count("TCEBRICOND") == 0) {
            writeInstrDef(os, "TCEBRICOND", "",
                          "GuardRegs:$gr, brtarget:$dst",
                          "! $gr $dst -> jump.1;",
                          "(brcond (not GuardRegs:$gr), bb:$dst)");
            opNames_["TCEBRICOND"] = "!jump";
        }
        os << "}" << std::endl;

        // generate brcc pseudo jumps. these are split in llvmtcebuilder into
        // separate comparison and jump.
    } else if (MachineInfo::supportsPortGuardedJumps(mach_)) {

        if  (!writePortGuardedJumpDefPair(os, "EQ", "NE")) {
            abortWithError("Required eq/ne op not found. please add to adf.");
        }
        if (!writePortGuardedJumpDefPair(os, "GT", "LE")) {
            abortWithError("Required gt/le op not found. please add to adf.");
        }
        if (!writePortGuardedJumpDefPair(os, "GTU", "LEU")) {
            abortWithError("Required gtu/leu op not found. please add to adf.");
        }

        // floating point ops are optional.
        writePortGuardedJumpDefPair(os, "EQF", "NEF", true);
        writePortGuardedJumpDefPair(os, "LEF", "GTF", true);
        writePortGuardedJumpDefPair(os, "LTF", "GEF", true);

        if (MachineInfo::supportsPortGuardedJump(mach_, false, "AND")) {
            writeInstrDef(os, "AND_JUMP", "",
                          "R32IRegs:$cmp1, R32IRegs:$cmp2, brtarget:$dst", "",
                          "(brcond (and R32IRegs:$cmp1, R32IRegs:$cmp2), bb:$dst)");
        } else {
            std::cerr << "Missing AND operation as true-guard source to jump." << std::endl;
            assert(false);
        }

        if (MachineInfo::supportsPortGuardedJump(mach_, false, "IOR")) {
            writeInstrDef(os, "IOR_JUMP", "",
                          "R32IRegs:$cmp1, R32IRegs:$cmp2, brtarget:$dst", "",
                          "(brcond (or R32IRegs:$cmp1, R32IRegs:$cmp2), bb:$dst)");
        } else {
            std::cerr << "Missing IOR operation as true-guard source to jump." << std::endl;
            assert(false);
        }

        os << std::endl << "let isTerminator = 1, isBranch = 1 in {" << std::endl;

        // dummy br(i)cond ops
        writeInstrDef(os, "TCEBRCOND", "",
                      "GuardRegs:$gr, brtarget:$dst",
                      "? $gr $dst -> jump.1;",
                      "");
        os << std::endl;

        writeInstrDef(os, "TCEBRICOND", "",
                      "GuardRegs:$gr, brtarget:$dst",
                      "? $gr $dst -> jump.1;",
                      "");
        os << std::endl;


        os << "}" << std::endl;

        os << "def: Pat<(brcc SETLT, R32IRegs:$cmp1, R32IRegs:$cmp2, bb:$dst),"
           << " (GT_JUMP R32IRegs:$cmp2, R32IRegs:$cmp1, brtarget:$dst)>;" << std::endl;


        os << "def: Pat<(brcc SETGE, R32IRegs:$cmp1, R32IRegs:$cmp2, bb:$dst),"
           << " (LE_JUMP R32IRegs:$cmp2, R32IRegs:$cmp1, brtarget:$dst)>;" << std::endl;

        os << "def: Pat<(brcc SETULT, R32IRegs:$cmp1, R32IRegs:$cmp2, bb:$dst),"
           << " (GTU_JUMP R32IRegs:$cmp2, R32IRegs:$cmp1, brtarget:$dst)>;" << std::endl;


        os << "def: Pat<(brcc SETUGE, R32IRegs:$cmp1, R32IRegs:$cmp2, bb:$dst),"
           << " (LEU_JUMP R32IRegs:$cmp2, R32IRegs:$cmp1, brtarget:$dst)>;" << std::endl;


        opNames_["AND_JUMP"] = "and+?jump";
        opNames_["IOR_JUMP"] = "ior+?jump";

        // conditional jumps without guard.
    } else {

        os << std::endl << "let isTerminator = 1, isBranch = 1 in {" << std::endl;

        if (cuOpset.count("BNZ1")) {
            // TODO: Check if has R1 regs or not!

            writeInstrDef(os, "TCEBRCOND", "", "R32IRegs:$gr, brtarget:$dst",
                "", "(brcond R32IRegs:$gr, bb:$dst)");
            os << std::endl;
            opNames_["TCEBRCOND"] = "BNZ1";
        } else {
            if (cuOpset.count("BNZ")) {
                writeInstrDef(os, "TCEBRCOND", "", "R32IRegs:$gr, brtarget:$dst",
                              "", "(brcond R32IRegs:$gr, bb:$dst)");
                os << std::endl;
                opNames_["TCEBRCOND"] = "BNZ";
            } else {
                std::cerr << "Does not have guarded jumps or neither bnz or bnz1" << std::endl;
            }
        }

        if (cuOpset.count("BZ1")) {
            // TODO: Check if has R1 regs or not!

            writeInstrDef(os, "TCEBRICOND", "", "R32IRegs:$gr, brtarget:$dst",
                "", "(brcond (not R32IRegs:$gr), bb:$dst)");
            os << std::endl;
            opNames_["TCEBRICOND"] = "BZ1";
        } else {
            if (cuOpset.count("BZ")) {
                writeInstrDef(os, "TCEBRICOND", "", "R32IRegs:$gr, brtarget:$dst",
                              "", "(brcond (not R32IRegs:$gr), bb:$dst)");
                os << std::endl;
                opNames_["TCEBRICOND"] = "BZ";
            } else {
                std::cerr << "Does not have guarded jumps or neither bz or bz1" << std::endl;
            }
        }

        if (cuOpset.count("BEQ")) {
            writeInstrDef(os, "TCEBREQrr", "", "R32IRegs:$c1, R32IRegs:$c2, brtarget:$dst",
                          "", "(brcond (i32 (seteq R32IRegs:$c1, R32IRegs:$c2)), bb:$dst)");
            opNames_["TCEBREQrr"] = "BEQ";
            writeInstrDef(os, "TCEBREQri", "", "R32IRegs:$c1, i32imm:$c2, brtarget:$dst",
                          "", "(brcond (i32 (seteq R32IRegs:$c1, i32MoveImm:$c2)), bb:$dst)");
            opNames_["TCEBREQri"] = "BEQ";
        }

        if (cuOpset.count("BNE")) {
            writeInstrDef(os, "TCEBRNErr", "", "R32IRegs:$c1, R32IRegs:$c2, brtarget:$dst",
                          "", "(brcond (i32 (setne R32IRegs:$c1, R32IRegs:$c2)), bb:$dst)");
            opNames_["TCEBRNErr"] = "BNE";
            writeInstrDef(os, "TCEBRNEri", "", "R32IRegs:$c1, i32imm:$c2, brtarget:$dst",
                          "", "(brcond (i32 (setne R32IRegs:$c1, i32MoveImm:$c2)), bb:$dst)");
            opNames_["TCEBRNEri"] = "BNE";
        }

        if (cuOpset.count("BGT")) {
            writeInstrDef(os, "TCEBRGTrr", "", "R32IRegs:$c1, R32IRegs:$c2, brtarget:$dst",
                          "", "(brcond (i32 (setgt R32IRegs:$c1, R32IRegs:$c2)), bb:$dst)");
            opNames_["TCEBRGTrr"] = "BGT";
            writeInstrDef(os, "TCEBRGTri", "", "R32IRegs:$c1, i32imm:$c2, brtarget:$dst",
                          "", "(brcond (i32 (setgt R32IRegs:$c1, i32MoveImm:$c2)), bb:$dst)");
            opNames_["TCEBRGTri"] = "BGT";
        }

        if (cuOpset.count("BGTU")) {
            writeInstrDef(os, "TCEBRGTUrr", "", "R32IRegs:$c1, R32IRegs:$c2, brtarget:$dst",
                          "", "(brcond (i32 (setugt R32IRegs:$c1, R32IRegs:$c2)), bb:$dst)");
            opNames_["TCEBRGTUrr"] = "BGTU";
            writeInstrDef(os, "TCEBRGTUri", "", "R32IRegs:$c1, i32imm:$c2, brtarget:$dst",
                          "", "(brcond (i32 (setugt R32IRegs:$c1, i32MoveImm:$c2)), bb:$dst)");
            opNames_["TCEBRGTUri"] = "BGTU";
        }

        if (cuOpset.count("BLT")) {
            writeInstrDef(os, "TCEBRLTrr", "", "R32IRegs:$c1, R32IRegs:$c2, brtarget:$dst",
                          "", "(brcond (i32 (setlt R32IRegs:$c1, R32IRegs:$c2)), bb:$dst)");
            opNames_["TCEBRLTrr"] = "BLT";
            writeInstrDef(os, "TCEBRLTri", "", "R32IRegs:$c1, i32imm:$c2, brtarget:$dst",
                          "", "(brcond (i32 (setlt R32IRegs:$c1, i32MoveImm:$c2)), bb:$dst)");
            opNames_["TCEBRLTri"] = "BLT";
        }

        if (cuOpset.count("BLTU")) {
            writeInstrDef(os, "TCEBRLTUrr", "", "R32IRegs:$c1, R32IRegs:$c2, brtarget:$dst",
                          "", "(brcond (i32 (setult R32IRegs:$c1, R32IRegs:$c2)), bb:$dst)");
            opNames_["TCEBRLTUrr"] = "BLTU";
            writeInstrDef(os, "TCEBRLTUri", "", "R32IRegs:$c1, i32imm:$c2, brtarget:$dst",
                          "", "(brcond (i32 (setult R32IRegs:$c1, i32MoveImm:$c2)), bb:$dst)");
            opNames_["TCEBRLTUri"] = "BLTU";
        }

        if (cuOpset.count("BLE")) {
            writeInstrDef(os, "TCEBRLErr", "", "R32IRegs:$c1, R32IRegs:$c2, brtarget:$dst",
                          "", "(brcond (i32 (setle R32IRegs:$c1, R32IRegs:$c2)), bb:$dst)");
            opNames_["TCEBRLErr"] = "BLE";
            writeInstrDef(os, "TCEBRLEri", "", "R32IRegs:$c1, i32imm:$c2, brtarget:$dst",
                          "", "(brcond (i32 (setle R32IRegs:$c1, i32MoveImm:$c2)), bb:$dst)");
            opNames_["TCEBRLEri"] = "BLE";
        }

        if (cuOpset.count("BLEU")) {
            writeInstrDef(os, "TCEBRLEUrr", "", "R32IRegs:$c1, R32IRegs:$c2, brtarget:$dst",
                          "", "(brcond (i32 (setule R32IRegs:$c1, R32IRegs:$c2)), bb:$dst)");
            opNames_["TCEBRLEUrr"] = "BLEU";
            writeInstrDef(os, "TCEBRLEUri", "", "R32IRegs:$c1, i32imm:$c2, brtarget:$dst",
                          "", "(brcond (i32 (setule R32IRegs:$c1, i32MoveImm:$c2)), bb:$dst)");
            opNames_["TCEBRLEUri"] = "BLEU";
        }

        if (cuOpset.count("BGE")) {
            writeInstrDef(os, "TCEBRGErr", "", "R32IRegs:$c1, R32IRegs:$c2, brtarget:$dst",
                          "", "(brcond (i32 (setge R32IRegs:$c1, R32IRegs:$c2)), bb:$dst)");
            opNames_["TCEBRGErr"] = "BGE";
            writeInstrDef(os, "TCEBRGEri", "", "R32IRegs:$c1, i32imm:$c2, brtarget:$dst",
                          "", "(brcond (i32 (setge R32IRegs:$c1, i32MoveImm:$c2)), bb:$dst)");
            opNames_["TCEBRGEri"] = "BGE";
        }

        if (cuOpset.count("BGEU")) {
            writeInstrDef(os, "TCEBRGEUrr", "", "R32IRegs:$c1, R32IRegs:$c2, brtarget:$dst",
                          "", "(brcond (i32 (setuge R32IRegs:$c1, R32IRegs:$c2)), bb:$dst)");
            opNames_["TCEBRGEUrr"] = "BGEU";
            writeInstrDef(os, "TCEBRGEUri", "", "R32IRegs:$c1, i32imm:$c2, brtarget:$dst",
                          "", "(brcond (i32 (setuge R32IRegs:$c1, i32MoveImm:$c2)), bb:$dst)");
            opNames_["TCEBRGEUri"] = "BGEU";
        }

        os << "}" << std::endl;
    }
}



bool TDGen::writePortGuardedJumpDefPair(
    std::ostream& os, const TCEString& tceop1, const TCEString& tceop2, bool fp) {

    TCEString llvmop1 = TDGen::llvmOperationName(tceop1).upper();
    TCEString llvmop2 = TDGen::llvmOperationName(tceop2).upper();

    TCEString regclass = fp ? "R32FPRegs" : "R32IRegs";
    if (MachineInfo::supportsPortGuardedJump(mach_, false, tceop1) ||
        MachineInfo::supportsPortGuardedJump(mach_, true, tceop2)) {

        os << std::endl << "let isTerminator = 1, isBranch = 1 in {" << std::endl;

        writeInstrDef(os, tceop1+"_JUMP", "",
                      regclass + ":$cmp1, "+ regclass + ":$cmp2, brtarget:$dst", "",
                      "(brcc "+ llvmop1 +", "+ regclass + ":$cmp1, " + regclass + ":$cmp2, bb:$dst)");

        os << "}" << std::endl;
        if (MachineInfo::supportsPortGuardedJump(mach_, false, tceop1)) {
            opNames_[tceop1 + "_JUMP"] = tceop1 + "+?jump";
        } else {
            opNames_[tceop1 + "_JUMP"] = tceop2 + "+!jump";
        }
    } else {
        std::cerr << "Missing "<< tceop1 << " operation as true-guard source"
                  << " or " << tceop2 << " operation as invarted guard source to jump." << std::endl;
        return false;
    }

    if (MachineInfo::supportsPortGuardedJump(mach_, true, tceop1) ||
        MachineInfo::supportsPortGuardedJump(mach_, false, tceop2)) {

        os << std::endl << "let isTerminator = 1, isBranch = 1 in {" << std::endl;


        writeInstrDef(os, tceop2+"_JUMP", "",
                      regclass + ":$cmp1, "+ regclass + ":$cmp2, brtarget:$dst", "",
                      "(brcc "+ llvmop2 +", "+ regclass + ":$cmp1, " + regclass + ":$cmp2, bb:$dst)");

        os << "}" << std::endl;

        if (MachineInfo::supportsPortGuardedJump(mach_, false, tceop2)) {
            opNames_[tceop2 + "_JUMP"] = tceop2 + "+?jump";
        } else {
            opNames_[tceop2 + "_JUMP"] = tceop1 + "+!jump";
        }
    } else {
        std::cerr << "Missing " << tceop2 << " operation as true-guard source"
                  << " or " << tceop1 << " operation as inverted guard source to jump."
                  << std::endl;
        return false;
    }
    return true;
}

/**
 * Writes .td pattern for the call instruction(s) to the output stream.
 */
void
TDGen::writeCallDef(std::ostream& o) {
    o << "let ";

    if (!argRegNames_.empty()) {
        o << "Uses = [";
        for (unsigned i = 0; i < argRegNames_.size(); i++) {
            if (i > 0) o << ", ";
            o << argRegNames_[i];
        }    
        o << "],";
    }

    o << "hasDelaySlot = 1, isCall = 1,";
    o << "Defs = [";

    writeCallDefRegs(o);
    
    o << "] in {" << std::endl;
    o << "def CALL : InstTCE<(outs), (ins calltarget:$dst),";
    o << "\"$dst -> call.1;\", []>;" << std::endl;

    o << "def CALL_MEMrr : InstTCE<(outs), (ins MEMrr:$ptr),";
    o << "\"$ptr -> call.1;\", [(call ADDRrr:$ptr)]>;" << std::endl;
    
    o << "def CALL_MEMri : InstTCE<(outs), (ins MEMri:$ptr),";
    o << "\"$ptr -> call.1;\", [(call ADDRri:$ptr)]>;" << std::endl;
    o << "}" << std::endl;

    o << "def : Pat<(call tglobaladdr:$dst), (CALL tglobaladdr:$dst)>;"
      << std::endl;

    o << "def : Pat<(call texternalsym:$dst), (CALL texternalsym:$dst)>;"
      << std::endl;

}

/**
 * Generates required function definitions for the backend plugin.
 *
 * @param o Output stream to write the c++ code to.
 */
void
TDGen::writeBackendCode(std::ostream& o) {
    // Register & operation info table initialization

    o << "void" << std::endl
      << "GeneratedTCEPlugin::initialize() {" << std::endl;

    // operation names
    std::map<std::string, std::string>::const_iterator iter =
        opNames_.begin();

    for (; iter != opNames_.end(); iter++) {
        o << "    opNames_[TCE::" << (*iter).first
          << "] = \"" << (*iter).second
          << "\";" << std::endl;
    }

    for (iter = truePredOps_.begin(); iter != truePredOps_.end(); iter++) {
	o << "    truePredOps_[TCE::" << (*iter).first
	  << "] = TCE::" << (*iter).second
	  << ";" << std::endl;
    }

    for (iter = falsePredOps_.begin(); iter != falsePredOps_.end(); iter++) {
	o << "    falsePredOps_[TCE::" << (*iter).first
	  << "] = TCE::" << (*iter).second
	  << ";" << std::endl;
    }

    // Register names & indices
    std::map<std::string, RegInfo>::const_iterator rIter = regs_.begin();
    rIter = regs_.begin();
    for (; rIter != regs_.end(); rIter++) {
        const RegInfo& regInfo = rIter->second;
        o << "    regNames_[TCE::" << (*rIter).first
          << "] = \"" << regInfo.rf
          << "\";" << std::endl;

        o << "    regIndices_[TCE::" << (*rIter).first
          << "] = " << regInfo.idx
          << ";" << std::endl;

        TCEString ttaRegName = regInfo.rf + "." + std::to_string(regInfo.idx);
        o << "    ttallvmRegMap_[\"" << ttaRegName << "\"] = "
          << "TCE::" << (*rIter).first << ";" << std::endl;
    }

    // Supported stack access opcodes
    o << std::endl;
    for (std::string opName : supportedStackAccessOperations(mach_)) {
        o << "    validStackAccessOperations_.insert(\"" << opName << "\");"
          << std::endl;
    }

    // data address space
    const TTAMachine::Machine::FunctionUnitNavigator& nav =
        mach_.functionUnitNavigator();
    std::string asName = "";
    for (int i = 0; i < nav.count(); i++) {
        if (nav.item(i) != mach_.controlUnit() &&
            nav.item(i)->addressSpace() != NULL) {
            asName = nav.item(i)->addressSpace()->name();
        }
    }

    if (asName == "") {
        std::string msg = "Couldn't determine data address space.";
        throw InvalidData(__FILE__, __LINE__, __func__, msg);
    }
    o << "    dataASName_ = \"" << asName << "\";" << std::endl;

    o << "}" << std::endl;


    bool hasSDIV = false;
    bool hasUDIV = false;
    bool hasSREM = false;
    bool hasUREM = false;
    bool hasMUL = false;
    bool hasROTL = false;
    bool hasROTR = false;
    bool hasSXHW = false;
    bool hasSXQW = false;
    bool hasSQRTF = false;
    bool hasSHR = false;
    bool hasSHRU = false;
    bool hasSHL = false;
    bool has8bitLoads = false;
    bool has16bitLoads = false;
//    bool has32bitLoads = false; // used only for 64-bit system

    const TTAMachine::Machine::FunctionUnitNavigator fuNav =
        mach_.functionUnitNavigator();

    for (int i = 0; i < fuNav.count(); i++) {
        const TTAMachine::FunctionUnit* fu = fuNav.item(i);
        for (int o = 0; o < fu->operationCount(); o++) {
            const std::string opName =
                StringTools::stringToLower(fu->operation(o)->name());

            if (mach_.is64bit()) {
                if (opName == "div64") hasSDIV = true;
                if (opName == "divu64") hasUDIV = true;
                if (opName == "mod64") hasSREM = true;
                if (opName == "modu64") hasUREM = true;
                if (opName == "mul64") hasMUL = true;
                if (opName == "rotl64") hasROTL = true;
                if (opName == "rotr64") hasROTR = true;
                if (opName == "sxh64") hasSXHW = true;
                if (opName == "sxq64") hasSXQW = true;
                if (opName == "shr64") hasSHR = true;
                if (opName == "shru64") hasSHRU = true;
                if (opName == "shl64") hasSHL = true;
            } else {
                if (opName == "div") hasSDIV = true;
                if (opName == "divu") hasUDIV = true;
                if (opName == "mod") hasSREM = true;
                if (opName == "modu") hasUREM = true;
                if (opName == "mul") hasMUL = true;
                if (opName == "rotl") hasROTL = true;
                if (opName == "rotr") hasROTR = true;
                if (opName == "sxhw") hasSXHW = true;
                if (opName == "sxqw") hasSXQW = true;
                if (opName == "shr") hasSHR = true;
                if (opName == "shru") hasSHRU = true;
                if (opName == "shl") hasSHL = true;
            }
                if (opName == "sqrtf") hasSQRTF = true;

            if (littleEndian_) {
                if (opName == "ld16" || opName == "ldu16") {
                    has16bitLoads = true;
                } else if(opName == "ld8" || opName == "ldu8") {
                    has8bitLoads = true;
//                } else if (opName == "ld32" || opName == "ldu32") {
//                    has32bitLoads = true;
                }
            } else {
                if (opName == "ldh" || opName == "ldhu") {
                    has16bitLoads = true;
                } else if (opName == "ldq" || opName == "ldqu") {
                    has8bitLoads = true;
//                } else if (opName == "ldw") {
//                    has32bitLoads = true;
                }
           }
        }
    }

    o << "bool GeneratedTCEPlugin::hasSDIV() const { return "
      << hasSDIV << "; }" << std::endl
      << "bool GeneratedTCEPlugin::hasUDIV() const { return "
      << hasUDIV << "; }" << std::endl
      << "bool GeneratedTCEPlugin::hasSREM() const { return "
      << hasSREM << "; }" << std::endl
      << "bool GeneratedTCEPlugin::hasUREM() const { return "
      << hasUREM << "; }" << std::endl
      << "bool GeneratedTCEPlugin::hasMUL() const { return "
      << hasMUL << "; }" << std::endl
      << "bool GeneratedTCEPlugin::hasROTL() const { return "
      << hasROTL << "; }" << std::endl
      << "bool GeneratedTCEPlugin::hasROTR() const { return "
      << hasROTR << "; }" << std::endl
      << "bool GeneratedTCEPlugin::hasSXHW() const { return "
      << hasSXHW << "; }" << std::endl
      << "bool GeneratedTCEPlugin::hasSXQW() const { return "
      << hasSXQW << "; }" << std::endl
      << "bool GeneratedTCEPlugin::hasSQRTF() const { return "
      << hasSQRTF << "; }" << std::endl
      << "bool GeneratedTCEPlugin::hasSHR() const { return "
      << hasSHR << "; }" << std::endl
      << "bool GeneratedTCEPlugin::hasSHL() const { return "
      << hasSHL << "; }" << std::endl
      << "bool GeneratedTCEPlugin::hasSHRU() const { return "
      << hasSHRU << ";}" << std::endl
      << "bool GeneratedTCEPlugin::has8bitLoads() const { return "
      << has8bitLoads << ";}" << std::endl
      << "bool GeneratedTCEPlugin::has16bitLoads() const { return "
      << has16bitLoads << ";}" << std::endl;
//      << "bool GeneratedTCEPlugin::has32bitLoads() const { return "
//      << has32bitLoads << ";}" << std::endl

    o << "int GeneratedTCEPlugin::maxVectorSize() const { return "
      << maxVectorSize_ << "; }" << std::endl;

    // create dummy version here
    genTCERegisterInfo_setReservedVectorRegs(o);

    generateLoadStoreCopyGenerator(o);
    createMinMaxGenerator(o);
    createParamDRegNums(o);
    createVectorRVDRegNums(o);
    createEndiannesQuery(o);
    createConstantMaterializationQuery(o);
    writeGetPointerAdjustmentQuery(o);
    createBranchAnalysis(o);

    genTCETargetLoweringSIMD_addVectorRegisterClasses(o);
    genTCETargetLoweringSIMD_associatedVectorRegClass(o);
    genTCETargetLoweringSIMD_getSetCCResultVT(o);
    genTCEInstrInfoSIMD_copyPhysVectorReg(o);
    genGeneratedTCEPlugin_getLoadOpcode(o);
    genGeneratedTCEPlugin_getAddOpcode(o);
    genGeneratedTCEPlugin_getShlOpcode(o);
    genGeneratedTCEPlugin_getIorOpcode(o);
}

void TDGen::genTCERegisterInfo_setReservedVectorRegs(
    std::ostream& os) const {
    os << "void TCERegisterInfo::setReservedVectorRegs("
       << "llvm::BitVector& reserved) const {" << std::endl;

    std::set<TCEString> processedRegs;
    for (auto rcIt : vRegClasses_) {
        const RegisterClass& regClass = rcIt.second;

        int width = regClass.valueType().width();
        if (regClass.numberOfRegisters() > 0) {
            const TCEString& name = regClass.registerInfo(0).regName_;
            if (processedRegs.find(name) == processedRegs.end() &&
                width > maxScalarWidth_) {
                processedRegs.insert(name);
                os << "reserved.set(TCE::" << name << ");" << std::endl;
            }
        }
    }
    os << "}" << std::endl;
}

/**
 * Writes a top level TCE.td file which includes generated .td definitions.
 *
 * @param o Output stream to the file.
 */
void
TDGen::writeTopLevelTD(std::ostream& o) {
    o << "include \"Target.td\"" << std::endl;
    o << "include \"TCEItinerary.td\"" << std::endl;
    o << "include \"GenRegisterInfo.td\"" << std::endl;
    o << "include \"GenTCEInstrFormats.td\"" << std::endl;
    o << "include \"TCEInstrInfo.td\"" << std::endl;
    o << "include \"GenCallingConv.td\"" << std::endl;
    o << "def TCEInstrInfo : InstrInfo { }" << std::endl;
    o << "class Proc<string Name, SchedMachineModel Model,";
    o << "                        list<SubtargetFeature> Features>";
    o << "  : ProcessorModel<Name, Model, Features>;";
    o << std::endl;
    o << "def  : Proc<\"generic\", TCEModelV0,[]>;";
    o << std::endl;
    o << "def TCE : Target { let InstructionSet = TCEInstrInfo; }"
      << std::endl;
}

/**
 * Writes details about instruction formatting
 *
 * @param o Output stream to the file.
 */
void
TDGen::writeInstrFormats(std::ostream& o) {
    o << "//" << endl;
    o << "// TCE Instruction formats." << endl;
    o << "//" << endl;
    o << "// Only one simple format is currently available" << endl;
    o << "//" << endl;
    o << "// Automatically generated file, do not edit!" << endl;
    o << "//" << endl;
    o << "" << endl;
    o << "class InstTCE<dag outOps, dag inOps, string asmstr," << endl;
    o << "              list<dag> pattern = []," << endl;
    o << "              InstrItinClass itin = IT_FU>" << endl;
    o << "             : Instruction {" << endl;
    o << "    let Namespace = \"TCE\";" << endl;
    o << "    dag InOperandList = inOps;" << endl;
    o << "    dag OutOperandList = outOps;" << endl;
    o << "    let AsmString = asmstr;" << endl;
    o << "    let Pattern = pattern;" << endl;
    o << "    let Itinerary = itin;" << endl;
    o << "}" << endl;
    o << "" << endl;
    o << "class Pseudo<dag outOps, dag inOps," << endl;
    o << "             string asmstr, list<dag> pattern>" << endl;
    o << "   : InstTCE<outOps, inOps, asmstr, pattern>;" << endl;
}

/*
 * Operand type characters defined by this TD generator:
 * a = Boolean vector
 * b = Boolean/predicate register
 * c
 * d
 * e = Float vector register
 * f = Float32 register
 * g = Half float vector register
 * h = Float16 register
 * i = Immediate integer
 * j = immediate boolean
 * k = immediate float?
 * l = immediate float16?
 * p
 * q
 * r = integer Register
 * s
 * t
 * u
 * y
 * z
 */
/**
 * Writes scalar operation definition(s).
 *
 * @param o Output stream to write the definition to.
 * @param op Operation to write definition for.
 * @param skipPattern True, if skip pattern generation.
 */
void
TDGen::writeOperationDefs(
    std::ostream& o,
    Operation& op,
    bool skipPattern) {

    std::string attrs;

    // These white listed operations have mayLoad/mayStore flag
    // inferred from the llvm pattern and declaring it
    // explicitly will display warning in tablegen.
    if (op.name() != "LDQ" && op.name() != "LDQU" &&
        op.name() != "LDH" && op.name() != "LDHU" &&
        op.name() != "LDW" && op.name() != "LDD" &&
        op.name() != "STQ" && op.name() != "STH" &&
        op.name() != "STW" && op.name() != "STD" &&
        op.name() != "ALDQ" && op.name() != "ALDQU" &&
        op.name() != "ALDH" && op.name() != "ALDHU" &&
        op.name() != "ALDW" && op.name() != "ALDD" &&
        op.name() != "ASTQ" && op.name() != "ASTH" &&
        op.name() != "ASTW" && op.name() != "ASTD" &&

        op.name() != "LD8" && op.name() != "LDU8" &&
        op.name() != "LD16" && op.name() != "LDU16" &&
        op.name() != "LD32" && op.name() != "LDU32" &&
        op.name() != "LD64" &&
        op.name() != "ST8" && op.name() != "ST16" &&
        op.name() != "ST32" && op.name() != "ST64" &&
        op.name() != "ALD8" && op.name() != "ALDU8" &&
        op.name() != "ALD16" && op.name() != "ALDU16" &&
        op.name() != "ALD32" && op.name() != "ALDU32" &&
        op.name() != "ALD64" &&
        op.name() != "AST8" && op.name() != "AST16" &&
        op.name() != "AST32" && op.name() != "AST64" &&
        op.name() != "CAS") {

        if (op.readsMemory()) attrs += " mayLoad = 1";
        if (op.readsMemory() && op.writesMemory()) attrs += ", ";
        if (op.writesMemory()) attrs += " mayStore = 1";
    }

    // no bool outs for some operatios
    if (op.name() == "CFI" || op.name() == "CFIU") {
        writeOperationDef(o, op, "rf", attrs, skipPattern);
        return;
    }

    if (op.name() == "CDL" || op.name() == "CDLU") {
        writeOperationDef(o, op, "sd", attrs, skipPattern);
        return;
    }

    if (op.name() == "CLD" || op.name() == "CLDU") {
        writeOperationDef(o, op, "ds", attrs, skipPattern);
        return;
    }

        // no bool outs for some operatios
    if (op.name() == "CFD") {
        writeOperationDef(o, op, "df", attrs, skipPattern);
        return;
    }

    // no bool outs for some operatios
    if (op.name() == "CDF") {
        writeOperationDef(o, op, "fd", attrs, skipPattern);
        return;
    }

    // rotations are allways n x n -> n bits.
    if (op.name() == "ROTL" || op.name() == "ROTR" ||
        op.name() == "SHL" || op.name() == "SHR" || op.name() == "SHRU") {
        writeOperationDefs(o, op, "rrr", attrs, skipPattern);
        return;
    }

    if (op.name() == "SXHW" || op.name() == "SXQW") {
        writeOperationDef(o, op, "rr", attrs, skipPattern);
        return;
    }

    if (op.name() == "SXW64" || op.name() == "ZXW64") {
        writeOperationDef(o, op, "ss", attrs, skipPattern);
        return;
    }

    // these can have 1-bit inputs
    // TODO: this is lacking some 64-bit ops??
    if (op.name() == "XOR" || op.name() == "IOR" || op.name() == "AND" ||
        op.name() == "ANDN" || op.name() == "ADD" || op.name() == "SUB" ||
        op.name() == "XOR64" || op.name() == "IOR64" || op.name() == "AND64") {
        writeOperationDefs(o, op, "bbb", attrs, skipPattern);
    }

    if (op.name() == "SELECT") {
        if (!hasConditionalMoves_) {
            writeOperationDef(o, op, "rrrb", attrs, skipPattern);
            writeOperationDef(o, op, "riib", attrs, skipPattern);
            writeOperationDef(o, op, "rrib", attrs, skipPattern);
            writeOperationDef(o, op, "rirb", attrs, skipPattern);
            writeOperationDef(o, op, "bbbb", attrs, skipPattern);
            writeOperationDef(o, op, "bjjb", attrs, skipPattern);
            writeOperationDef(o, op, "bjbb", attrs, skipPattern);
            writeOperationDef(o, op, "bbjb", attrs, skipPattern);
            // TODO: what about floating-point values?
            writeOperationDef(o, op, "fffb", attrs, skipPattern);
            writeOperationDef(o, op, "hhhb", attrs, skipPattern);

            // ADFs with boolan values in GPRs
            writeOperationDef(o, op, "rrrr", attrs, skipPattern);
            writeOperationDef(o, op, "riir", attrs, skipPattern);
            writeOperationDef(o, op, "rrir", attrs, skipPattern);
            writeOperationDef(o, op, "rirr", attrs, skipPattern);
            writeOperationDef(o, op, "bbbr", attrs, skipPattern);
            writeOperationDef(o, op, "bjjr", attrs, skipPattern);
            writeOperationDef(o, op, "bjbr", attrs, skipPattern);
            writeOperationDef(o, op, "bbjr", attrs, skipPattern);
            // TODO: what about floating-point values?
            writeOperationDef(o, op, "fffr", attrs, skipPattern);
            writeOperationDef(o, op, "hhhr", attrs, skipPattern);

            hasSelect_ = true;
            return;
        } else {
            std::cerr << "The target architecture has both"
                      << "conditional moves and select instruction."
                      << "Ignoring select and using conditional moves instead"
                      <<  std::endl;
            return;
        }
    }

    // store likes this. store immediate to immediate address
    if (op.numberOfInputs() == 2 && op.numberOfOutputs() == 0) {
        Operand& operand1 = op.operand(1);
        Operand& operand2 = op.operand(2);
        /// @note This todo marking is from TDGen.
        // TODO: add an else branch here for float immediates
        if ((operand1.type() == Operand::UINT_WORD ||
             operand1.type() == Operand::SINT_WORD ||
             operand1.type() == Operand::RAW_DATA) &&
           (operand2.type() == Operand::UINT_WORD || 
            operand2.type() == Operand::SINT_WORD ||
            operand2.type() == Operand::RAW_DATA)) {

            if (mach_.is64bit()) {
                writeOperationDef(o, op, "aa", attrs, skipPattern);
            } else {
                writeOperationDef(o, op, "ii", attrs, skipPattern);
            }
        }
    }

    std::string operandTypes = createDefaultOperandTypeString(op);
    // this the ordinary def

    // then try with immediates.
    /// @note This todo marking is from TDGen.
    // TODO: this should be 2^n loop instead of n loop, to get
    // all permutations.

    writeOperationDefs(o, op, operandTypes, attrs, skipPattern);

    // then with boolean outs, and vector versions.
    if (op.numberOfOutputs() == 1 && !op.readsMemory()) {
        Operand& outOperand = op.operand(op.numberOfInputs()+1);
        if (outOperand.type() == Operand::UINT_WORD || 
            outOperand.type() == Operand::SINT_WORD ||
            outOperand.type() == Operand::ULONG_WORD ||
            outOperand.type() == Operand::SLONG_WORD) {

            // 32/64 to 1-bit operations
            operandTypes[0] = OT_REG_BOOL;
            writeOperationDefs(o, op, operandTypes, attrs, skipPattern);
        } 

        // create vector versions.
    }
}

char regOperandCharToImmOperandChar(char c) {
    switch(c) {
    case TDGen::OT_REG_LONG:
        return TDGen::OT_IMM_LONG;
    case TDGen::OT_REG_INT:
        return TDGen::OT_IMM_INT;
    case TDGen::OT_REG_BOOL:
        return TDGen::OT_IMM_BOOL;
    case TDGen::OT_REG_FP:
        return TDGen::OT_IMM_FP;
    case TDGen::OT_REG_HFP:
        return TDGen::OT_IMM_HFP;
    default:
        return 0;
    }
}

/**
 * Writes operation defs for single operation, with different immediate params.
 *
 * @param o Output stream to write the definition to.
 * @param op Operation to write definition for.
 * @param operandTypes value types of operands.
 */
void
TDGen::writeOperationDefs(
    std::ostream& o, Operation& op, const std::string& operandTypes,
    const std::string& attrs, bool skipPattern, std::string backendPrefix) {

    // first without imms.
    writeOperationDef(o, op, operandTypes, attrs, skipPattern, backendPrefix);

    for (int i = 0; i < op.numberOfInputs(); i++) {
        bool canSwap = false;

        // those ops SHOULD be can-swap but are not. kludge-fix.
        // consider making them can-swap.
        if (op.name() == "ALDQ" || op.name() == "ALDQU" ||
            op.name() == "ALDH" || op.name() == "ALDHU" ||
            op.name() == "ALDW" || op.name() == "ALDD" ||
            op.name() == "ASTQ" || op.name() == "ASTH" ||
            op.name() == "ASTW" || op.name() == "ASTD" ||

            op.name() == "ALD8" || op.name() == "ALDU8" ||
            op.name() == "ALD16"|| op.name() == "ALDU16" ||
            op.name() == "ALD32"|| op.name() == "ALD64" ||
            op.name() == "AST8" || op.name() == "AST16" ||
            op.name() == "AST32"|| op.name() == "AST64") {
            canSwap = true;
        }

        for (int j = i+1 ; j < op.numberOfInputs(); j++) {
            if (op.canSwap(i+1, j+1)) {
                canSwap = true;
                break;
            }
        }
        // setcc of LLVM is not commutative and get confused if we don't generate
        // a pattern with immediate elsewhere than the last operand        
        canSwap = canSwap && !(op.name().upper() == "EQ" || op.name().upper() == "NE");

        if (!canSwap) {
            std::string opTypes = operandTypes;
            char& c = opTypes[i + op.numberOfOutputs()];
            c = regOperandCharToImmOperandChar(c);
            if (c) {
                writeOperationDef(o, op, opTypes, attrs, skipPattern, backendPrefix);
            }
        }
    }
}


/**
 * Writes special store patterns for i1 types.
 */
void
TDGen::writeBooleanStorePatterns(std::ostream& os) {
    std::string storeOp("");
    auto storeOps = littleEndian_
                        ? std::vector<std::string>{"ST8ri", "ST8rr"}
                        : std::vector<std::string>{"STQri", "STQrr"};
    for (auto op : storeOps) {
        if (opNames_.count(op)) {
            storeOp = op;
            break;
        }
    }
    if (storeOp.empty()) return;

    // TODO: is this 32 correct for 64-bit ADFs?
    auto storeConstBit = [=](const std::string& bitVal) -> std::string {
        return std::string("(") + storeOp + " ADDRrr:$addr, " +
               (storeOp.back() == 'i' ? "(i32 " : "(MOVI32ri ") + bitVal +
               "))";
    };

    // Patterns to avoid i1 "true" values to be emitted as -1, which breaks
    // on, for example, BZ and BNZ if the condition is loaded later, inverted
    // using XOR and finally the result is bypassed: this results the -1 to be
    // converted to -2, which is interpreted still as true.
    os << "def : Pat<(store (i1 -1), ADDRrr:$addr), " << std::endl
       << "          " << storeConstBit("1") << ">;" << std::endl;
    os << "def : Pat<(store (i1 1), ADDRrr:$addr), " << std::endl
       << "          " << storeConstBit("1") << ">;" << std::endl;
    os << "def : Pat<(store (i1 0), ADDRrr:$addr), " << std::endl
       << "          " << storeConstBit("0") << ">;" << std::endl;
}

/**
 * Writes single immediate operand definitions to the stream.
 *
 * @param o The output stream.
 * @param defName The name of the immediate operand definition.
 * @param operandType The target type (i.e "i32").
 * @param predicate The predicate expression without return statement or ';'
 *                  at the end.
 */
void
TDGen::writeImmediateDef(
    std::ostream& o,
    const std::string& defName,
    const std::string& operandType,
    const std::string& predicate) {

    o << "def " << defName << " : Operand<" << operandType
      << "> , ImmLeaf<" << operandType << ", [{" << std::endl
      << "    return " << predicate << ";}]>;" << std::endl;
}

/**
 * Writes LLVM instruction definition in TCE format to the stream.
 *
 */
void
TDGen::writeInstrDef(
        std::ostream& o,
        const std::string& instrDefName,
        const std::string& outs,
        const std::string& ins,
        const std::string& asmString,
        const std::string& pattern) {

    o << "def " << instrDefName << " : InstTCE<" << std::endl
      << "    (outs " << outs << ")," << std::endl
      << "    (ins " << ins << ")," << std::endl
      << "    \"" << asmString << "\", " << std::endl
      << "    [" << pattern << "]>;" << std::endl;
}

/**
 * Writes a single operation def for single operation.
 *
 * @param o Output stream to write the definition to.
 * @param op Operation to write definition for.
 * @param operandTypes value types of operands.
 */
void 
TDGen::writeOperationDef(
    std::ostream& o,
    Operation& op, const std::string& operandTypes, const std::string& attrs,
    bool skipPattern, std::string backendPrefix) {
    assert (operandTypes.size() > 0);

    // Skip definition if the operation has immediate to operand which the
    // machine can not transport short immediates to.
    if (!areImmediateOperandsLegal(op, operandTypes)) {
        if (Application::verboseLevel() >
        Application::VERBOSE_LEVEL_DEFAULT) {
            Application::logStream()
                << "Skipped writing operation pattern for: "
                << op.name() + operandTypes
                << ": Can not have immediate operand."
                << std::endl;
        }
        return;
    }
    
    char operandChar = mach_.is64bit() ? OT_REG_LONG : OT_REG_INT;

    std::string outputs, inputs, asmstr, pattern;
    outputs = "(outs" + patOutputs(op, operandTypes) + ")";
    inputs = "(ins " + patInputs(op, operandTypes) + ")";
    std::string predicatedInputs = 
	"(ins GuardRegs:$pred, " +patInputs(op, operandTypes)+ ")";

    asmstr = "\"\"";
    
    if (!skipPattern) {
        if (llvmOperationPattern(op, operandChar) != "" ||
            op.dagCount() == 0) {
            OperationDAG* trivial = createTrivialDAG(op);
            pattern = operationPattern(op, *trivial, operandTypes);
            delete trivial;
        } else {
            auto* dagToUse = getMatchableOperationDAG(op);
            if (dagToUse)
                pattern = operationPattern(op, *dagToUse, operandTypes);
        }
    }
    
    if (attrs != "") {
        o << "let" << attrs << " in { " << std::endl;
    }
    
    std::string opcEnum = 
        StringTools::stringToUpper(op.name()) + operandTypes;
    
    o << "def " << opcEnum << " : " 
      << "InstTCE<"
      << outputs << ", "
      << inputs << ", "
      << asmstr << ", " ;
    if(operandTypes[0] == 'b') { //instruction can be used for comparison
	o  << "[" << pattern << "]>"
	   << std::endl
	   << "{ let isCompare = 1;}" << std::endl;
    } else {
	o  << "[" << pattern << "]>;"
	   << std::endl;
    }

    bool opCanBePredicated = canBePredicated(op, operandTypes);

    if (opCanBePredicated) {
        // write predicated versions
        o << "def PRED_TRUE_" << opcEnum << " : "
          << "InstTCE<"
          << outputs << ", "
          << predicatedInputs << ", "
          << asmstr << ", "
          << "[]>;"
          << std::endl;
        // write predicated versions
        o << "def PRED_FALSE_" << opcEnum << " : "
          << "InstTCE<"
          << outputs << ", "
          << predicatedInputs << ", "
          << asmstr << ", "
          << "[]>;"
          << std::endl;
    }

    if (attrs != "") {
        o << "}" << std::endl;
    }        
    opNames_[opcEnum] = backendPrefix + op.name();

    if (opCanBePredicated) {
        opNames_["PRED_TRUE_" + opcEnum] = "?" + backendPrefix + op.name();
        opNames_["PRED_FALSE_" + opcEnum] = "!" + backendPrefix + op.name();
        truePredOps_[opcEnum] = "PRED_TRUE_" + opcEnum;
        falsePredOps_[opcEnum] = "PRED_FALSE_" + opcEnum;
    }
}

/**
 *
 * Returns corresponding character for given operand type.
 *
 * @param operand Operand under inspection.
 * @return Corresponding character of operand type.
 */
char 
TDGen::operandChar(Operand& operand) {
    if (operand.isVector()) {
        if (operand.type() == Operand::BOOL) {
            return OT_VREG_BOOL;
        } else if (operand.type() == Operand::HALF_FLOAT_WORD) {
            return OT_VREG_HFP;
        } else if (operand.type() == Operand::FLOAT_WORD) {
            return OT_VREG_FP;
        } else {
            if (operand.elementWidth() == 1) {
                return OT_VREG_BOOL;
            } else if (operand.elementWidth() == 8) {
                return OT_VREG_INT8;
            } else if (operand.elementWidth() == 16) {
                return OT_VREG_INT16;
            } else {
                return OT_VREG_INT32;
            }
        }
    }

    if (operand.type() == Operand::BOOL) {
        return OT_REG_BOOL;
    } else if (operand.type() == Operand::HALF_FLOAT_WORD) {
        return OT_REG_HFP;
    } else if (operand.type() == Operand::ULONG_WORD ||
               operand.type() == Operand::SLONG_WORD ||
               operand.type() == Operand::RAW_DATA) {
        return mach_.is64bit() ? OT_REG_LONG : OT_REG_INT;
    } else if (operand.type() != Operand::UINT_WORD &&
               operand.type() != Operand::SINT_WORD) {
        return OT_REG_FP;
    } else {
        return OT_REG_INT;
    }
}

/**
 * Writes operation emulation pattern in .td format to an output stream.
 *
 * @param o Output stream to write the definition to.
 * @param op Emulated operation.
 * @param dag Emulation pattern.
 */
void
TDGen::writeEmulationPattern(
    std::ostream& o,
    const Operation& op,
    const OperationDAG& dag) {

    char operandCh = mach_.is64bit() ? OT_REG_LONG : OT_REG_INT;

    const OperationDAGNode* res = *(dag.endNodes().begin());
    if (dag.endNodes().empty()) {
        std::cerr << "end nodes of dag for operation: " << op.name()
                  << " is empty!" << std::endl;
        assert(false);
    }

    const OperationNode* opNode = dynamic_cast<const OperationNode*>(res);
    if (opNode == NULL) {
        assert(dag.inDegree(*res) ==  1);
        const OperationDAGEdge& edge = dag.inEdge(*res, 0);
        res = dynamic_cast<OperationNode*>(&dag.tailNode(edge));
        assert(res != NULL);
    }

    int inputCount = op.numberOfInputs();

    bool ok = true;
    std::string llvmPat = llvmOperationPattern(op, operandCh);
    if (llvmPat == "") {
        std::cerr << "unknown op: " << op.name() << std::endl;
    }
    assert(llvmPat != "" && "Unknown operation to emulate.");

    boost::format match1(llvmPat);

    int outputs = op.numberOfOutputs();

    std::string operandTypes;
    for (int i = 0; i < outputs; i++) {
        operandTypes += operandChar(op.operand(i+inputCount + 1));
    }

    for (int i = 0; i < op.numberOfInputs(); i++) {
        char inputType = operandChar(op.operand(i+1));

        std::string immDef = immediateOperandNameForEmulatedOperation(
            dag, op.operand(i + 1));

        operandTypes += inputType;
        match1 % operandToString(op.operand(i + 1), false, inputType, immDef);
    }
    if (ok) {
        o << "def : Pat<(" << match1.str() << "), "
            << dagNodeToString(op, dag, *res, true, operandTypes)
            << ">;" << std::endl;

        // need to generate emulation patterns for boolean out
        // for these comparison operations.
        if (op.name() == "LTF" || op.name() == "LTUF" ||
            op.name() == "EQF" || op.name() == "EQUF" ||
            op.name() == "GEF" || op.name() == "GEUF" ||
            op.name() == "LEF" || op.name() == "LEUF" ||
            op.name() == "GTF" || op.name() == "GTUF" ||
            op.name() == "NEF" || op.name() == "NEUF" ||
            op.name() == "EQ" || op.name() == "NE" ||
            op.name() == "EQ64" || op.name() == "NE64" ||
            op.name() == "GE" ||op.name() == "GEU" ||
            op.name() == "GE64" ||op.name() == "GEU64" ||
            op.name() == "GT" || op.name() == "GTU" ||
            op.name() == "GT64" || op.name() == "GTU64" ||
            op.name() == "LE" || op.name() == "LEU" ||
            op.name() == "LE64" || op.name() == "LEU64" ||
            op.name() == "LT" || op.name() == "LTU" ||
            op.name() == "LT64" || op.name() == "LTU64" ||
            op.name() == "LTD" || op.name() == "LTUD" ||
            op.name() == "EQD" || op.name() == "EQUD" ||
            op.name() == "GED" || op.name() == "GEUD" ||
            op.name() == "LED" || op.name() == "LEUD" ||
            op.name() == "GTD" || op.name() == "GTUD" ||
            op.name() == "NED" || op.name() == "NEUD" ||
            op.name() == "ORDD" || op.name() == "UORDD" ||
            op.name() == "ORDF" || op.name() == "UORDF") {
            std::string boolOperandTypes = operandTypes;
            boolOperandTypes[0] = 'b';
            o << "def : Pat<(" << match1.str() << "), "
              << dagNodeToString(op, dag, *res, true, boolOperandTypes)
              << ">;" << std::endl;
        }
    }
}
/**
 * Returns LLVM operation pattern for given OSAL operation.
 *
 * @param op Operation for which the LLVM pattern should be returned.
 * @return Operation pattern in llvm.
 */
TCEString
TDGen::llvmOperationPattern(const Operation& op, char /*operandType*/) const {
    TCEString opName = StringTools::stringToLower(op.name());

    if (opName == "add") return "add %1%, %2%";
    if (opName == "add64") return "add %1%, %2%";
    if (opName == "sub") return "sub %1%, %2%";
    if (opName == "sub64") return "sub %1%, %2%";
    if (opName == "mul") return "mul %1%, %2%";
    if (opName == "mul64") return "mul %1%, %2%";
    if (opName == "div") return "sdiv %1%, %2%";
    if (opName == "divu") return "udiv %1%, %2%";
    if (opName == "div64") return "sdiv %1%, %2%";
    if (opName == "divu64") return "udiv %1%, %2%";
    if (opName == "mod") return "srem %1%, %2%";
    if (opName == "modu") return "urem %1%, %2%";
    if (opName == "mod64") return "srem %1%, %2%";
    if (opName == "modu64") return "urem %1%, %2%";

    if (opName == "shl") return "shl %1%, %2%";
    if (opName == "shr") return "sra %1%, %2%";
    if (opName == "shru") return "srl %1%, %2%";
    if (opName == "rotl") return "rotl %1%, %2%";
    if (opName == "rotr") return "rotr %1%, %2%";


    if (opName == "shl64") return "shl %1%, %2%";
    if (opName == "shr64") return "sra %1%, %2%";
    if (opName == "shru64") return "srl %1%, %2%";
    if (opName == "rotl64") return "rotl %1%, %2%";
    if (opName == "rotr64") return "rotr %1%, %2%";

    if (opName == "and") return "and %1%, %2%";
    if (opName == "ior") return "or %1%, %2%";
    if (opName == "xor") return "xor %1%, %2%";

    if (opName == "and64") return "and %1%, %2%";
    if (opName == "ior64") return "or %1%, %2%";
    if (opName == "xor64") return "xor %1%, %2%";

    if (opName == "eq") return "seteq %1%, %2%";
    if (opName == "eq64") return "seteq %1%, %2%";
    if (opName == "ne") return "setne %1%, %2%";
    if (opName == "ne64") return "setne %1%, %2%";
    if (opName == "lt") return "setlt %1%, %2%";
    if (opName == "lt64") return "setlt %1%, %2%";
    if (opName == "le") return "setle %1%, %2%";
    if (opName == "le64") return "setle %1%, %2%";
    if (opName == "gt") return "setgt %1%, %2%";
    if (opName == "gt64") return "setgt %1%, %2%";
    if (opName == "ge") return "setge %1%, %2%";
    if (opName == "ge64") return "setge %1%, %2%";
    if (opName == "ltu") return "setult %1%, %2%";
    if (opName == "ltu64") return "setult %1%, %2%";
    if (opName == "leu") return "setule %1%, %2%";
    if (opName == "leu64") return "setule %1%, %2%";
    if (opName == "gtu") return "setugt %1%, %2%";
    if (opName == "gtu64") return "setugt %1%, %2%";
    if (opName == "geu") return "setuge %1%, %2%";
    if (opName == "geu64") return "setuge %1%, %2%";

    if (opName == "eqf" || opName == "eqd") return "setoeq %1%, %2%";
    if (opName == "nef" || opName == "ned") return "setone %1%, %2%";
    if (opName == "ltf" || opName == "ltd") return "setolt %1%, %2%";
    if (opName == "lef" || opName == "led") return "setole %1%, %2%";
    if (opName == "gtf" || opName == "gtd") return "setogt %1%, %2%";
    if (opName == "gef" || opName == "ged") return "setoge %1%, %2%";

    if (opName == "equf" || opName == "equd") return "setueq %1%, %2%";
    if (opName == "neuf" || opName == "neud") return "setune %1%, %2%";
    if (opName == "ltuf" || opName == "ltud") return "setult %1%, %2%";
    if (opName == "leuf" || opName == "leud") return "setule %1%, %2%";
    if (opName == "gtuf" || opName == "gtud") return "setugt %1%, %2%";
    if (opName == "geuf" || opName == "geud") return "setuge %1%, %2%";

    if (opName == "ordf" || opName == "ordd") return "seto %1%, %2%";
    if (opName == "uordf"|| opName == "uordd")return "setuo %1%, %2%";

    if (opName == "addf" || opName == "addd") return "fadd %1%, %2%";
    if (opName == "subf" || opName == "subd") return "fsub %1%, %2%";
    if (opName == "mulf" || opName == "muld") return "fmul %1%, %2%";
    if (opName == "divf" || opName == "divd") return "fdiv %1%, %2%";
    if (opName == "absf" || opName == "absd") return "fabs %1%";
    if (opName == "negf" || opName == "negd") return "fneg %1%";
    if (opName == "sqrtf"|| opName == "sqrtd")return "fsqrt %1%";

    if (opName == "cif") return "sint_to_fp %1%";
    if (opName == "cfi") return "fp_to_sint %1%";
    if (opName == "cifu") return "uint_to_fp %1%";
    if (opName == "cfiu") return "fp_to_uint %1%";

    if (opName == "cld") return "sint_to_fp %1%";
    if (opName == "cdl") return "fp_to_sint %1%";
    if (opName == "cldu") return "uint_to_fp %1%";
    if (opName == "cdlu") return "fp_to_uint %1%";

    if (opName == "cfh" || opName == "cdf") return "fpround %1%";
    if (opName == "chf") return "f32 (fpextend %1%)";
    if (opName == "cfd") return "f64 (fpextend %1%)";

    if (opName == "cih") return "sint_to_fp %1%";
    if (opName == "chi") return "i32 (fp_to_sint %1%)";
    if (opName == "cihu") return "uint_to_fp %1%";
    if (opName == "chiu") return "i32 (fp_to_uint %1%)";

    if (opName == "neuh") return "setune %1%, %2%";
    if (opName == "eqh") return "setoeq %1%, %2%";
    if (opName == "neh") return "setone %1%, %2%";
    if (opName == "lth") return "setolt %1%, %2%";
    if (opName == "leh") return "setole %1%, %2%";
    if (opName == "gth") return "setogt %1%, %2%";
    if (opName == "geh") return "setoge %1%, %2%";

    if (opName == "ordh") return "seto %1%, %2%";
    if (opName == "uordh") return "setuo %1%, %2%";

    if (opName == "addh") return "fadd %1%, %2%";
    if (opName == "subh") return "fsub %1%, %2%";
    if (opName == "mulh") return "fmul %1%, %2%";
    if (opName == "divh") return "fdiv %1%, %2%";
    if (opName == "absh") return "fabs %1%";
    if (opName == "negh") return "fneg %1%";
    if (opName == "sqrth") return "fsqrt %1%";

    if (opName == "cih") return "sint_to_fp %1%";
    if (opName == "chi") return "fp_to_sint %1%";
    if (opName == "cihu") return "uint_to_fp %1%";
    if (opName == "chiu") return "fp_to_uint %1%";

    if (opName == "csh") return "sint_to_fp %1%";
    if (opName == "cshu") return "uint_to_fp %1%";
    if (opName == "chs") return "fp_to_sint %1%";
    if (opName == "chsu") return "fp_to_uint %1%";

    if (littleEndian_) {
        if (opName == "ld8") return "sextloadi8 %1%";
        if (opName == "ldu8") return "zextloadi8 %1%";
        if (opName == "ld16") return "sextloadi16 %1%";
        if (opName == "ldu16") return "zextloadi16 %1%";
        if (mach_.is64bit()) {
            if (opName == "ld32") return "sextloadi32 %1%";
            if (opName == "ldu32") return "zextloadi32 %1%";
        } else {
            if (opName == "ld32" || opName == "ldu32") return "load %1%";
        }
        if (opName == "ld64") return "load %1%";
        //if (opName == "ldd") return "load";
        
        if (opName == "st8") return "truncstorei8 %2%, %1%";
        if (opName == "st16") return "truncstorei16 %2%, %1%";
        if (mach_.is64bit()) {
            if (opName == "st32") return "truncstorei32 %2%, %1%";
        } else {
            if (opName == "st32") return "store %2%, %1%";
        }
        if (opName == "st64") return "store %2%, %1%";
    } else {
        if (opName == "ldq") return "sextloadi8 %1%";
        if (opName == "ldqu") return "zextloadi8 %1%";
        if (opName == "ldh") return "sextloadi16 %1%";
        if (opName == "ldhu") return "zextloadi16 %1%";
        if (opName == "ldw") return "load %1%";
        //if (opName == "ldd") return "load";
        
        if (opName == "stq") return "truncstorei8 %2%, %1%";
        if (opName == "sth") return "truncstorei16 %2%, %1%";
        if (opName == "stw") return "store %2%, %1%";
        //if (opName == "std") return "load";
    } 

    if (opName == "sxw64") {
        return "sext_inreg %1%, i32";
    }

    if (opName == "sxb64") {
        return "sext_inreg %1%, i1";
    }

    if (opName == "sxq64") {
        return "sext_inreg %1%, i8";
    }
    if (opName == "sxh64") {
        return "sext_inreg %1%, i16";
    }

    if (opName == "sxhw") {
      return "sext_inreg %1%, i16";
    }
    if (opName == "sxqw") {
      return "sext_inreg %1%, i8";
    }

    if (opName == "sxw")
        return mach_.is64bit() ? "sext_inreg %1%, i64": "sext_inreg %1%, i32";

    if (opName == "sxbw") return "sext_inreg %1%, i1"; 

    if (opName == "truncwh" || opName == "truncwb" || opName == "trunchb") 
        return "trunc %1%";

    if (opName == "neg") return "ineg %1%";
    if (opName == "not") return "not %1%";

    if (opName == "cas") return "atomic_cmp_swap_32 %1%, %2%, %3%";
    if (opName == "select") return "select %3%, %1%, %2%";

    // Unknown operation name.
    return "";
}

/**
 * Returns llvm operation name for the given OSAL operation name, if any.
 */

TCEString
TDGen::llvmOperationName(const TCEString& operationName) const {
    
    TCEString opName = StringTools::stringToLower(operationName);

    if (opName == "add") return "add";
    if (opName == "sub") return "sub";
    if (opName == "mul") return "mul";
    if (opName == "div") return "sdiv";
    if (opName == "divu") return "udiv";
    if (opName == "mod") return "srem";
    if (opName == "modu") return "urem";

    if (opName == "add64") return "add";
    if (opName == "sub64") return "sub";
    if (opName == "mul64") return "mul";
    if (opName == "div64") return "sdiv";
    if (opName == "divu64") return "udiv";
    if (opName == "mod64") return "srem";
    if (opName == "modu64") return "urem";

    if (opName == "shl") return "shl";
    if (opName == "shr") return "sra";
    if (opName == "shru") return "srl";
    if (opName == "rotl") return "rotl";
    if (opName == "rotr") return "rotr";

    if (opName == "shl64") return "shl";
    if (opName == "shr64") return "sra";
    if (opName == "shru64") return "srl";
    if (opName == "rotl64") return "rotl";
    if (opName == "rotr64") return "rotr";

    if (opName == "and") return "and";
    if (opName == "ior") return "or";
    if (opName == "xor") return "xor";

    if (opName == "and64") return "and";
    if (opName == "ior64") return "or";
    if (opName == "xor64") return "xor";

    if (opName == "eq") return "seteq";
    if (opName == "eq64") return "seteq";
    if (opName == "ne") return "setne";
    if (opName == "ne64") return "setne";
    if (opName == "lt") return "setlt";
    if (opName == "lt64") return "setlt";
    if (opName == "le") return "setle";
    if (opName == "le64") return "setle";
    if (opName == "gt") return "setgt";
    if (opName == "gt64") return "setgt";
    if (opName == "ge") return "setge";
    if (opName == "ltu") return "setult";
    if (opName == "ltu64") return "setult";
    if (opName == "leu") return "setule";
    if (opName == "leu64") return "setule";
    if (opName == "gtu") return "setugt";
    if (opName == "gtu64") return "setugt";
    if (opName == "geu") return "setuge";
    if (opName == "geu64") return "setuge";

    if (opName == "eqf" || opName == "eqd") return "setoeq";
    if (opName == "nef" || opName == "ned") return "setone";
    if (opName == "ltf" || opName == "ltd") return "setolt";
    if (opName == "lef" || opName == "led") return "setole";
    if (opName == "gtf" || opName == "gtd") return "setogt";
    if (opName == "gef" || opName == "ged") return "setoge";

    if (opName == "equf" || opName == "equd") return "setueq";
    if (opName == "neuf" || opName == "neud") return "setune";
    if (opName == "ltuf" || opName == "ltud") return "setult";
    if (opName == "leuf" || opName == "leud") return "setule";
    if (opName == "gtuf" || opName == "gtud") return "setugt";
    if (opName == "geuf" || opName == "geud") return "setuge";

    if (opName == "ordf" || opName =="ordd") return "seto";
    if (opName == "uordf" || opName == "uordd") return "setuo";

    if (opName == "addf") return "fadd";
    if (opName == "subf") return "fsub";
    if (opName == "mulf") return "fmul";
    if (opName == "divf") return "fdiv";
    if (opName == "absf") return "fabs";
    if (opName == "negf") return "fneg";
    if (opName == "sqrtf") return "fsqrt";

    if (opName == "cif") return "sint_to_fp";
    if (opName == "cfi") return "fp_to_sint";
    if (opName == "cifu") return "uint_to_fp";
    if (opName == "cfiu") return "fp_to_uint";

    if (opName == "cfh") return "fpround";
    if (opName == "chf") return "fpextend";

    if (littleEndian_) {
        if (opName == "ld8") return "sextloadi8";
        if (opName == "ldu8") return "zextloadi8";
        if (opName == "ld16") return "sextloadi16";
        if (opName == "ldu16") return "zextloadi16";
        if (mach_.is64bit()) {
            if (opName == "ld32") return "sextloadi32";
            if (opName == "ldu32") return "zextloadi32";
        } else {
            if (opName == "ld32" || opName =="ldu32") return "load";
        }
        if (opName == "ld64") return "load";

        //if (opName == "ldd") return "load";
        
        if (opName == "st8") return "truncstorei8";
        if (opName == "st16") return "truncstorei16";
        if (opName == "st32") {
            if (mach_.is64bit()) {
                return "truncstorei32";
            } else {
                return "store";
            }
        }

        if (opName == "st32") return "store";
        if (opName == "st64") return "store";
        //if (opName == "std") return "load";
    } else { // big-endian
        if (opName == "ldq") return "sextloadi8";
        if (opName == "ldqu") return "zextloadi8";
        if (opName == "ldh") return "sextloadi16";
        if (opName == "ldhu") return "zextloadi16";
        if (opName == "ldw") return "load";
        //if (opName == "ldd") return "load";

        if (opName == "stq") return "truncstorei8";
        if (opName == "sth") return "truncstorei16";
        if (opName == "stw") return "store";
        //if (opName == "std") return "load";
    }

    if (opName.length() >2 && opName.substr(0,2) == "sx") {
        return "sext_inreg";
    }

    if (opName == "truncwh" || opName == "truncwb" || opName == "trunchb") 
        return "trunc"; 

    if (opName == "neg") return "ineg";
    if (opName == "not") return "not";
    if (opName == "cas") return "atomic_cmp_swap_32";

    if (opName == "select") return "select";

    // Unknown operation name.
    return "";
}


/**
 * Pattern for tce generated custom op patterns.
 */
std::string
TDGen::tceOperationPattern(const Operation& op) {
    std::string opList = "";
    for (int i = 0; i < op.numberOfInputs(); i++) {
        opList += " %" + Conversion::toString(i+1) + "%";
    }
    return op.name() + opList;
}

/**
 * Check if operation can be matched with llvm pattern.
 *
 * Check if operation has llvmOperationPatters or 
 * one of it's DAGs contain only operations, which can be matched.
 */
bool
TDGen::operationCanBeMatched(
    const Operation& op, std::set<std::string>* recursionCycleCheck,
    bool recursionHasStore) {

    // LLVM does not support operations with >255 inputs.
    if (op.numberOfInputs() > 255) {
        return false;
    }
    // TODO: this should have been changed?
    char operandChar = mach_.is64bit() ? OT_REG_LONG : OT_REG_INT;
    // if operation has llvm pattern
    if (llvmOperationPattern(op,operandChar) != "") {
        return true;
    }

    std::set<std::string> useSet;
    if (recursionCycleCheck != NULL) {
        useSet = *recursionCycleCheck;
    }    
    useSet.insert(op.name());

    // check if one of dags of operation is ok
    for (int i = 0; i < op.dagCount(); i++) {
        OperationDAG& dag = op.dag(i);
        if (operationDAGCanBeMatched(dag, &useSet, recursionHasStore)) {
            return true;
        }
    }

    return false;
}


bool
TDGen::operationDAGCanBeMatched(
    const OperationDAG& opDag,
    std::set<std::string>* recursionCycleCheck,
    bool recursionHasStore) {

    bool hasStore = false;

    for (int j = 0; j < opDag.nodeCount(); j++) {
        OperationNode* opNode = dynamic_cast<OperationNode*>(&opDag.node(j));
        if (opNode) {
            Operation& refOp = opNode->referencedOperation();
            if (refOp.writesMemory()) {
                if (recursionHasStore || hasStore) {
                    return false;
                } else {
                    hasStore = true;
                }
            }

            // check that the same operation is not used recursively
            if (recursionCycleCheck &&
                recursionCycleCheck->count(refOp.name()) != 0) {
                return false;
            }

            // check if referenced op can be matched
            if (!operationCanBeMatched(refOp, recursionCycleCheck,
                                       hasStore)) {
                return false;
            }
        }
    }
    return true;
}

/**
 * Returns first operation DAG that can be used in LLVM pattern.
 *
 * Operation DAGs are traversed in the order of trigger-semantics in the
 * first .opp file found in path that defines the Operation.
 *
 * This method does not transfer ownership.
 *
 * @return The matchable operation DAG. Nullptr if could not find.
 */
const OperationDAG*
TDGen::getMatchableOperationDAG(
    const Operation& op) {

    // check if one of dags of operation is ok
    for (int i = 0; i < op.dagCount(); i++) {
        OperationDAG& dag = op.dag(i);
        if (op.dagError(i) != "") {
            std::cerr << "Broken dag in operation " << op.name()
                      << op.dagCode(i) << std::endl;
            assert(0);
        }

        if (operationDAGCanBeMatched(dag)) {
            return &dag;
        }
    }
    return nullptr;
}

/**
 * Returns a vector of DAGs that can be used in LLVM pattern.
 *
 * Operation DAGs are traversed in the order of trigger-semantics in the
 * first .opp file found in path that defines the Operation.
 *
 * This method does not transfer ownership.
 *
 * @return The matchable operation DAGs. Empty if could not find.
 */
const std::vector<OperationDAG*>
TDGen::getMatchableOperationDAGs(
    const Operation& op) {
    std::vector<OperationDAG*> matchableDAGs;
    for (int i = 0; i < op.dagCount(); i++) {
        OperationDAG& dag = op.dag(i);
        if (op.dagError(i) != "") {
            std::cerr << "Broken dag in operation " << op.name()
                      << op.dagCode(i) << std::endl;
            continue;
        }

        if (operationDAGCanBeMatched(dag)) {
            matchableDAGs.push_back(&dag);
        }
    }
    return matchableDAGs;
}


/**
 * Returns operation pattern in llvm .td format.
 *
 * @param op Operation to return pattern for.
 * @param dag Operation pattern's DAG.
 * @param immOp Index of an operand to define as an immediate operand,
 *              or 0, if all operands should be in registers.
 *
 * @return Pattern string.
 */
std::string
TDGen::operationPattern(
    const Operation& op,
    const OperationDAG& dag,
    const std::string& operandTypes) {

    std::string retVal;
    for (OperationDAG::NodeSet::iterator i = dag.endNodes().begin(); 
         i != dag.endNodes().end(); ++i) {
        if (i != dag.endNodes().begin()) {
            retVal += ",";
        }
        const OperationDAGNode& res = **(i);
        retVal += dagNodeToString(op, dag, res, false, operandTypes);
    }
    return retVal;
}

std::string 
TDGen::createDefaultOperandTypeString(const Operation& op) {
    std::string operandTypes;

    int inputs = op.numberOfInputs();
    int outputs = op.numberOfOutputs();

    for (int i = 0; i < outputs; i++) {
        Operand& operand = op.operand(i + inputs +1);
        operandTypes += operandChar(operand);
    }

    for (int i = 0; i < inputs ; i++) {
        Operand& operand = op.operand(i +1);
        operandTypes += operandChar(operand);
    }
    return operandTypes;
}

/**
 * Return operation pattern is llvm .td format without outputs.
 *
 * This pattern can be used as sub-pattern of bigger pattern.
 * The operation must have only one output.
 *
 * @param op Operation to return pattern for.
 * @param dag Operation pattern's DAG.
 * @return Pattern string.
 */
std::string
TDGen::subPattern(
    const Operation& op,
    const OperationDAG& dag) {

    if (dag.endNodes().size() != 1) {
	throw InvalidData(
	    __FILE__,__LINE__,__func__,
	    "Cannot create subpattern: not exactly 1 end node!");
    }

    OperationDAG::NodeSet::iterator i = dag.endNodes().begin(); 
    const OperationDAGNode& res = **(i);
    OperationDAG::NodeSet preds = dag.predecessors(res);
    if (preds.size() != 1) {
        throw InvalidData(
            __FILE__,__LINE__,__func__,
            "Cannot create subpattern: not single data source for end node.");
    }
    
    std::string operandTypes = createDefaultOperandTypeString(op);
    // TODO: what about immediate operands?

    // TODO: size of the operand string?
    return dagNodeToString(
        op, dag, **(preds.begin()), false, operandTypes);
}
/**
 * Converts single OperationDAG node to llvm pattern fragment string.
 *
 * @param op Operation that the whole DAG is for.
 * @param dag Whole operation DAG.
 * @param node DAG node to return string for.
 * @param immOp Index of an operand to define as an immediate or 0 if none.
 * @param emulationPattern True, if the returned string should be in
 *                         emulation pattern format.
 * @return DAG node as a llvm .td string.
 */
std::string
TDGen::dagNodeToString(
    const Operation& op, const OperationDAG& dag, const OperationDAGNode& node,
    bool emulationPattern, const std::string& operandTypes,
    const Operation* emulatingOp, const OperationDAGNode* successor) {
    const OperationNode* oNode = dynamic_cast<const OperationNode*>(&node);
    if (oNode != NULL) {
        assert(
            dag.inDegree(*oNode) ==
            oNode->referencedOperation().numberOfInputs());

        return operationNodeToString(
            op, dag, *oNode, emulationPattern, operandTypes);
    }

    const TerminalNode* tNode = dynamic_cast<const TerminalNode*>(&node);
    if (tNode != NULL) {
        const Operand& operand = op.operand(tNode->operandIndex());
        if (dag.inDegree(*tNode) == 0) {
            // Input operand for the whole operation.
            assert(operand.isInput());

            char operandType = 
                operandTypes[operand.index()-1 + op.numberOfOutputs()];
            bool imm =  (operandType == OT_IMM_INT ||
                         operandType == OT_IMM_BOOL);

            if (imm && !canBeImmediate(dag, *tNode)) {
                std::string msg = 
                    "Invalid immediate operand for " + op.name() +
                    " operand #" + Conversion::toString(tNode->operandIndex());
                throw InvalidData(__FILE__, __LINE__, __func__, msg);
            }

            if (emulationPattern) {
                assert(emulatingOp != nullptr);
                return operandToString(operand, false, operandType,
                    immOperandDefs_[ImmInfo::key(
                        *emulatingOp, operand)]);
            } else {
                return operandToString(operand, false, operandType,
                    immOperandDefs_[ImmInfo::key(op, operand)]);
            }
        } else {
            // Output operand for the whole operation.
            assert(dag.inDegree(*tNode) == 1);
            assert(op.operand(tNode->operandIndex()).isOutput());
            assert(operand.isOutput());
            int globalOperandIndex = 
                tNode->operandIndex() - op.numberOfInputs();
            assert(globalOperandIndex == 1);
            
            const OperationDAGEdge& edge = dag.inEdge(node, 0);
            const OperationDAGNode& srcNode = dag.tailNode(edge);

            // Multiple-output operation nodes not supported in the middle
            // of dag:
            assert(dag.outDegree(srcNode) == 1);
            
            std::string dnString = 
                dagNodeToString(
                    op, dag, srcNode, emulationPattern, operandTypes,
                    emulatingOp, successor);

            bool needTrunc = (operandTypes[globalOperandIndex-1] ==
                              OT_REG_BOOL &&
                              operandTypes[1] != OT_REG_BOOL &&
                              operandTypes[1] != OT_IMM_BOOL);

            // handle setcc's without trunc
            // also loads and extends.
            if (needTrunc) {
                if (dnString.substr(0,4) == "(set" ||
                    dnString.substr(0,5) == "(zext" ||
                    dnString.substr(0,5) == "(load") {
                    needTrunc = false;
                }
            }

            if (needTrunc) {
                std::string pattern =
                    "(set " + operandToString(
                        operand, emulationPattern, operandTypes[0])
                    + ", (trunc " + dnString + "))";
                return pattern;
            } else {
                std::string pattern =
                    "(set " + operandToString(
                        operand, emulationPattern, operandTypes[0])
                    + ", " + dnString + ")";
                return pattern;
            }
        }
    }

    // Constant values.
    const ConstantNode* cNode = dynamic_cast<const ConstantNode*>(&node);
    if (cNode != NULL) {
        return constantNodeString(op, dag, *cNode, operandTypes, successor);
    }

    abortWithError("Unknown OperationDAG node type.");
    return "";
}

std::string 
TDGen::constantNodeString(
    const Operation& op, 
    const OperationDAG& dag,
    const ConstantNode& node,
    const std::string& operandTypes,
    const OperationDAGNode* successor) {

    assert(dag.inDegree(node) == 0);
    assert(dag.outDegree(node) == 1 || successor);
    const OperationDAGNode& succ = successor ?
        *successor : dag.headNode(dag.outEdge(node,0));
    const OperationNode* opn = dynamic_cast<const OperationNode*>(&succ);
    OperationDAG::NodeSet siblings = dag.predecessors(*opn);
    for (OperationDAG::NodeSet::iterator i = siblings.begin();
         i!= siblings.end(); i++) {
        const TerminalNode* tNode = dynamic_cast<const TerminalNode*>(*i);
        if (tNode != NULL) {
            const Operand& operand = op.operand(tNode->operandIndex());
            assert(operand.isInput());
            char operandType = 
                operandTypes[operand.index()-1 + op.numberOfOutputs()];
            // Retrieve largest short immediate for the operand.
            switch (operandType) {
            case OT_REG_BOOL:
            case OT_IMM_BOOL:
                return "(i1 " + Conversion::toString(node.value()) + ")";
            case OT_REG_DOUBLE:
                return "(f64 " + Conversion::toString(node.value()) +")";
            case OT_REG_HFP:
                return "(f16 " + Conversion::toString(node.value()) + ")";
                // TODO: f16 vectors not yet implemented
            case OT_REG_FP:
            case OT_IMM_FP:
                return "(f32 " + Conversion::toString(node.value()) + ")";
            case OT_REG_LONG:
            case OT_IMM_LONG:
                return "(i64 " + Conversion::toString(node.value()) + ")";
            case OT_REG_INT:
            case OT_IMM_INT:
                return mach_.is64bit() ?
                    "(i64 " + Conversion::toString(node.value()) + ")":
                    "(i32 " + Conversion::toString(node.value()) + ")";
            default:
                break;
            }
        }
    }
    // TODO: Declaring without type specifier is not valid?
    // The constnode would return this if it is the outer most op in the dag
    return mach_.is64bit() ?
                    "(i64 " + Conversion::toString(node.value()) + ")":
                    "(i32 " + Conversion::toString(node.value()) + ")";
}

/**
 * Returns an llvm name for an operation node in an emulation dag.
 *
 * @param op the operation being emulated.
 * @param dag dag of the emulated operation
 * @param node node whose name is being asked
 * @param operandTypes string containing oeprand types for the emulated op.
 */
std::string
TDGen::emulatingOpNodeLLVMName(
    const Operation& op, const OperationDAG& dag, const OperationNode& node,
    const std::string& operandTypes) {
    const Operation& operation = node.referencedOperation();
    std::string operationName = StringTools::stringToUpper(operation.name());


    int inputs = operation.numberOfInputs();

    // Look at outgoing nodes. If operand goes to another op,
    // the value is in register.
    // if it's terminal, get the type from the paramete string.
    for (int i = 1 ; i < operation.numberOfOutputs() + 1; i++) {
        char c = 0;
        for (int e = 0; e < dag.outDegree(node); e++) {
            const OperationDAGEdge& edge = dag.outEdge(node, e);
            int dst = edge.srcOperand();
            if (dst == i + operation.numberOfInputs()) {
                TerminalNode* t = 
                    dynamic_cast<TerminalNode*>(
                        &(dag.headNode(edge)));
                if (t != NULL) {
                    int strIndex = t->operandIndex() -1 -
                        op.numberOfInputs();
                    assert((int)operandTypes.length() > strIndex &&
                           strIndex >= 0);
                    if (c != 0 && c != operandTypes[strIndex]) {
                        throw InvalidData(__FILE__,__LINE__,__func__,
                                          "conflicting output types!");
                    }
                    c = operandTypes[strIndex];
                } else {
                    Operand &operand = 
                        operation.operand(i+operation.numberOfInputs());
                    char type = operandChar(operand);
                    if (c != 0 && c!= type) {
                        throw InvalidData(__FILE__,__LINE__,__func__,
                                              "conflicting output types!");
                    }
                    c = type;
                }
            }
        }
        if (c == 0) {
            throw InvalidData(__FILE__,__LINE__,__func__,"output not found.");
        }
        operationName += c;
    }

    // Look at incoming nodes. If operand comes from another op,
    // the value is in register. If operand comes from constant,
    // it's immediate.
    // if it's terminal, get the type from the parm string.
    for (int i = 1; i < inputs + 1; i++) {
        Operand &operand = operation.operand(i);
        for (int e = 0; e < dag.inDegree(node); e++) {
            const OperationDAGEdge& edge = dag.inEdge(node, e);
            int dst = edge.dstOperand();
            if (dst == i) {
                if (dynamic_cast<OperationNode*>(&(dag.tailNode(edge)))) {
                    operationName += operandChar(operand);
                } else {
                    if (dynamic_cast<ConstantNode*>(
                            &(dag.tailNode(edge)))) {
                        if (operand.type() == Operand::SINT_WORD ||
                            operand.type() == Operand::UINT_WORD) {
                            operationName += OT_IMM_INT;
                        } else if (operand.type() == Operand::RAW_DATA ||
                                   operand.type() == Operand::SLONG_WORD ||
                                   operand.type() == Operand::ULONG_WORD) {
                            operationName += mach_.is64bit() ?
                                OT_IMM_LONG : OT_IMM_INT;
                        }
                    } else {
                        TerminalNode* t = 
                            dynamic_cast<TerminalNode*>(
                                &(dag.tailNode(edge)));
                        assert (t != NULL);
                        int strIndex = t->operandIndex() -1 + 
                            op.numberOfOutputs();
                        if ((int)operandTypes.length() <= strIndex ||
                            strIndex <= 0) {
                            std::cerr << "Invalid operand types length or"
                                      << "strIndex in operation:" 
                                      << operation.name() 
                                      << " OperandTypes string is: " 
                                      << operandTypes 
                                      << " strIndex is: " << strIndex 
                                      << std::endl;
                            assert(false);
                        }
                        operationName += operandTypes[strIndex];
                    }
                }
            }
        }
    }
    return operationName;
}

/**
 * Converts OSAL dag operation node to llvm .td pattern fragment string.
 *
 * @param op Operation which this operation node is part of.
 * @param dag Parent DAG of the operation node.
 * @param node Node to convert to string.
 * @param immOp Index of an operand to define as immediate or 0 if none.
 * @param emulationPattern True, if the string should be in emulation pattern
 *                         format.
 */
std::string
TDGen::operationNodeToString(
    const Operation& op, const OperationDAG& dag, const OperationNode& node,
    bool emulationPattern, const std::string& operandTypes) {
    const Operation& operation = node.referencedOperation();

    std::string operationPat;

    if (emulationPattern) {
        operationPat = emulatingOpNodeLLVMName(
            op, dag, node, operandTypes);
        
        for (int i = 0; i < operation.numberOfInputs(); i++) {
            if (i > 0) {
                operationPat += ", ";
            } else {
                operationPat += " ";
            }
            operationPat =
                operationPat + "%" + Conversion::toString(i + 1) + "%";
        }
    } else {
        operationPat = llvmOperationPattern(operation, operandTypes[0]);
        
        // generate pattern for operation if not llvmOperation (can match 
        // custom op patterns)
        if (operationPat == "") {
            auto* dagToUse = getMatchableOperationDAG(operation);
            if (dagToUse) {
                return subPattern(operation, *dagToUse);
            } else {
                operationPat = tceOperationPattern(operation);
            }
        }
    }

    if (operationPat == "") {
        std::string msg("Unknown operation node in dag: " + 
                        std::string(operation.name()));
        
        throw InvalidData(__FILE__, __LINE__, __func__, msg);
    }

    boost::format pattern(operationPat);

    int inputs = operation.numberOfInputs();
#ifndef NDEBUG
    int outputs = 
#endif
    operation.numberOfOutputs();

    assert(outputs == 0 || outputs == 1);
    
    for (int i = 1; i < inputs + 1; i++) {
        for (int e = 0; e < dag.inDegree(node); e++) {
            const OperationDAGEdge& edge = dag.inEdge(node, e);
            int dst = edge.dstOperand();
            if (dst == i) {
                const OperationDAGNode& in = dag.tailNode(edge);
                std::string dagNodeString =  dagNodeToString(
                    op, dag, in, emulationPattern, operandTypes,
                    &operation, &node);
                try {
                    pattern % dagNodeString;
                } catch(...) {
                    TCEString msg = "Boost format threw exception! ";
                    msg << "Input format: " << operationPat;
                    throw InvalidData(__FILE__, __LINE__, __func__, msg);
                }
            }
        }
    }

    return std::string("(") + pattern.str() + ")";
}

/**
 * Returns LLVM .td format string for an operand.
 *
 * @param operand Operand to return string for.
 * @param match True, if the string should be in the matching pattern format.
 * @param operandType Character that identifies the type of the operand.
 * @return Operand string to be used in a llvm .td pattern.
 */
// TODO is operandToString correct?
std::string
TDGen::operandToString(
    const Operand& operand,
    bool match,
    char operandType,
    const std::string& immDefName) {
    int idx = operand.index();

    if (operand.isVector() && !operand.isAddress()) {
        switch (operandType) {
            case OT_VREG_BOOL:
            case OT_VREG_INT8:
            case OT_VREG_INT16:
            case OT_VREG_INT32:
            case OT_VREG_FP:
            case OT_VREG_HFP: {
                TCEString regClass = associatedVectorRegisterClass(operand);
                return regClass + ":$op" + Conversion::toString(idx);
            }
        }
    }

    // from TDGenSIMD: if (operand.isVector && operand.isAddress()
    /// @todo Vector of pointers once supported. Change operand types
    /// for GATHER and SCATTER to be mem-data and mem-address, because
    /// that will direct the address operand into this if-clause.

    if (operand.isAddress()) {
        switch (operandType) {
        case OT_IMM_INT:
        case OT_IMM_LONG:
            if (match) {
                return "MEMri:$op" + Conversion::toString(idx);
            } else {
                return "ADDRri:$op" + Conversion::toString(idx);
            }
        case OT_REG_INT:
        case OT_REG_LONG:
            if (match) {
                return  "MEMrr:$op" + Conversion::toString(idx);
            } else {
                return  "ADDRrr:$op" + Conversion::toString(idx);
            }
        default:
            std::string msg = 
                "invalid operation type for mem operand:";
            msg += operandType;
            throw InvalidData(__FILE__, __LINE__, __func__, msg);
        }
    } else if (operand.type() == Operand::SINT_WORD ||
               operand.type() == Operand::UINT_WORD ||
               operand.type() == Operand::RAW_DATA ||
               operand.type() == Operand::BOOL) {

        // imm
        switch (operandType) {
        case OT_IMM_LONG:
            if (match) {
                return "i64imm:$op" + Conversion::toString(idx);
            } else {
                return "(i64 imm:$op" + Conversion::toString(idx) + ")";
            }
        case OT_IMM_INT:
            assert(!immDefName.empty() &&
                "No immediate operand defined for the operand.");
            return immDefName + ":$op" + Conversion::toString(idx);
        case OT_IMM_BOOL:
            if (match) {
                return "i1imm:$op" + Conversion::toString(idx);
            } else {
                return "(i1 imm:$op" + Conversion::toString(idx) + ")";
            }
        case OT_REG_INT:
            return mach_.is64bit() ?
                "R64IRegs:$op" + Conversion::toString(idx):
                "R32IRegs:$op" + Conversion::toString(idx);
        case OT_REG_LONG:
            return "R64IRegs:$op" + Conversion::toString(idx);
        case OT_REG_BOOL:
            return "R1Regs:$op" + Conversion::toString(idx);
        case OT_REG_FP:
            if (operand.type() == Operand::RAW_DATA) {
                return "FPRegs:$op" + Conversion::toString(idx);
            }
            /* fall through */
        case OT_REG_HFP:
            if (operand.type() == Operand::RAW_DATA) {
                return "HFPRegs:$op" + Conversion::toString(idx);
            }
            /* fall through */
        default:
            std::string msg = 
                "invalid operation type for integer operand:";
            msg += operandType;
            throw InvalidData(__FILE__, __LINE__, __func__, msg);
        }
    } else if (operand.type() == Operand::FLOAT_WORD ) {

        switch (operandType) {
        case OT_IMM_INT:
        case OT_IMM_FP:
            if (match) {
                return "f32imm:$op" + Conversion::toString(idx);
            } else {
                return "(f32 fpimm:$op" + Conversion::toString(idx) + ")";
            }
            case OT_REG_INT:
        case OT_REG_FP:
            return "FPRegs:$op" + Conversion::toString(idx);

        default:
            std::string msg = 
                "invalid operation type for float operand:";
            msg += operandType;
            throw InvalidData(__FILE__, __LINE__, __func__, msg);
        }
    } else if (operand.type() == Operand::HALF_FLOAT_WORD) {

        switch (operandType) {
        case OT_IMM_INT:
        case OT_IMM_FP:
        case OT_IMM_HFP:
            if (match) {
                return "f16imm:$op" + Conversion::toString(idx);
            } else {
                return "(f16 fpimm:$op" + Conversion::toString(idx) + ")";
            }
        case OT_REG_INT:
        case OT_REG_HFP:
            return "HFPRegs:$op" + Conversion::toString(idx);
        default:
            std::string msg = 
                "invalid operation type for half operand:";
            msg += operandType;
            throw InvalidData(__FILE__, __LINE__, __func__, msg);
        }
    } else if (operand.type() == Operand::DOUBLE_WORD) {
        // TODO: immediate check??
        return "R64DFPRegs:$op" + Conversion::toString(idx);
    } else if (operand.type() == Operand::SLONG_WORD ||
               operand.type() == Operand::ULONG_WORD) {
        if (operandType == OT_REG_BOOL) {
            return "R1Regs:$op" + Conversion::toString(idx);
        }
        if (operandType == OT_IMM_BOOL) {
            if (match) {
                return "i1imm:$op" + Conversion::toString(idx);
            } else {
                return "(i1 imm:$op" + Conversion::toString(idx) + ")";
            }
        }
        if (operandType == OT_IMM_LONG) {
            return match ?
                "i64imm:$op" + Conversion::toString(idx) :
                "(i64 imm:$op" + Conversion::toString(idx) + ")";
        }
        return mach_.is64bit() ?
            "R64IRegs:$op" + Conversion::toString(idx):
            "R32IRegs:$op" + Conversion::toString(idx);
    } else {
        std::cerr << "Unknown operand type: " << operandType << std::endl;
        assert(false && "Unknown operand type.");
    }
    abortWithError("Unknown operand type on osal? Should not get here.");
    return "";
}

/**
 * Returns llvm input definition list for an operation.
 *
 * @param op Operation to define inputs for.
 * @return String defining operation inputs in llvm .td format.
 */
std::string
TDGen::patInputs(const Operation& op, const std::string& inputTypes) {

    std::string ins;
    for (int i = 0; i < op.numberOfInputs(); i++) {
        if (i > 0) {
            ins += ", ";
        }

        char inputType = inputTypes[i + op.numberOfOutputs()];
        std::string immDef("");
        // NOTE: !op.operand(i+1).isAddress() is kludge fix for MEMri/ADDri
        // operands. They do not have immediate operand definitions (yet).
        if (inputType == OT_IMM_INT && !op.operand(i+1).isAddress()) {
            assert(immOperandDefs_.count(
                ImmInfo::key(op, op.operand(i+1)))
                && "Missing immediate operand definition for the operand.");
            immDef = immOperandDefs_[ImmInfo::key(op, op.operand(i+1))];
        }
        ins += operandToString(op.operand(i + 1), true, inputType, immDef);
    }
    return ins;
}


/**
 * Returns llvm output definition list for an operation.
 *
 * @param op Operation to define outputs for.
 * @return String defining operation outputs in llvm .td format.
 */
std::string
TDGen::patOutputs(const Operation& op, const std::string& operandTypes) {
    std::string outs;
    for (int i = 0; i < op.numberOfOutputs(); i++) {
        assert(op.operand(op.numberOfInputs() + 1 + i).isOutput());
        outs += (i > 0) ? (",") : (" ");
        outs += operandToString(
            op.operand(op.numberOfInputs() + 1 + i), true, operandTypes[i]);
    }
    return outs;
}

/**
 * Creates a dummy dag for an OSAL operation.
 *
 * @param op Operation to create OperationDAG for.
 * @return OperationDAG consisting of only one operation node referencing
 *         the given operation.
 */
OperationDAG*
TDGen::createTrivialDAG(Operation& op) {

    OperationDAG* dag = new OperationDAG(op.impl());
    OperationNode* opNode = new OperationNode(op);
    dag->addNode(*opNode);

    for (int i = 0; i < op.numberOfInputs() + op.numberOfOutputs(); i++) {
        const Operand& operand = op.operand(i + 1);
        TerminalNode* t = new TerminalNode(operand.index());
        dag->addNode(*t);
        if (operand.isInput()) {
            OperationDAGEdge* e = new OperationDAGEdge(1, operand.index());
            dag->connectNodes(*t, *opNode, *e);
        } else {
            OperationDAGEdge* e = new OperationDAGEdge(operand.index(), 1);
            dag->connectNodes(*opNode, *t, *e);
        }
    }
    return dag;
}


/**
 * Returns true if the operand corresponding to the given TerminalNode in
 * an OperationDAG can be immediate in the llvm pattern.
 *
 * @param dag DAG of the whole operation.
 * @param node TerminalNode corresponding to the operand queried.
 */
bool
TDGen::canBeImmediate(
    const OperationDAG& dag, const TerminalNode& node) {

    if (dag.inDegree(node) != 0) {
        return false;
    }

    for (int i = 0; i < dag.outDegree(node); i++) {
        const OperationDAGEdge& edge = dag.outEdge(node, i);
        const OperationDAGNode& dstNode = dag.headNode(edge);
        const OperationNode* opNode =
            dynamic_cast<const OperationNode*>(&dstNode);

        if (opNode == NULL) {
            return false;
        }
    }
    return true;
}

#if 0
void
TDGen::generateLoadStoreCopyGenerator(std::ostream& os) {
    // vector store/load generation code

    TCEString prefix = "&"; // address of -operator
    TCEString rcpf = "RegsRegClass";
    TCEString rapf = "TCE::RARegRegClass";
    TCEString boolStore = littleEndian_ ? "ST8Brb;" : "STQBrb;";
    TCEString intStore =  littleEndian_ ? "ST32"   : "STW";
    TCEString halfStore = littleEndian_ ? "ST16"   : "STH";
    TCEString longStore = "ST64";

    os << "#include <stdio.h>" << std::endl 
       << "int GeneratedTCEPlugin::getStore(const TargetRegisterClass *rc)"
       << " const {" << std::endl;
    
    os << "\tif (rc == " << prefix << rapf
       << ") return TCE::" << intStore << "RArr;"
       << std::endl;

    for (RegClassMap::iterator ri = regsInClasses_.begin(); 
         ri != regsInClasses_.end(); ri++) {

        std::cerr << "Checking reg class: " << ri->first << " for stores." << std::endl;

        if (ri->first.find("R1") == 0 ||
            ri->first.find(guardRegTemplateName) == 0) {
            os << "\tif (rc == " << prefix << "TCE::" << ri->first
               << rcpf << ") return TCE::" << boolStore << std::endl;
        }
        if (ri->first.find("R32") == 0) {
            os << "\tif (rc == " << prefix << "TCE::" << ri->first
               << rcpf << ") return TCE::" << intStore << "rr;" << std::endl;
            
            os << "\tif (rc == " << prefix << "TCE::"  << ri->first
               << "I" << rcpf << ") return TCE::" << intStore << "rr;" << std::endl;
            
            os << "\tif (rc == " << prefix << "TCE::"  << ri->first
               << "FP" << rcpf << ") return TCE::" << intStore << "fr;" << std::endl;
            
            os << "\tif (rc == " << prefix << "TCE::"  << ri->first
               << "HFP" << rcpf << ") return TCE::" << halfStore << "hr;" << std::endl;
        }
        if (mach_.is64bit()) {
            if (ri->first.find("R64") == 0) {
                os << "\tif (rc == " << prefix << "TCE::" << ri->first
                   << rcpf << ") return TCE::" << longStore << "ss;" << std::endl;
            }

            if (ri->first.find("R64") == 0) {
                os << "\tif (rc == " << prefix << "TCE::" << ri->first
                   << "I" << rcpf << ") return TCE::" << longStore <<  "ss;" << std::endl;
            }

            if (ri->first.find("R64") == 0) {
                os << "\tif (rc == &TCE::" << ri->first << "FP" << rcpf
                   << ") return TCE::" << longStore <<  "fs;" << std::endl;
            }

            if (ri->first.find("R64") == 0) {
                os << "\tif (rc == &TCE::" << ri->first << "HFP" << rcpf
                   << ") return TCE::" << longStore <<  "hs;" << std::endl;
            }

            if (ri->first.find("R64") == 0) {
                os << "\tif (rc == &TCE::" << ri->first << "DFP" << rcpf
                   << ") return TCE::" << longStore <<  "ds;" << std::endl;
            }

        }
    }

    if (use64bitForFP_) {
        os << "\tif (rc == &TCE::FPRegsRegClass) return TCE::ST64fs;"
           << std::endl;

        os << "\tif (rc == &TCE::HFPRegsRegClass) return TCE::ST64hs;"
           << std::endl;
    } else {
        os << "\tif (rc == &TCE::FPRegsRegClass) return TCE::"
           << intStore << "fr;" << std::endl;

        os << "\tif (rc == &TCE::HFPRegsRegClass) return TCE::"
           << intStore << "hr;" << std::endl;
    }

    for (RegClassMap::iterator ri = regsInRFClasses_.begin(); 
         ri != regsInRFClasses_.end(); ri++) {
            os << "\tif (rc == " 
               << prefix << "TCE::R32_"  << ri->first << "_"
               << rcpf << ") return TCE::" <<intStore << "rr;" << std::endl;    
    }
    
    TCEString boolLoad = littleEndian_ ? "LD8" : "LDQ";
    if (!mach_.hasOperation(boolLoad)) {
        boolLoad = littleEndian_ ? "LDU8" : "LDQU";
        if (!mach_.hasOperation(boolLoad)) {
            boolLoad="";
        }
    }

    TCEString intLoad = littleEndian_ ? "LD32" : "LDW";
    TCEString halfLoad = littleEndian_ ? "LD16" : "LDH";
    TCEString longLoad = "LD64";
    if (!mach_.hasOperation(halfLoad)) {
        halfLoad = littleEndian_ ? "LDU16;" : "LDHU";
        if (!mach_.hasOperation(halfLoad)) {
            halfLoad="";
        }
    }

    os  << "\tprintf(\"regclass of size %d \\n\",rc->MC->RegsSize);"
        << std::endl
        << "\tassert(0&&\"Storing given regclass to stack not supported. "
        << "Bug in backend?\");"
        << std::endl
        << "} " << std::endl
        << std::endl
              
        << "int GeneratedTCEPlugin::getLoad(const TargetRegisterClass *rc)"
        << " const {" << std::endl;

    os << "\tif (rc == " << prefix << rapf << ") return TCE::" << intLoad << "RAr;"
       << std::endl;

    if (!mach_.hasOperation(intLoad) && mach_.hasOperation("LDU32")) {
        intLoad = "LDU32";
    }

    for (RegClassMap::iterator ri = regsInClasses_.begin(); 
         ri != regsInClasses_.end(); ri++) {
        std::cerr << "Checking reg class: " << ri->first << " for loads." << std::endl;
        if (ri->first.find("R1") == 0) {
            if (boolLoad != "") {
                os << "\tif (rc == " << prefix << "TCE::" << ri->first
                   << rcpf <<") return TCE::" << boolLoad << "Br;" << std::endl;
            } else {
                os << "\tif (rc == " << prefix << "TCE::" << ri->first
                   << rcpf <<") return TCE::" << intLoad << "Br;" << std::endl;
            }
        }
        if (ri->first.find("R32") == 0) {
            os << "\tif (rc == " << prefix << "TCE::" << ri->first
               << rcpf << ") return TCE::" << intLoad << "rr;" << std::endl;
            
            os << "\tif (rc == " << prefix << "TCE::" << ri->first
               << "I" << rcpf << ") return TCE::" << intLoad << "rr;" << std::endl;
            
            os << "\tif (rc == " << prefix << "TCE::" << ri->first
               << "FP" << rcpf << ") return TCE::" << intLoad << "fr;" << std::endl;
            
            if (halfLoad != "") {
                os << "\tif (rc == " << prefix << "TCE::" << ri->first
                   << "HFP" << rcpf << ") return TCE::" << halfLoad << "hr;" << std::endl;
            }
        }

        if (mach_.is64bit()) {
            if (ri->first.find("R64") == 0) {
                os << "\tif (rc == " << prefix << "TCE::" << ri->first
                   << rcpf << ") return TCE::" << longLoad << "ss;" << std::endl;

                os << "\tif (rc == " << prefix << "TCE::" << ri->first
                   << "I" << rcpf << ") return TCE::" << longLoad << "ss;" << std::endl;

                os << "\tif (rc == &TCE::" << ri->first << "FP" << rcpf
                   << ") return TCE::" << longLoad << "fs;" << std::endl;

                os << "\tif (rc == &TCE::" << ri->first << "HFP" << rcpf
                   << ") return TCE::" << longLoad << "hs;" << std::endl;

                os << "\tif (rc == &TCE::" << ri->first << "DFP" << rcpf
                   << ") return TCE::" << longLoad << "ds;" << std::endl;
            }
        }
        // TODO: double load!

    }

    // TODO: what is this?
    for (RegClassMap::iterator ri = regsInRFClasses_.begin(); 
         ri != regsInRFClasses_.end(); ri++) {
 
            os << "\tif (rc == " << prefix 
               << "TCE::R32_"  << ri->first << "_"
               << rcpf << ") return TCE::" << intLoad << "rr;" << std::endl;
            
    }

    if (use64bitForFP_) {
        os << "\tif (rc == &TCE::FPRegsRegClass) return TCE::LD64fs;"
           << std::endl;

        os << "\tif (rc == &TCE::HFPRegsRegClass) return TCE::LD64hs;"
           << std::endl;
    } else {
        os << "\tif (rc == &TCE::FPRegsRegClass) return TCE::" << intLoad << "fr;"
           << std::endl;

        os << "\tif (rc == &TCE::HFPRegsRegClass) return TCE::" << intLoad << "hr;"
           << std::endl;
    }

    os  << "\tprintf(\"regclass of size %d \\n\",rc->MC->RegsSize);"
        << std::endl
        << "\tassert(0&&\"loading from stack to given regclass not supported."
        << " Bug in backend?\");"
        << std::endl
        << "} " << std::endl
        << std::endl;
    }
}

#endif

/**
 * Generates implementations of getLoad() and getStore() to Backend.inc file.
 *
 * @param os Output stream to the file.
 */
void
TDGen::generateLoadStoreCopyGenerator(std::ostream& os) {
    genGeneratedTCEPlugin_getStore(os);
    genGeneratedTCEPlugin_getLoad(os);
}

void TDGen::createMinMaxDef(
    const TCEString& opName, const TCEString& valueName, std::ostream& os) {
    if (opNames_.find(opName) != opNames_.end()) {
	os << "\tif (vt == MVT::" << valueName << ") return TCE::"
	   << opName << ";" << std::endl;
    }
}


void 
TDGen::createVectorMinMaxDef(
    const TCEString& baseOpName, 
    int bits, 
    char llvmTypeChar, 
    const TCEString& postFix,
    std::ostream& os) {

    for (int elemCount=2; elemCount <= 1024; elemCount<<=1) {
	TCEString opName = baseOpName; 
	opName << bits << "X" << elemCount << postFix;
	TCEString vecType = "v";
	vecType << elemCount << (TCEString)(llvmTypeChar) << bits;
	createMinMaxDef(opName, vecType, os);
    }
}


void
TDGen::createMinMaxGenerator(std::ostream& os) {

    bool is64bit = mach_.is64bit();
    // MIN
    os  << "int GeneratedTCEPlugin::getMinOpcode(SDNode* n) const {" << std::endl
        << "\tEVT vt = n->getOperand(1).getValueType();" << std::endl;
    if (!is64bit) {
        if (opNames_.find("MINrrr") != opNames_.end()) {
            os << "if (vt == MVT::i32) return TCE::MINrrr;" << std::endl;
        }
    } else {
        if (opNames_.find("MIN64sss") != opNames_.end()) {
            os << "if (vt == MVT::i64) return TCE::MIN64sss;" << std::endl;
        }
    }
    if (opNames_.find("MINFfff") != opNames_.end()) {
        os << "if (vt == MVT::f32) return TCE::MINFfff;" << std::endl;
    }

    createVectorMinMaxDef("MINH", 16, 'f', "ggg", os);
    createVectorMinMaxDef("MINF", 32, 'f', "eee", os);
    createVectorMinMaxDef("MIN", 32, 'i', "uuu", os);
    createVectorMinMaxDef("MIN", 16, 'i', "ttt", os);

    os << "\treturn -1; " << std::endl << "}" << std::endl;

    // MAX
    os  << "int GeneratedTCEPlugin::getMaxOpcode(SDNode* n) const {" << std::endl
        << "\tEVT vt = n->getOperand(1).getValueType();" << std::endl;

    if (!is64bit) {
        if (opNames_.find("MAXrrr") != opNames_.end()) {
            os << "if (vt == MVT::i32) return TCE::MAXrrr;" << std::endl;
        }
    } else {
        if (opNames_.find("MAX64sss") != opNames_.end()) {
            os << "if (vt == MVT::i64) return TCE::MAX64sss;" << std::endl;
        }

    }
    if (opNames_.find("MAXFfff") != opNames_.end()) {
        os << "if (vt == MVT::f32) return TCE::MAXFfff;" << std::endl;
    }

    createVectorMinMaxDef("MAXH", 16, 'f', "ggg", os);
    createVectorMinMaxDef("MAXF", 32, 'f', "eee", os);
    createVectorMinMaxDef("MAX", 32, 'i', "uuu", os);
    createVectorMinMaxDef("MAX", 16, 'i', "ttt", os);

    os << "\treturn -1; " << std::endl << "}" << std::endl;
    
    // MINU
    os  << "int GeneratedTCEPlugin::getMinuOpcode(SDNode* n) const {" << std::endl;
    os << "\tEVT vt = n->getOperand(1).getValueType();" << std::endl;

    if (!is64bit) {
        if (opNames_.find("MINUrrr") != opNames_.end()) {
            os << "if (vt == MVT::i32) return TCE::MINUrrr;" << std::endl;
        }
    } else {
        if (opNames_.find("MINU64sss") != opNames_.end()) {
            os << "if (vt == MVT::i64) return TCE::MINU64sss;" << std::endl;
        }
    }
    createVectorMinMaxDef("MINU", 32, 'i', "uuu", os);
    createVectorMinMaxDef("MINU", 16, 'i', "ttt", os);

    os << "\treturn -1; " << std::endl << "}" << std::endl;

    // MAXU
    os  << "int GeneratedTCEPlugin::getMaxuOpcode(SDNode* n) const {" << std::endl;
    os << "\tEVT vt = n->getOperand(1).getValueType();" << std::endl;


    if (!is64bit) {
        if (opNames_.find("MAXUrrr") != opNames_.end()) {
            os << "if (vt == MVT::i32) return TCE::MAXUrrr;" << std::endl;
        }
    } else {
        if (opNames_.find("MAXU64sss") != opNames_.end()) {
            os << "if (vt == MVT::i64) return TCE::MAXU64sss;" << std::endl;
        }
    }

    createVectorMinMaxDef("MAXU", 32, 'i', "uuu", os);
    createVectorMinMaxDef("MAXU", 16, 'i', "ttt", os);
    os << "\treturn -1; " << std::endl << "}" << std::endl;
}

void TDGen::createEndiannesQuery(std::ostream& os) {
    bool is64bit = mach_.is64bit();
    os << "bool GeneratedTCEPlugin::isLittleEndian() const {" << std::endl;
    os << "return " << littleEndian_ << "; }" << std::endl;
    os << "bool GeneratedTCEPlugin::is64bit() const {" << std::endl;
    os << "return " << is64bit << "; }" << std::endl;
}

/**
 * Creates query function for TCEISelLowering for testing if otherwise
 * unsupported constant as immediate can be materialized instead of converting
 * it to constant pool load.
 */
void
TDGen::createConstantMaterializationQuery(std::ostream& os) {

    if (constantMaterializationPredicates_.empty()) {
        os << "bool GeneratedTCEPlugin::canMaterializeConstant("
           << "const ConstantInt&) const { " << std::endl
           << "    return false;" << std::endl
           << "};" << std::endl;
        return;
    }

    os << "bool GeneratedTCEPlugin::canMaterializeConstant("
       << "const ConstantInt& ci) const {"
       << std::endl
       << "    int64_t Imm = ci.getSExtValue();" << std::endl
       << "    if (";
    bool first = true;
    for (auto& predicate : constantMaterializationPredicates_) {
        if (!first) {
            os << std::endl << "        || ";
        }
        os << "(" << predicate << ")";
        first = false;
    }
    os << ") {"
       << "        return true;" << std::endl
       << "    }" << std::endl
       << "    return false;" << std::endl
       << "}" << std::endl;
}

void TDGen::createByteExtLoadPatterns(std::ostream& os) {
    TCEString load = littleEndian_ ? "LD8" : "LDQ";
    TCEString uload = littleEndian_ ? "LDU8" : "LDQU";
    TCEString destSize = mach_.is64bit() ? "64" : "32";
    int bits = mach_.is64bit() ? 64 : 32;
    TCEString ANDIMM = mach_.is64bit() ? "AND64ssa" : "ANDrri";
    TCEString ANDREG = mach_.is64bit() ? "AND64sss" : "ANDrrr";
    TCEString SUBIMM = mach_.is64bit() ? "SUB64sas" : "SUBrir";
    TCEString EXTOP = mach_.is64bit() ? "SXQ64" : "SXQW";
    TCEString EXTOPC = mach_.is64bit() ? "SXQ64sr" : "SXQWrr";
    TCEString SHL = mach_.is64bit() ? "SHL" : "SHL64";
    TCEString SHR = mach_.is64bit() ? "SHR" : "SHR64";
    TCEString SHRU = mach_.is64bit() ? "SHRU" : "SHRU64ssa";
    TCEString SHLOPC = mach_.is64bit() ? "SHLrri" : "SHL64ssa";
    TCEString SHROPC = mach_.is64bit() ? "SHRrri" : "SHR64ssa";
    TCEString SHRUOPC = mach_.is64bit() ? "SHRUrri" : "SHRU64ssa";
    TCEString SHL4 = mach_.is64bit() ? "SHL4_32" : "SHL4_64";
    TCEString SHR4 = mach_.is64bit() ? "SHR4_32" : "SHR4_64";
    TCEString SHRU4 = mach_.is64bit() ? "SHRU4_32" : "SHRU4_64ssa";
    TCEString regSrcChar = mach_.is64bit() ? OT_REG_LONG : OT_REG_INT;
    TCEString dstTypeChar = OT_REG_INT;
    TCEString immSrcChar = mach_.is64bit() ? OT_IMM_LONG : OT_IMM_INT;
    TCEString loadOpcReg = load + dstTypeChar + regSrcChar;
    TCEString loadOpcImm = load + dstTypeChar + immSrcChar;

    if (mach_.hasOperation(load)) {
        if (!mach_.hasOperation(uload)) {

            // emulate zero ext with sing-ext and and
            os << "def : Pat<(i" << destSize << " (zextloadi8 ADDRrr:$addr)), "
               << "(" << ANDIMM << " ("
               << loadOpcReg << " ADDRrr:$addr), 255)>;"
               << std::endl;
            os << "def : Pat<(i" << destSize << " (zextloadi8 ADDRri:$addr)), "
               << "(" << ANDIMM << " ("
               << loadOpcImm << " ADDRri:$addr), 255)>;"
               << std::endl;
        }
    } else {
        // if no sign ext load, try zero ext load
        if (!mach_.hasOperation(uload)) {
            std::cerr << "Warning: The architecture is missing any 8-bit loads."
                      << " All code may not compile!"
                      << std::endl;
            return;
        }
        loadOpcReg = uload + dstTypeChar + regSrcChar;
        loadOpcImm = uload + dstTypeChar + immSrcChar;

        if (mach_.hasOperation(EXTOP)) {
            // use zextload + sext for sextload
            os << "def : Pat<(i" << destSize
               << " (sextloadi8 ADDRrr:$addr)), "
               << "(" << EXTOPC << " ("
               << loadOpcReg << " ADDRrr:$addr))>;" << std::endl;
            os << "def : Pat<(i" << destSize
               << " (sextloadi8 ADDRri:$addr)), "
               << "(" << EXTOPC << " ("
               << loadOpcImm << " ADDRri:$addr))>;" << std::endl;

        } else {
            if (mach_.hasOperation(SHL) && mach_.hasOperation(SHR)) {

                int sb = bits -8;
                os << "def : Pat<(i" << destSize
                   << " (sextloadi16 ADDRrr:$addr)), "
                   << "(" << SHROPC << " (" << SHLOPC << " ("
                   << load << "rr ADDRrr:$addr), "
                   << sb << "), " << sb << ")>;" << std::endl;

                os << "def : Pat<(i" << destSize
                   << " (sextloadi16 ADDRri:$addr)), "
                   << "(" << SHROPC << " (" << SHLOPC << " ("
                   << load << "ri ADDRri:$addr), "
                   << sb << "), " << sb << ")>;" << std::endl;
            } else if (mach_.hasOperation(SHL4) &&
                       mach_.hasOperation(SHR4)) {
                if (!mach_.is64bit()) {
                    os << "def : Pat<(i32 (sextloadi8 ADDRrr:$addr)), "
                       << "(SHR4_32rr (SHR4_32rr (SHR4_32rr (SHR4_32rr "
                       << "(SHR4_32rr (SHR4_32rr "
                       << "(SHL4_32rr (SHL4_32rr (SHL4_32rr (SHL4_32rr "
                       << "(SHL4_32rr (SHL4_32rr ("
                       << load << "rr ADDRrr:$addr)))))))))))))>;"
                       << std::endl;

                    os << "def : Pat<(i32 (sextloadi8 ADDRri:$addr)), "
                       << "(SHR4_32rr (SHR4_32rr (SHR4_32rr (SHR4_32rr "
                       << "(SHR4_32rr (SHR4_32rr "
                       << "(SHL4_32rr (SHL4_32rr (SHL4_32rr (SHL4_32rr "
                       << "(SHL4_32rr (SHL4_32rr ("
                       << load << "ri ADDRri:$addr)))))))))))))>;"
                       << std::endl;
                } else {
                    os << "def : Pat<(i64 (sextloadi8 ADDRrr:$addr)), "
                       << "(SHR4_64ss (SHR4_64ss (SHR4_64ss (SHR4_64ss "
                       << "(SHR4_64ss (SHR4_64ss (SHR4_64ss (SHR4_64ss "
                       << "(SHR4_64ss (SHR4_64ss (SHR4_64ss (SHR4_64ss "
                       << "(SHR4_64ss (SHR4_64ss "
                       << "(SHL4_64ss (SHL4_64ss (SHL4_64ss (SHL4_64ss "
                       << "(SHL4_64ss (SHL4_64ss (SHL4_64ss (SHL4_64ss "
                       << "(SHL4_64ss (SHL4_64ss (SHL4_64ss (SHL4_64ss "
                       << "(SHL4_64ss (SHL4_64ss ("
                       << load << "rr ADDRrr:$addr)))))))))))))>;"
                       << std::endl;

                    os << "def : Pat<(i64 (sextloadi8 ADDRri:$addr)), "
                       << "(SHR4_64ss (SHR4_64ss (SHR4_64ss (SHR4_64ss "
                       << "(SHR4_64ss (SHR4_64ss (SHR4_64ss (SHR4_64ss "
                       << "(SHR4_64ss (SHR4_64ss (SHR4_64ss (SHR4_64ss "
                       << "(SHR4_64ss (SHR4_64ss "
                       << "(SHL4_64ss (SHL4_64ss (SHL4_64ss (SHL4_64ss "
                       << "(SHL4_64ss (SHL4_64ss (SHL4_64ss (SHL4_64ss "
                       << "(SHL4_64ss (SHL4_64ss (SHL4_64ss (SHL4_64ss "
                       << "(SHL4_64ss (SHL4_64ss ("
                       << load << "ri ADDRri:$addr)))))))))))))>;"
                       << std::endl;
                }
            } else {
                std::cerr << "Warning: no sign-extending 8-bit loads,"
                          << " 8-bit sign extension instruction or suitable"
                          << " shifts for sext-emulation in the processor."
                          << " All code may not compile!" << std::endl;
            }
        }
    }

    // TODO: is there a more capable version of this code
    // commented out above???

    os << "def : Pat<(i" << destSize << " (zextloadi1 ADDRrr:$addr)), ("
       << loadOpcReg << " ADDRrr:$addr)>;"
       << std::endl
       << "def : Pat<(i" << destSize << " (zextloadi1 ADDRri:$addr)), ("
       << loadOpcImm << " ADDRri:$addr)>;"
       << std::endl;

    os << "def : Pat<(i" << destSize << " (sextloadi1 ADDRrr:$addr)), "
       << "(" << SUBIMM << " 0, "
       << "(" << ANDREG << " ("
       << loadOpcReg << " ADDRrr:$addr), 1))>;"
       << std::endl
       << "def : Pat<(i" << destSize << " (sextloadi1 ADDRri:$addr)), "
       << "(" << SUBIMM << " 0, "
       <<  "(" << ANDREG << " ("
       << loadOpcImm << " ADDRri:$addr), 1))>;"
       << std::endl
       << "// anyextloads" << std::endl;

    os << "def : Pat<(i" << destSize << " (extloadi1 ADDRrr:$src)), ("
       << loadOpcReg << " ADDRrr:$src)>;" << std::endl
       << "def : Pat<(i" << destSize << " (extloadi1 ADDRri:$src)), ("
       << loadOpcImm << " ADDRri:$src)>;" << std::endl
       << "def : Pat<(i" << destSize << " (extloadi8 ADDRrr:$src)), ("
       << loadOpcReg << " ADDRrr:$src)>;" << std::endl
       << "def : Pat<(i" << destSize << " (extloadi8 ADDRri:$src)), ("
       << loadOpcImm << " ADDRri:$src)>;" << std::endl
       << std::endl;
}

void TDGen::createShortExtLoadPatterns(std::ostream& os) {
    TCEString load = littleEndian_ ? "LD16" : "LDH";
    TCEString destSize = mach_.is64bit() ? "64" : "32";
    int bits = mach_.is64bit() ? 64 : 32;
    TCEString ANDOPC = mach_.is64bit() ? "AND64ssa" : "ANDrri";
    TCEString uload = littleEndian_ ? "LDU16" : "LDHU";
    TCEString EXTOP = mach_.is64bit() ? "SXH64" : "SXHW";
    TCEString EXTOPC = mach_.is64bit() ? "SXH64sr" : "SXHWrr";
    TCEString regSrcChar = mach_.is64bit() ? OT_REG_LONG : OT_REG_INT;
    TCEString dstTypeChar = OT_REG_INT;
    TCEString immSrcChar = mach_.is64bit() ? OT_IMM_LONG : OT_IMM_INT;
    TCEString loadOpcReg = load + dstTypeChar + regSrcChar;
    TCEString loadOpcImm = load + dstTypeChar + immSrcChar;

    TCEString SHL = mach_.is64bit() ? "SHL" : "SHL64";
    TCEString SHR = mach_.is64bit() ? "SHR" : "SHR64";
    TCEString SHRU = mach_.is64bit() ? "SHRU" : "SHRU64ssa";
    TCEString SHLOPC = mach_.is64bit() ? "SHLrri" : "SHL64ssa";
    TCEString SHROPC = mach_.is64bit() ? "SHRrri" : "SHR64ssa";
    TCEString SHRUOPC = mach_.is64bit() ? "SHRUrri" : "SHRU64ssa";
    TCEString SHL4 = mach_.is64bit() ? "SHL4_32" : "SHL4_64";
    TCEString SHR4 = mach_.is64bit() ? "SHR4_32" : "SHR4_64";
    TCEString SHRU4 = mach_.is64bit() ? "SHRU4_32" : "SHRU4_64ssa";

    if (mach_.hasOperation(load)) {
        if (!mach_.hasOperation(uload)) {
            // emulate zero ext with sing-ext and and
            os << "def : Pat<(i" << destSize
               << " (zextloadi16 ADDRrr:$addr)), "
               << "(" << ANDOPC << " (" << loadOpcReg
               << " ADDRrr:$addr), 65535)>;" << std::endl;
            os << "def : Pat<(i" << destSize
               << " (zextloadi16 ADDRri:$addr)), "
               << "(" << ANDOPC << " ("
               << loadOpcImm << " ADDRri:$addr), 65535)>;" << std::endl;
        }
    } else {
        if (!mach_.hasOperation(uload)) {
            std::cerr << "Warning: The architecture is missing any 16-bit loads."
                      << std::endl;
            return;
        }
        loadOpcReg = uload + dstTypeChar + regSrcChar;
        loadOpcImm = uload + dstTypeChar + immSrcChar;

        if (mach_.hasOperation(EXTOP)) {
            // use zextload + sext for sextload
            os << "def : Pat<(i" << destSize
               << " (sextloadi16 ADDRrr:$addr)), "
               << "(" << EXTOPC
               << " (" << loadOpcReg << " ADDRrr:$addr))>;" << std::endl;
            os << "def : Pat<(i" << destSize
               << " (sextloadi16 ADDRri:$addr)), "
               << "(" << EXTOPC
               << " (" << loadOpcImm << " ADDRri:$addr))>;" << std::endl;
        } else {
            if (mach_.hasOperation(SHL) && mach_.hasOperation(SHR)) {
                int sb = bits -16;
                os << "def : Pat<(i" << destSize
                   << " (sextloadi16 ADDRrr:$addr)), "
                   << "(" << SHROPC << " (" << SHLOPC << " ("
                   << load << "rr ADDRrr:$addr), "
                   << sb << "), " << sb << ")>;" << std::endl;

                os << "def : Pat<(i" << destSize
                   << " (sextloadi16 ADDRri:$addr)), "
                   << "(" << SHROPC << " (" << SHLOPC << " ("
                   << load << "ri ADDRri:$addr), "
                   << sb << "), " << sb << ")>;" << std::endl;
            } else if (mach_.hasOperation(SHL4) &&
                       mach_.hasOperation(SHR4)) {
                if (!mach_.is64bit()) {
                    os << "def : Pat<(i32 (sextloadi16 ADDRrr:$addr)), "
                       << "(SHR4_32rr (SHR4_32rr (SHR4_32rr (SHR4_32rr "
                       << "(SHL4_32rr (SHL4_32rr (SHL4_32rr (SHL4_32rr ("
                       << load << "rr ADDRrr:$addr)))))))))>;" << std::endl;

                    os << "def : Pat<(i32 (sextloadi16 ADDRri:$addr)), "
                       << "(SHR4_32rr (SHR4_32rr (SHR4_32rr (SHR4_32rr "
                       << "(SHL4_32rr (SHL4_32rr (SHL4_32rr (SHL4_32rr ("
                       << load << "ri ADDRri:$addr)))))))))>;" << std::endl;
                } else {
                    os << "def : Pat<(i64 (sextloadi8 ADDRrr:$addr)), "
                       << "(SHR4_64ss (SHR4_64ss (SHR4_64ss (SHR4_64ss "
                       << "(SHR4_64ss (SHR4_64ss (SHR4_64ss (SHR4_64ss "
                       << "(SHR4_64ss (SHR4_64ss (SHR4_64ss (SHR4_64ss "
                       << "(SHL4_64ss (SHL4_64ss (SHL4_64ss (SHL4_64ss "
                       << "(SHL4_64ss (SHL4_64ss (SHL4_64ss (SHL4_64ss "
                       << "(SHL4_64ss (SHL4_64ss (SHL4_64ss (SHL4_64ss ("
                       << load << "rr ADDRrr:$addr)))))))))))))>;"
                       << std::endl;

                    os << "def : Pat<(i64 (sextloadi8 ADDRri:$addr)), "
                       << "(SHR4_64ss (SHR4_64ss (SHR4_64ss (SHR4_64ss "
                       << "(SHR4_64ss (SHR4_64ss (SHR4_64ss (SHR4_64ss "
                       << "(SHR4_64ss (SHR4_64ss (SHR4_64ss (SHR4_64ss "
                       << "(SHL4_64ss (SHL4_64ss (SHL4_64ss (SHL4_64ss "
                       << "(SHL4_64ss (SHL4_64ss (SHL4_64ss (SHL4_64ss "
                       << "(SHL4_64ss (SHL4_64ss (SHL4_64ss (SHL4_64ss ("
                       << load << "ri ADDRri:$addr)))))))))))))>;"
                       << std::endl;
                }
            } else {
                std::cerr << "Warning: no sign-extending 16-bit loads,"
                          << " 16-bit sign extension instruction or suitable"
                          << " shifts for sext-emulation in the processor."
                          << " All code may not compile!" << std::endl;
            }
        }
    }
    // anyext
    os << "def : Pat<(i" << destSize << " (extloadi16 ADDRrr:$src)), ("
       << loadOpcReg << " ADDRrr:$src)>;" << std::endl
       << "def : Pat<(i" << destSize << " (extloadi16 ADDRri:$src)), ("
       << loadOpcImm << " ADDRri:$src)>;" << std::endl;
}

void TDGen::create32BitExtLoadPatterns(std::ostream& os) {
    TCEString load = "LD32";
    const TCEString uload = "LDU32";
    TCEString ZXOP = "ZXW64";
    TCEString ZXOPC = "ZXW64ss";

    if (mach_.hasOperation(load)) {
        if (!mach_.hasOperation(uload)) {
            if (!mach_.hasOperation(ZXOP)) {
                // emulate zero ext with sing-ext and and
                os << "def : Pat<(i64 (zextloadi32 ADDRrr:$addr)), "
                   << "(AND64ssa (LD32ss ADDRrr:$addr),"
                   << "0xffffffff)>;" << std::endl;

                os << "def : Pat<(i64 (zextloadi32 ADDRri:$addr)), "
                   << "(AND64ssa (LD32sa ADDRri:$addr),"
                   << "0xffffffff)>;" << std::endl;
            } else {
                // use zxw64 instr for zext
                os << "def : Pat<(i64 (zextloadi32 ADDRrr:$addr)), "
                   << "(" << ZXOPC << " (LD32ss ADDRrr:$addr))>;" <<std::endl;

                os << "def : Pat<(i64 (zextloadi32 ADDRri:$addr)), "
                   << "(" << ZXOPC << " (LD32sa ADDRri:$addr))>;" <<std::endl;
            }
        }
    } else {
        if (!mach_.hasOperation(uload)) {
            std::cerr << "Warning: The architecture is missing any 16-bit loads."
                      << std::endl;
            return;
        }
        load = uload;

        if (mach_.hasOperation("SXW64")) {
            // use zextload + sext for sextload
            os << "def : Pat<(i64 (sextloadi32 ADDRrr:$addr)), "
               << "(SXW64ss (LDU32ss ADDRrr:$addr))>;" << std::endl;

            os << "def : Pat<(i64 (sextloadi32 ADDRri:$addr)), "
               << "(SXW64ss (LDU32sa ADDRri:$addr))>;" << std::endl;
        } else {
            std::cerr << "Warning: no sign-extending 32-bit loads or"
                      << " 32-bit sign extension instruction!"
                      << " in the processor. All code may not compile!"
                      << std::endl;
        }
    }
    // anyext.
    os << "def : Pat<(i64 (extloadi32 ADDRrr:$src)), "
       << "(" << load << "ss ADDRrr:$src)>;" << std::endl

       << "def : Pat<(i64 (extloadi32 ADDRri:$src)), "
       << "(" << load << "sa ADDRrr:$src)>;" << std::endl;
}

/**
 * Writes details about function arguments and returns values.
 *
 * @param o Output stream to the file.
 */
void
TDGen::writeCallingConv(std::ostream& os) {
    writeCallingConvLicenceText(os);

    int bits = mach_.is64bit() ? 64 : 32;
    int bytes = mach_.is64bit() ? 8 : 4;
    // Function return value types.
    os << "// Function return value types." << endl;
    os << "def RetCC_TCE : CallingConv<[" << endl;
    if (mach_.is64bit()) {
        os << "  CCIfType<[i1], CCPromoteToType<i64>>," << endl
           << "  CCIfType<[i32], CCPromoteToType<i64>>," << endl
           << "  CCIfType<[i64], CCAssignToReg<[IRES0]>>," << endl
           << "  CCIfType<[f64], CCAssignToReg<[IRES0]>>," << endl << endl;
    } else {
        os << "  CCIfType<[i1], CCPromoteToType<i32>>," << endl;
        os << "  CCIfType<[i32], CCAssignToReg<[IRES0]>>," << endl;
    }

    os << "  CCIfType<[f16], CCAssignToReg<[IRES0]>>," << endl
       << "  CCIfType<[f32], CCAssignToReg<[IRES0]>>," << endl << endl;

    os << "  // Vector value types." << endl;
    std::map<TCEString, RegisterClass>::const_iterator rcIt;
    for (rcIt = vRegClasses_.begin(); rcIt != vRegClasses_.end(); ++rcIt) {
        const TCEString& vtStr = rcIt->first;
        const RegisterClass& regClass = rcIt->second;

        if (regClass.numberOfRegisters() > 0) {
            os << "  CCIfType<[" << vtStr << "], CCAssignToReg<[";
            if (regClass.valueType().width() <= bits) {
                os << "IRES0";
            } else {
                os << regClass.registerInfo(0).regName_;
            }
            os << "]>>," << endl;
        } else {
            verbose("ValueType " + vtStr + " can not be a return value");
        }
    }

    os << "  CCAssignToStack<" << bytes << ", " << bytes << ">" << endl
       << "]>;" << endl << endl;

    // Function argument value types.
    os << "// Function argument value types." << endl;
    os << "def CC_TCE : CallingConv<[" << endl;
    if (mach_.is64bit()) {
        os << "  CCIfType<[i1, i8, i16, i32], CCPromoteToType<i64>>," << endl
           << "  CCIfType<[i64], CCAssignToReg<[IRES0]>>," << endl << endl;

        for (unsigned int i = 4; i < (3 + argRegCount_); i++) {
            os << "  CCIfType<[i64], CCAssignToReg<[A" << i << "]>>," << endl;
        }

    } else {
        os << "  CCIfType<[i1, i8, i16], CCPromoteToType<i32>>," << endl
           << "  CCIfType<[i32], CCAssignToReg<[IRES0]>>," << endl << endl;

        for (unsigned int i = 4; i < (3 + argRegCount_); i++) {
            os << "  CCIfType<[i32], CCAssignToReg<[A" << i << "]>>," << endl;
        }
    }

    if (mach_.is64bit()) {
        os << "  // 64-bit integer values get stored in stack slots that are"
           << endl
           << "  // 8 bytes insize and 8-byte aligned." << endl
           << "  CCIfType<[i64, f64, f32], CCAssignToStack<8, 8>>,"
           << endl << endl;
    } else {
        os << "  // Integer values get stored in stack slots that are" << endl
           << "  // 4 bytes insize and 4-byte aligned." << endl
           << "  CCIfType<[i32, f32], CCAssignToStack<4, 4>>," << endl << endl;
    }
    os << "  // Double values get stored in stack slots that are" << endl
       << "  // 8 bytes in size and 8-byte aligned." << endl
       << "  CCIfType<[f64], CCAssignToStack<8, 8>>";

    if (vRegClasses_.size() > 0) {
        os << ",";
    }
    os << endl << endl;

    os << "  // Vector value types." << endl;
    for (rcIt = vRegClasses_.begin(); rcIt != vRegClasses_.end(); ++rcIt) {
        const TCEString& vtStr = rcIt->first;
        const RegisterClass& regClass = rcIt->second;

        int byteAlignment = regClass.alignment() / 8;
        if (byteAlignment < bytes) {
            byteAlignment = bytes;
        }

        os << "  CCIfType<[" << vtStr << "], CCAssignToStack<"
               << byteAlignment << ", " << byteAlignment << ">>";
        if (rcIt != --vRegClasses_.end()) {
            os << ",";
        }
        os << endl;
    }

    os << "]>;" << endl;
}

void
TDGen::writeCallingConvLicenceText(std::ostream& os) {
    os << "//===- GenCallingConv.td - Calling Conventions TCE ---------*- "
       << "tablegen -*-===//" << std::endl
       << "// " << std::endl
       << "//                     The LLVM Compiler Infrastructure" << std::endl
       << "//" << std::endl
       << "// This file is distributed under the University of "
       << "Illinois Open Source" << std::endl
       << "// License. See LICENSE.TXT for details." << std::endl
       << "// " << std::endl
       << "//===--------------------------------------------------------"
       << "--------------===//" << std::endl
       << "//" << std::endl
       << "// This describes the calling conventions for the TCE "
       << "architectures." << std::endl
       << "//" << std::endl
       << "//===--------------------------------------------------------"
       << "--------------===//" << std::endl << std::endl;
}

void 
TDGen::writeArgRegsArray(std::ostream& os) {
    os << "#ifndef ARGREGS_HH" << std::endl
       << "#define ARGREGS_HH" << std::endl << std::endl
       << "static const unsigned ArgRegs[] = { TCE::IRES0" << std::endl;
    for (unsigned int i = 4; i < (3 + argRegCount_); i++) {
        os << ",TCE::A" << i;
    }
    os << "};" << std::endl;

    os << "static const int argRegCount = " << argRegCount_ <<";" << std::endl
       << "#endif" << std::endl;
}

void
TDGen::createParamDRegNums(std::ostream& os) {
    os << 
        "std::vector<unsigned>" << std::endl <<
        "GeneratedTCEPlugin::getParamDRegNums() const {" << std::endl <<
        "std::vector<unsigned> res;" << std::endl;
    for (unsigned int i = 4; i < argRegCount_ + 3; i++) {
        os << "res.push_back(TCE::A" << i << ");" << std::endl;
    }
    os << "return res;}" << std::endl;
}

void
TDGen::createVectorRVDRegNums(std::ostream& os) {
    os << 
        "std::vector<unsigned>" << std::endl <<
        "GeneratedTCEPlugin::getVectorRVDRegNums() const {" << std::endl <<
        "std::vector<unsigned> res;" << std::endl;

    std::set<TCEString> processedRegs;
    for (auto rcIt : vRegClasses_) {
        const RegisterClass& regClass = rcIt.second;

        int width = regClass.valueType().width();
        if (regClass.numberOfRegisters() > 0) {
            const TCEString& name = regClass.registerInfo(0).regName_;
            if (processedRegs.find(name) == processedRegs.end() &&
                width > maxScalarWidth_) {
                processedRegs.insert(name);

                os << "res.push_back(TCE::" << name << ");" << std::endl;
            }
        }
    }
    os << "return res;}" << std::endl;
}

// TODO: this should be based on vector widths???
void
TDGen::createGetMaxMemoryAlignment(std::ostream& os) const {
    if (mach_.is64bit()) {
        os << std::endl
           << "unsigned GeneratedTCEPlugin::getMaxMemoryAlignment() const {"
           << std::endl
           << "\treturn 8;"
           << std::endl << "}" << std::endl;
    } else {
        os << std::endl
           << "unsigned GeneratedTCEPlugin::getMaxMemoryAlignment() const {"
           << std::endl
           << "\treturn 4;"
           << std::endl << "}" << std::endl;
    }
}


void TDGen::createSelectPatterns(std::ostream& os) {
    os << "// Creating select patterns. " << std::endl;
    if (!hasSelect_) {
        os << "// Does not have select instr. " << std::endl;
        if (!hasConditionalMoves_) {
            std::string NEG = mach_.is64bit() ? "NEG64" : "NEG";
            std::string NEGOPC = mach_.is64bit() ? "NEG64ss" : "NEGrr";
            std::string gprRegs = mach_.is64bit() ? "R64IRegs" : "R32IRegs";
            std::string typeStr = mach_.is64bit() ? "ssa" : "rri";
            std::string typeStrInv = mach_.is64bit() ? "sas" : "rir";

            const char* sub = mach_.is64bit() ? "SUB64" : "SUB";
            const char* iand = mach_.is64bit() ? "AND64" : "AND";
            std::string ior = mach_.is64bit() ? "IOR64" : "IOR";
            std::string defType = mach_.is64bit() ? "i64" : "i32";
            std::string andReg = mach_.is64bit() ? "AND64sss" : "ANDrrr";
            std::string iorReg = mach_.is64bit() ? "IOR64sss" : "IORrrr";

            std::string iorBool = mach_.is64bit() ? "IOR64bbb" : "IORbbb";
            std::string andBool = mach_.is64bit() ? "AND64bbb" : "ANDbbb";
            std::string xorBooli = mach_.is64bit() ? "XOR64bbj" : "XORbbj";

            bool hasNeg = opNames_.count("NEGrr");

            std::string condRC = regs1bit_.empty() ? gprRegs : "R1Regs";
            std::string truncOp = regs1bit_.empty() ? std::string(iand) + typeStr : "ANDext";
            OperationPool opPool;
            std::string truncPattern = regs1bit_.empty()
                ? std::string("$c")
                : std::string("(") + truncOp + " " + condRC + ":$c" + ", 1)";
            std::string falseMask = getLLVMPatternWithConstants(
                opPool.operation(sub), typeStr, truncPattern, "1");

            std::string trueMask = hasNeg ?
                std::string("(" + NEGOPC + " ") + truncPattern +
                std::string(")")
                : getLLVMPatternWithConstants(
                    opPool.operation(sub), typeStrInv, "0", truncPattern);

            std::string applyTrueMaskOnImm = getLLVMPatternWithConstants(
                opPool.operation(iand), typeStr, trueMask, "imm:$t");
            std::string applyFalseMaskOnImm = getLLVMPatternWithConstants(
                opPool.operation(iand), typeStr, falseMask, "imm:$f");

#if 0 // TODO: why is this commented out???
            if (mach_.is64bit()) {
            os << std::endl
               << "def : Pat<(i64 (select R1Regs:$c, R64Regs:$t, R64Regs:$f)), "
               << "(IOR64sss (AND64sss R32Regs:$t, (SUB64sas 0, (ANDext R1Regs:$c, 1))),"
               << "(AND64sss R32Regs:$f, (SUB64ssa (ANDext R1Regs:$c, 1), 1)))>;"
               << std::endl << std::endl

               << "def : Pat<(i64 (select R1Regs:$c, (i64 imm:$t),(i64 imm:$f))),"
               << "(IOR64sss (AND64ssa (SUB64sas 0, (ANDext R1Regs:$c, 1)), imm:$t),"
               << "(AND64ssa (SUB64ssa (ANDext R1Regs:$c, 1), 1), imm:$f))>;"
               << std::endl << std::endl

               << "def : Pat<(i64 (select R1Regs:$c, R64Regs:$t, (i64 imm:$f))),"
               << "(IOR64sss (AND64sss (SUB64sas 0, (ANDext R1Regs:$c, 1)), R32Regs:$t),"
               << "(AND64ssa (SUB64ssa (ANDext R1Regs:$c, 1), 1), imm:$f))>;"
               << std::endl << std::endl

               << "def : Pat<(i64 (select R1Regs:$c, (i64 imm:$t), R64Regs:$f)),"
               << "(IOR64sss (AND64ssa (SUB64sas 0, (ANDext R1Regs:$c, 1)), imm:$t),"
               << "(AND64sss (SUB64ssa (ANDext R1Regs:$c, 1), 1), R32Regs:$f))>;"
               << std::endl << std::endl;

             os << std::endl
               << "def : Pat<(i64 (select R1Regs:$c, R64Regs:$t, R64Regs:$f)), "
               << "(IOR64sss (AND64sss R64Regs:$t, (SUB64sas 0, (ANDext R1Regs:$c, 1))),"
               << "(AND64sss R64Regs:$f, (SUB64ssa (ANDext R1Regs:$c, 1), 1)))>;"
               << std::endl << std::endl

               << "def : Pat<(i64 (select R1Regs:$c, (i64 imm:$t),(i64 imm:$f))),"
               << "(IOR64sss (AND64ssa (SUB64sas 0, (ANDext R1Regs:$c, 1)), imm:$t),"
               << "(AND64ssa (SUB64ssa (ANDext R1Regs:$c, 1), 1), imm:$f))>;"
               << std::endl << std::endl

               << "def : Pat<(i64 (select R1Regs:$c, R64Regs:$t, (i64 imm:$f))),"
               << "(IOR64sss (AND64sss (SUB64sas 0, (ANDext R1Regs:$c, 1)), R64Regs:$t),"
               << "(AND64ssa (SUB64ssa (ANDext R1Regs:$c, 1), 1), imm:$f))>;"
               << std::endl << std::endl

               << "def : Pat<(i64 (select R1Regs:$c, (i64 imm:$t), R64Regs:$f)),"
               << "(IOR64sss (ANDssa (SUB64sas 0, (ANDext R1Regs:$c, 1)), imm:$t),"
               << "(ANDsss (SUBssa (ANDext R1Regs:$c, 1), 1), R64Regs:$f))>;"
               << std::endl << std::endl

               << "def : Pat<(i1 (select R1Regs:$c, R1Regs:$t, R1Regs:$f)),"
               << "(IOR64bbb (AND64bbb R1Regs:$c, R1Regs:$t), "
               << "(AND64bbb (XOR64bbj R1Regs:$c, 1), R1Regs:$f))>;"
               << std::endl << std::endl

               << "def : Pat<(i1 (select R1Regs:$c, (i1 0), R1Regs:$f)),"
               << "(AND64bbb (XOR64bbj R1Regs:$c, 1), R1Regs:$f)>;"
               << std::endl << std::endl

               << "def : Pat<(i1 (select R1Regs:$c, R1Regs:$t, (i1 -1))),"
               << "(IOR64bbb (AND64bbb R1Regs:$c, R1Regs:$t),"
               << "(XOR64bbj R1Regs:$c, 1))>;"
               << std::endl << std::endl;
            } //else {
#endif

            if (mach_.is64bit()) {
                // optimize select with zero.
                os << "def : Pat<(i64 (select "
                   << condRC << ":$c, R64Regs:$t, (i64 0)))," << std::endl
                   << "          (AND64sss "
                   << trueMask << ", R64Regs:$t)>;"
                   << std::endl << std::endl

                   << "def : Pat<(i64 (select "
                   << condRC << ":$c, (i64 0), R64Regs:$f))," << std::endl
                   << "(AND64sss " << falseMask << ", R64Regs:$f)>;"
                   << std::endl << std::endl;

                // also select with zero and some other imm
                os << "def : Pat<(i64 (select "
                   << condRC << ":$c, (i64 imm:$t),(i64 0)))," << std::endl
                   << "         " << applyTrueMaskOnImm << ">;"
                   << std::endl << std::endl;

                os << "def : Pat<(i64 (select "
                   << condRC << ":$c, (i64 0),(i64 imm:$f)))," << std::endl
                   << "         " << applyFalseMaskOnImm << ">;"
                   << std::endl << std::endl;
            } else {
                // optimize select with zero.
                os << "def : Pat<(i32 (select "
                   << condRC << ":$c, R32Regs:$t, (i32 0)))," << std::endl
                   << "          (ANDrrr "
                   << trueMask << ", R32Regs:$t)>;"
                   << std::endl << std::endl

                   << "def : Pat<(i32 (select "
                   << condRC << ":$c, (i32 0), R32Regs:$f))," << std::endl
                   << "(ANDrrr " << falseMask << ", R32Regs:$f)>;"
                   << std::endl << std::endl;

                // also select with zero and some other imm
                os << "def : Pat<(i32 (select "
                   << condRC << ":$c, (i32 imm:$t),(i32 0)))," << std::endl
                   << "         " << applyTrueMaskOnImm << ">;"
                   << std::endl << std::endl;

                os << "def : Pat<(i32 (select "
                   << condRC << ":$c, (i32 0),(i32 imm:$f)))," << std::endl
                   << "         " << applyFalseMaskOnImm << ">;"
                   << std::endl << std::endl;
            }


            // TODO: similar patterns could be made with -1, avoiding one AND?

            // then the more generic ones
            os << std::endl
               << "def : Pat<(" << defType << " (select "
               << condRC << ":$c, " << gprRegs << ":$t, " << gprRegs << ":$f)), " << std::endl
               << "          (" << iorReg << " (" << andReg << " " << gprRegs << ":$t, "
               << trueMask << "),"
               << "(" << andReg << " " << gprRegs << ":$f, " << falseMask << "))>;"
               << std::endl << std::endl

               << "def : Pat<(" << defType << " (select "
               << condRC << ":$c, (" << defType << " imm:$t),(" << defType << " imm:$f)))," << std::endl
               << "          (" << iorReg << " " << applyTrueMaskOnImm << ","
               << applyFalseMaskOnImm << ")>;"
               << std::endl << std::endl

               << "def : Pat<(" << defType << " (select "
               << condRC << ":$c, " << gprRegs << ":$t, (" << defType << " imm:$f)))," << std::endl
               << "          (" << iorReg << " (" << andReg << " "
               << trueMask << ", " << gprRegs << ":$t)," << applyFalseMaskOnImm << ")>;"
               << std::endl << std::endl

               << "def : Pat<(" << defType << " (select "
               << condRC << ":$c, (" << defType << " imm:$t), " << gprRegs << ":$f))," << std::endl
               << "          (" << iorReg << " " << applyTrueMaskOnImm << ","
               << "(" << andReg << " " << falseMask << ", " << gprRegs << ":$f))>;"
               << std::endl << std::endl;

            if (!regs1bit_.empty()) {
                os << "def : Pat<(i1 (select R1Regs:$c, R1Regs:$t, R1Regs:$f)),"
                   << std::endl
                   << "          (" << iorBool << " (" << andBool << " R1Regs:$c, R1Regs:$t), "
                   << "(" << andBool << " (" << xorBooli << " R1Regs:$c, 1), R1Regs:$f))>;"
                   << std::endl << std::endl

                   << "def : Pat<(i1 (select R1Regs:$c, (i1 0), R1Regs:$f)),"
                   << std::endl
                   << "          (" << andBool << " (" << xorBooli << " R1Regs:$c, 1), R1Regs:$f)>;"
                   << std::endl << std::endl

                   << "def : Pat<(i1 (select R1Regs:$c, R1Regs:$t, (i1 -1))),"
                   << std::endl
                   << "          (" << iorBool << " (" << andBool << " R1Regs:$c, R1Regs:$t),"
                   << "(" << xorBooli << " R1Regs:$c, 1))>;"
                   << std::endl << std::endl;
            }
            
            os << "def : Pat<(f32 (select " << condRC << ":$c, "
                  "FPRegs:$t,FPRegs:$f))," << std::endl
               << "          (IORfff (ANDfff FPRegs:$t, (SUBfir 0, "
               << truncPattern << ")),"
               << "(ANDfff FPRegs:$f, (SUBfri "
               << truncPattern << ",1)))>;"
               << std::endl << std::endl

               << "def : Pat<(f16 (select " << condRC << ":$c, "
                  "R32HFPRegs:$t, R32HFPRegs:$f))," << std::endl
               << "          (IORhhh (ANDhhh R32HFPRegs:$t, (SUBhir 0, "
               << truncPattern << ")),"
               << "(ANDhhh R32HFPRegs:$f, (SUBhri "
               << truncPattern << ",1)))>;"
               << std::endl << std::endl;
//            }
        } else {  // has conditional moves.
            opNames_["SELECT_I1bb"] = "CMOV_SELECT";
            opNames_["SELECT_I1bj"] = "CMOV_SELECT";
            opNames_["SELECT_I1jb"] = "CMOV_SELECT";
            opNames_["SELECT_I1jj"] = "CMOV_SELECT";
            opNames_["SELECT_I32rr"] = "CMOV_SELECT";
            opNames_["SELECT_I32ir"] = "CMOV_SELECT";
            opNames_["SELECT_I32ri"] = "CMOV_SELECT";
            opNames_["SELECT_I32ii"] = "CMOV_SELECT";
            opNames_["SELECT_F32"] = "CMOV_SELECT";
            opNames_["SELECT_F16"] = "CMOV_SELECT";

	    // TODO: why does this break??
	    os << " let isSelect = 1 in {" << std::endl;

            if (mach_.is64bit()) {
                opNames_["SELECT_I64rr"] = "CMOV_SELECT";
                opNames_["SELECT_I64ir"] = "CMOV_SELECT";
                opNames_["SELECT_I64ri"] = "CMOV_SELECT";
                opNames_["SELECT_I64ii"] = "CMOV_SELECT";
                opNames_["SELECT_F64"] = "CMOV_SELECT";



                os  << "def SELECT_I64rr : InstTCE<(outs R64IRegs:$dst),"
                    << "(ins R1Regs:$c, R64IRegs:$T, R64IRegs:$F),"
                    << "\"# SELECT_I64 PSEUDO!\","
                    << "[(set R64IRegs:$dst,"
                    << "(select R1Regs:$c, R64IRegs:$T, R64IRegs:$F))]>;"
                    << std::endl << std::endl

                    << "def SELECT_I64ri : InstTCE<(outs R64IRegs:$dst),"
                    << "(ins R64IRegs:$c, R64IRegs:$T, i64imm:$F),"
                    << "\"# SELECT_I64 PSEUDO!\","
                    << "[(set R64IRegs:$dst,"
                    << "(select R64IRegs:$c, R64IRegs:$T, (i64 imm:$F)))]>;"
                    << std::endl << std::endl

                    << "def SELECT_I64ir : InstTCE<(outs R64IRegs:$dst),"
                    << "(ins R64IRegs:$c, i64imm:$T, R64IRegs:$F),"
                    << "\"# SELECT_I64 PSEUDO!\","
                    << "[(set R64IRegs:$dst,"
                    << "(select R64IRegs:$c, (i64 imm:$T), R64IRegs:$F))]>;"
                    << std::endl << std::endl

                    << "def SELECT_I64ii : InstTCE<(outs R64IRegs:$dst),"
                    << "(ins R64IRegs:$c, i64imm:$T, i64imm:$F),"
                    << "\"# SELECT_I64 PSEUDO!\","
                    << "[(set R64IRegs:$dst,"
                    << "(select R64IRegs:$c, (i64 imm:$T), (i64 imm:$F)))]>;"
                    << std::endl << std::endl

                    << "def SELECT_F64 : InstTCE<(outs R64DFPRegs:$dst),"
                    << "(ins R1Regs:$c, R64DFPRegs:$T, R64DFPRegs:$F),"
                    << "\"# SELECT_F64 PSEUDO!\","
                    << "[(set R64DFPRegs:$dst,"
                    << "(select R1Regs:$c, R64DFPRegs:$T, R64DFPRegs:$F))]>;"
                    << std::endl << std::endl;
            }

            os << "def SELECT_I1bb : InstTCE<(outs R1Regs:$dst),"
               << "(ins GuardRegs:$c, R1Regs:$T, R1Regs:$F),"
               << "\"# SELECT_I1 PSEUDO!\","
               << " [(set R1Regs:$dst,"
               << "(select GuardRegs:$c, R1Regs:$T, R1Regs:$F))]>;"
               << std::endl << std::endl
                
               << "def SELECT_I1bj : InstTCE<(outs R1Regs:$dst),"
               << " (ins GuardRegs:$c, R1Regs:$T, i1imm:$F),"
               << "\"# SELECT_I1 PSEUDO!\","
               << "[(set R1Regs:$dst,"
               << "(select GuardRegs:$c, R1Regs:$T, (i1 imm:$F)))]>;"
               << std::endl << std::endl
                
               << "def SELECT_I1jb : InstTCE<(outs R1Regs:$dst),"
               << "(ins GuardRegs:$c, i1imm:$T, R1Regs:$F),"
               << "\"# SELECT_I1 PSEUDO!\","
               << "[(set R1Regs:$dst," 
               << "(select GuardRegs:$c, (i1 imm:$T), R1Regs:$F))]>;"
               << std::endl << std::endl
                
               << "def SELECT_I1jj : InstTCE<(outs R1Regs:$dst),"
               << "(ins GuardRegs:$c, i1imm:$T, i1imm:$F),"
               << "\"# SELECT_I1 PSEUDO!\","
               << "[(set R1Regs:$dst,"
               << "(select GuardRegs:$c, (i1 imm:$T), (i1 imm:$F)))]>;"
               << std::endl << std::endl
                
               << "def SELECT_I32rr : InstTCE<(outs R32IRegs:$dst),"
               << "(ins GuardRegs:$c, R32IRegs:$T, R32IRegs:$F),"
               << "\"# SELECT_I32 PSEUDO!\","
               << "[(set R32IRegs:$dst,"
               << "(select GuardRegs:$c, R32IRegs:$T, R32IRegs:$F))]>;"
               << std::endl << std::endl
// select with the cond in an i32 (produced by expanded vselects with i32 cond vectors)

               << "def SELECT_I32ri : InstTCE<(outs R32IRegs:$dst),"
               << "(ins R32IRegs:$c, R32IRegs:$T, i32imm:$F),"
               << "\"# SELECT_I32 PSEUDO!\","
               << "[(set R32IRegs:$dst,"
               << "(select R32IRegs:$c, R32IRegs:$T, i32MoveImm:$F))]>;"
               << std::endl << std::endl
                
               << "def SELECT_I32ir : InstTCE<(outs R32IRegs:$dst),"
               << "(ins R32IRegs:$c, i32imm:$T, R32IRegs:$F),"
               << "\"# SELECT_I32 PSEUDO!\","
               << "[(set R32IRegs:$dst,"
               << "(select R32IRegs:$c, i32MoveImm:$T, R32IRegs:$F))]>;"
               << std::endl << std::endl
                
               << "def SELECT_I32ii : InstTCE<(outs R32IRegs:$dst),"
               << "(ins R32IRegs:$c, i32imm:$T, i32imm:$F),"
               << "\"# SELECT_I32 PSEUDO!\","
               << "[(set R32IRegs:$dst,"
               << "(select R32IRegs:$c, i32MoveImm:$T, i32MoveImm:$F))]>;"
               << std::endl << std::endl
                
               << "def SELECT_F32 : InstTCE<(outs FPRegs:$dst),"
               << "(ins GuardRegs:$c, FPRegs:$T, FPRegs:$F),"
               << "\"# SELECT_F32 PSEUDO!\","
               << "[(set FPRegs:$dst,"
               << "(select GuardRegs:$c, FPRegs:$T, FPRegs:$F))]>;"
               << std::endl << std::endl
                
               << "def SELECT_F16 : InstTCE<(outs HFPRegs:$dst),"
               << "(ins GuardRegs:$c, HFPRegs:$T, HFPRegs:$F),"
               << "\"# SELECT_F16 PSEUDO!\","
               << "[(set HFPRegs:$dst, "
               << "(select GuardRegs:$c, HFPRegs:$T, HFPRegs:$F))]>;"
               << std::endl << std::endl;

	    os << "}" << std::endl << std::endl;

	    if (mach_.is64bit()) {
		os << "def : Pat<(i64 (select R64IRegs:$c, R64IRegs:$T, R64IRegs:$F)),"
		   << "(SELECT_I64rr (MOVI64I1ss R64Regs:$c),"
		   << "R64IRegs:$T, R64IRegs:$F)>;"
		   << std::endl << std::endl;
	    }
	    
            os << "def : Pat<(i32 (select R32IRegs:$c, R32IRegs:$T, R32IRegs:$F)),"
               << "(SELECT_I32rr (MOVI32I1rr R32Regs:$c),"
               << "R32IRegs:$T, R32IRegs:$F)>;"
               << std::endl << std::endl;

        }            
    } else {
        os << "// Has select instr!. " << std::endl;
    }
    std::map<TCEString, RegisterClass>::const_iterator it;
    for (it = vRegClasses_.begin(); it != vRegClasses_.end(); ++it) {
        const RegisterClass& regClass = it->second;
        TCEString regClassName = regClass.name();
        ValueType valType = regClass.valueType();
        TCEString valTypeName = valType.valueTypeStr();

        os << "def SELECT_" << valTypeName << " : InstTCE<(outs "
           << regClassName << ":$dst), (ins R1Regs:$c, " << regClassName
           << ":$T, " << regClassName << ":$F), \"\","
           << "[(set " << regClassName << ":$dst,"
           << "(select R1Regs:$c, " << regClassName << ":$T, " << regClassName
           << ":$F))]>;" << std::endl
           << std::endl;

        opNames_["SELECT_" + valTypeName] = "CMOV_SELECT";
    }
}

/**
 * Generates llvm patterns for constants which are not supported directly as
 * immediates by the target machine.
 *
 * For example, if a target does not support sign extended immediates, a pattern
 * is generated that transforms negative constants C to (SUB 0, -C).
 */
void
TDGen::createConstantMaterializationPatterns(std::ostream& os) {

    // TODO: this is used when when not needed!
    // creating unnecessary NEG operations!

    // LLVM XForm fragments //

    // Transformation functions definitions that creates new constants
    // from unsupported constants.
    os << std::endl << "// Arithmetic negation XForm fragment."
       << std::endl << "def aneg_xform : SDNodeXForm<imm, [{"
       << std::endl << "    return CurDAG->getTargetConstant("
       << std::endl << "        -(N->getSExtValue()), SDLoc(N), MVT::i32);"
       << std::endl << "}]>;"
       << std::endl << std::endl;

    // Constant select operand definitions //

    // Used in Constant materialization patterns to select the constant values
    // that are valid for the pattern transformation. The predicate must
    // test that the original constant can be accepted by the materialization
    // pattern itself and test that the resulted constants by XForms are
    // supported too by the target machine.

    // Negative constant materialization: C -> NEG(-C) or C -> SUB(0, -C)
    // selecting the cheapest.
    std::vector<std::string> ops;
    if (opNames_.count("NEGri")) ops.push_back("NEG");
    if (opNames_.count("SUBrri")) ops.push_back("SUB");
    for (auto op : ops) {
        // Predicate that accepts the negative constants
        std::string predicate = "Imm < 0";
        std::pair<int64_t, int64_t> tmp = immInfo_->immediateValueBounds(
            ImmInfoKey{op , 1} , 32);
        if (tmp.second < 2) continue;

        predicate += " && Imm >= -" + Conversion::toString(tmp.second-1);

        writeImmediateDef(os, "minus_i32imm", "i32", predicate);
        // store predicates for the materialization queries by ISelLowering,
        // where decisions to bail unsupported constants to constant pool are
        // made. The stored predicates are written to Backend.inc later on.
        constantMaterializationPredicates_.push_back(predicate);

        os << std::endl << "def : Pat<(i32 minus_i32imm:$imm)," << std::endl;
        if (op == "NEG") {
            os << "          (NEGri (aneg_xform imm:$imm))>;";
        } else if (op == "SUB") {
            os << "          (SUBrri (MOVI32ri 0), (aneg_xform imm:$imm))>;";
        }
        os << std::endl;
        break; // We need only the cheapest materialization pattern.
    }
}

/**
 * Writes query method for retrieving LLVM instruction for pointer adjustment
 * and suitable offset value.
 *
 * Some machines may not SUBrri definition (second operand can not take an
 * immediate) and, therefore, pointer adjustments must use other operation
 * for it.
 */
void
TDGen::writeGetPointerAdjustmentQuery(std::ostream& os) const {

    TCEString ADDIMM = mach_.is64bit() ? "ADD64ssa" : "ADDrri";
    TCEString SUBIMM = mach_.is64bit() ? "SUB64ssa" : "SUBrri";



    boost::format queryTmpl(
        "// <MI opcode, adjusted offset>\n"
        "std::tuple<int, int> GeneratedTCEPlugin::getPointerAdjustment(\n"
        "    int offset) const {\n"
        "    if (offset > 0)  // Adjust pointer up.\n"
        "        return std::make_tuple(TCE::%1%, offset);\n"
        "    else\n"
        "        return std::make_tuple(TCE::%2%, %3%);\n"
        "}\n");

    // Prefer non-negative offset
    if (opNames_.count(SUBIMM)) {
        os << queryTmpl % ADDIMM % SUBIMM % "-offset" << std::endl;
        return;
    } else if (opNames_.count("ADDrri")) {
        os << queryTmpl % ADDIMM % ADDIMM % "offset" << std::endl;
        return;
    }

    THROW_EXCEPTION(
        InvalidData,
        "The machine is missing ADD or SUB operation with direct\n"
        "immediate source for pointer adjustment operations.");
    return;
}


/**
 * Returns llvm predicate expression for short immediate constraint.
 */
std::string
TDGen::immediatePredicate(
    int64_t lowerBoundInclusive,
    uint64_t upperBoundInclusive) {

    // If can transport any 64-bit imm, no need for any checks.
    if (upperBoundInclusive == UINT64_MAX ||
        lowerBoundInclusive == INT64_MIN) {
        return std::string("(true)");
    }

    return std::string("(")
        + Conversion::toString(lowerBoundInclusive) + "ll"
        + " <= Imm && Imm <= "
        + Conversion::toString(upperBoundInclusive) + "ll)";
}

/**
 * Returns corresponding immediate operand name for the emulated operation.
 *
 * @param dag The emulation code of the emulated operation.
 * @param operand The operand.
 * @return The operand immediate name if found. Otherwise return empty string.
 */
std::string
TDGen::immediateOperandNameForEmulatedOperation(
    const OperationDAG& dag,
    const Operand& operand) {

    for (const auto& sourceNode : dag.rootNodes()) {
        const TerminalNode* sourceTerminal =
            dynamic_cast<TerminalNode*>(sourceNode);

        if (sourceTerminal == nullptr) continue;

        for (const auto& destEdge : dag.outEdges(*sourceTerminal)) {
            const OperationNode* opNode =
                dynamic_cast<OperationNode*>(&dag.headNode(*destEdge));

            assert(opNode != nullptr &&
                "TerminalNode points to other than OperandNode.");
            const Operation& operation = opNode->referencedOperation();
            ImmInfoKey key = ImmInfo::key(operation, operand);
            if (immOperandDefs_.count(key)) {
                return immOperandDefs_.at(key);
            } else {
                return "";
            }
        }
    }
    return "";
}

/**
 * Checks if the operation can have the specified immediate operands.
 *
 */
bool
TDGen::areImmediateOperandsLegal(
        const Operation& operation,
        const std::string& operandTypes) const {

    assert(static_cast<int>(operandTypes.size()) >= operation.operandCount());

    for (int i = 0 ; i < operation.numberOfInputs(); i++) {
        if (operandTypes.at(i + operation.numberOfOutputs()) != OT_IMM_INT) {
            continue;
        }

        const Operand& operand = operation.operand(i+1);

        // Kludge fix for ADDri/MEMri operands. LLVM wants some times to select
        // store/load instruction, where the address operand is immediate.
        // However, the machine may not have capability to directly transport
        // immediate to the address operand. Considering the case as illegal
        // will reject operation definitions using ADDri/MEMri operands and
        // breaks the instruction selection later on. Temporarily accept
        // ADDri/MEMri operands until the issue is fixed.
        if (operand.isAddress()) {
            continue;
        }

        if (immInfo_->count(operation, operand) == 0) {
            for (int swappableWith : operand.swap()) {
                const Operand& otherOperand = operation.operand(swappableWith);
                if (immInfo_->count(operation, otherOperand)
                    && operation.name() != "EQ"
                    && operation.name() != "NE") {
                    // ^ EQ and NE are not commutative in LLVM
                    return true;
                }
            }
            return false;
        }
    }

    return true;
}


/**
 * Returns true if the instruction can be predicated.
 *
 * TODO
 */
bool
TDGen::canBePredicated(Operation& /*op*/, const std::string& operandTypes) {

    // Predicating operands with immediates can currently lead to
    // unschedulable code in case there's no bus that has both the
    // predicate and the immediate transfer capability. Disable
    // generating the predicated versions for immediate operand
    // patterns for now.
    if (operandTypes.find(OT_IMM_INT) != std::string::npos) return false;

    if (hasConditionalMoves_) return true;

    return false;
}

/**
 * Returns OSAL operation names valid for stack accesses.
 *
 * @param mach The target machine.
 */
std::vector<std::string>
TDGen::supportedStackAccessOperations(const TTAMachine::Machine& mach) {

    using namespace TTAMachine;

    std::vector<std::string> opcodes;

    auto collectMemoryOps = [](
        std::vector<std::string>& newOpcodes,
        const FunctionUnit& fu) -> void {

        for (int i = 0; i < fu.operationCount(); i++) {
            const Operation& op = MachineInfo::osalOperation(*fu.operation(i));
            if (!op.readsMemory() && !op.writesMemory()) {
                continue;
            }
            newOpcodes.push_back(StringTools::stringToUpper(op.name()));
        }
    };

    std::map<const AddressSpace*, std::set<const FunctionUnit*>>
       addrSpaceLsuMap;
    for (const FunctionUnit* fu : mach.functionUnitNavigator()) {
        if (fu->hasAddressSpace()) {
            addrSpaceLsuMap[fu->addressSpace()].insert(fu);
        }
    }

    const int stackAddressSpaceId = 0;
    if (addrSpaceLsuMap.size() == 1) {
        // Only one data address space. Stack resides there.
        for (auto lsu : addrSpaceLsuMap.begin()->second) {
            collectMemoryOps(opcodes, *lsu);
        }
    } else {
        // Multiple data address spaces. Find LSUs having the stack address
        // space ID.
        for (auto asLsusPair : addrSpaceLsuMap) {
            if (asLsusPair.first->hasNumericalId(stackAddressSpaceId)) {
                for (auto lsu : asLsusPair.second) {
                    collectMemoryOps(opcodes, *lsu);
                }
                break;
            }
        }
    }

    return opcodes;
}

/**
 * Returns LLVM pattern for the expression with constant value(s) supported by
 * target machine.
 *
 * For example, for pattern (SUBrri (foo ...), 123) the method returns
 * (SUBrrr (foo ...), (MOVI32ri 123)) if the target machine can not supply
 * the constant "123" as an immediate.
 *
 * The method should be called after all operation definitions have been
 * created (after all calls to writeOperationDef()).
 *
 * Return empty string if the given pattern can not be handled because:
 * - Operation is not supported by target machine.
 *
 * @param op The operation.
 * @param operandTypes The preferred operand types.
 * @param operand0 The pattern for the first operand or constant indicated by
 *                 operandTypes.
 * @param operand1 The pattern for the second operand or constant indicated by
 *                 operandTypes.
 */
TCEString
TDGen::getLLVMPatternWithConstants(
    const Operation& op, const std::string& operandTypes,
    const std::string& operand0,
    const std::string& operand1) const {

    //TODO If operation is not supported, try to replace it with an emulation
    //     pattern first and then deal with the new patterns with constant
    //     operands (and take account possible infinite recursion).
    //TODO Cache the calculated results?

    assert(op.numberOfInputs() == 2);

    auto makePattern = [](
        const std::string& opc,
        const std::string& opdTypes,
        const std::string& operand0,
        const std::string& operand1) -> TCEString {
            return std::string("(") + opc + opdTypes + " " + operand0
            + ", " + operand1 + ")";
        };

    std::string opc = StringTools::stringToUpper(op.name());
    const std::string outputOpdTypes = operandTypes.substr(
        0, op.numberOfOutputs());
    const std::string inputOpdTypes = operandTypes.substr(
        op.numberOfOutputs());
    assert(inputOpdTypes.size() == 2);
    const auto regOperandsOnly = operandTypesToRegisters(operandTypes);
    if (!opNames_.count(opc + regOperandsOnly)) {
        // Operation is not supported at all by the target machine.
        return "";
    }

    if (opNames_.count(opc + operandTypes)) {
        // Supported as is.
        return makePattern(opc, operandTypes, operand0, operand1);
    }

    std::vector<std::string> supportedOperandTypes;
    for (auto i = 0u; i < inputOpdTypes.size(); i++) {
        std::string oneInputChanged = inputOpdTypes;
        oneInputChanged[i] = operandTypeToRegister(oneInputChanged[i]);
        if (oneInputChanged == inputOpdTypes) continue;
        if (opNames_.count(opc + outputOpdTypes + oneInputChanged)) {
            supportedOperandTypes.push_back(oneInputChanged);
        }
    }

    supportedOperandTypes.push_back(regOperandsOnly);

    std::vector<const std::string*> inputOpdValues{&operand0, &operand1};

    // Now legalize pattern by changing unsupported immediate operands with
    // immediate moves.
    for (const auto& supportedOpdType : supportedOperandTypes) {

        // Loop throught immediate opd types. Check if constant is encodable
        // for the operation.
        bool isSupported = true;
        for (auto i = 0u; i < supportedOpdType.size(); i++) {
            auto& opdType = supportedOpdType.at(i);
            auto osalOpdIdx = i - op.numberOfInputs() + 1;

            bool ok = false;
            switch (opdType) {
                default:
                    ok = true; // Ok. Not immediate type.
                    break;
                case OT_IMM_INT: {
                    bool isOperandReference =
                        inputOpdValues.at(i)->find(":$") != std::string::npos;
                    if (isOperandReference) {
                        // Size of the constant is not known, must expect the
                        // worst.
                        ok = false;
                        break;
                    }
                    int64_t value = Conversion::toInt(
                        *inputOpdValues.at(i));
                    ok = immInfo_->canTakeImmediate(op, osalOpdIdx, value,
                                                    32);
                }
                    break;
                case OT_IMM_FP:
                    ok = immInfo_->canTakeImmediateByWidth(
                        op, osalOpdIdx, 32);
                    break;
                case OT_IMM_HFP:
                    ok = immInfo_->canTakeImmediateByWidth(
                        op, osalOpdIdx, 16);
                    break;
                case OT_IMM_BOOL:
                    ok = immInfo_->canTakeImmediateByWidth(
                        op, osalOpdIdx, 1);
                    break;
            }
            if (!ok) {
                isSupported = false;
                break;
            }
        }

        if (!isSupported) continue;

        std::string result = std::string("(") + opc + outputOpdTypes
            + supportedOpdType + " ";
        if (supportedOpdType.at(0) == inputOpdTypes.at(0)) {
            result += *inputOpdValues.at(0);
        } else {
            result += getMovePattern(
                inputOpdTypes.at(0), *inputOpdValues.at(0));
        }
        for (auto i = 1u; i < supportedOpdType.size(); i++) {
            result += ", ";
            if (supportedOpdType.at(i) == inputOpdTypes.at(i)) {
                result += *inputOpdValues.at(i);
            } else {
                result += getMovePattern(
                    inputOpdTypes.at(i), *inputOpdValues.at(i));
            }
        }
        result += ")";
        return result;
    }

    return "";
}

std::string
TDGen::operandTypesToRegisters(const std::string& opdTypes) const {
    std::string result(opdTypes);
    for (char& type : result) {
        type = operandTypeToRegister(type);
    }
    return result;
}

char
TDGen::operandTypeToRegister(const char& opdType) const {
    switch (opdType) {
        case OT_IMM_INT: return OT_REG_INT; // Integer immediate.
        case OT_IMM_BOOL: return OT_REG_BOOL; // Boolean immediate.
        case OT_IMM_FP: return OT_REG_FP; // 32-bit floating point immediate.
        case OT_IMM_HFP: return OT_REG_HFP; // 15-bi floating point immediate.
        // TODO: vector immediate types.
        default:
            break;
    }
    return opdType; // Return as is (should be register type).
}


TCEString
TDGen::getMovePattern(
    const char& opdType, const std::string& inputPattern) const {

    TCEString pat = "(MOV";
    // Note: This listing is incomplete.
    switch (opdType) {
    case OT_IMM_INT: // Integer immediate.
        pat += std::string("I32") + operandTypeToRegister(opdType) + opdType;
        break;
    case OT_REG_INT: // Integer register.
        pat += std::string("I32") + operandTypeToRegister(opdType) + opdType;
        break;
    case OT_IMM_BOOL: // Boolean immediate.
        pat += "I1ri";  //TODO: this is inconsistent
        break;
    case OT_REG_BOOL: // Boolean register.
        pat += "I1rr"; //TODO: this is inconsistent
        break;
    case OT_IMM_FP: // Single precision floating point immediate.
    case OT_REG_FP: // -||- register.
        pat += std::string("F32") + operandTypeToRegister(opdType) + opdType;
        break;
    case OT_IMM_HFP: // Half precision floating point immediate.
    case OT_REG_HFP: // -||- register.
        pat += std::string("F16") + operandTypeToRegister(opdType) + opdType;
        break;
        // TODO: vector register and immediate types.
    default:
        assert(false && "Handling for a type not implemented");
        break;
    }
    pat += " " + inputPattern + ")";
    return pat;
}

void TDGen::writeCallSeqStart(std::ostream& os) {

    bool is64bit = mach_.is64bit();

  if (!is64bit) {
      os << "def SDT_TCECallSeqStart : SDCallSeqStart<[ SDTCisVT<0, i32>,"
         << "SDTCisVT<1, i32> ]>;" << std::endl << std::endl;
  } else {
      os << "def SDT_TCECallSeqStart : SDCallSeqStart<[ SDTCisVT<0, i64>,"
         << "SDTCisVT<1, i64> ]>;" << std::endl << std::endl;
  }
  os << "def callseq_start : SDNode<\"ISD::CALLSEQ_START\", "
     << "SDT_TCECallSeqStart, [SDNPHasChain, SDNPOutGlue]>;" << std::endl
     << std::endl
     << "let Defs = [SP], Uses = [SP] in {" << std::endl
     << "def ADJCALLSTACKDOWN : Pseudo<(outs),";
  if (!is64bit) {
      os << "(ins i32imm:$amt1, i32imm:$amt2),";
  } else {
      os << "(ins i64imm:$amt1, i64imm:$amt2),";
  }
  os << "\"# ADJCALLSTACKDOWN $amt1, $amt2\","
     << "[(callseq_start timm:$amt1, timm:$amt2)]>;}"
     << std::endl << std::endl;
}

void TDGen::createBranchAnalysis(std::ostream& os) {
    os << "bool GeneratedTCEPlugin::analyzeCCBranch(" << std::endl
       << "\tllvm::MachineInstr& i," << std::endl
       << "\tllvm::SmallVectorImpl<llvm::MachineOperand>& cond) const {"
       << std::endl
       << std::endl;

    if (!MachineInfo::supportsBoolRegisterGuardedJumps(mach_) &&
        MachineInfo::supportsPortGuardedJumps(mach_)) {

        os << "\tif (i.getOpcode() == TCE::EQ_JUMP) {" << std::endl
           << "\t\tcond.push_back(i.getOperand(0));" << std::endl
           << "\t\tcond.push_back(i.getOperand(1));" << std::endl
           << "\t\tcond.push_back(MachineOperand::CreateImm(2));"
           << std::endl
           << "\t\treturn false; }" << std::endl;

        os << "\tif (i.getOpcode() == TCE::NE_JUMP) {" << std::endl
           << "\t\tcond.push_back(i.getOperand(0));" << std::endl
           << "\t\tcond.push_back(i.getOperand(1));" << std::endl
           << "\t\tcond.push_back(MachineOperand::CreateImm(3));"
           << std::endl
           << "\t\treturn false; }" << std::endl;

        os << "\tif (i.getOpcode() == TCE::GT_JUMP) {" << std::endl
           << "\t\tcond.push_back(i.getOperand(0));" << std::endl
           << "\t\tcond.push_back(i.getOperand(1));" << std::endl
           << "\t\tcond.push_back(MachineOperand::CreateImm(4));"
           << std::endl
           << "\t\treturn false; }" << std::endl;

        os << "\tif (i.getOpcode() == TCE::LE_JUMP) {" << std::endl
           << "\t\tcond.push_back(i.getOperand(0));" << std::endl
           << "\t\tcond.push_back(i.getOperand(1));" << std::endl
           << "\t\tcond.push_back(MachineOperand::CreateImm(5));"
           << std::endl
           << "\t\treturn false; }" << std::endl;

        os << "\tif (i.getOpcode() == TCE::GTU_JUMP) {" << std::endl
           << "\t\tcond.push_back(i.getOperand(0));" << std::endl
           << "\t\tcond.push_back(i.getOperand(1));" << std::endl
           << "\t\tcond.push_back(MachineOperand::CreateImm(6));"
           << std::endl
           << "\t\treturn false; }" << std::endl;

        os << "\tif (i.getOpcode() == TCE::LEU_JUMP) {" << std::endl
           << "\t\tcond.push_back(i.getOperand(0));" << std::endl
           << "\t\tcond.push_back(i.getOperand(1));" << std::endl
           << "\t\tcond.push_back(MachineOperand::CreateImm(7));"
           << std::endl
           << "\t\treturn false; }" << std::endl;

        if (opNames_.find("EQF_JUMP") != opNames_.end()) {
            os << "\tif (i.getOpcode() == TCE::EQF_JUMP) {" << std::endl
               << "\t\tcond.push_back(i.getOperand(0));" << std::endl
               << "\t\tcond.push_back(i.getOperand(1));" << std::endl
               << "\t\tcond.push_back(MachineOperand::CreateImm(8));"
               << std::endl
               << "\t\treturn false; }" << std::endl;
        }

        if (opNames_.find("NEF_JUMP") != opNames_.end()) {
            os << "\tif (i.getOpcode() == TCE::NEF_JUMP) {" << std::endl
               << "\t\tcond.push_back(i.getOperand(0));" << std::endl
               << "\t\tcond.push_back(i.getOperand(1));" << std::endl
               << "\t\tcond.push_back(MachineOperand::CreateImm(9));"
               << std::endl
               << "\t\treturn false; }" << std::endl;
        }

        if (opNames_.find("LEF_JUMP") != opNames_.end()) {
            os << "\tif (i.getOpcode() == TCE::LEF_JUMP) {" << std::endl
               << "\t\tcond.push_back(i.getOperand(0));" << std::endl
               << "\t\tcond.push_back(i.getOperand(1));" << std::endl
               << "\t\tcond.push_back(MachineOperand::CreateImm(10));"
               << std::endl
               << "\t\treturn false; }" << std::endl;
        }

        if (opNames_.find("GEF_JUMP") != opNames_.end()) {
            os << "\tif (i.getOpcode() == TCE::GEF_JUMP) {" << std::endl
               << "\t\tcond.push_back(i.getOperand(0));" << std::endl
               << "\t\tcond.push_back(i.getOperand(1));" << std::endl
               << "\t\tcond.push_back(MachineOperand::CreateImm(10));"
               << std::endl
               << "\t\treturn false; }" << std::endl;
        }

        if (opNames_.find("LTF_JUMP") != opNames_.end()) {
            os << "\tif (i.getOpcode() == TCE::LTF_JUMP) {" << std::endl
               << "\t\tcond.push_back(i.getOperand(0));" << std::endl
               << "\t\tcond.push_back(i.getOperand(1));" << std::endl
               << "\t\tcond.push_back(MachineOperand::CreateImm(12));"
               << std::endl
               << "\t\treturn false; }" << std::endl;
        }

        if (opNames_.find("GTF_JUMP") != opNames_.end()) {
            os << "\tif (i.getOpcode() == TCE::GTF_JUMP) {" << std::endl
               << "\t\tcond.push_back(i.getOperand(0));" << std::endl
               << "\t\tcond.push_back(i.getOperand(1));" << std::endl
               << "\t\tcond.push_back(MachineOperand::CreateImm(13));"
               << std::endl
               << "\t\treturn false; }" << std::endl;
        }
    }

    if (!MachineInfo::supportsBoolRegisterGuardedJumps(mach_) &&
        !MachineInfo::supportsPortGuardedJumps(mach_)) {
        if (opNames_.count("TCEBREQrr")) {
            os << "\tif (i.getOpcode() == TCE::TCEBREQrr) {" << std::endl
               << "\t\tcond.push_back(i.getOperand(0));" << std::endl
               << "\t\tcond.push_back(i.getOperand(1));" << std::endl
               << "\t\tcond.push_back(MachineOperand::CreateImm(2));"
               << std::endl
               << "\t\treturn false;" << std::endl
               << "\t}" << std::endl;

            os << "\tif (i.getOpcode() == TCE::TCEBREQri) {" << std::endl
               << "\t\tcond.push_back(i.getOperand(0));" << std::endl
               << "\t\tcond.push_back(i.getOperand(1));" << std::endl
               << "\t\tcond.push_back(MachineOperand::CreateImm(102));"
               << std::endl
               << "\t\treturn false;" << std::endl
               << "\t}" << std::endl;
        }

        if (opNames_.count("TCEBRNErr")) {
            os << "\tif (i.getOpcode() == TCE::TCEBRNErr) {" << std::endl
               << "\t\tcond.push_back(i.getOperand(0));" << std::endl
               << "\t\tcond.push_back(i.getOperand(1));" << std::endl
               << "\t\tcond.push_back(MachineOperand::CreateImm(3));"
               << std::endl
               << "\t\treturn false;" << std::endl
               << "\t}" << std::endl;

            os << "\tif (i.getOpcode() == TCE::TCEBRNEri) {" << std::endl
               << "\t\tcond.push_back(i.getOperand(0));" << std::endl
               << "\t\tcond.push_back(i.getOperand(1));" << std::endl
               << "\t\tcond.push_back(MachineOperand::CreateImm(103));"
               << std::endl
               << "\t\treturn false;" << std::endl
               << "\t}" << std::endl;
        }

        if (opNames_.count("TCEBRGTrr")) {
            os << "\tif (i.getOpcode() == TCE::TCEBRGTrr) {" << std::endl
               << "\t\tcond.push_back(i.getOperand(0));" << std::endl
               << "\t\tcond.push_back(i.getOperand(1));" << std::endl
               << "\t\tcond.push_back(MachineOperand::CreateImm(4));"
               << std::endl
               << "\t\treturn false;" << std::endl
               << "\t}" << std::endl;

            os << "\tif (i.getOpcode() == TCE::TCEBRGTri) {" << std::endl
               << "\t\tcond.push_back(i.getOperand(0));" << std::endl
               << "\t\tcond.push_back(i.getOperand(1));" << std::endl
               << "\t\tcond.push_back(MachineOperand::CreateImm(104));"
               << std::endl
               << "\t\treturn false;" << std::endl
               << "\t}" << std::endl;
        }

        if (opNames_.count("TCEBRGTUrr")) {
            os << "\tif (i.getOpcode() == TCE::TCEBRGTUrr) {" << std::endl
               << "\t\tcond.push_back(i.getOperand(0));" << std::endl
               << "\t\tcond.push_back(i.getOperand(1));" << std::endl
               << "\t\tcond.push_back(MachineOperand::CreateImm(6));"
               << std::endl
               << "\t\treturn false;" << std::endl
               << "\t}" << std::endl;

            os << "\tif (i.getOpcode() == TCE::TCEBRGTUri) {" << std::endl
               << "\t\tcond.push_back(i.getOperand(0));" << std::endl
               << "\t\tcond.push_back(i.getOperand(1));" << std::endl
               << "\t\tcond.push_back(MachineOperand::CreateImm(106));"
               << std::endl
               << "\t\treturn false;" << std::endl
               << "\t}" << std::endl;
        }

        if (opNames_.count("TCEBRLTrr")) {
            os << "\tif (i.getOpcode() == TCE::TCEBRLTrr) {" << std::endl
               << "\t\tcond.push_back(i.getOperand(0));" << std::endl
               << "\t\tcond.push_back(i.getOperand(1));" << std::endl
               << "\t\tcond.push_back(MachineOperand::CreateImm(14));"
               << std::endl
               << "\t\treturn false;" << std::endl
               << "\t}" << std::endl;

            os << "\tif (i.getOpcode() == TCE::TCEBRLTri) {" << std::endl
               << "\t\tcond.push_back(i.getOperand(0));" << std::endl
               << "\t\tcond.push_back(i.getOperand(1));" << std::endl
               << "\t\tcond.push_back(MachineOperand::CreateImm(114));"
               << std::endl
               << "\t\treturn false;" << std::endl
               << "\t}" << std::endl;
        }

        if (opNames_.count("TCEBRLTUrr")) {
            os << "\tif (i.getOpcode() == TCE::TCEBRLTUrr) {" << std::endl
               << "\t\tcond.push_back(i.getOperand(0));" << std::endl
               << "\t\tcond.push_back(i.getOperand(1));" << std::endl
               << "\t\tcond.push_back(MachineOperand::CreateImm(15));"
               << std::endl
               << "\t\treturn false;" << std::endl
               << "\t}" << std::endl;

            os << "\tif (i.getOpcode() == TCE::TCEBRLTUri) {" << std::endl
               << "\t\tcond.push_back(i.getOperand(0));" << std::endl
               << "\t\tcond.push_back(i.getOperand(1));" << std::endl
               << "\t\tcond.push_back(MachineOperand::CreateImm(115));"
               << std::endl
               << "\t\treturn false;" << std::endl
               << "\t}" << std::endl;
        }

        if (opNames_.count("TCEBRLErr")) {
            os << "\tif (i.getOpcode() == TCE::TCEBRLErr) {" << std::endl
               << "\t\tcond.push_back(i.getOperand(0));" << std::endl
               << "\t\tcond.push_back(i.getOperand(1));" << std::endl
               << "\t\tcond.push_back(MachineOperand::CreateImm(5));"
               << std::endl
               << "\t\treturn false;" << std::endl
               << "\t}" << std::endl;

            os << "\tif (i.getOpcode() == TCE::TCEBRLEri) {" << std::endl
               << "\t\tcond.push_back(i.getOperand(0));" << std::endl
               << "\t\tcond.push_back(i.getOperand(1));" << std::endl
               << "\t\tcond.push_back(MachineOperand::CreateImm(105));"
               << std::endl
               << "\t\treturn false;" << std::endl
               << "\t}" << std::endl;
        }

        if (opNames_.count("TCEBRLEUrr")) {
            os << "\tif (i.getOpcode() == TCE::TCEBRLEUrr) {" << std::endl
               << "\t\tcond.push_back(i.getOperand(0));" << std::endl
               << "\t\tcond.push_back(i.getOperand(1));" << std::endl
               << "\t\tcond.push_back(MachineOperand::CreateImm(7));"
               << std::endl
               << "\t\treturn false;" << std::endl
               << "\t}" << std::endl;

            os << "\tif (i.getOpcode() == TCE::TCEBRLEUri) {" << std::endl
               << "\t\tcond.push_back(i.getOperand(0));" << std::endl
               << "\t\tcond.push_back(i.getOperand(1));" << std::endl
               << "\t\tcond.push_back(MachineOperand::CreateImm(107));"
               << std::endl
               << "\t\treturn false;" << std::endl
               << "\t}" << std::endl;
        }

        if (opNames_.count("TCEBRGErr")) {
            os << "\tif (i.getOpcode() == TCE::TCEBRGErr) {" << std::endl
               << "\t\tcond.push_back(i.getOperand(0));" << std::endl
               << "\t\tcond.push_back(i.getOperand(1));" << std::endl
               << "\t\tcond.push_back(MachineOperand::CreateImm(16));"
               << std::endl
               << "\t\treturn false;" << std::endl
               << "\t}" << std::endl;

            os << "\tif (i.getOpcode() == TCE::TCEBRGEri) {" << std::endl
               << "\t\tcond.push_back(i.getOperand(0));" << std::endl
               << "\t\tcond.push_back(i.getOperand(1));" << std::endl
               << "\t\tcond.push_back(MachineOperand::CreateImm(116));"
               << std::endl
               << "\t\treturn false;" << std::endl
               << "\t}" << std::endl;
        }

        if (opNames_.count("TCEBRGEUrr")) {
            os << "\tif (i.getOpcode() == TCE::TCEBRGEUrr) {" << std::endl
               << "\t\tcond.push_back(i.getOperand(0));" << std::endl
               << "\t\tcond.push_back(i.getOperand(1));" << std::endl
               << "\t\tcond.push_back(MachineOperand::CreateImm(17));"
               << std::endl
               << "\t\treturn false;" << std::endl
               << "\t}" << std::endl;

            os << "\tif (i.getOpcode() == TCE::TCEBRGEUri) {" << std::endl
               << "\t\tcond.push_back(i.getOperand(0));" << std::endl
               << "\t\tcond.push_back(i.getOperand(1));" << std::endl
               << "\t\tcond.push_back(MachineOperand::CreateImm(117));"
               << std::endl
               << "\t\treturn false;" << std::endl
               << "\t}" << std::endl;
        }
    }
    os << "\treturn true;"
       << "}" << std::endl << std::endl;

    // insert branch

    os << "#include <llvm/CodeGen/MachineInstrBuilder.h>" << std::endl
       << std::endl;

    os << "void TCEInstrInfo::insertCCBranch( " << std::endl
       << "\tMachineBasicBlock& mbb," << std::endl
       << "\tMachineBasicBlock& tbb," << std::endl
       << "\tArrayRef<MachineOperand> cond," << std::endl
       << "const DebugLoc& dl) const {" << std::endl
       << "\tassert(cond.size() == 3);" << std::endl;

    os << "\tint opcode;" << std::endl;

    if (!MachineInfo::supportsBoolRegisterGuardedJumps(mach_) &&
        MachineInfo::supportsPortGuardedJumps(mach_)) {
        os << "\tswitch (cond[2].getImm()) {" << std::endl
           << "\t\tcase 2: opcode = TCE::EQ_JUMP;break;" << std::endl
           << "\t\tcase 3: opcode = TCE::NE_JUMP;break;" << std::endl
           << "\t\tcase 4: opcode = TCE::GT_JUMP;break;" << std::endl
           << "\t\tcase 5: opcode = TCE::LE_JUMP;break;" << std::endl
           << "\t\tcase 6: opcode = TCE::GTU_JUMP;break;" << std::endl
           << "\t\tcase 7: opcode = TCE::LEU_JUMP;break;" << std::endl;

        if (opNames_.find("EQF_JUMP") != opNames_.end()) {
            os << "\t\tcase 8: opcode = TCE::EQF_JUMP;break;" << std::endl;
        }
        if (opNames_.find("NEF_JUMP") != opNames_.end()) {
            os << "\t\tcase 9: opcode = TCE::NEF_JUMP;break;" << std::endl;
        }
        if (opNames_.find("LEF_JUMP") != opNames_.end()) {
            os << "\t\tcase 10: opcode = TCE::LEF_JUMP;break;" << std::endl;
        }
        if (opNames_.find("GEF_JUMP") != opNames_.end()) {
            os << "\t\tcase 11: opcode = TCE::GEF_JUMP;break;" << std::endl;
        }
        if (opNames_.find("LTF_JUMP") != opNames_.end()) {
            os << "\t\tcase 12: opcode = TCE::LTF_JUMP;break;" << std::endl;
        }
        if (opNames_.find("GEF_JUMP") != opNames_.end()) {
            os << "\t\tcase 13: opcode = TCE::GTF_JUMP;break;" << std::endl;
        }

        os << "\t\tdefault: assert(false && \"Unknown condition code\");}"
           << std::endl;
    } else if (!MachineInfo::supportsBoolRegisterGuardedJumps(mach_) &&
               !MachineInfo::supportsPortGuardedJumps(mach_)) {
        os << "\tswitch (cond[2].getImm()) {" << std::endl;
        if (opNames_.count("TCEBREQrr")) {
            os << "\t\tcase 2: opcode = TCE::TCEBREQrr; break;" << std::endl
               << "\t\tcase 102: opcode = TCE::TCEBREQri; break;"
               << std::endl;
        }
        if (opNames_.count("TCEBRNErr")) {
            os << "\t\tcase 3: opcode = TCE::TCEBRNErr; break;" << std::endl
               << "\t\tcase 103: opcode = TCE::TCEBRNEri; break;"
               << std::endl;
        }
        if (opNames_.count("TCEBRGTrr")) {
            os << "\t\tcase 4: opcode = TCE::TCEBRGTrr; break;" << std::endl
               << "\t\tcase 104: opcode = TCE::TCEBRGTri; break;"
               << std::endl;
        }
        if (opNames_.count("TCEBRGTUrr")) {
            os << "\t\tcase 6: opcode = TCE::TCEBRGTUrr; break;" << std::endl
               << "\t\tcase 106: opcode = TCE::TCEBRGTUri; break;"
               << std::endl;
        }
        if (opNames_.count("TCEBRLTrr")) {
            os << "\t\tcase 14: opcode = TCE::TCEBRLTrr; break;" << std::endl
               << "\t\tcase 114: opcode = TCE::TCEBRLTri; break;"
               << std::endl;
        }
        if (opNames_.count("TCEBRLTUrr")) {
            os << "\t\tcase 15: opcode = TCE::TCEBRLTUrr; break;" << std::endl
               << "\t\tcase 115: opcode = TCE::TCEBRLTUri; break;"
               << std::endl;
        }
        if (opNames_.count("TCEBRLErr")) {
            os << "\t\tcase 5: opcode = TCE::TCEBRLErr; break;" << std::endl
               << "\t\tcase 105: opcode = TCE::TCEBRLEri; break;"
               << std::endl;
        }
        if (opNames_.count("TCEBRLEUrr")) {
            os << "\t\tcase 7: opcode = TCE::TCEBRLEUrr; break;" << std::endl
               << "\t\tcase 107: opcode = TCE::TCEBRLEUri; break;"
               << std::endl;
        }
        if (opNames_.count("TCEBRGErr")) {
            os << "\t\tcase 16: opcode = TCE::TCEBRGErr; break;" << std::endl
               << "\t\tcase 116: opcode = TCE::TCEBRGEri; break;"
               << std::endl;
        }
        if (opNames_.count("TCEBRGEUrr")) {
            os << "\t\tcase 17: opcode = TCE::TCEBRGEUrr; break;" << std::endl
               << "\t\tcase 117: opcode = TCE::TCEBRGEUri; break;"
               << std::endl;
        }
        os << "\t\tdefault: assert(false && \"Unknown condition code\");}"
           << std::endl;
    } else {
        os << "\tassert(false && \"Unknown condition code\");" << std::endl;
    }

    os << "\tif (cond[1].isReg()) {" << std::endl
       << "\t\tBuildMI(&mbb, dl, get(opcode)).addReg(cond[0].getReg())"
       << std::endl
       << "\t\t       .addReg(cond[1].getReg()).addMBB(&tbb);" << std::endl
       << "\t} else {" << std::endl
       << "\t\tBuildMI(&mbb, dl, get(opcode)).addReg(cond[0].getReg())"
       << std::endl
       << "\t\t        .addImm(cond[1].getImm()).addMBB(&tbb);" << std::endl
       << "\t}" << std::endl;
    os << "}";
}

void TDGen::write64bitMoveDefs(std::ostream& o) {
    o << std::endl << "// 64-bit register->register move definitions."
      << std::endl << "let isAsCheapAsAMove = 1 in {" << std::endl;

    o << "def MOV64rr : InstTCE<(outs R64Regs:$dst), (ins R64Regs:$src),"
      << "	           \"$src -> $dst;\", []>;" << std::endl;

    o << "def PRED_TRUE_MOV64rr : InstTCE<(outs R64Regs:$dst), "
      << "(ins R1Regs:$pred, R64Regs:$src), \"$src -> $dst;\", []>;"
      << std::endl;

    o << "def PRED_FALSE_MOV64rr : InstTCE<(outs R64Regs:$dst), "
      << "(ins R1Regs:$pred, R64Regs:$src), \"$src -> $dst;\", []>;"
      << std::endl;

    o << "} // end of is as cheap as move" << std::endl;
}

void TDGen::genTCEInstrInfo_copyPhys64bitReg(std::ostream&o) const {
    o << std::endl
      << "#include <llvm/CodeGen/MachineInstrBuilder.h>" << std::endl
      << "// copies 64-bit reg to a another" << std::endl
      << "bool TCEInstrInfo::copyPhys64bitReg(" << std::endl
      << "\tMachineBasicBlock& mbb," << std::endl
      << "\tMachineBasicBlock::iterator mbbi," << std::endl
      <<  "const DebugLoc& dl," << std::endl
      << "\tunsigned destReg, unsigned srcReg," << std::endl
      << "\tbool killSrc) const {" << std::endl
      << std::endl;

    if (mach_.is64bit()) {
        o << "\tif (TCE::R64RegsRegClass.contains(destReg, srcReg)) {\n"
          << "\t\tBuildMI(mbb, mbbi, dl, get(TCE::MOV64rr), destReg)\n"
          << "\t\t\t.addReg(srcReg, getKillRegState(killSrc));" << std::endl
          << "\t\treturn true;" << std::endl
          << "}" << std::endl;
    }
    o << "\treturn false;" << std::endl
      << "}";
}

void TDGen::writeAddressingModeDefs(std::ostream& o) {
    if (!mach_.is64bit()) {
        o << std::endl
          << "// Addressing modes." << std::endl
          << "def ADDRrr : ComplexPattern<i32, 2, \"SelectADDRrr\", [], []>;" << std::endl
          << "def ADDRri : ComplexPattern<i32, 2, \"SelectADDRri\", [frameindex], []>;" << std::endl
          << std::endl
          << "// Address operands" << std::endl
          << "def MEMrr : Operand<i32> {" << std::endl
          << "let PrintMethod = \"printMemOperand\";" << std::endl
          << "let MIOperandInfo = (ops R32IRegs, R32IRegs);" << std::endl
          << "}" << std::endl
          << "def MEMri : Operand<i32> {" << std::endl
          << "let PrintMethod = \"printMemOperand\";" << std::endl
          << "let MIOperandInfo = (ops R32IRegs, i32imm);" << std::endl
          << "}" << std::endl
          << std::endl
          << "// Branch targets have OtherVT type." << std::endl
          << "def brtarget : Operand<OtherVT>; " << std::endl
          << "def calltarget : Operand<i32>;" << std::endl;

        o << "def SDT_TCECall    : SDTypeProfile<0, -1, [SDTCisVT<0, i32>]>;" << std::endl;
    } else {
        o << std::endl
          << "// Addressing modes." << std::endl
          << "def ADDRrr : ComplexPattern<i64, 2, \"SelectADDRrr\", [], []>;" << std::endl
          << "def ADDRri : ComplexPattern<i64, 2, \"SelectADDRri\", [frameindex], []>;" << std::endl
          << std::endl
          << "// Address operands" << std::endl
          << "def MEMrr : Operand<i64> {" << std::endl
          << "let PrintMethod = \"printMemOperand\";" << std::endl
          << "let MIOperandInfo = (ops R64IRegs, R64IRegs);" << std::endl
          << "}" << std::endl
          << "def MEMri : Operand<i64> {" << std::endl
          << "let PrintMethod = \"printMemOperand\";" << std::endl
          << "let MIOperandInfo = (ops R64IRegs, i64imm);" << std::endl
          << "}" << std::endl
          << std::endl
          << "// Branch targets have OtherVT type." << std::endl
          << "def brtarget : Operand<OtherVT>; " << std::endl
          << "def calltarget : Operand<i64>;" << std::endl;

        o << "def SDT_TCECall    : SDTypeProfile<0, -1, [SDTCisVT<0, i64>]>;" << std::endl;
    }
}

void TDGen::writeMiscPatterns(std::ostream& o) {
    if (!mach_.is64bit()) {
        o << "// zero extending moves used in some patterns" << std::endl
          << "def ANDext : InstTCE<(outs R32IRegs:$dst), (ins R1Regs:$src, i32imm:$val), \"\", []>;" << std::endl
          << "def PRED_TRUE_ANDext : InstTCE<(outs R32IRegs:$dst),"
          << " (ins R1Regs:$pred, R1Regs:$src, i32imm:$val), \"\", []>;" << std::endl
          << "def PRED_FALSE_ANDext : InstTCE<(outs R32IRegs:$dst),"
          << " (ins R1Regs:$pred, R1Regs:$src, i32imm:$val),\"\", []>;" << std::endl
          << "def XORbicmp: InstTCE<(outs R1Regs:$dst),"
          << " (ins R1Regs:$src, i32imm:$val), \"\", []>;" << std::endl
          << "def PRED_TRUE_XORbicmp: InstTCE<(outs R1Regs:$dst),"
          << " (ins R1Regs:$pred, R1Regs:$src, i32imm:$val), \"\", []>;" << std::endl
          << "def PRED_FALSE_XORbicmp: InstTCE<(outs R1Regs:$dst),"
          << " (ins R1Regs:$pred, R1Regs:$src, i32imm:$val), \"\", []>;" << std::endl;

        o << "def: Pat <(i32 (anyext R1Regs:$src)), (ANDext R1Regs:$src, 1)>;" << std::endl
          << "def: Pat <(i32 (zext R1Regs:$src)), (ANDext R1Regs:$src, 1)>;" << std::endl;

        o << "// select of 1 or 0." << std::endl
          << "def : Pat<(i32 (select R1Regs:$c, (i32 1), (i32 0))),"
          << " (ANDext R1Regs:$c, 1)>;" << std::endl;

        o << std::endl
          << "def: Pat <(i32 (sext R1Regs:$src)), (SUBrir 0,(ANDext R1Regs:$src, 1))>;"
          << std::endl;

        o << "// ------ Shift (emulation) patterns. " << std::endl
          << "def: Pat <(i32 (shl R32IRegs:$val, (i32 1))),"
          << " (ADDrrr R32Regs:$val, R32Regs:$val)>;" << std::endl
          << "def: Pat <(i32 (TCESHLConst R32IRegs:$val, (i32 1))),"
          << " (ADDrrr R32IRegs:$val, R32IRegs:$val)>;" << std::endl;

        o << "// ----- Global addresses, constant pool entries ------" << std::endl
          << "def TCEGlobalAddr : SDNode<\"TCEISD::GLOBAL_ADDR\", SDTIntUnaryOp>;" << std::endl
          << "def TCEConstPool : SDNode<\"TCEISD::CONST_POOL\", SDTIntUnaryOp>;" << std::endl
          << "def : Pat<(TCEGlobalAddr tglobaladdr:$in), (MOVI32ri tglobaladdr:$in)>;" << std::endl
          << "def : Pat<(TCEGlobalAddr tconstpool:$in), (MOVI32ri tconstpool:$in)>;" << std::endl
          << "def : Pat<(TCEConstPool tglobaladdr:$in), (MOVI32ri tglobaladdr:$in)>;" << std::endl
          << "def : Pat<(TCEConstPool tconstpool:$in), (MOVI32ri tconstpool:$in)>;" << std::endl;


        o << "// some peephole patterns." << std::endl
          << "// 1-bit select with imm values - xor or mov." << std::endl
          << "def : Pat<(i1 (select R1Regs:$c, (i1 0), (i1 -1))), (XORbbj R1Regs:$c, 1)>;" << std::endl
          << "def : Pat<(i1 (select R1Regs:$c, (i1 -1), (i1 0))), (MOVI1rr R1Regs:$c)>;" << std::endl
          << "def : Pat<(i1 (select R1Regs:$c, (i1 -1), R1Regs:$F)), (IORbbb R1Regs:$c, R1Regs:$F)>;" << std::endl
          << "def : Pat<(i1 (select R1Regs:$c, R1Regs:$T, (i1 0))), (ANDbbb R1Regs:$c, R1Regs:$T)>;" << std::endl;

        o <<  "// 1-bit comparison between booleans - xor or xnor(implemented with 2 xors)" << std::endl
          << "def : Pat<(i1 (setne R1Regs:$op1, R1Regs:$op2)), (XORbbb R1Regs:$op1, R1Regs:$op2)>;" << std::endl
          << "// TODO: should the temp values be converted to i32? usually more i32 regs." << std::endl
          << "def : Pat<(i1 (seteq R1Regs:$op1, R1Regs:$op2)), (XORbbj (XORbbb R1Regs:$op1, R1Regs:$op2), 1)>;" << std::endl;

        o << "def TCEBlockAddress : SDNode<\"TCEISD::BLOCK_ADDR\", SDTIntUnaryOp>;" << std::endl
          << "def : Pat<(TCEBlockAddress tblockaddress:$src1), (MOVI32ri tblockaddress:$src1)>;" << std::endl;

    } else {
        o << "// zero extending moves used in some patterns" << std::endl
          << "def ANDext : InstTCE<(outs R64IRegs:$dst),"
          << " (ins R1Regs:$src, i64imm:$val), \"\", []>;" << std::endl
          << "def PRED_TRUE_ANDext : InstTCE<(outs R64IRegs:$dst),"
          << " (ins R1Regs:$pred, R1Regs:$src, i64imm:$val), \"\", []>;" << std::endl
          << "def PRED_FALSE_ANDext : InstTCE<(outs R64IRegs:$dst),"
          << " (ins R1Regs:$pred, R1Regs:$src, i64imm:$val),\"\", []>;" << std::endl
          << "def XORbicmp: InstTCE<(outs R1Regs:$dst),"
          << " (ins R1Regs:$src, i64imm:$val), \"\", []>;" << std::endl
          << "def PRED_TRUE_XORbicmp: InstTCE<(outs R1Regs:$dst),"
          << " (ins R1Regs:$pred, R1Regs:$src, i64imm:$val), \"\", []>;" << std::endl
          << "def PRED_FALSE_XORbicmp: InstTCE<(outs R1Regs:$dst),"
          << " (ins R1Regs:$pred, R1Regs:$src, i64imm:$val), \"\", []>;" << std::endl;

        o << "def: Pat <(i64 (anyext R1Regs:$src)), (ANDext R1Regs:$src, 1)>;" << std::endl
          << "def: Pat <(i64 (zext R1Regs:$src)), (ANDext R1Regs:$src, 1)>;" << std::endl;

        o << "// select of 1 or 0." << std::endl
          << "def : Pat<(i64 (select R1Regs:$c, (i64 1), (i64 0))), (ANDext R1Regs:$c, 1)>;" << std::endl;

        o << std::endl
          << "def: Pat <(i64 (sext R1Regs:$src)), (SUB64sas 0,(ANDext R1Regs:$src, 1))>;"
          << std::endl;

        o << "// ------ Shift (emulation) patterns. " << std::endl
          << "def: Pat <(i64 (shl R64IRegs:$val, (i64 1))),"
          << " (ADD64sss R64Regs:$val, R64Regs:$val)>;" << std::endl
          << "def: Pat <(i64 (TCESHLConst R64IRegs:$val, (i64 1))),"
          << " (ADD64sss R64IRegs:$val, R64IRegs:$val)>;" << std::endl;

        o << "// ----- Global addresses, constant pool entries ------" << std::endl
          << "def TCEGlobalAddr : SDNode<\"TCEISD::GLOBAL_ADDR\", SDTIntUnaryOp>;" << std::endl
          << "def TCEConstPool : SDNode<\"TCEISD::CONST_POOL\", SDTIntUnaryOp>;" << std::endl
          << "def : Pat<(TCEGlobalAddr tglobaladdr:$in), (MOVI64sa tglobaladdr:$in)>;" << std::endl
          << "def : Pat<(TCEGlobalAddr tconstpool:$in), (MOVI64sa tconstpool:$in)>;" << std::endl
          << "def : Pat<(TCEConstPool tglobaladdr:$in), (MOVI64sa tglobaladdr:$in)>;" << std::endl
          << "def : Pat<(TCEConstPool tconstpool:$in), (MOVI64sa tconstpool:$in)>;" << std::endl;

        o << "// some peephole patterns." << std::endl
          << "// 1-bit select with imm values - xor or mov." << std::endl
          << "def : Pat<(i1 (select R1Regs:$c, (i1 0), (i1 -1))), (XOR64bbj R1Regs:$c, 1)>;" << std::endl
          << "def : Pat<(i1 (select R1Regs:$c, (i1 -1), (i1 0))), (MOVI1rr R1Regs:$c)>;" << std::endl
          << "def : Pat<(i1 (select R1Regs:$c, (i1 -1), R1Regs:$F)), (IOR64bbb R1Regs:$c, R1Regs:$F)>;" << std::endl
          << "def : Pat<(i1 (select R1Regs:$c, R1Regs:$T, (i1 0))), (AND64bbb R1Regs:$c, R1Regs:$T)>;" << std::endl;

        o <<  "// 1-bit comparison between booleans - xor or xnor(implemented with 2 xors)" << std::endl
          << "def : Pat<(i1 (setne R1Regs:$op1, R1Regs:$op2)), (XOR64bbb R1Regs:$op1, R1Regs:$op2)>;" << std::endl
          << "// TODO: should the temp values be converted to i64? usually more i64 regs." << std::endl
          << "def : Pat<(i1 (seteq R1Regs:$op1, R1Regs:$op2)), (XOR64bbj (XOR64bbb R1Regs:$op1, R1Regs:$op2), 1)>;" << std::endl;

        o << "def TCEBlockAddress : SDNode<\"TCEISD::BLOCK_ADDR\", SDTIntUnaryOp>;" << std::endl
          << "def : Pat<(TCEBlockAddress tblockaddress:$src1), (MOVI64sa tblockaddress:$src1)>;" << std::endl;
    }
}

void TDGen::writeConstShiftPat(std::ostream& os,
                               const TCEString& nodeName,
                               const TCEString& opNameBase, int i) {

    if (!mach_.is64bit()) {
        TCEString opName = opNameBase; opName << i << "_32rr";
        if (opNames_.find(opName) != opNames_.end()) {
            os << "def : Pat<(i32 (" << nodeName
               << " R32IRegs:$val, (i32 " << i << "))), ("
               << opName << " R32IRegs:$val)>;" << std::endl;
        }
    } else {
        TCEString opName = opNameBase; opName << i << "_64rr";
        if (opNames_.find(opName) != opNames_.end()) {
            os << "def : Pat<(i64 (" << nodeName
               << " R64IRegs:$val, (i64 " << i << "))), ("
               << opName << " R64IRegs:$val)>;" << std::endl;
        }
    }
}


void TDGen::createConstShiftPatterns(std::ostream& os) {
    int bits = mach_.is64bit() ? 64: 32;
    for (int i = 1; i < bits; i++) {
        writeConstShiftPat(os, "TCESRAConst", "SHR", i);
        writeConstShiftPat(os, "TCESRLConst", "SHRU", i);
        writeConstShiftPat(os, "TCESHLConst", "SHL", i);
    }
}

void TDGen::createBoolAndHalfLoadPatterns(std::ostream& os) {

    // TODO: what about true/false versions of these ops?

    TCEString load = littleEndian_ ? "LD8" : "LDQ";
    TCEString uload = littleEndian_ ? "LDU8" : "LDQU";
    TCEString wload = littleEndian_ ?
        (mach_.is64bit() ? "LD64" : "LD32") : "LDW";
    if (mach_.hasOperation(load)) {
        os << "def " << load
           << "Br : InstTCE<(outs R1Regs:$op2), (ins MEMrr:$op1), \"\", "
           << "[(set R1Regs:$op2, (sextloadi1 ADDRrr:$op1))]>;" << std::endl
           << "def " << load
           << "Bi : InstTCE<(outs R1Regs:$op2), (ins MEMri:$op1), \"\", "
           << "[(set R1Regs:$op2, (sextloadi1 ADDRri:$op1))]>; " << std::endl;

        opNames_[load + "Br"] = load;
        opNames_[load + "Bi"] = load;

    }
    if (mach_.hasOperation(uload)) {
        os << "def " << uload
           << "Br : InstTCE<(outs R1Regs:$op2), (ins MEMrr:$op1), \"\", "
           << "[(set R1Regs:$op2, (zextloadi1 ADDRrr:$op1))]>;" << std::endl
           << "def " << uload
           << "Bi : InstTCE<(outs R1Regs:$op2), (ins MEMri:$op1), \"\", "
           << "[(set R1Regs:$op2, (zextloadi1 ADDRri:$op1))]>;" << std::endl;
        opNames_[uload + "Br"] = uload;
        opNames_[uload + "Bi"] = uload;

        os << "def : Pat<(i1 (load ADDRrr:$addr)), ("
           << uload << "Br ADDRrr:$addr)>;" << std::endl;
        os << "def : Pat<(i1 (load ADDRri:$addr)), ("
           << uload << "Bi ADDRri:$addr)>;" << std::endl;
    } else {
        if (mach_.hasOperation(load)) {
            os << "def : Pat<(i1 (load ADDRrr:$addr)), ("
               << load << "Br ADDRrr:$addr)>;" << std::endl;
            os << "def : Pat<(i1 (load ADDRri:$addr)), ("
               << load << "Bi ADDRri:$addr)>;" << std::endl;
        }
    }

    // if no 8-bit loads, create 32/64-bit loads for stack access but
    // no patterns for isel as only the stack is 32/64-bit aligned.
    // 1- and 8-bit loads on isel will be handled by lowering.
    if (!mach_.hasOperation(load) &&
        !mach_.hasOperation(uload) &&
        mach_.hasOperation(wload)) {
        os << "def " << wload
           << "Br : InstTCE<(outs R1Regs:$op2), (ins MEMrr:$op1), \"\", "
           << "[]>;" << std::endl
        << "def " << wload
        << "Bi : InstTCE<(outs R1Regs:$op2), (ins MEMri:$op1), \"\", "
        << "[]>;" << std::endl;

        opNames_[wload + "Br"] = wload;
        opNames_[wload + "Bi"] = wload;
    }

    TCEString halfLoad = littleEndian_ ? "LD16" : "LDH";
    if (!mach_.hasOperation(halfLoad)) {
        TCEString halfULoad = littleEndian_ ? "LDU16" : "LDHU";
        if (mach_.hasOperation(halfULoad)) {
            halfLoad = halfULoad;
        } else {
            return;
        }
    }

    os << "def " << halfLoad << "hr : InstTCE<(outs HFPRegs:$op2), "
       << "(ins MEMrr:$op1), \"\", [(set HFPRegs:$op2, "
       << "(load ADDRrr:$op1))]>;" << std::endl;

    os << "def " << halfLoad << "hi : InstTCE<(outs HFPRegs:$op2), "
       << "(ins MEMri:$op1), \"\", [(set HFPRegs:$op2, "
       << "(load ADDRri:$op1))]>;" << std::endl;

    opNames_[halfLoad + "hr"] = halfLoad;
    opNames_[halfLoad + "hi"] = halfLoad;

    // TODO: what about 32-bit?
}

// ---------------------------------------------------------------------------
// ValueType implementation.

/**
 * Constructor.
 *
 * @param subwWidth Subword width.
 * @param subwCount Subword count.
 * @param isFloat True, if is a floating point type.
 */
ValueType::ValueType(int subwWidth, int subwCount, bool isFloat)
    : subwWidth_(subwWidth), subwCount_(subwCount), isFloat_(isFloat) {
    assert(subwWidth > 0 && "Invalid value type.");
    assert(subwCount > 0 && "Invalid value type.");
}

/**
 * Constructor.
 *
 * @param vtStr Value type string in LLVM format.
 */
ValueType::ValueType(const TCEString& vtStr) {
    ValueType vt = valueType(vtStr);
    this->subwWidth_ = vt.subwWidth_;
    this->subwCount_ = vt.subwCount_;
    this->isFloat_ = vt.isFloat_;
}

/**
 * Constructor.
 *
 * @param opnd Operand to be converted to ValueType.
 */
ValueType::ValueType(const Operand& opnd) {
    ValueType vt = valueType(opnd);
    this->subwWidth_ = vt.subwWidth_;
    this->subwCount_ = vt.subwCount_;
    this->isFloat_ = vt.isFloat_;
}

/**
 * Copy constructor.
 *
 * @param other Value to be copied.
 */
ValueType::ValueType(const ValueType& other)
    : subwWidth_(other.subwWidth_),
      subwCount_(other.subwCount_),
      isFloat_(other.isFloat_) {}

/**
 * Value assignment.
 *
 * @param other Value to be assigned.
 * @return Reference with the assigned values.
 */
ValueType&
ValueType::operator=(const ValueType& other) {
    this->subwWidth_ = other.subwWidth_;
    this->subwCount_ = other.subwCount_;
    this->isFloat_ = other.isFloat_;
    return *this;
}

/**
 * Tells whether ValueType is supported by LLVM or not.
 *
 * @return True, if ValueType is supported by LLVM.
 */
bool
ValueType::isSupportedByLLVM() const {
    if (SUPPORTED_LLVM_VALUE_TYPES.find(valueTypeStr()) !=
        SUPPORTED_LLVM_VALUE_TYPES.end()) {
        return true;
    }
    return false;
}

/**
 * Returns ValueType's whole bitwidth.
 *
 * @return ValueType's whole bitwidth.
 */
int
ValueType::width() const {
    return subwWidth_ * subwCount_;
}

/**
 * Returns ValueType's subword bitwidth.
 *
 * @return ValueType's subword bitwidth.
 */
int
ValueType::subwordWidth() const {
    return subwWidth_;
}

/**
 * Returns ValueType's subword count.
 *
 * @return ValueType's subword count.
 */
int
ValueType::subwordCount() const {
    return subwCount_;
}

/**
 * Tells whether ValueType is a floating point type or not.
 *
 * @return True, is ValueType is a float type.
 */
bool
ValueType::isFloat() const {
    return isFloat_;
}

/**
 * Tells whether ValueType is a vector type or not.
 *
 * @return True, is ValueType is a vector type.
 */
bool
ValueType::isVector() const {
    return subwCount_ > 1;
}

/**
 * Returns the Operand::OperandType of ValueType.
 *
 * @return OperandType of ValueType.
 */
Operand::OperandType
ValueType::operandType() const {
    if (!isFloat_ && subwWidth_ == 1) {
        return Operand::BOOL;
    } else if (isFloat_ && subwWidth_ == 16) {
        return Operand::HALF_FLOAT_WORD;
    } else if (isFloat_ && subwWidth_ == 32) {
        return Operand::FLOAT_WORD;
    } else if (!isFloat_) {
        return Operand::UINT_WORD;
    }

    return Operand::UINT_WORD;
}

/**
 * Returns ValueType in LLVM value type string format, e.g. "v2i32".
 *
 * @return Value type string in LLVM format.
 */
TCEString
ValueType::valueTypeStr() const {
    const TCEString subwWidthStr = Conversion::toString(subwWidth_);

    // Check if the value type is a scalar.
    if (subwCount_ == 1) {
        if (isFloat_) {
            return "f" + subwWidthStr;
        } else {
            return "i" + subwWidthStr;
        }
    }

    // The value type is a vector.
    if (isFloat_) {
        return "v" + Conversion::toString(subwCount_) + "f" + subwWidthStr;
    } else {
        return "v" + Conversion::toString(subwCount_) + "i" + subwWidthStr;
    }
}

/**
 * Returns Operand in LLVM value type string format, e.g. "v2i32".
 *
 * @param operand Operand that will be transformed to value type string.
 * @return Value type string in LLVM format.
 */
TCEString
ValueType::valueTypeStr(const Operand& operand) {
    if (operand.type() == Operand::FLOAT_WORD ||
        operand.type() == Operand::HALF_FLOAT_WORD) {
        ValueType vt(operand.elementWidth(), operand.elementCount(), true);
        return vt.valueTypeStr();
    } else {
        ValueType vt(operand.elementWidth(), operand.elementCount(), false);
        return vt.valueTypeStr();
    }
}

/**
 * Returns given LLVM value type string (e.g. "v2i32") as ValueType.
 *
 * @param vtStr LLVM value type string to be converted to ValueType.
 * @return ValueType object of the LLVM value type string.
 */
ValueType
ValueType::valueType(const TCEString& vtStr) {
    assert(vtStr.size() > 1);

    // Check if the value type is a scalar.
    if (vtStr[0] != 'v') {
        int subwWidthStr = Conversion::toInt(vtStr.substr(1));

        if (vtStr[0] == TDGen::OT_REG_FP) {
            return ValueType(subwWidthStr, 1, true);
        } else {
            return ValueType(subwWidthStr, 1, false);
        }
    }

    // The value type is a vector, find float/integer type ("f"/"i")
    // indicator.
    size_t vtI = vtStr.length() - 1;
    while (vtI > 0 && std::isdigit(vtStr[vtI])) --vtI;

    int subwCount = Conversion::toInt(vtStr.substr(1, vtI - 1));
    int subwWidth = Conversion::toInt(vtStr.substr(vtI + 1));

    if (vtStr[vtI] == TDGen::OT_REG_FP) {
        return ValueType(subwWidth, subwCount, true);
    } else {
        return ValueType(subwWidth, subwCount, false);
    }
}

/**
 * Returns given Operand as a value type object.
 *
 * @param operand Operand to be converted to ValueType.
 * @return ValueType presentation of the Operand.
 */
ValueType
ValueType::valueType(const Operand& operand) {
    return valueType(valueTypeStr(operand));
}

/**
 * Returns a list of vector value types that match the given width.
 *
 * Currently filters out unsupported LLVM value types.
 *
 * @param width The width of the desired value types.
 * @param onlyInts If true, floating point types are not included to output.
 * @return Value types for given width in a container.
 */
std::vector<ValueType>
ValueType::vectorTypesOfWidth(int width, bool onlyInts) {
    std::vector<ValueType> vts;

    int subwCount = 2;
    int subwWidth = width / subwCount;

    while (subwWidth > 0) {
        ValueType intVt(subwWidth, subwCount, false);
        if (intVt.isSupportedByLLVM()) {
            vts.push_back(intVt);
        }

        if (!onlyInts) {
            ValueType floatVt(subwWidth, subwCount, true);
            if (floatVt.isSupportedByLLVM()) {
                vts.push_back(floatVt);
            }
        }

        subwWidth /= 2;
        subwCount *= 2;
    }

    return vts;
}

/**
 * Returns a list of vector value types that match the given subword width.
 *
 * Currently filters out unsupported LLVM value types.
 *
 * @param subwordWidth The subword width of the desired value types.
 * @param onlyInts If true, floating point types are not included to output.
 * @return Value types for given subword width in a container.
 */
std::vector<ValueType>
ValueType::vectorTypesOfSubwordWidth(int subwordWidth, bool onlyInts) {
    std::vector<ValueType> vts;

    int subwCount = 2;
    int subwWidth = subwordWidth;
    int MAX_SUBW_COUNT = SIMD_WORD_WIDTH / BYTE_BITWIDTH;

    while (subwWidth * subwCount <= SIMD_WORD_WIDTH &&
           subwCount <= MAX_SUBW_COUNT) {
        ValueType intVt(subwWidth, subwCount, false);
        if (intVt.isSupportedByLLVM()) {
            vts.push_back(intVt);
        }

        if (!onlyInts) {
            ValueType floatVt(subwWidth, subwCount, true);
            if (floatVt.isSupportedByLLVM()) {
                vts.push_back(floatVt);
            }
        }

        subwCount *= 2;
    }

    return vts;
}

// ---------------------------------------------------------------------------
// RegisterClass implementation.

/**
 * Constructor.
 *
 * @param vt ValueType that is supported by the RegisterClass.
 * @param name Name of the RegisterClass.
 */
RegisterClass::RegisterClass(const ValueType& vt, const TCEString& name)
    : name_(name), vt_(vt), registers_(std::vector<RegisterInfo>()) {
    if (vt.width() < 8) {
        alignment_ = 8;
    } else {
        alignment_ = vt.width();
    }
}

/**
 * Copy constructor.
 *
 * @param other RegisterClass to be copied.
 */
RegisterClass::RegisterClass(const RegisterClass& other)
    : name_(other.name_),
      vt_(other.vt_),
      alignment_(other.alignment_),
      registers_(other.registers_) {}

/**
 * Value assignment.
 *
 * @param other RegisterClass to be assigned.
 * @return Reference with the assigned values.
 */
RegisterClass&
RegisterClass::operator=(const RegisterClass& other) {
    this->name_ = other.name_;
    this->vt_ = other.vt_;
    this->alignment_ = other.alignment_;
    this->registers_ = other.registers_;
    return *this;
}

/**
 * Returns RegisterClass's name.
 *
 * @return Name of the RegisterClass.
 */
TCEString
RegisterClass::name() const {
    return name_;
}

/**
 * Returns the supported ValueType.
 *
 * @return Supported ValueType.
 */
ValueType
RegisterClass::valueType() const {
    return vt_;
}

/**
 * Returns the RegisterClass's alignment in bits.
 *
 * @return Alignment of RegisterClass in bits.
 */
int
RegisterClass::alignment() const {
    return alignment_;
}

/**
 * Returns RegisterInfo pointed by the index.
 *
 * @return RegisterInfo pointed by the index.
 */
RegisterInfo
RegisterClass::registerInfo(int index) const {
    assert(
        index >= static_cast<int>(0) &&
        index < static_cast<int>(registers_.size()));
    return registers_[index];
}

/**
 * Returns how many registers is used by this RegisterClass.
 *
 * @return A number of registers used by this RegisterClass.
 */
size_t
RegisterClass::numberOfRegisters() const {
    return registers_.size();
}

/**
 * Sets the registers used by this RegisterClass.
 *
 * @param registers The new registers used by this RegisterClass.
 */
void
RegisterClass::addRegisters(const std::vector<RegisterInfo>& registers) {
    registers_.insert(registers_.end(), registers.begin(), registers.end());
}
