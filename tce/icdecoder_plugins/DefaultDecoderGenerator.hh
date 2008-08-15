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
 * @file DefaultDecoderGenerator.hh
 *
 * Declaration of DefaultDecoderGenerator class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 */

#ifndef TTA_DEFAULT_DECODER_GENERATOR_HH
#define TTA_DEFAULT_DECODER_GENERATOR_HH

#include <string>
#include <set>

#include "Exception.hh"

namespace TTAMachine {
    class Machine;
    class Bus;
    class FunctionUnit;
    class PortGuard;
    class Guard;
    class RegisterGuard;
    class Socket;
    class RegisterFile;
    class RFPort;
    class BaseFUPort;
    class HWOperation;
    class InstructionTemplate;
    class BaseRegisterFile;
    class ImmediateUnit;
}

namespace ProGe {
    class Netlist;
    class NetlistGenerator;
    class NetlistPort;
    class NetlistBlock;
}

class BinaryEncoding;
class GuardEncoding;
class GPRGuardEncoding;
class FUGuardEncoding;
class SourceField;
class SocketEncoding;
class PortCode;
class CentralizedControlICGenerator;
class SlotField;


/**
 * Generates the default instruction decoder in VHDL.
 */
class DefaultDecoderGenerator {
public:
    DefaultDecoderGenerator(
        const TTAMachine::Machine& machine, 
        const BinaryEncoding& bem,
        const CentralizedControlICGenerator& icGenerator);
    virtual ~DefaultDecoderGenerator();

    void completeDecoderBlock(
        const ProGe::NetlistGenerator& nlGenerator,
        ProGe::Netlist& netlist);
    void generateInstructionDecoder(
        const ProGe::NetlistGenerator& nlGenerator,
        const std::string& dstDirectory)
        throw (IOException);
    int requiredRFLatency(const TTAMachine::ImmediateUnit& iu) const;
    void verifyCompatibility() const
        throw (InvalidData);

private:
    /// Set type for buses.
    typedef std::set<TTAMachine::Bus*> BusSet;

    void addLockReqPortToDecoder() const;
    void addGlockPortToDecoder() const;
    int glockRequestWidth() const;

    void writeInstructionDecoder(std::ostream& stream) const;
    void writeSourceDestinationAndGuardSignals(std::ostream& stream) const;
    void writeImmediateSlotSignals(std::ostream& stream) const;
    void writeLongImmediateTagSignal(std::ostream& stream) const;
    void writeSquashSignals(std::ostream& stream) const;
    void writeSocketCntrlSignals(std::ostream& stream) const;
    void writeFUCntrlSignals(std::ostream& stream) const;
    void writeFUCntrlSignals(
        const TTAMachine::FunctionUnit& fu,
        std::ostream& stream) const;
    void writeRFCntrlSignals(std::ostream& stream) const;
    void writeInstructionDismembering(std::ostream& stream) const;
    void writeSquashSignalGenerationProcesses(std::ostream& stream) const;
    void writeSquashSignalGenerationProcess(
        const TTAMachine::Bus& bus,
        std::ostream& stream) const;
    void writeLongImmediateWriteProcess(std::ostream& stream) const;
    void writeControlRegisterMappings(std::ostream& stream) const;
    void writeMainDecodingProcess(std::ostream& stream) const;
    void writeResettingOfControlRegisters(std::ostream& stream) const;
    void writeInstructionDecoding(std::ostream& stream) const;
    void writeRulesForSourceControlSignals(std::ostream& stream) const;
    void writeRulesForDestinationControlSignals(std::ostream& stream) const;

    void writeBusControlRulesOfOutputSocket(
        const TTAMachine::Socket& socket,
        std::ostream& stream) const;
    void writeBusControlRulesOfSImmSocketOfBus(
        const TTAMachine::Bus& bus,
        std::ostream& stream) const;
    void writeControlRulesOfRFReadPort(
        const TTAMachine::RFPort& port,
        std::ostream& stream) const;
    void writeControlRulesOfFUOutputPort(
        const TTAMachine::BaseFUPort& port,
        std::ostream& stream) const;
    void writeControlRulesOfFUInputPort(
        const TTAMachine::BaseFUPort& port,
        std::ostream& stream) const;
    void writeControlRulesOfRFWritePort(
        const TTAMachine::RFPort& port,
        std::ostream& stream) const;
    void writeInstructionTemplateProcedures(
        const TTAMachine::InstructionTemplate& iTemp,
        int indLevel,
        std::ostream& stream) const;

