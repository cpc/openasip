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
 * @file TDGen.hh
 *
 * Declaration of TDGen class.
 *
 * @author Veli-Pekka Jääskeläinen 2008 (vjaaskel-no.spam-cs.tut.fi)
 * @author Pekka Jääskeläinen 2010
 * @author Heikki Kultala 2012
 */

#ifndef TTA_TDGEN_HH
#define TTA_TDGEN_HH

#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "Exception.hh"
#include "ImmInfo.hh"
#include "MachinePart.hh"
#include "Operand.hh"
#include "OperationDAGSelector.hh"
#include "TCEString.hh"

class Operation;
class Operand;
class OperationDAG;
class OperationNode;
class OperationDAGNode;
class TerminalNode;
class ConstantNode;

namespace TTAMachine {
    class Machine;
    class RegisterFile;
}

namespace TDGenerator {
struct RegisterInfo;
struct InstructionInfo;
class ValueType;
class RegisterClass;
}  // namespace TDGenerator

/**
 * TCE Backend plugin source code and .td definition generator.
 *
 * Generates files for building target architecture plugin for LLVM-TCE
 * backend. This version generates the backend files for the "RISC
 * instruction set style" output and provides useful methods.
 */
class TDGen {
public:
    TDGen(const TTAMachine::Machine& mach);
    virtual ~TDGen();
    void generateBackend(std::string& path);
    // todo clear out virtual functions. they are a remaind of removed
    // TDGenSIMD.
protected:
    bool writeRegisterInfo(std::ostream& o);
    void writeStartOfRegisterInfo(std::ostream& o);
    void writeOperandDefs(std::ostream& o);
    void writeIntegerImmediateDefs(std::ostream& o, const ImmInfo& iivis);
    void writeMoveImmediateDefs(std::ostream& o);

    void writeInstrInfo(std::ostream& o);
    void writeBackendCode(std::ostream& o);
    void writeTopLevelTD(std::ostream& o);
    void writeInstrFormats(std::ostream& o);

    enum RegType {
        GPR = 0,
        RESERVED,
        ARGUMENT,
        RESULT
    };

    struct TerminalDef {
        std::string registerPat;
        std::string registerDag;
        std::string immPat;
        std::string immDag;
    };

    struct RegInfo {
        std::string rf;
        unsigned idx;

        // Comparison operator for ordering in set.
        bool operator<(const RegInfo& other) const {
            if (rf < other.rf ||
                (rf == other.rf && idx < other.idx)) {

                return true;
            }

            return false;
        }
    };

    enum RegsToProcess {
        ALL_REGISTERS,
        ONLY_EXTRAS,
        ONLY_LANES,
        ONLY_NORMAL
    };
    // Gather & associate machine information
    // TODO maybe remove this comment, because in TDGenSIMD made sense, but
    // here is more confusing
    bool checkRequiredRegisters();
    void analyzeRegisters();
    void analyzeRegisterFileClasses();
    void analyzeRegisters(RegsToProcess regsToProcess);
    void gatherAllMachineOperations();
    void analyzeMachineVectorRegisterClasses();
    void analyzeMachineRegisters();
    void associateRegistersWithVectorRegisterClasses();
    void orderEqualWidthRegistersToRoundRobin();

    void verbose(const TCEString& msg) const;

    // Helper functions.
    bool isVectorLoadOperation(const Operation& op) const;
    bool isVectorStoreOperation(const Operation& op) const;
    bool isWrongEndianessVectorOp(const Operation& op) const;
    bool isVectorBitwiseOperation(const Operation& op) const;
    bool hasRawOperands(const Operation& op) const;
    int subwordWidthOfRawData(const Operation& op) const;
    bool hasRegisterClassSupport(const Operation& op) const;
    bool hasRegisterClassSupport(const TDGenerator::ValueType& vt) const;
    TCEString associatedVectorRegisterClass(const Operand& operand) const;
    void writePatternReplacement(
        std::ostream& o, const TCEString& origPat,
        const TCEString& replacerPat) const;
    void writeOperationDefUsingGivenOperandTypes(
        std::ostream& o, Operation& op, bool skipPattern,
        std::vector<TDGenerator::ValueType> inputs,
        std::vector<TDGenerator::ValueType> outputs,
        TCEString instrSuffix = "");

