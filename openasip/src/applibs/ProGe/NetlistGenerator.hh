/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file NetlistGenerator.hh
 *
 * Declaration of NetlistGenerator class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_NETLIST_GENERATOR_HH
#define TTA_NETLIST_GENERATOR_HH

#include <iostream>
#include <map>
#include <vector>

#include "Exception.hh"
#include "FUImplementationLocation.hh"
#include "HDBTypes.hh"
#include "ProGeTypes.hh"
#include "RFImplementationLocation.hh"
#include "TCEString.hh"
#include "ProGeOptions.hh"

namespace IDF {
    class MachineImplementation;
    class FUGenerated;
    class RFGenerated;
}

namespace HDB {
    class FUEntry;
    class RFEntry;
    class FUImplementation;
}

namespace TTAMachine {
    class Unit;
    class FunctionUnit;
    class ControlUnit;
    class FUPort;
    class Machine;
    class BaseRegisterFile;
    class AddressSpace;
    class Port;
    class ImmediateUnit;
}

namespace ProGe {

    class ICDecoderGeneratorPlugin;
    class Netlist;
    class NetlistBlock;
    class NetlistPort;
    class ProGeContext;
    class Signal;
    class GeneratableFUNetlistBlock;
    class GeneratableRFNetlistBlock;

    /**
     * Generates a netlist of processor building blocks. [DEPRECATED]
     */
    class NetlistGenerator {
        friend class GeneratableFUNetlistBlock;
        friend class GeneratableRFNetlistBlock;

    public:
        NetlistGenerator(
            const ProGeContext& context, ICDecoderGeneratorPlugin& plugin);
        virtual ~NetlistGenerator();

        NetlistBlock* generate(
            const ProGeOptions& options, int imemWidthInMAUs,
            TCEString entityNameStr, std::ostream& warningStream);

        NetlistPort& netlistPort(
            const TTAMachine::Port& port, Direction dir = IN) const;
        NetlistBlock& netlistBlock(const TTAMachine::Unit& unit) const;
        NetlistPort& loadPort(const NetlistPort& port) const;
        bool hasOpcodePort(const NetlistPort& port) const;
        NetlistPort& rfOpcodePort(const NetlistPort& port) const;
        NetlistPort& rfGuardPort(const NetlistBlock& rfBlock) const;
        NetlistPort& fuOpcodePort(const NetlistBlock& fuBlock) const;
        NetlistPort& fuGuardPort(const NetlistPort& fuPort) const;
        NetlistPort& clkPort(const NetlistBlock& block) const;
        NetlistPort& rstPort(const NetlistBlock& block) const;
        bool hasGlockPort(const NetlistBlock& block) const;
        NetlistPort& glockPort(const NetlistBlock& block) const;
        bool hasGlockReqPort(const NetlistBlock& block) const;
        NetlistPort& glockReqPort(const NetlistBlock& block) const;

        NetlistPort& immediateUnitWritePort(
            const TTAMachine::ImmediateUnit& iu) const;
        NetlistPort& gcuReturnAddressInPort() const;
        NetlistPort& gcuReturnAddressOutPort() const;

        NetlistBlock& ttaCore() const;
        NetlistBlock& instructionDecoder() const;
        NetlistBlock& instructionDecompressor() const;
        NetlistBlock& instructionFetch() const;

        HDB::FUEntry& fuEntry(const std::string& fuName) const;
        HDB::RFEntry& rfEntry(const std::string& rfName) const;
        bool rfHasEntry(const std::string& rfName) const;

        const ProGeContext& context() const { return context_; }

