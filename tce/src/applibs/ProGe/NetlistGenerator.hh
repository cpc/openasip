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
 * @file NetlistGenerator.hh
 *
 * Declaration of NetlistGenerator class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_NETLIST_GENERATOR_HH
#define TTA_NETLIST_GENERATOR_HH

#include <map>

#include "RFImplementationLocation.hh"
#include "FUImplementationLocation.hh"
#include "Exception.hh"

namespace IDF {
    class MachineImplementation;
}

namespace HDB {
    class FUEntry;
}

namespace TTAMachine {
    class FunctionUnit;
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

/**
 * Generates a netlist of processor building blocks.
 */
class NetlistGenerator {
public:
    NetlistGenerator(
        const TTAMachine::Machine& machine,
        const IDF::MachineImplementation& implementation,
        ICDecoderGeneratorPlugin& plugin);
    virtual ~NetlistGenerator();

    Netlist* generate(int imemWidthInMAUs)
        throw (IOException, InvalidData, OutOfRange, InstanceNotFound);

    NetlistPort& netlistPort(const TTAMachine::Port& port) const
        throw (InstanceNotFound);
    NetlistPort& loadPort(const NetlistPort& port) const
        throw (InstanceNotFound);
    NetlistPort& rfOpcodePort(const NetlistPort& port) const
        throw (InstanceNotFound);
    NetlistPort& rfGuardPort(const NetlistBlock& rfBlock) const
        throw (InstanceNotFound);
    NetlistPort& fuOpcodePort(const NetlistBlock& fuBlock) const
        throw (InstanceNotFound);
    NetlistPort& fuGuardPort(const NetlistPort& fuPort) const
        throw (InstanceNotFound);
    NetlistPort& clkPort(const NetlistBlock& block) const
        throw (InstanceNotFound);
    NetlistPort& rstPort(const NetlistBlock& block) const
        throw (InstanceNotFound);
    bool hasGlockPort(const NetlistBlock& block) const;
    NetlistPort& glockPort(const NetlistBlock& block) const
        throw (InstanceNotFound);
    bool hasGlockReqPort(const NetlistBlock& block) const;
    NetlistPort& glockReqPort(const NetlistBlock& block) const
        throw (InstanceNotFound);

    NetlistPort& immediateUnitWritePort(
        const TTAMachine::ImmediateUnit& iu) const
        throw (InstanceNotFound);
    NetlistPort& gcuReturnAddressInPort() const
        throw (InstanceNotFound);
    NetlistPort& gcuReturnAddressOutPort() const
        throw (InstanceNotFound);

    NetlistBlock& instructionDecoder() const
        throw (InstanceNotFound);

    HDB::FUEntry& fuEntry(const std::string& fuName) const
        throw (InstanceNotFound);

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
    // Name of address width parameter
    static const std::string ADDR_WIDTH;

private:
    /// Map type to map ADF ports to NetlistPorts.
    typedef std::map<const TTAMachine::Port*, NetlistPort*>
    PortCorrespondenceMap;
    /// Map type for NetlistPorts.
    typedef std::map<const NetlistPort*, NetlistPort*> PortRelationMap;
    /// Map type for NetlistPorts.
    typedef std::map<const NetlistBlock*, NetlistPort*> PortPurposeMap;
    /// Map type for port of immediate units.
    typedef std::map<const TTAMachine::ImmediateUnit*, NetlistPort*> IUPortMap;
    /// Map type for FUImplementation.
    typedef std::map<std::string, HDB::FUEntry*> FUEntryMap;

    void addGCUToNetlist(NetlistBlock& toplevelBlock, int imemWidthInMAUs);
    void addFUToNetlist(
        const IDF::FUImplementationLocation& location,
        Netlist& netlist)
        throw (IOException, InvalidData);
    void addRFToNetlist(
        const IDF::RFImplementationLocation& location,
        Netlist& netlist)
        throw (IOException, InvalidData);
    void addIUToNetlist(
        const IDF::RFImplementationLocation& location,
        Netlist& netlist)
        throw (IOException, InvalidData);
    void addBaseRFToNetlist(
        const TTAMachine::BaseRegisterFile& regFile,
        const IDF::RFImplementationLocation& location,
        Netlist& netlist,
        const std::string& blockNamePrefix)
        throw (IOException, InvalidData);

    void mapNetlistPort(
        const TTAMachine::Port& adfPort,
        NetlistPort& netlistPort);
    void mapLoadPort(const NetlistPort& port, NetlistPort& loadPort);
    void mapRFOpcodePort(const NetlistPort& port, NetlistPort& opcodePort);
    void mapClockPort(const NetlistBlock& block, NetlistPort& clkPort);
    void mapResetPort(const NetlistBlock& block, NetlistPort& resetPort);
    void mapGlobalLockPort(
        const NetlistBlock& block,
        NetlistPort& glockPort);
    void mapGlobalLockRequestPort(
        const NetlistBlock& block,
        NetlistPort& glockReqPort);
    void mapRFGuardPort(const NetlistBlock& block, NetlistPort& guardPort);
    void mapFUGuardPort(const NetlistPort& dataPort, NetlistPort& guardPort);
    void mapFUOpcodePort(const NetlistBlock& block, NetlistPort& opcodePort);
    void mapImmediateUnitWritePort(
        const TTAMachine::ImmediateUnit& iu,
        NetlistPort& port);
    bool isParameterizable(
        const std::string& paramName,
        const HDB::FUEntry* fuEntry) const;
    unsigned int calculateAddressWidth(TTAMachine::FunctionUnit const* fu) const;

    static int opcodePortWidth(const HDB::FUEntry& fu)
        throw (InvalidData);
    static TTAMachine::FUPort& findCorrespondingPort(
        const TTAMachine::FunctionUnit& fuToSearch,
        const TTAMachine::FunctionUnit& origFU,
        const std::string& portName)
        throw (InstanceNotFound);
    static int instructionMemoryAddressWidth(
        const TTAMachine::Machine& machine)
        throw (IllegalMachine);
    static int instructionMemoryWidth(
        const TTAMachine::Machine& machine)
        throw (IllegalMachine);
    static TTAMachine::AddressSpace& instructionMemory(
        const TTAMachine::Machine& machine)
        throw (IllegalMachine);

    /// The machine.
    const TTAMachine::Machine& machine_;
    /// The machine implementation.
    const IDF::MachineImplementation& machImplementation_;
    /// The generator plugin.
    ICDecoderGeneratorPlugin& plugin_;
    /// Maps the ADF ports to the netlist ports.
    PortCorrespondenceMap portCorrespondenceMap_;
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
    /// The instruction decoder block.
    NetlistBlock* instructionDecoder_;
    /// Maps FU implementations for different FU's.
    FUEntryMap fuEntryMap_;
    /// Return address in port in GCU (ifetch).
    NetlistPort* raInPort_;
    /// Returns address out port in GCU (ifetch).
    NetlistPort* raOutPort_;

};
}

#endif