    void writeRegisterDef(
        std::ostream& o,
        const RegInfo& reg,
        const std::string regName,
        const std::string regTemplate,
        const std::string aliases,
        RegType type);

    void write64bitRegisterInfo(std::ostream& o);
    void write32bitRegisterInfo(std::ostream& o);
    void write16bitRegisterInfo(std::ostream& o);
    void write8bitRegisterInfo(std::ostream& o);
    void write1bitRegisterInfo(std::ostream& o);
    void writeRARegisterInfo(std::ostream& o);
//    void writeVectorRegisterInfo(std::ostream& o);
//    void writeVectorRegisterInfo(std::ostream& o, int width);
    void writeGuardRegisterClassInfo(std::ostream& o);

    // these generate vectorregister info in the GenRegisterInfo.td
    // definitions
    void writeVectorRegisterBaseClasses(std::ostream& o) const;
    void writeVectorRegisterNames(std::ostream& o);
    void writeVectorRegisterClasses(std::ostream& o) const;

    // These generate vector GenInstrInfo.td definitions.
    void writeVectorOperationDefs(
        std::ostream& o, Operation& op, bool skipPattern);
    void writeVectorOperationDef(
        std::ostream& o, Operation& op, TCEString valueTypes,
        const TCEString& attributes, bool skipPattern);
    void saveAdditionalVectorOperationInfo(
        const Operation& op, const TCEString& valueTypes, bool isRegisterOp);
    void writeVectorMemoryOperationDefs(
        std::ostream& o, Operation& op, bool skipPattern);
    void writeVectorBitwiseOperationDefs(
        std::ostream& o, Operation& op, bool skipPattern);

    void writeVectorRegisterMoveDefs(std::ostream& o);
    void writeVectorTruncStoreDefs(std::ostream& o) const;
    void writeScalarToVectorDefs(std::ostream& o) const;
    void writeVectorBitConversions(std::ostream& o) const;
    void writeScalarOperationExploitations(std::ostream& o);
    void writeVectorLoadStoreOperationExploitations(std::ostream& o);
    void writeWiderVectorOperationExploitations(std::ostream& o);

    // These generate vector Backend.inc definitions.
    void genGeneratedTCEPlugin_getStore(std::ostream& o) const;
    void genGeneratedTCEPlugin_getLoad(std::ostream& o) const;
    void genGeneratedTCEPlugin_isVectorRegisterMove(std::ostream& o) const;
    void genGeneratedTCEPlugin_getVectorValueType(std::ostream& o) const;
    void genGeneratedTCEPlugin_getVectorBroadcastOpcode(
        std::ostream& o) const;
    void genGeneratedTCEPlugin_getVectorPackOpcode(std::ostream& o) const;
    void genGeneratedTCEPlugin_getVectorSelectOpcode(std::ostream& o) const;
    void genGeneratedTCEPlugin_getVectorShuffle1Opcode(std::ostream& o) const;
    void genGeneratedTCEPlugin_getVectorShuffle2Opcode(std::ostream& o) const;
    void genGeneratedTCEPlugin_getConstantVectorShuffleOpcode(
        std::ostream& o) const;
    void genGeneratedTCEPlugin_getExtractElemOpcode(std::ostream& o) const;
    void genGeneratedTCEPlugin_getVectorShlSameOpcode(std::ostream& o) const;
    void genGeneratedTCEPlugin_getVectorShrSameOpcode(std::ostream& o) const;
    void genGeneratedTCEPlugin_getVectorShruSameOpcode(std::ostream& o) const;
    void genGeneratedTCEPlugin_getVectorAndSameOpcode(std::ostream& o) const;
    void genGeneratedTCEPlugin_getVectorIorSameOpcode(std::ostream& o) const;
    void genGeneratedTCEPlugin_getVectorXorSameOpcode(std::ostream& o) const;
    void genTCETargetLoweringSIMD_addVectorRegisterClasses(
        std::ostream& o) const;
    void genTCETargetLoweringSIMD_associatedVectorRegClass(
        std::ostream& o) const;
    void genTCETargetLoweringSIMD_getSetCCResultVT(std::ostream& o) const;
    void genTCEInstrInfoSIMD_copyPhysVectorReg(std::ostream& o) const;

