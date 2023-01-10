/*
 Copyright (C) 2021-2022 Tampere University.

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
 * @file RV32MicroCodeGenerator.hh
 *
 * Declaration of RV32MicroCodeGenerator class.
 *
 * @author Kari Hepola 2021-2022 (kari.hepola@tuni.fi)
 * @note rating: red
 */

#ifndef TTA_RV32_TRANSLATOR_HH
#define TTA_RV32_TRANSLATOR_HH


#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <set>

#include "MicroCodeGenerator.hh"
#include "MachineConnectivityCheck.hh"

namespace TTAMachine {
    class Machine;
    class Socket;
    class Bus;
    class RFPort;
    class FUPort;
    class BaseFUPort;
    class FunctionUnit;
    class RegisterFile;
    class Port;
    class SpecialRegisterPort;
}

namespace TTAProgram {
    class Instruction;
}

class InstructionBitVector;
class BinaryEncoding;
class HDLTemplateInstantiator;
class ProgramImageGenerator;

using namespace TTAMachine;
using namespace TTAProgram;

namespace ProGe {

class RV32MicroCodeGenerator : public MicroCodeGenerator {

public:
    RV32MicroCodeGenerator(const Machine& machine, const BinaryEncoding& bem,
    const std::string& entityName);
    ~RV32MicroCodeGenerator();

    void generateRTL(HDLTemplateInstantiator& instantiator,
    const std::string& fileDst);

    void setBypassInstructionRegister(const bool& value);


private:
    std::unordered_map<std::string, BaseFUPort*> rs1Ports_;
    std::unordered_map<std::string, BaseFUPort*> rs2Ports_;
    std::unordered_map<std::string, BaseFUPort*> rdPorts_;
    std::unordered_map<std::string, BaseFUPort*> simmPorts_;

    void initializeOperations();
    
    std::unordered_map<Port*, int> sourcePortID_;

    std::unordered_map<Port*, std::vector<std::string>>
    sourceOperationMap_;

    void findOperationSources();

    void generateFUTargetProcess(std::ofstream& stream);

    void addRs1ForwardingConditions(std::map<std::string, std::string>
    ops, std::unordered_map<std::string, InstructionBitVector*>
    (ProGe::RV32MicroCodeGenerator::*instructionFunc)(Port* p1, Port* p2) const,
    std::ofstream& stream) const;

    void addRs2ForwardingConditions(std::map<std::string, std::string>
    ops, std::unordered_map<std::string, InstructionBitVector*>
    (ProGe::RV32MicroCodeGenerator::*instructionFunc)(Port* p1, Port* p2) const,
    std::ofstream& stream) const;

    std::string generateOperationLatencyLogic(HDLTemplateInstantiator& 
    instantiator);

    std::map<std::string, std::string> rOperations_;
    std::map<std::string, std::string> iOperations_;
    std::map<std::string, std::string> sOperations_;
    std::map<std::string, std::string> bOperations_;
    std::map<std::string, std::string> ujOperations_;


    std::set<Port*> operationPorts(
    const std::unordered_map<std::string, BaseFUPort*>& ports) const;

    void validateOperations() const;

    FunctionUnit* mapFunctionUnit(const std::string& operation) const;

    void addRPorts(const std::string& opName);
    void addIPorts(const std::string& opName);
    void addSPorts(const std::string& opName);
    void addBPorts(const std::string& opName);
    void addUJPorts(const std::string& opName);
    
    void findOperationPorts();
     
    void connectRF();
    void findRF();

    void findBusWidths();

    bool findConnectedBusses(Connection& rs1,
    Connection& rs2, Connection& rd, Connection& simm, const bool& forwarding)
    const;

    std::unordered_map<std::string, InstructionBitVector*>
    constructRInstructions(Port* src1, Port* src2) const;
    std::unordered_map<std::string, InstructionBitVector*>
    constructIInstructions(Port* src1, Port* src2) const;
    std::unordered_map<std::string, InstructionBitVector*>
    constructSInstructions(Port* src1, Port* src2) const;
    std::unordered_map<std::string, InstructionBitVector*>
    constructBInstructions(Port* src1, Port* src2) const;
    std::unordered_map<std::string, InstructionBitVector*>
    constructUJInstructions() const;

    void addBitsToMap(
        std::unordered_map<std::string, InstructionBitVector*> instructions,
        const std::map<std::string, std::string> encodings,
        std::ofstream& stream) const;

    void generateMap(const std::string& dstDirectory);

    void generateWrapper(HDLTemplateInstantiator& instantiator,
    const std::string& fileDst);

    void generateNOP();

    void throwOperationNotFoundError(const std::string& op) const;

    void throwTriggeringPortError(const BaseFUPort* port,
    const std::string& type) const;

    void throwInputPortError(const BaseFUPort* port,
    const std::string& type) const;

    void throwOutputPortError(const BaseFUPort* port,
    const std::string& type) const;

    void throwOperandCountError(
    const std::string& op, int required, int found) const;

    std::vector<Bus*> busses_;

    RegisterFile* RF_;

    int rs1BusWidth_;
    int rs2BusWidth_;
    int rdBusWidth_;
    int simmBusWidth_;

    int rs1BusStart_;
    int rs2BusStart_;
    int rdBusStart_;
    int simmBusStart_;


    int rs1RFStart_;
    int rs2RFStart_;
    int rdRFStart_;
    int simmRFStart_;

    Bus* rs1Bus_;
    Bus* rs2Bus_;
    Bus* rdBus_;
    Bus* simmBus_;


    RFPort* rs1RFPort_;
    RFPort* rs2RFPort_;
    RFPort* rdRFPort_;

    ProgramImageGenerator* pig_;

    std::string NOP_;
    bool bypassInstructionRegister_;
    bool hasForwarding_;
    bool variableLengthOpLatency_;
    bool eVariant_;

};
}
#endif
