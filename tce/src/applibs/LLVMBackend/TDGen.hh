/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
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
    void generateBackend(std::string& path) throw (Exception);

private:
    bool writeRegisterInfo(std::ostream& o) throw (Exception);
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

    bool checkRequiredRegisters() throw (Exception);
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

    std::string patOutputs(const Operation& op, bool intToBool);
    std::string patInputs(const Operation& op, int immOp, bool intToBool);

    std::string operandToString(
        const Operand& operand,
        bool match,
        bool immediate, int intToBool);


    std::string operationNodeToString(
        const Operation& op,
        const OperationDAG& dag,
        const OperationNode& node,
        int immOp,
        bool emulationPattern,
        int intToBool) throw (InvalidData);

    std::string dagNodeToString(
        const Operation& op,
        const OperationDAG& dag,
        const OperationDAGNode& node,
        int immOp,
        bool emulationPattern, int intToBool) throw (InvalidData);

    std::string operationPattern(
        const Operation& op,
        const OperationDAG& dag,
        int immOp, int intToBool);

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

    std::map<std::string, std::string> opNames_;

    /// Minimum number of 32 bit registers.
    unsigned static const REQUIRED_I32_REGS;
    /// List of register that are associated with a guard on a bus.
    std::set<RegInfo> guardedRegs_;
    /// True, if the target machine is fully connected with all busses<->ports
    bool fullyConnected_;
};

#endif