    void genGeneratedTCEPlugin_getVectorImmediateOpcode(
        std::ostream& o) const;
    void genGeneratedTCEPlugin_getGatherOpcode(std::ostream& o) const;
    void genGeneratedTCEPlugin_getLoadOpcode(std::ostream& o) const;
    void genGeneratedTCEPlugin_getAddOpcode(std::ostream& o) const;
    void genGeneratedTCEPlugin_getShlOpcode(std::ostream& o) const;
    void genGeneratedTCEPlugin_getIorOpcode(std::ostream& o) const;

    void writeVectorImmediateWriteDefs(std::ostream& instrInfoTD);

    void createMinMaxDef(
	const TCEString& opName, const TCEString& valueName, std::ostream&os);

    void createVectorMinMaxDef(
        const TCEString& opName,
        int bits,
        char llvmTypeChar,
        const TCEString& postFix,
        std::ostream& os);

    void writeOperationDefs(std::ostream& o, Operation& op, bool skipPattern);

    void writeOperationDef(
        std::ostream& o, Operation& op, const std::string& operandTypes,
        const std::string& attrs, bool skipPattern,
        std::string backendPrefix = "");

    std::string emulatingOpNodeLLVMName(
        const Operation& op, const OperationDAG& dag, const OperationNode& node,
        const std::string& operandTypes);

    void writeEmulationPattern(
        std::ostream& o, const Operation& op, const OperationDAG& dag);

    void write64bitMoveDefs(std::ostream& o);

    void writeControlFlowInstrDefs(std::ostream& os);
    void writeCondBranchDefs(std::ostream& os);
    void writeCallDef(std::ostream& o);
    virtual void writeCallDefRegs(std::ostream& o);

    void writeRegisterClasses(std::ostream& o);

    virtual TCEString llvmOperationPattern(
        const Operation& op, char operandType = ' ') const;

    virtual TCEString
    llvmOperationName(const TCEString& opName) const;

    bool operationCanBeMatched(
        const Operation& op, 
        std::set<std::string>* recursionCycleCheck = NULL,
        bool recursionHasStore = false);
    bool operationDAGCanBeMatched(
        const OperationDAG& op,
        std::set<std::string>* recursionCycleCheck = NULL,
        bool recursionHasStore = false);
    const OperationDAG* getMatchableOperationDAG(
        const Operation& op);

    std::string tceOperationPattern(const Operation& op);

    std::string patOutputs(const Operation& op, const std::string& oprTypes);
    std::string patInputs(const Operation& op, const std::string& oprTypes);

    virtual std::string operandToString(
        const Operand& operand,
        bool match,
        char operandType,
        const std::string& immDefName = "");

    std::string operationNodeToString(
        const Operation& op, const OperationDAG& dag,
        const OperationNode& node, bool emulationPattern,
        const std::string& operandTypes);

    std::string constantNodeString(
        const Operation& op, 
        const OperationDAG& dag,
        const ConstantNode& node,
        const std::string& operandTypes,
        const OperationDAGNode* successor = nullptr);

    std::string dagNodeToString(
        const Operation& op, const OperationDAG& dag,
        const OperationDAGNode& node, bool emulationPattern,
        const std::string& operandTypes, const Operation* emulatingOp = nullptr,
        const OperationDAGNode* successor = nullptr);

    std::string operationPattern(
        const Operation& op, const OperationDAG& dag,
        const std::string& operandTypes);

    virtual char operandChar(Operand& operand);
    
    std::string createDefaultOperandTypeString(const Operation& op);