        /// Instruction word port name in instruction decoder.
        static const std::string DECODER_INSTR_WORD_PORT;
        /// Reset port name in instruction decoder.
        static const std::string DECODER_RESET_PORT;
        /// Clock port name in instruction decoder.
        static const std::string DECODER_CLOCK_PORT;
        /// RA load port name in instruction decoder.
        static const std::string DECODER_RA_LOAD_PORT;
        /// PC load port name in instruction decoder.
        static const std::string DECODER_PC_LOAD_PORT;
        /// PC opcode port name in instruction decoder.
        static const std::string DECODER_PC_OPCODE_PORT;
        /// Lock request out port name in instruction decoder.
        static const std::string DECODER_LOCK_REQ_OUT_PORT;
        /// Lock request in port name in instruction decoder.
        static const std::string DECODER_LOCK_REQ_IN_PORT;
        // Name of lock status port in instruction decoder.
        static const std::string DECODER_LOCK_STATUS_PORT;
        /// Lock request in port name in decompressor.
        static const std::string DECOMP_LOCK_REQ_IN_PORT;
        /// Global lock out port name in decompressor.
        static const std::string DECOMP_GLOCK_PORT;
        /// Instruction word port name in decompressor.
        static const std::string DECOMP_INSTR_WORD_PORT;
        // Name of address width parameter
        static const std::string ADDR_WIDTH;
        // Name of the fetch block port
        static const std::string FETCHBLOCK_PORT_NAME;

        void mapGlobalLockRequestPort(
            const NetlistBlock& block, NetlistPort& glockReqPort);

    private:
        /// Multimap type to map ADF ports to NetlistPorts.
        typedef std::multimap<const TTAMachine::Port*, NetlistPort*>
            PortCorrespondenceMap;
        /// Map type to map ADF Units to NetlistBlocks
        typedef std::map<const TTAMachine::Unit*, NetlistBlock*>
            UnitCorrespondenceMap;
        /// Map type for NetlistPorts.
        typedef std::map<const NetlistPort*, NetlistPort*> PortRelationMap;
        /// Map type for NetlistPorts.
        typedef std::map<const NetlistBlock*, NetlistPort*> PortPurposeMap;
        /// Map type for port of immediate units.
        typedef std::map<const TTAMachine::ImmediateUnit*, NetlistPort*>
            IUPortMap;
        /// Map type for FUImplementation.
        typedef std::map<std::string, HDB::FUEntry*> FUEntryMap;
        /// Map type for RFImplementation.
        typedef std::map<std::string, HDB::RFEntry*> RFEntryMap;

        void addGCUToNetlist(NetlistBlock& toplevelBlock, int imemWidthInMAUs);
        void addFUToNetlist(
            const IDF::FUImplementationLocation& location,
            NetlistBlock& netlistBlock, std::ostream& warningStream);
        GeneratableFUNetlistBlock* addGeneratableFUsToNetlist(
            const IDF::FUGenerated& fug, NetlistBlock& netlistBlock);
        GeneratableRFNetlistBlock* addGeneratableRFsToNetlist(
            const IDF::RFGenerated& rfg, NetlistBlock& netlistBlock);
        void addFUExternalPortsToNetlist(
            const HDB::FUImplementation& fuImplementation,
            NetlistBlock& coreBlock, NetlistBlock& fuBlock,
            const TTAMachine::FunctionUnit& adfFU);
        void addRFToNetlist(
            const ProGeOptions& options,
            const IDF::RFImplementationLocation& location,
            NetlistBlock& netlistBlock);
        void addIUToNetlist(
            const ProGeOptions& options,
            const IDF::RFImplementationLocation& location,
            NetlistBlock& netlistBlock);
        void addBaseRFToNetlist(
            const ProGeOptions& options,
            const TTAMachine::BaseRegisterFile& regFile,
            const IDF::RFImplementationLocation& location,
            NetlistBlock& netlistBlock, const std::string& blockNamePrefix);
        void mapNetlistPort(
            const TTAMachine::Port& adfPort, NetlistPort& netlistPort);
        void mapNetlistBlock(
            const TTAMachine::Unit& unit, NetlistBlock& netlistBlock);
        bool netlistPortIsMapped(const TTAMachine::Port& adfPort);
        void mapLoadPort(const NetlistPort& port, NetlistPort& loadPort);
        void mapRFOpcodePort(const NetlistPort& port, NetlistPort& opcodePort);
        void mapClockPort(const NetlistBlock& block, NetlistPort& clkPort);
        void mapResetPort(const NetlistBlock& block, NetlistPort& resetPort);
        void
        mapGlobalLockPort(const NetlistBlock& block, NetlistPort& glockPort);
        void mapRFGuardPort(const NetlistBlock& block, NetlistPort& guardPort);
        void
        mapFUGuardPort(const NetlistPort& dataPort, NetlistPort& guardPort);
        void
        mapFUOpcodePort(const NetlistBlock& block, NetlistPort& opcodePort);
        void mapImmediateUnitWritePort(
            const TTAMachine::ImmediateUnit& iu, NetlistPort& port);
        bool isParameterizable(
            const std::string& paramName, const HDB::FUEntry* fuEntry) const;
        unsigned int
        calculateAddressWidth(TTAMachine::FunctionUnit const* fu) const;

