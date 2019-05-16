/*
    Copyright (c) 2002-2010 Tampere University.

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
 * @author Veli-Pekka J‰‰skel‰inen 2008 (vjaaskel-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2010
 * @author Heikki Kultala 2012
 */

#ifndef TTA_TDGEN_HH
#define TTA_TDGEN_HH

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include "Exception.hh"
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
    virtual ~TDGen() {}
    virtual void generateBackend(std::string& path);

protected:
    virtual bool writeRegisterInfo(std::ostream& o);
    virtual void writeInstrInfo(std::ostream& o);
    virtual void writeBackendCode(std::ostream& o);
    virtual void writeTopLevelTD(std::ostream& o);
   
   
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
    bool checkRequiredRegisters();
    void analyzeRegisters();
    void analyzeRegisterFileClasses();
    void analyzeRegisters(RegsToProcess regsToProcess);

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
    void writeVectorRegisterInfo(std::ostream& o);
    void writeVectorRegisterInfo(std::ostream& o, int width);

    virtual void writeOperationDefs(
        std::ostream& o, Operation& op, bool skipPattern);

    void writeOperationDef(
        std::ostream& o, Operation& op, 
        const std::string& operandTypes,
        const std::string& attrs,
        bool skipPattern,
        std::string backendPrefix = "");

    std::string emulatingOpNodeLLVMName(
        const Operation& op, const OperationDAG& dag, const OperationNode& node,
        const std::string& operandTypes);

    void writeEmulationPattern(
        std::ostream& o,
        const Operation& op,
        const OperationDAG& dag);

    void writeCallDef(std::ostream& o);

    void writeRegisterClasses(std::ostream& o);

    virtual TCEString llvmOperationPattern(
        const Operation& op, char operandType = ' ');
    virtual TCEString llvmOperationName(
        const Operation& op);
    bool operationCanBeMatched(
        const Operation& op, 
        std::set<std::string>* recursionCycleCheck = NULL,
        bool recursionHasStore = false);

    std::string tceOperationPattern(const Operation& op);

    std::string patOutputs(const Operation& op, const std::string& oprTypes);
    std::string patInputs(const Operation& op, const std::string& oprTypes);

    virtual std::string operandToString(
        const Operand& operand,
        bool match,
        char operandType);

    std::string operationNodeToString(
        const Operation& op, const OperationDAG& dag, const OperationNode& node,
        bool emulationPattern, const std::string& operandTypes);

    std::string constantNodeString(
        const Operation& op, 
        const OperationDAG& dag,
        const ConstantNode& node,
        const std::string& operandTypes);

    std::string dagNodeToString(
        const Operation& op, const OperationDAG& dag,
        const OperationDAGNode& node, bool emulationPattern,
        const std::string& operandTypes);

    std::string operationPattern(
        const Operation& op,
        const OperationDAG& dag,
        const std::string& operandTypes);

    virtual char operandChar(Operand& operand);
    
    std::string createDefaultOperandTypeString(const Operation& op);

    void writeOperationDefs(
        std::ostream& o, Operation& op, 
        const std::string& operandTypes,
        const std::string& attrs,
        bool skipPattern,
        std::string backendPrefix = "");

    void writeVectorStoreDefs(std::ostream& o,
			      const TCEString& opName,
			      const TCEString& opNameSuffix, bool addrImm,
			      const TCEString& dataType);

    void writeVectorStoreDefs(std::ostream& o, Operation& op, int vectorLen);

    void writeVectorTruncStoreDefs(
	std::ostream& o,
	Operation& op, int bitsize, int vectorLen);

    void writeVectorAnyextPattern(
       std::ostream& o,
       Operation& op, const TCEString& loadPatternName, int vectorLen);

    void writeVectorLoadDefs(std::ostream& o, const TCEString& opName, const TCEString& opNameSuffix, bool addrImm,
			     const TCEString& resultType, const TCEString& loadPatternName);

/*
    void writeVectorLoadDefs(
	std::ostream& o, std::string& opNameLLVM,
	bool addrImm,
	const TCEString& resultType);
*/
    void writeVectorLoadDefs(
	std::ostream& o, 
	Operation& op, 
	const TCEString& loadPatternName, 
	int vectorLen);

    std::string subPattern(
	const Operation& op,
	const OperationDAG& dag);

    OperationDAG* createTrivialDAG(Operation& op);
    bool canBeImmediate(const OperationDAG& dag, const TerminalNode& node);

    virtual void createMinMaxGenerator(std::ostream& os);

    virtual void generateLoadStoreCopyGenerator(std::ostream& os);

    virtual void createGetMaxMemoryAlignment(std::ostream& os) const;
    
    virtual void writeCallingConv(std::ostream& os);
    void writeCallingConvLicenceText(std::ostream& os);

    void createSelectPatterns(std::ostream& os);

    void createByteExtLoadPatterns(std::ostream& os);
    void createShortExtLoadPatterns(std::ostream& os);
    void createEndiannesQuery(std::ostream& os);
    void createConstShiftPatterns(std::ostream& os);

    void writeConstShiftPat(std::ostream& os,
                            const TCEString& nodeName,
                            const TCEString& opNameBase, int i);

    void createBoolAndHalfLoadPatterns(std::ostream& os);

    void writeCallSeqStart(std::ostream& os);

    const TTAMachine::Machine& mach_;

    // Current dwarf register number.
    unsigned dregNum_;

    // List of 1-bit registers in the target machine.
    std::vector<RegInfo> regs1bit_;
    // List of 8-bit registers in the target machine.
    std::vector<RegInfo> regs8bit_;
    // List of 16-bit registers in the target machine.
    std::vector<RegInfo> regs16bit_;
    // List of 32-bit registers in the target machine.
    std::vector<RegInfo> regs32bit_;
    // List of 64-bit registers in the target machine.
    std::vector<RegInfo> regs64bit_;

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

    bool littleEndian_;
    /// Minimum number of 32 bit registers.
    unsigned static const REQUIRED_I32_REGS;
    /// List of register that are associated with a guard on a bus.
    std::set<RegInfo> guardedRegs_;

    /// Register files whose last reg reserved for temp reg copies.
    std::vector<TTAMachine::RegisterFile*> tempRegFiles_;

    typedef std::map<std::string, std::vector<std::string> > RegClassMap;
    /// All registers in certain group
    RegClassMap regsInClasses_;
    // Registers grouped by corresponding RFs
    RegClassMap regsInRFClasses_;
};

#endif