    void writeVectorStoreDefs(
        std::ostream& o,
        const TCEString& opName,
        const TCEString& opNameSuffix,
        bool addrImm,
        const TCEString& dataType,
        bool writePredicatedVersions);

    void genTCEInstrInfo_copyPhys64bitReg(std::ostream&o) const;

    void writeArgRegsArray(std::ostream& os);
    virtual void createSelectPatterns(std::ostream& os);
    void writeAddressingModeDefs(std::ostream& o);

    void createByteExtLoadPatterns(std::ostream& os);
    void createShortExtLoadPatterns(std::ostream& os);
    void create32BitExtLoadPatterns(std::ostream& os);

    void createEndiannesQuery(std::ostream& os);
    void createConstantMaterializationQuery(std::ostream& os);
    void createConstShiftPatterns(std::ostream& os);

    void writeOperationDefs(
        std::ostream& o, Operation& op, const std::string& operandTypes,
        const std::string& attrs, bool skipPattern,
        std::string backendPrefix = "");

    void writeVectorStoreDefs(std::ostream& o, Operation& op, int vectorLen);

    void writeVectorTruncStoreDefs(
        std::ostream& o, Operation& op, int bitsize, int vectorLen);

    void createGetMaxMemoryAlignment(std::ostream& os) const;

    void writeVectorAnyextPattern(
        std::ostream& o, Operation& op, const TCEString& loadPatternName,
        int vectorLen);

    void writeVectorLoadDefs(
        std::ostream& o, const TCEString& opName, const TCEString& opNameSuffix,
        bool addrImm, const TCEString& resultType,
        const TCEString& loadPatternName, bool writePredicatedVersions);

    virtual void writeImmediateDef(
        std::ostream& o, const std::string& defName,
        const std::string& operandType, const std::string& predicate);
    void writeInstrDef(
        std::ostream& o, const std::string& instrDefName,
        const std::string& outs, const std::string& ins,
        const std::string& asmString, const std::string& pattern);

    void writeVectorLoadDefs(
        std::ostream& o, Operation& op, const TCEString& loadPatternName,
        int vectorLen);

    void writeBooleanStorePatterns(std::ostream& os);

    std::string immediatePredicate(
        int64_t lowerBoundInclusive,
        uint64_t upperBoundInclusive);
    std::string immediateOperandNameForEmulatedOperation(
        const OperationDAG&,
        const Operand& operand);
    bool areImmediateOperandsLegal(
        const Operation& operation,
        const std::string& operandTypes) const;

    void writeBroadcastDefs(std::ostream& o, Operation& op, int vectorLen);

    bool writePortGuardedJumpDefPair(
        std::ostream& os, const TCEString& tceop1, const TCEString& tceop2,
        bool fp = false);

    std::string subPattern(const Operation& op, const OperationDAG& dag);

    OperationDAG* createTrivialDAG(Operation& op);
    bool canBeImmediate(const OperationDAG& dag, const TerminalNode& node);

    virtual void createMinMaxGenerator(std::ostream& os);

    void writeCallSeqStart(std::ostream& os);
    void writeMiscPatterns(std::ostream& o);
    void generateLoadStoreCopyGenerator(std::ostream& os);

    void createParamDRegNums(std::ostream& os);
    virtual void createVectorRVDRegNums(std::ostream& os);

    void writeCallingConv(std::ostream& os);
    void writeCallingConvLicenceText(std::ostream& os);

    void writeConstShiftPat(
        std::ostream& os, const TCEString& nodeName,
        const TCEString& opNameBase, int i);

    ImmInfo* immInfo_ = nullptr;
    /// Maps (operation, operand) pairs to i32 immediate operand definition
    /// names.
    std::map<ImmInfoKey, std::string> immOperandDefs_;

    void createBoolAndHalfLoadPatterns(std::ostream& os);

    virtual void createConstantMaterializationPatterns(std::ostream& os);

    void createBranchAnalysis(std::ostream& os);