        TCEString checkInstanceName(const TCEString& baseInstanceName,
            const TCEString& moduleName) const;

        bool isLSU(const TTAMachine::FunctionUnit& fu) const;
        static bool isLSUDataPort(
            const TTAMachine::FunctionUnit& adfFU, const std::string& portName);
        static Signal inferLSUSignal(const std::string& portName);

        static int opcodePortWidth(
            const HDB::FUEntry& fu, std::ostream& warningStream);
        static int opcodePortWidth(const TTAMachine::ControlUnit& gcu);
        static TTAMachine::FUPort& findCorrespondingPort(
            const TTAMachine::FunctionUnit& fuToSearch,
            const TTAMachine::FunctionUnit& origFU,
            const std::string& portName);
        static int instructionMemoryAddressWidth(
            const TTAMachine::Machine& machine);
        static int instructionMemoryWidth(const TTAMachine::Machine& machine);
        static TTAMachine::AddressSpace& instructionMemory(
            const TTAMachine::Machine& machine);
        static Direction translateDirection(HDB::Direction direction);

        const ProGeContext& context_;
        /// The generator plugin.
        ICDecoderGeneratorPlugin& plugin_;
        /// Maps the ADF ports to the netlist ports.
        PortCorrespondenceMap portCorrespondenceMap_;
        /// Maps the ADF units to the netlist blocks.
        UnitCorrespondenceMap unitCorrespondenceMap_;
        /// Maps loads ports.
        PortRelationMap loadPortMap_;
        /// Maps opcode ports.
        PortRelationMap rfOpcodePortMap_;
        /// Maps clock ports.
        PortPurposeMap clkPorts_;
        /// Maps reset ports.
        PortPurposeMap rstPorts_;
        /// Maps RF guard ports.
        PortPurposeMap rfGuardPorts_;
        /// Maps global lock ports.
        PortPurposeMap glockPorts_;
        /// Maps global lock request ports.
        PortPurposeMap glockReqPorts_;
        /// Maps FU guard ports.
        PortRelationMap fuGuardPortMap_;
        /// Maps FU opcode ports.
        PortPurposeMap fuOpcodePorts_;
        /// Maps the created netlist ports to immediate units
        IUPortMap iuPortMap_;
        /// The TTA core block
        NetlistBlock* coreBlock_;
        /// The instruction decoder block.
        NetlistBlock* instructionDecoder_;
        /// The instruction decompressor block
        NetlistBlock* instructionDecompressor_;
        /// The instruction fetch block
        NetlistBlock* instructionFetch_;
        /// Maps FU implementations for different FU's.
        FUEntryMap fuEntryMap_;
        /// Maps RF implementations for different RF's.
        RFEntryMap rfEntryMap_;
        /// Return address in port in GCU (ifetch).
        NetlistPort* raInPort_;
        /// Returns address out port in GCU (ifetch).
        NetlistPort* raOutPort_;
    };
}

#endif
