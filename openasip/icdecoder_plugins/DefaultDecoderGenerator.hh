/*
    Copyright (c) 2002-2011 Tampere University.

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
 * @file DefaultDecoderGenerator.hh
 *
 * Declaration of DefaultDecoderGenerator class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @author Pekka Jääskeläinen 2011
 * @author Vinogradov Viacheslav(added Verilog generating) 2012 
 */

#ifndef TTA_DEFAULT_DECODER_GENERATOR_HH
#define TTA_DEFAULT_DECODER_GENERATOR_HH

#include <string>
#include <vector>
#include <set>
#include <map>

#include "ProGeTypes.hh"
#include "TCEString.hh"
#include "Exception.hh"

namespace TTAMachine {
    class Machine;
    class Unit;
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
    class ControlUnit;
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

    void setGenerateDebugger(bool generate);
    void setGenerateNoLoopbackGlock(bool generate);
    void setSyncReset(bool value);
    void setGenerateBusEnable(bool value);

    void SetHDL(ProGe::HDL language);
    
    void completeDecoderBlock(
        const ProGe::NetlistGenerator& nlGenerator,
        ProGe::NetlistBlock& coreBlock);
    void generateInstructionDecoder(
        const ProGe::NetlistGenerator& nlGenerator,
        const std::string& dstDirectory);
    std::set<int> requiredRFLatencies(
        const TTAMachine::ImmediateUnit& iu) const;
    void verifyCompatibility() const;
    int glockRequestWidth() const;
    int glockPortWidth() const;

    void setGenerateLockTrace(bool generate);
    void setLockTraceStartingCycle(unsigned int startCycle);

    static const std::string RISCV_SIMM_PORT_IN_NAME;
    static const std::string GLOCK_PORT_NAME;

private:
    /// Set type for buses.
    typedef std::set<TTAMachine::Bus*> BusSet;
    /// Types for mapping global lock and global lock request signals
    typedef int GlockBitType;
    typedef int GlockReqBitType;
    typedef std::map<GlockBitType, const TTAMachine::Unit*>
        UnitGlockBitMapType;
    typedef std::map<const TTAMachine::Unit*, GlockReqBitType>
        UnitGlockReqBitMapType;

    void addLockReqPortToDecoder();
    void addGlockPortToDecoder();

    void writeComment(std::ostream& stream, int indent,
                      std::string comment) const;
    void writeSignalDeclaration(std::ostream& stream, ProGe::DataType type,
                                std::string sigName, int width) const;