    void genTCERegisterInfo_setReservedVectorRegs(std::ostream& os) const;

    void writeGetPointerAdjustmentQuery(std::ostream& os) const;

    bool canBePredicated(Operation& op, const std::string& operandTypes);
    static std::vector<std::string> supportedStackAccessOperations(
        const TTAMachine::Machine& mach);

    TCEString getLLVMPatternWithConstants(
        const Operation& op, const std::string& operandTypes,
        const std::string& operand0, const std::string& operand1) const;

    std::string operandTypesToRegisters(const std::string& opdTypes) const;
    char operandTypeToRegister(const char& opdType) const;

    TCEString getMovePattern(
        const char& opdType, const std::string& inputPattern) const;

    const TTAMachine::Machine& mach_;

    // Current dwarf register number.
    unsigned dregNum_;

    /// Float type subword width.
    static const int FP_SUBW_WIDTH;
    /// Half float type subword width.
    static const int HFP_SUBW_WIDTH;
    /// Bool type subword width.
    static const int BOOL_SUBW_WIDTH;
    /// Distincts wide vs scalar registers.
    static const int MAX_SCALAR_WIDTH;
    /// Maximum number of subwords that any SIMD operation can have.
    static const int MAX_SUBW_COUNT;

    /// If set to true, smaller vector value types can be stored to larger
    /// register files, e.g. v4i8 vectors can be stored to registers that
    /// are over 32 bits in size.
    static const bool EXPLOIT_BIGGER_REGISTERS;

public:
    // Characters for differend operand types.
    static const char OT_REG_BOOL;
    static const char OT_REG_INT;
    static const char OT_REG_LONG;
    static const char OT_REG_FP;
    static const char OT_REG_HFP;
    static const char OT_REG_DOUBLE;
    static const char OT_IMM_BOOL;
    static const char OT_IMM_INT;
    static const char OT_IMM_FP;
    static const char OT_IMM_HFP;
    static const char OT_IMM_LONG;
    static const char OT_VREG_BOOL;
    static const char OT_VREG_INT8;
    static const char OT_VREG_INT16;
    static const char OT_VREG_INT32;
    static const char OT_VREG_FP;
    static const char OT_VREG_HFP;

protected:
    /// Contains <BaseOpName, OpPattern> key-value pairs.
    static const std::map<TCEString, TCEString> OPERATION_PATTERNS_;

    /// Contains all operation names in upper case.
    OperationDAGSelector::OperationSet allOpNames_;

    /// Contains all scalar operations (<Name, Operation>).
    std::map<TCEString, Operation*> scalarOps_;

    /// Contains all vector operations (<Name, Operation>).
    std::map<TCEString, Operation*> vectorOps_;

    /// Contains vector base classes for register files (<Width, Name>).
    std::map<int, TCEString> baseClasses_;

    /// Contains registers fit for being vector registers (<Width, Registers>).
    std::map<int, std::vector<TDGenerator::RegisterInfo> > registers_;

    /// Contains required vector register classes (<ValueType, RegClass>).
    std::map<TCEString, TDGenerator::RegisterClass> vRegClasses_;

    /// All register store operations (<ValueType, InstrInfo>).
    std::map<TCEString, TDGenerator::InstructionInfo> registerStores_;
    /// All register load operations (<ValueType, InstrInfo>).
    std::map<TCEString, TDGenerator::InstructionInfo> registerLoads_;
    /// All immediate store operations (<ValueType, InstrInfo>).
    std::map<TCEString, TDGenerator::InstructionInfo> immediateStores_;
    /// All immediate load operations (<ValueType, InstrInfo>).
    std::map<TCEString, TDGenerator::InstructionInfo> immediateLoads_;

