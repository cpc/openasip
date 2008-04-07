/**
 * @file TDGen.hh
 *
 * Declaration of TDGen class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2008 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_TDGEN_HH
#define TTA_TDGEN_HH

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include "Exception.hh"

class Operation;
class Operand;
class OperationDAG;
class OperationNode;
class OperationDAGNode;
class TerminalNode;

namespace TTAMachine {
    class Machine;
}

/**
 * TCE Backend plugin source code and .td definition generator.
 *
 * Generates files for building target architecture plugin for LLVM-TCE
 * backend.
 */
class TDGen {
public:
    TDGen(const TTAMachine::Machine& mach);
    void generateBackend(std::string& path);

private:
    bool writeRegisterInfo(std::ostream& o);
    void writeInstrInfo(std::ostream& o);
    void writeBackendCode(std::ostream& o);
    void writeTopLevelTD(std::ostream& o);
   
   
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

    bool checkRequiredRegisters();
    void analyzeRegisters();

    void writeRegisterDef(
        std::ostream& o,
        const RegInfo& reg,
        const std::string regName,
        const std::string regTemplate,
        const std::string aliases,
        RegType type
        );

    void write64bitRegisterInfo(std::ostream& o);
    void write32bitRegisterInfo(std::ostream& o);
    void write16bitRegisterInfo(std::ostream& o);
    void write8bitRegisterInfo(std::ostream& o);
    void write1bitRegisterInfo(std::ostream& o);
    void writeRARegisterInfo(std::ostream& o);

    void writeOperationDef(std::ostream& o, Operation& op);
    void writeEmulationPattern(
        std::ostream& o,
        const Operation& op,
        const OperationDAG& dag);

    void writeCallDef(std::ostream& o);

    std::string llvmOperationPattern(const std::string& osalOperationName);

    std::string patOutputs(const Operation& op);
    std::string patInputs(const Operation& op, int immOp);

    std::string operandToString(
        const Operand& operand,
        bool match,
        bool immediate);


    std::string operationNodeToString(
        const Operation& op,
        const OperationDAG& dag,
        const OperationNode& node,
        int immOp,
        bool emulationPattern) throw (InvalidData);

    std::string dagNodeToString(
        const Operation& op,
        const OperationDAG& dag,
        const OperationDAGNode& node,
        int immOp,
        bool emulationPattern) throw (InvalidData);

    std::string operationPattern(
        const Operation& op,
        const OperationDAG& dag,
        int immOp);

    OperationDAG* createTrivialDAG(Operation& op);
    bool canBeImmediate(const OperationDAG& dag, const TerminalNode& node);

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

    std::vector<std::pair<std::string, std::string> > opcodes_;

    /// Minimum number of 32 bit registers.
    unsigned static const REQUIRED_I32_REGS;
    /// List of register that are associated with a guard on a bus.
    std::set<RegInfo> guardedRegs_;
    /// True, if the target machine is fully connected with all busses<->ports
    bool fullyConnected_;
    /// List of warning messages.
    std::vector<std::string> warnings_;
};

#endif