    static void writeSquashSignalSubstitution(
        const TTAMachine::Bus& bus,
        const GuardEncoding& enc,
        const TTAMachine::Guard& guard,
        std::ostream& stream,
        int indLevel);
    static bool containsSimilarGuard(
        const std::set<TTAMachine::PortGuard*>& guardSet, 
        const TTAMachine::PortGuard& guard);
    static bool containsSimilarGuard(
        const std::set<TTAMachine::RegisterGuard*>& guardSet,
        const TTAMachine::RegisterGuard& guard);
    static bool needsBusControl(const TTAMachine::Socket& socket);
    static bool needsDataControl(const TTAMachine::Socket& socket);

    TTAMachine::RegisterGuard& findGuard(
        const GPRGuardEncoding& encoding) const;
    TTAMachine::PortGuard& findGuard(const FUGuardEncoding& encoding) const;

    static std::string simmDataPort(const std::string& busName);
    static std::string simmControlPort(const std::string& busName);
    static int simmPortWidth(const TTAMachine::Bus& bus);
    static std::string simmDataSignalName(const std::string& busName);
    static std::string simmCntrlSignalName(const std::string& busName);
    static std::string fuLoadCntrlPort(
        const std::string& fuName,
        const std::string& portName);
    static std::string fuLoadSignalName(
        const std::string& fuName,
        const std::string& portName);
    static std::string fuOpcodeCntrlPort(const std::string& fu);
    static std::string fuOpcodeSignalName(const std::string& fu);
    static std::string rfLoadCntrlPort(
        const std::string& rfName,
        const std::string& portName);
    static std::string rfLoadSignalName(
        const std::string& rfName,
        const std::string& portName);
    static std::string rfOpcodeSignalName(
        const std::string& rfName, 
        const std::string& portName);
    static std::string rfOpcodeCntrlPort(
        const std::string& rfName,
        const std::string& portName);
    static std::string iuReadOpcodeCntrlPort(
        const std::string& unitName,
        const std::string& portName);
    static std::string iuReadLoadCntrlPort(
        const std::string& unitName,
        const std::string& portName);
    static std::string iuWritePort(const std::string& iuName);
    static std::string iuWriteOpcodeCntrlPort(
        const std::string& unitName);
    static std::string iuWriteLoadCntrlPort(
        const std::string& unitName);

    static std::string socketBusControlPort(const std::string& name);
    static std::string socketDataControlPort(const std::string& name);
    static std::string guardPortName(const TTAMachine::Guard& guard);
    static std::string srcFieldSignal(const std::string& busName);
    static std::string dstFieldSignal(const std::string& busName);
    static std::string guardFieldSignal(const std::string& busName);
    static std::string immSlotSignal(const std::string& immSlot);
    static std::string squashSignal(const std::string& busName);
    static std::string socketBusCntrlSignalName(const std::string& name);
    static std::string socketDataCntrlSignalName(const std::string& name);
    static std::string gcuDataPort(const std::string& nameInADF);

    int opcodeWidth(const TTAMachine::FunctionUnit& fu) const;
    static int busControlWidth(const TTAMachine::Socket& socket);
    static int dataControlWidth(const TTAMachine::Socket& socket);
    static int rfOpcodeWidth(const TTAMachine::BaseRegisterFile& rf);

    static BusSet connectedBuses(const TTAMachine::Socket& socket);
    static std::string socketEncodingCondition(
        const SlotField& srcField,
        const std::string& socketName);
    static std::string portCodeCondition(
        const SocketEncoding& socketEnc,
        const PortCode& code);
    std::string instructionTemplateCondition(
        const std::string& iTempName) const;
    static std::string rfOpcodeFromSrcOrDstField(
        const SocketEncoding& socketEnc,
        const PortCode& code);

    std::string busCntrlSignalPinOfSocket(
        const TTAMachine::Socket& socket,
        const TTAMachine::Bus& bus) const;
    int opcode(const TTAMachine::HWOperation& operation) const;
    static std::string indentation(unsigned int level);

    /// The machine.
    const TTAMachine::Machine& machine_;
    /// The binary encoding map.
    const BinaryEncoding& bem_;
    /// The IC generator.
    const CentralizedControlICGenerator& icGenerator_;
    /// The netlist generator.
    const ProGe::NetlistGenerator* nlGenerator_;    

    /// The instruction decoder block in the netlist.
    ProGe::NetlistBlock* decoderBlock_;
};

#endif