    /// Contains machine's PACK instructions (<ValueType, InstrName>).
    std::map<TCEString, TCEString> packOperations_;
    /// Contains machine's VBCAST instructions (<ValueType, InstrName>).
    std::map<TCEString, TCEString> vbcastOperations_;
    /// Contains machine's TRUNCxx/CFH instructions (<ValueType, InstrName>).
    std::vector<std::pair<const Operation*, TCEString> > truncOperations_;
    /// Contains machine's VSELECT instructions (<instrName, ValueType>).
    std::map<TCEString, TCEString> vselectOperations_;
    /// Contains machine's VSHUFFLE1 instructions (<ValueType, InstrName>).
    std::map<TCEString, TCEString> vshuffle1Operations_;
    /// Contains machine's VSHUFFLE2 instructions (<ValueType, InstrName>).
    std::map<TCEString, TCEString> vshuffle2Operations_;
    /// Contains machine's VCSHUFFLE instructions
    /// (<<ValueType, ConstantSelects>, InstrName>).
    std::map<std::pair<TCEString, std::vector<int>>, TCEString>
        vcshuffleOperations_;
    /// Contains machine's EXTRACTELEM instructions (<ValueType, InstrName>).
    std::map<TCEString, TCEString> extractElemOperations_;
    /// Contains machine's SHLSAME instructions (<ValueType, InstrName>).
    std::map<TCEString, TCEString> shlSameOperations_;
    /// Contains machine's SHRSAME instructions (<ValueType, InstrName>).
    std::map<TCEString, TCEString> shrSameOperations_;
    /// Contains machine's SHRUSAME instructions (<ValueType, InstrName>).
    std::map<TCEString, TCEString> shruSameOperations_;
    /// Contains machine's ANDSAME instructions (<ValueType, InstrName>).
    std::map<TCEString, TCEString> andSameOperations_;
    /// Contains machine's IORSAME instructions (<ValueType, InstrName>).
    std::map<TCEString, TCEString> iorSameOperations_;
    /// Contains machine's XORSAME instructions (<ValueType, InstrName>).
    std::map<TCEString, TCEString> xorSameOperations_;
    /// Contains machine's GATHER instructions (<ValueType, InstrName>).
    std::map<TCEString, TCEString> gatherOperations_;
    /// Contains machine's add instructions (<ValueType, InstrName>).
    std::map<TCEString, TCEString> addOperations_;
    /// Contains machine's shl instructions (<ValueType, InstrName>).
    std::map<TCEString, TCEString> shlOperations_;
    /// Contains machine's shl instructions (<ValueType, InstrName>).
    std::map<TCEString, TCEString> iorOperations_;

    /// Contains all moves between register classes (<InstrName>).
    std::set<TCEString> movOperations_;

    // List of all 1-bit registers in the target machine.
    std::vector<RegInfo> regs1bit_;
    // List of 8-bit registers in the target machine.
    std::vector<RegInfo> regs8bit_;
    // List of 16-bit registers in the target machine.
    std::vector<RegInfo> regs16bit_;
    // List of 32-bit registers in the target machine.
    std::vector<RegInfo> regs32bit_;
    // List of 64-bit registers in the target machine.
    std::vector<RegInfo> regs64bit_;
    /// The LLVM register defs used as guards.
    std::vector<std::string> llvmGuardRegs_;

    ///  Map of generated llvm register names to
    /// physical register in the machine.
    std::map<std::string, RegInfo> regs_;

    std::vector<std::string> argRegNames_;
    std::vector<std::string> resRegNames_;
    std::vector<std::string> gprRegNames_;

    std::map<std::string, std::string> opNames_;

    std::map<std::string, std::string> truePredOps_;
    std::map<std::string, std::string> falsePredOps_;

    int maxVectorSize_;

    int highestLaneInt_;
    int highestLaneBool_;

    bool hasExBoolRegs_;
    bool hasExIntRegs_;
    bool hasSelect_;

    bool hasConditionalMoves_;

    int maxScalarWidth_;

    bool littleEndian_;
    unsigned int argRegCount_;
    /// Minimum number of 32 bit registers.
    unsigned int requiredI32Regs_;
    unsigned int requiredI64Regs_;
    bool prebypassStackIndeces_;
    bool use64bitForFP_;
 
    /// List of register that are associated with a guard on a bus.
    std::set<RegInfo> guardedRegs_;