    void writeInstructionDecoder(std::ostream& stream);
    ///void writeDecompressSignalsVHDL(std::ostream& stream) const; TBR
    void writeLockDumpCode(std::ostream& stream) const;
    void writeMoveFieldSignals(std::ostream& stream) const;
    void writeImmediateSlotSignals(std::ostream& stream) const;
    void writeLongImmediateTagSignal(std::ostream& stream) const;
    void writeSquashSignals(std::ostream& stream) const;
    void writeSocketCntrlSignals(std::ostream& stream);
    void writeFUCntrlSignals(std::ostream& stream);
    void writeFUCntrlSignals(const TTAMachine::FunctionUnit& fu,
                             std::ostream& stream);
    void writeRFCntrlSignals(std::ostream& stream);
    void writeGlockHandlingSignals(std::ostream& stream) const;
    void writePipelineFillSignals(std::ostream& stream) const;
    void writeFullNOPConstant(std::ostream& stream) const;
    std::string writeNOPEncodingVHDL() const;
    //void writeDecompressTableVHDL(std::ostream& stream) const; TBR
    void writeDismemberingAndITDecompression(std::ostream& stream) const;
    void writeInstructionDismembering(std::ostream& stream) const;
    void writeSquashSignalGenerationProcesses(std::ostream& stream) const;
    void writeSquashSignalGenerationProcess(
        const TTAMachine::Bus& bus,
        std::ostream& stream) const;
    void writeLongImmediateWriteProcess(std::ostream& stream) const;
    void writeControlRegisterMappings(std::ostream& stream) const;
    void writeRFSRAMDecodingProcess(std::ostream& stream) const;
    void writeMainDecodingProcess(std::ostream& stream) const;
    void writeGlockMapping(std::ostream& stream) const;
    void writePipelineFillProcess(std::ostream& stream) const;
    void writeResettingOfControlRegisters(std::ostream& stream) const;
    void writeInstructionDecoding(std::ostream& stream) const;
    void writeRulesForSourceControlSignals(std::ostream& stream) const;
    void writeRulesForDestinationControlSignals(std::ostream& stream) const;
//    void writeCUOpcodeSettings(
//        std::ostream& stream, const TTAMachine::ControlUnit& cu) const;
    void writeSimmDataSignal(
        const TTAMachine::Bus& bus, std::ostream& stream) const;
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
        const ProGe::HDL language,
        const TTAMachine::InstructionTemplate& iTemp,
        int indLevel,
        std::ostream& stream) const;
    void writeBusMuxControlLogic(const TTAMachine::Bus& bus,
            const std::set<TTAMachine::Socket*> outputSockets,
            std::ostream& stream) const;

    static void writeSquashSignalSubstitution(
        const ProGe::HDL language,
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
        const std::string& portName,
        bool async = false);
    static std::string rfOpcodeSignalName(
        const std::string& rfName, 
        const std::string& portName,
        bool async = false);
    static std::string rfOpcodeCntrlPort(
        const std::string& rfName,
        const std::string& portName);
    static std::string iuReadOpcodeCntrlPort(
        const std::string& unitName,
        const std::string& portName);
    static std::string iuReadOpcodeCntrlSignal(
        const std::string& unitName,
        const std::string& portName);
    static std::string iuReadLoadCntrlPort(
        const std::string& unitName,
        const std::string& portName);
    static std::string iuReadLoadCntrlSignal(
        const std::string& unitName,
        const std::string& portName);
    static std::string iuWritePort(const std::string& iuName);
    static std::string iuWriteSignal(const std::string& iuName);
    static std::string iuWriteOpcodeCntrlPort(
        const std::string& unitName);
    static std::string iuWriteOpcodeCntrlSignal(
        const std::string& unitName);
    static std::string iuWriteLoadCntrlPort(
        const std::string& unitName);
    static std::string iuWriteLoadCntrlSignal(
        const std::string& unitName);
    static std::string busMuxCntrlSignal(const TTAMachine::Bus& bus);
    static std::string busMuxCntrlRegister(const TTAMachine::Bus& bus);
    static std::string busMuxEnableSignal(const TTAMachine::Bus& bus);
    static std::string busMuxEnableRegister(const TTAMachine::Bus& bus);

    static std::string socketBusControlPort(const std::string& name);
    static std::string socketDataControlPort(const std::string& name);
    static std::string moveFieldSignal(const std::string& busName);
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
        const ProGe::HDL language,
        const SlotField& srcField,
        const std::string& socketName);
    static std::string portCodeCondition(
        const ProGe::HDL language,
        const SocketEncoding& socketEnc,
        const PortCode& code);
    std::string instructionTemplateCondition(
        const ProGe::HDL language,
        const std::string& iTempName) const;
    static std::string rfOpcodeFromSrcOrDstField(
        const ProGe::HDL language,
        const SocketEncoding& socketEnc,
        const PortCode& code);

    std::string busCntrlSignalPinOfSocket(
        const TTAMachine::Socket& socket,
        const TTAMachine::Bus& bus) const;
    int opcode(const TTAMachine::HWOperation& operation) const;
    static std::string indentation(unsigned int level);
    bool sacEnabled(const std::string& rfName) const;
    
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
    /// Tells whether to generate global lock tracing code.
    bool generateLockTrace_;
    TCEString entityNameStr_;
    ProGe::HDL language_;
    /// Generate debugger signals?
    bool generateDebugger_;
	/// Reset synchronously (otherwise asynchronous)
    bool syncReset_;
    /// Bus enable signals for bustrace
    bool generateBusEnable_;
    /// The starting cycle for bus tracing.
    unsigned int lockTraceStartingCycle_;
    /// The flag to generate global lock request handling in decoder.
    /// False means delegating the lock request towards instruction fetch.
    bool generateAlternateGlockReqHandling_;
    /// Maps connected glock port bits to associated TTA Units
    UnitGlockBitMapType unitGlockBitMap_;
    /// Maps TTA Units to associated glock request port bits.
    UnitGlockReqBitMapType unitGlockReqBitMap_;

    /// Bookkeeping for reset-needing signals
    std::vector<std::string> registerVectors;
    std::vector<std::string> registerBits;
};

#endif