    /// Register files whose last reg reserved for temp reg copies.
    std::set<
        const TTAMachine::RegisterFile*, TTAMachine::MachinePart::Comparator>
    tempRegFiles_;

    typedef std::map<std::string, std::vector<std::string> > RegClassMap;
    /// All registers in certain group
    RegClassMap regsInClasses_;
    // Registers grouped by corresponding RFs
    RegClassMap regsInRFClasses_;
    /// All predicates used in constant materialization patterns.
    std::vector<std::string> constantMaterializationPredicates_;

    static const std::string guardRegTemplateName;
};

namespace TDGenerator {

/**
 * Class to represent info of a single register in a register file.
 */
struct RegisterInfo {
    RegisterInfo(
        const TCEString& regName, const TCEString& regFileName,
        unsigned regIndex, unsigned regWidth)
        : regName_(regName),
          regFileName_(regFileName),
          regIndex_(regIndex),
          regWidth_(regWidth) {}

    /// Register name in GenRegisterInfo.td, e.g. "KLUDGE_REGISTER".
    TCEString regName_;
    /// Name of the register file the register belongs to, e.g. "RF".
    TCEString regFileName_;
    /// Register index in the register file.
    unsigned regIndex_;
    /// Register width in bits.
    unsigned regWidth_;
};

/**
 * Class to represent information of an instruction record.
 */
struct InstructionInfo {
    InstructionInfo(const TCEString& osalOpName, const TCEString& instrName)
        : osalOpName_(osalOpName), instrName_(instrName) {}

    /// The OSAL operation used to create the record, e.g. "ADD32X4".
    TCEString osalOpName_;
    /// The instruction record name, e.g. "ADD32X4uuu"
    TCEString instrName_;
};

/**
 * Represents an LLVM value type to express different value types.
 */
class ValueType {
public:
    ValueType(int subwWidth, int subwCount, bool isFloat);
    ValueType(const TCEString& vtStr);
    ValueType(const Operand& opnd);

    ValueType(const ValueType& other);
    ValueType& operator=(const ValueType& other);

    bool isSupportedByLLVM() const;
    int width() const;
    int subwordWidth() const;
    int subwordCount() const;
    bool isFloat() const;
    bool isVector() const;
    Operand::OperandType operandType() const;
    TCEString valueTypeStr() const;

    static TCEString valueTypeStr(const Operand& operand);
    static ValueType valueType(const TCEString& vtStr);
    static ValueType valueType(const Operand& operand);
    static std::vector<ValueType> vectorTypesOfWidth(
        int width, bool onlyInts = false);
    static std::vector<ValueType> vectorTypesOfSubwordWidth(
        int subwordWidth, bool onlyInt = false);

    // Public for faster access.

    /// Subword width of the value type.
    int subwWidth_;
    /// Subword count of the value type.
    int subwCount_;
    /// If true, the value type is a floating point type.
    bool isFloat_;

    /// Contains all supported LLVM value types (<ValueType>).
    static const std::set<TCEString> SUPPORTED_LLVM_VALUE_TYPES;
};

/**
 * Represents TableGen RegisterClass class.
 */
class RegisterClass {
public:
    RegisterClass(const ValueType& vt, const TCEString& name);

    RegisterClass(const RegisterClass& other);
    RegisterClass& operator=(const RegisterClass& other);

    TCEString name() const;
    ValueType valueType() const;
    int alignment() const;
    std::vector<RegisterInfo> registers() const;
    RegisterInfo registerInfo(int index) const;
    size_t numberOfRegisters() const;

    void addRegisters(const std::vector<RegisterInfo>& registers);

private:
    /// RegisterClass name.
    TCEString name_;
    /// Value type that is supported by this RegisterClass, e.g. v4i32.
    ValueType vt_;
    /// RegisterClass alignment in bits, at least 8.
    int alignment_;
    /// Register file registers that this RegisterClass uses.
    std::vector<RegisterInfo> registers_;
};
}  // namespace TDGenerator

#endif
