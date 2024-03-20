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
 * @file NetlistGenerator.cc
 *
 * Implementation of NetlistGenerator class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @author Otto Esko 2008 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#include <boost/format.hpp>
#include <iostream>
#include <set>
#include <string>
#include <utility> // for make_pair()

#include "ICDecoderGeneratorPlugin.hh"
#include "Netlist.hh"
#include "NetlistBlock.hh"
#include "NetlistGenerator.hh"
#include "NetlistPort.hh"
#include "NetlistPortGroup.hh"
#include "Parameter.hh"
#include "ProcessorGenerator.hh"
#include "SignalGroupTypes.hh"

#include "FUImplementationLocation.hh"
#include "MachineImplementation.hh"

#include "ControlUnit.hh"
#include "FUPort.hh"
#include "FunctionUnit.hh"
#include "HWOperation.hh"
#include "ImmediateUnit.hh"
#include "Machine.hh"
#include "SpecialRegisterPort.hh"

#include "FUArchitecture.hh"
#include "FUEntry.hh"
#include "FUExternalPort.hh"
#include "FUImplementation.hh"
#include "FUPortImplementation.hh"
#include "HDBManager.hh"
#include "HDBRegistry.hh"
#include "RFArchitecture.hh"
#include "RFEntry.hh"
#include "RFExternalPort.hh"
#include "RFImplementation.hh"
#include "RFPortImplementation.hh"

#include "BinaryEncoding.hh"

#include "Conversion.hh"
#include "MapTools.hh"
#include "MathTools.hh"
#include "ProGeContext.hh"

#include "MemoryBusInterface.hh"

#include "GeneratableFUNetlistBlock.hh"
#include "GeneratableRFNetlistBlock.hh"
#include "ProGeTools.hh"

using namespace IDF;
using namespace HDB;
using namespace TTAMachine;

using std::set;
using std::string;
using boost::format;

const string TOPLEVEL_BLOCK_DEFAULT_NAME = "toplevel";
const string CLOCK_PORT_NAME = "clk";
const string RESET_PORT_NAME = "rstx";
const string RA_OUT_PORT_NAME = "ra_out";
const string BUSY_PORT_NAME = "busy";
const string READ_ENABLE_PORT_NAME = "imem_en_x";
const string ADDRESS_PORT_NAME = "imem_addr";
const string DATA_PORT_NAME = "imem_data";
const string PC_IN_PORT_NAME = "pc_in";
const string RA_IN_PORT_NAME = "ra_in";
const string FETCH_PORT_NAME = "fetch_en";
const string LOCK_PORT_NAME = "lock";
const string FETCHBLOCK_PORT_NAME = "fetchblock";
const string PC_READ_PORT_NAME = "pc_r";
const string PC_LOAD_PORT_NAME = "pc_load";
const string RA_LOAD_PORT_NAME = "ra_load";
const string PC_OPCODE_PORT_NAME = "pc_opcode";
const string INSTRUCTIONWORD_PORT_NAME = "instructionword";
const string GLOBAL_LOCK_PORT_NAME = "glock";
const string LOCK_REQUEST_PORT_NAME = "lock_r";
const string LOCK_STATUS_PORT_NAME = "locked";

const string IMEMADDRWIDTH = "IMEMADDRWIDTH";
const string IMEMWIDTHFORMULA = "IMEMWIDTHINMAUS*IMEMMAUWIDTH";
const string INSTRUCTIONWIDTH = "INSTRUCTIONWIDTH";

const string IFETCH_BLOCK_NAME = "ifetch";
const string DECOMPRESSOR_BLOCK_NAME = "decompressor";
const string DECODER_BLOCK_NAME = "decoder";

const string CALL = "CALL";
const string JUMP = "JUMP";

const string FU_NAME_PREFIX = "fu_";
const string RF_NAME_PREFIX = "rf_";
const string IU_NAME_PREFIX = "iu_";

const TCEString INSTANCE_SUFFIX = "_instance";

namespace ProGe {

    const std::string NetlistGenerator::DECODER_INSTR_WORD_PORT =
        INSTRUCTIONWORD_PORT_NAME;
    const std::string NetlistGenerator::DECODER_RESET_PORT = RESET_PORT_NAME;
    const std::string NetlistGenerator::DECODER_CLOCK_PORT = CLOCK_PORT_NAME;
    const std::string NetlistGenerator::DECODER_RA_LOAD_PORT =
        RA_LOAD_PORT_NAME;
    const std::string NetlistGenerator::DECODER_PC_LOAD_PORT =
        PC_LOAD_PORT_NAME;
    const std::string NetlistGenerator::DECODER_PC_OPCODE_PORT =
        PC_OPCODE_PORT_NAME;
    const std::string NetlistGenerator::DECODER_LOCK_REQ_OUT_PORT =
        LOCK_REQUEST_PORT_NAME;
    const std::string NetlistGenerator::DECODER_LOCK_REQ_IN_PORT =
        LOCK_PORT_NAME;
    const std::string NetlistGenerator::DECODER_LOCK_STATUS_PORT =
        LOCK_STATUS_PORT_NAME;
    const std::string NetlistGenerator::DECOMP_LOCK_REQ_IN_PORT =
        LOCK_REQUEST_PORT_NAME;
    const std::string NetlistGenerator::DECOMP_GLOCK_PORT =
        GLOBAL_LOCK_PORT_NAME;
    const std::string NetlistGenerator::DECOMP_INSTR_WORD_PORT =
        INSTRUCTIONWORD_PORT_NAME;
    const std::string NetlistGenerator::FETCHBLOCK_PORT_NAME = "fetchblock";

    NetlistGenerator::NetlistGenerator(
        const ProGeContext& context, ICDecoderGeneratorPlugin& plugin)
        : context_(context), plugin_(plugin), coreBlock_(NULL),
          instructionDecoder_(NULL), instructionDecompressor_(NULL),
          instructionFetch_(NULL), raInPort_(NULL), raOutPort_(NULL) {}

    /**
     * The destructor.
     */
    NetlistGenerator::~NetlistGenerator() {
        MapTools::deleteAllValues(fuEntryMap_);
        MapTools::deleteAllValues(rfEntryMap_);
    }

    /**
     * Generates the netlist block of the processor.
     *
     * @param imemWidthInMAUs Width of instruction memory in MAUs.
     * @param entityNameStr The name string used to make the netlist blocks
     *                      uniquely named.
     * @return The newly generated netlist block representing a single TTA Core.
     * @exception IOException If some of the HDBs given in IDF cannot be
     *                        accessed.
     * @exception InvalidData If HDB or IDF is invalid.
     * @exception OutOfRange If the given width of instruction memory is not
     *                       positive
     * @exception InstanceNotFound Something missing missing from HDB.
     */
    NetlistBlock*
    NetlistGenerator::generate(
        const ProGeOptions& options, int imemWidthInMAUs,
        TCEString entityNameStr = TOPLEVEL_BLOCK_DEFAULT_NAME,
        std::ostream& warningStream = std::cerr) {
        if (imemWidthInMAUs < 1) {
            string errorMsg =
                "Instruction memory width in MAUs must be positive.";
            throw OutOfRange(__FILE__, __LINE__, __func__, errorMsg);
        }

        // add toplevel block
        coreBlock_ = new NetlistBlock(entityNameStr, "tta_core");
        coreBlock_->addPackage(context_.coreEntityName() + "_imem_mau");
        coreBlock_->addPackage(context_.coreEntityName() + "_globals");

        // add GCU to the netlist
        addGCUToNetlist(*coreBlock_, imemWidthInMAUs);

        // add Genrated FUs to the toplevel netlist.
        for (auto fug : context_.idf().FUGenerations()) {
            addGeneratableFUsToNetlist(fug, *coreBlock_);
        }

        // add function units to the netlist
        for (int i = 0; i < context_.idf().fuImplementationCount(); i++) {
            IDF::FUImplementationLocation& location =
                context_.idf().fuImplementation(i);
            addFUToNetlist(location, *coreBlock_, warningStream);
        }

        // add Generated RFs to the toplevel netlist.
        for (auto rfg : context_.idf().RFGenerations()) {
            addGeneratableRFsToNetlist(rfg, *coreBlock_);
        }

        // add register files to the netlist
        for (int i = 0; i < context_.idf().rfImplementationCount(); i++) {
            RFImplementationLocation& location =
                context_.idf().rfImplementation(i);
            addRFToNetlist(options, location, *coreBlock_);
        }

        // add immediate units to the netlist
        for (int i = 0; i < context_.idf().iuImplementationCount(); i++) {
            RFImplementationLocation& location =
                context_.idf().iuImplementation(i);
            addIUToNetlist(options, location, *coreBlock_);
        }

        plugin_.completeNetlist(*coreBlock_, *this);

        return coreBlock_;
    }

    /**
     * Returns the netlist port which is corresponding to the given port in
     * the
     * machine object model.
     *
     * @param port The port in the machine object model
     * @return The corresponding port in the netlist.
     * @exception InstanceNotFound If the netlist does not contain the port.
     */
    NetlistPort&
    NetlistGenerator::netlistPort(
        const TTAMachine::Port& port, Direction dir) const {
        if (portCorrespondenceMap_.count(&port) > 0) {
            // Override direction with port direction if the port is
            // unidirectional
            if (!(port.isInput() && port.isOutput())) {
                dir = port.isInput() ? IN : OUT;
            }

            auto range = portCorrespondenceMap_.equal_range(&port);
            for (auto i = range.first; i != range.second; ++i) {
                if(i->second->direction() == dir)
                    return *i->second;
            }
            return *MapTools::valueForKey<NetlistPort*>(
                portCorrespondenceMap_, &port);
        }

        throw InstanceNotFound(__FILE__, __LINE__, __func__,
            (boost::format(
                "Cannot find netlist port '%s' in parent unit: '%s'") %
                port.name() % port.parentUnit()->name())
                .str());
    }

    /**
     * Returns the netlist block which is corresponding to the given unit in
     * the
     * machine object model.
     *
     * @param unit The Unit in the machine object model
     * @return The corresponding block in the netlist.
     * @exception InstanceNotFound If the netlist does not contain the
     * block.
     */
    NetlistBlock&
    NetlistGenerator::netlistBlock(const TTAMachine::Unit& unit) const {
        try {
            return *MapTools::valueForKey<NetlistBlock*>(
                unitCorrespondenceMap_, &unit);
        } catch (const Exception&) {
            throw InstanceNotFound(__FILE__, __LINE__, __func__,
                (boost::format(
                     "Cannot find corresponding netlist block for '%s'") %
                    unit.name())
                    .str());
        }
    }

    /**
     * Returns the load enable port of the given port in the netlist.
     *
     * @param port The architectural port in the netlist.
     * @return The load enable port of the given port.
     * @exception InstanceNotFound If the netlist does not contain the port.
     */
    NetlistPort&
    NetlistGenerator::loadPort(const NetlistPort& port) const {
        try {
            return *MapTools::valueForKey<NetlistPort*>(loadPortMap_, &port);
        } catch (const Exception&) {
            throw InstanceNotFound(__FILE__, __LINE__, __func__,
                (boost::format(
                     "Cannot find load port '%s' in instance name: '%s', "
                     "module name: '%s'") %
                    port.name() % port.parentBlock().instanceName() %
                    port.parentBlock().moduleName())
                    .str());
        }
    }

    /**
     * Returns true if the given RF port has opcode port. Otherwise, returns
     * false.
     */
    bool NetlistGenerator::hasOpcodePort(const NetlistPort& port) const {
        return MapTools::containsKey(rfOpcodePortMap_, &port);
    }

    /**
     * Returns the opcode port of the given RF port in the netlist.
     *
     * @param port The architectural RF port in the netlist.
     * @return The opcode port of the given port.
     * @exception InstanceNotFound If the netlist does not contain the port.
     */
    NetlistPort&
    NetlistGenerator::rfOpcodePort(const NetlistPort& port) const {
        try {
            return *MapTools::valueForKey<NetlistPort*>(
                rfOpcodePortMap_, &port);
        } catch (const Exception&) {
            throw InstanceNotFound(__FILE__, __LINE__, __func__,
                (boost::format("Cannot find rf opcode port '%s' in "
                               "instance name: '%s', "
                               "module name: '%s'") %
                    port.name() % port.parentBlock().instanceName() %
                    port.parentBlock().moduleName())
                    .str());
        }
    }

    /**
     * Returns the guard port of the given RF block in the netlist.
     *
     * @param rfBlock The RF block in the netlist.
     * @return The guard port of the given block.
     * @exception InstanceNotFound If the netlist does not contain the port.
     */
    NetlistPort&
    NetlistGenerator::rfGuardPort(const NetlistBlock& rfBlock) const {
        try {
            return *MapTools::valueForKey<NetlistPort*>(
                rfGuardPorts_, &rfBlock);
        } catch (const Exception&) {
            throw InstanceNotFound(__FILE__, __LINE__, __func__,
                (boost::format("Cannot find guard port in register file "
                               "instance name: '%s',"
                               "module name: '%s'") %
                    rfBlock.instanceName() % rfBlock.moduleName())
                    .str());
        }
    }

    /**
     * Returns the opcode port of the given FU.
     *
     * @param fuBlock The FU.
     * @return The opcode port.
     * @exception InstanceNotFound If the given FU does not have an opcode
     * port.
     */
    NetlistPort&
    NetlistGenerator::fuOpcodePort(const NetlistBlock& fuBlock) const {
        try {
            return *MapTools::valueForKey<NetlistPort*>(
                fuOpcodePorts_, &fuBlock);
        } catch (const Exception&) {
            throw InstanceNotFound(__FILE__, __LINE__, __func__,
                (boost::format("Cannot find opcode port in function unit "
                               "instance name: '%s',"
                               "module name: '%s'") %
                    fuBlock.instanceName() % fuBlock.moduleName())
                    .str());
        }
    }

    /**
     * Returns the guard port of the given FU data port.
     *
     * @param fuPort The FU data port.
     * @return The guard port.
     * @exception InstanceNotFound If the netlist does not contain the port.
     */
    NetlistPort&
    NetlistGenerator::fuGuardPort(const NetlistPort& fuPort) const {
        try {
            return *MapTools::valueForKey<NetlistPort*>(
                fuGuardPortMap_, &fuPort);
        } catch (const Exception&) {
            throw InstanceNotFound(__FILE__, __LINE__, __func__,
                (boost::format(
                     "Cannot find guard port '%s' in instance name: '%s', "
                     "module name: '%s'") %
                    fuPort.name() % fuPort.parentBlock().instanceName() %
                    fuPort.parentBlock().moduleName())
                    .str());
        }
    }

    /**
     * Returns the clock port of the given block in the netlist.
     *
     * @param block The block in the netlist.
     * @return The clock port of the given block.
     * @exception InstanceNotFound If the netlist does not contain the port.
     */
    NetlistPort&
    NetlistGenerator::clkPort(const NetlistBlock& block) const {
        try {
            return *MapTools::valueForKey<NetlistPort*>(clkPorts_, &block);
        } catch (const Exception&) {
            throw InstanceNotFound(__FILE__, __LINE__, __func__,
                (boost::format("Cannot find clock port in netlist block "
                               "instance name: "
                               "'%s', module name: '%s'") %
                    block.instanceName() % block.moduleName())
                    .str());
        }
    }

    /**
     * Returns the reset port of the given block in the netlist.
     *
     * @param block The block in the netlist.
     * @return The reset port of the given block.
     * @exception InstanceNotFound If the netlist does not contain the port.
     */
    NetlistPort&
    NetlistGenerator::rstPort(const NetlistBlock& block) const {
        try {
            return *MapTools::valueForKey<NetlistPort*>(rstPorts_, &block);
        } catch (const Exception&) {
            throw InstanceNotFound(__FILE__, __LINE__, __func__,
                (boost::format("Cannot find reset port in netlist block "
                               "instance name: "
                               "'%s', module name: '%s'") %
                    block.instanceName() % block.moduleName())
                    .str());
        }
    }

    /**
     * Tells whether the given netlist block has a global lock port.
     *
     * @param block The netlist block.
     * @return True if the block has a global lock port, otherwise false.
     */
    bool NetlistGenerator::hasGlockPort(const NetlistBlock& block) const {
        return MapTools::containsKey(glockPorts_, &block);
    }

    /**
     * Returns the global lock port of the given block in the netlist.
     *
     * @param block The block in the netlist.
     * @return The global lock port of the given block.
     * @exception InstanceNotFound If the netlist does not contain the port.
     */
    NetlistPort&
    NetlistGenerator::glockPort(const NetlistBlock& block) const {
        try {
            return *MapTools::valueForKey<NetlistPort*>(glockPorts_, &block);
        } catch (const Exception&) {
            throw InstanceNotFound(__FILE__, __LINE__, __func__,
                (boost::format("Cannot find global lock port in netlist block "
                               "instance name:"
                               " '%s', module name: '%s'") %
                    block.instanceName() % block.moduleName())
                    .str());
        }
    }

    /**
     * Tells whether the given netlist block has global lock request port.
     *
     * @param block The netlist block.
     * @return True if the block has a global lock request port, otherwise
     * false.
     */
    bool NetlistGenerator::hasGlockReqPort(const NetlistBlock& block) const {
        return MapTools::containsKey(glockReqPorts_, &block);
    }

    /**
     * Returns the global lock request port of the given block in the
     * netlist.
     *
     * @param block The block in the netlist.
     * @return The global lock request port of the given block.
     * @exception InstanceNotFound If the netlist does not contain the port.
     */
    NetlistPort&
    NetlistGenerator::glockReqPort(const NetlistBlock& block) const {
        try {
            return *MapTools::valueForKey<NetlistPort*>(glockReqPorts_, &block);
        } catch (const Exception&) {
            throw InstanceNotFound(__FILE__, __LINE__, __func__,
                (boost::format("Cannot find global lock request port in "
                               "netlist block "
                               "instance name: '%s', module name: '%s'") %
                    block.instanceName() % block.moduleName())
                    .str());
        }
    }

    /**
     * Returns a netlist port that is the write port of the given immediate
     * unit.
     *
     * @param iu The immediate unit.
     * @return The netlist port.
     * @exception InstanceNotFound If the port is not found.
     */
    NetlistPort&
    NetlistGenerator::immediateUnitWritePort(
        const TTAMachine::ImmediateUnit& iu) const {
        try {
            return *MapTools::valueForKey<NetlistPort*>(iuPortMap_, &iu);
        } catch (const Exception&) {
            throw InstanceNotFound(__FILE__, __LINE__, __func__,
                (boost::format("Cannot find immediate unit '%s' write port.") %
                    iu.name())
                    .str());
        }
    }

    /**
     * Returns the return address in port of GCU.
     *
     * @return The return address in port.
     * @exception InstanceNotFound If GCU does not have the port.
     */
    NetlistPort&
    NetlistGenerator::gcuReturnAddressInPort() const {
        if (raInPort_ != NULL) {
            return *raInPort_;
        } else {
            throw InstanceNotFound(__FILE__, __LINE__, __func__,
                "Cannot find return address of in port of GCU");
        }
    }

    /**
     * Returns the return address out port of GCU.
     *
     * @return The return address out port.
     * @exception InstanceNotFound If GCU does not have the port.
     */
    NetlistPort&
    NetlistGenerator::gcuReturnAddressOutPort() const {
        if (raOutPort_ != NULL) {
            return *raOutPort_;
        } else {
            throw InstanceNotFound(__FILE__, __LINE__, __func__,
                "Cannot find return address of out port of GCU");
        }
    }

    /**
     * Returns the TTA core block in the netlist.
     *
     * @return The TTA core block.
     * @exception InstanceNotFound If there is no instruction decoder in the
     *                             netlist.
     */
    NetlistBlock&
    NetlistGenerator::ttaCore() const {
        if (coreBlock_ == NULL) {
            throw InstanceNotFound(__FILE__, __LINE__, __func__,
                "Cannot find TTA core block from the netlist.");
        }
        return *coreBlock_;
    }

    /**
     * Returns the instruction decoder block in the netlist.
     *
     * @return The instruction decoder block.
     * @exception InstanceNotFound If there is no instruction decoder in the
     *                             netlist.
     */
    NetlistBlock&
    NetlistGenerator::instructionDecoder() const {
        if (instructionDecoder_ == NULL) {
            throw InstanceNotFound(__FILE__, __LINE__, __func__,
                "Cannot find instruction decoder block from the netlist.");
        } else {
            return *instructionDecoder_;
        }
    }

    /**
     * Returns the instruction fetch block in the netlist.
     *
     * @return The instruction fetch block.
     * @exception InstanceNotFound If there is no instruction fetch in the
     *                             netlist.
     */
    NetlistBlock&
    NetlistGenerator::instructionFetch() const {
        if (instructionFetch_ == NULL) {
            throw InstanceNotFound(__FILE__, __LINE__, __func__,
                "Cannot find instruction fetch block from the netlist.");
        } else {
            return *instructionFetch_;
        }
    }

    /**
     * Returns the instruction decompressor block in the netlist.
     *
     * @return The instruction decompressor block.
     * @exception InstanceNotFound If there is no instruction decompressor
     * in
     * the
     *                             netlist.
     */
    NetlistBlock&
    NetlistGenerator::instructionDecompressor() const {
        if (instructionDecompressor_ == NULL) {
            throw InstanceNotFound(__FILE__, __LINE__, __func__,
                "Cannot find instruction decompressor block from the "
                "netlist.");
        } else {
            return *instructionDecompressor_;
        }
    }

    /**
     * Returns the FU entry which was selected to represent the given FU.
     *
     * @param fuName Name of the FU in ADF.
     * @return The FU entry.
     * @exception InstanceNotFound If the netlist is not created yet or if
     * there
     * was
     *                             no FU entry for the given FU.
     */
    FUEntry&
    NetlistGenerator::fuEntry(const std::string& fuName) const {
        try {
            return *MapTools::valueForKey<FUEntry*>(fuEntryMap_, fuName);
        } catch (const Exception&) {
            throw InstanceNotFound(__FILE__, __LINE__, __func__,
                "Cannot find function unit entry from the netlist.");
        }
    }

    /**
     * Returns the RF entry which was selected to represent the given RF.
     *
     * @param fuName Name of the RF in ADF.
     * @return The RF entry.
     * @exception InstanceNotFound If the netlist is not created yet or if
     * there
     *                             was no RF entry for the given RF.
     */
    HDB::RFEntry&
    NetlistGenerator::rfEntry(const std::string& rfName) const {
        try {
            return *MapTools::valueForKey<RFEntry*>(rfEntryMap_, rfName);
        } catch (const Exception&) {
            throw InstanceNotFound(__FILE__, __LINE__, __func__,
                "Cannot find register file entry from the netlist.");
        }
    }

    /**
     * Returns true if there is an entry for the RF name given as parameter.
     *
     * @param rfName Name of the RF in ADF.
     * @return True if RF entry exists, false otherwise.
     */
    bool
    NetlistGenerator::rfHasEntry(const std::string& rfName) const {
        return MapTools::containsKey(rfEntryMap_, rfName) ? true : false;
    }

    /**
     * Adds the global control unit to the netlist as sub-block of the given
     * top-level block.
     *
     * @param toplevelBlock The top-level block of the netlist.
     * @param imemWidthInMAUs Width of the instruction memory in MAUs.
     */
    void NetlistGenerator::addGCUToNetlist(
        NetlistBlock& toplevelBlock, int imemWidthInMAUs) {
        if (imemWidthInMAUs != 4 && context_.adf().isRISCVMachine()) {
            throw InvalidData(__FILE__, __LINE__, __func__,
                "Imem width fixed to 4 MAUs for RISC-V machines");
        } else if (imemWidthInMAUs != 1 && !context_.adf().isRISCVMachine()) {
            throw InvalidData(__FILE__, __LINE__, __func__,
                "Imem width is currently fixed to 1 MAU");
        }
        Netlist& netlist = toplevelBlock.netlist();

        Parameter coreIdParam("core_id", "integer", "0", "0");
        toplevelBlock.addParameter(coreIdParam);

        NetlistPort* tlClkPort =
            new InBitPort(CLOCK_PORT_NAME, toplevelBlock, SignalType::CLOCK);
        NetlistPort* tlRstPort = new InBitPort(RESET_PORT_NAME, toplevelBlock,
            Signal(SignalType::RESET, ActiveState::LOW));

        // Instruction fetch bus //
        NetlistPort* tlBusyPort = new InBitPort(BUSY_PORT_NAME,
            Signal(SignalType::READ_REQUEST_READY, ActiveState::LOW));
        NetlistPort* tlReadEnablePort = new OutBitPort(READ_ENABLE_PORT_NAME,
            Signal(SignalType::READ_REQUEST, ActiveState::LOW));
        NetlistPort* tlAddressPort = new OutPort(ADDRESS_PORT_NAME,
            IMEMADDRWIDTH, BIT_VECTOR, Signal(SignalType::ADDRESS));
        NetlistPort* tlDataPort = new InPort(DATA_PORT_NAME, IMEMWIDTHFORMULA,
            BIT_VECTOR, Signal(SignalType::FETCHBLOCK));
        NetlistPortGroup* toplevelInstructionLine =
            new NetlistPortGroup(SignalGroupType::INSTRUCTION_LINE, tlBusyPort,
                tlReadEnablePort, tlAddressPort, tlDataPort);
        toplevelBlock.addPortGroup(toplevelInstructionLine);

        NetlistPort* tlLockStatusPort = new NetlistPort(LOCK_STATUS_PORT_NAME,
            "1", BIT, OUT, toplevelBlock, SignalType::CORE_HALT_STATUS);

        mapClockPort(toplevelBlock, *tlClkPort);
        mapResetPort(toplevelBlock, *tlRstPort);

        // add ifetch block
        ControlUnit* gcu = context_.adf().controlUnit();
        instructionFetch_ = new NetlistBlock(
            context_.coreEntityName() + "_" + IFETCH_BLOCK_NAME, "inst_fetch");
        toplevelBlock.addSubBlock(instructionFetch_);
        NetlistPort* ifetchClkPort =
            new NetlistPort(CLOCK_PORT_NAME, "1", BIT, IN, *instructionFetch_);
        NetlistPort* ifetchRstPort =
            new NetlistPort(RESET_PORT_NAME, "1", BIT, IN, *instructionFetch_);
        NetlistPort* ifetchRAOutPort = new NetlistPort(RA_OUT_PORT_NAME,
            IMEMADDRWIDTH, BIT_VECTOR, OUT, *instructionFetch_);
        raOutPort_ = ifetchRAOutPort;
        NetlistPort* ifetchRAInPort = new NetlistPort(
            RA_IN_PORT_NAME, IMEMADDRWIDTH, BIT_VECTOR, IN, *instructionFetch_);
        raInPort_ = ifetchRAInPort;

        NetlistPort* ifetchBusyPort = new InBitPort(BUSY_PORT_NAME,
            Signal(SignalType::READ_REQUEST_READY, ActiveState::LOW));
        NetlistPort* ifetchReadEnablePort =
            new OutBitPort(READ_ENABLE_PORT_NAME,
                Signal(SignalType::READ_REQUEST, ActiveState::LOW));
        NetlistPort* ifetchAddressPort = new OutPort(ADDRESS_PORT_NAME,
            IMEMADDRWIDTH, BIT_VECTOR, Signal(SignalType::ADDRESS));
        NetlistPort* ifetchDataPort = new InPort(DATA_PORT_NAME,
            IMEMWIDTHFORMULA, BIT_VECTOR, Signal(SignalType::FETCHBLOCK));
        NetlistPortGroup* ifetchInstructionLine = new NetlistPortGroup(
            SignalGroupType::INSTRUCTION_LINE, ifetchBusyPort,
            ifetchReadEnablePort, ifetchAddressPort, ifetchDataPort);
        instructionFetch_->addPortGroup(ifetchInstructionLine);

        NetlistPort* ifetchPCInPort = new NetlistPort(
            PC_IN_PORT_NAME, IMEMADDRWIDTH, BIT_VECTOR, IN, *instructionFetch_);
        NetlistPort* ifetchPCLoadPort = new NetlistPort(
            PC_LOAD_PORT_NAME, "1", BIT, IN, *instructionFetch_);
        NetlistPort* ifetchRALoadPort = new NetlistPort(
            RA_LOAD_PORT_NAME, "1", BIT, IN, *instructionFetch_);
        NetlistPort* ifetchPCOpcodePort = new NetlistPort(PC_OPCODE_PORT_NAME,
            Conversion::toString(opcodePortWidth(*gcu)), opcodePortWidth(*gcu),
            BIT_VECTOR, IN, *instructionFetch_);
        NetlistPort* ifetchFetchPort =
            new NetlistPort(FETCH_PORT_NAME, "1", BIT, IN, *instructionFetch_);
        NetlistPort* ifetchGlockPort = new NetlistPort(
            GLOBAL_LOCK_PORT_NAME, "1", BIT, OUT, *instructionFetch_);
        NetlistPort* ifetchFetchBlockPort =
            new NetlistPort(FETCHBLOCK_PORT_NAME, IMEMWIDTHFORMULA, BIT_VECTOR,
                OUT, *instructionFetch_);

        // connect ifetch to toplevel
        netlist.connect(*tlClkPort, *ifetchClkPort);
        netlist.connect(*tlRstPort, *ifetchRstPort);

        netlist.connect(*toplevelInstructionLine, *ifetchInstructionLine);

        if (gcu->operationPortCount() > 0) {
            mapNetlistPort(*gcu->triggerPort(), *ifetchPCInPort);
        }

        // Add non-standard GCU ports to the block.
        for (int i = 0; i < gcu->portCount(); i++) {
            Port* port = gcu->port(i);
            if (gcu->returnAddressPort()->name() == port->name()) {
                continue;
            }
            if (!netlistPortIsMapped(*port)) {
                Direction dir;
                if (port->isInput()) {
                    dir = IN;
                } else if (port->isOutput()) {
                    dir = OUT;
                } else {
                    assert(false && "Unconneced ADF port.");
                }
                // Data port
                NetlistPort* dataPort = new NetlistPort(port->name() + "_data",
                    Conversion::toString(port->width()), BIT_VECTOR, dir,
                    *instructionFetch_);
                mapNetlistPort(*port, *dataPort);

                // Load port if port is input
                if (port->isInput()) {
                    NetlistPort* loadPort =
                        new NetlistPort(port->name() + "_load", "1", BIT, dir,
                            *instructionFetch_);
                    mapLoadPort(*dataPort, *loadPort);
                }
            }
        }

        // add decompressor block
        NetlistBlock* decompressorBlock = new NetlistBlock(
            context_.coreEntityName() + "_" + DECOMPRESSOR_BLOCK_NAME,
            "decomp");
        instructionDecompressor_ = decompressorBlock;
        toplevelBlock.addSubBlock(decompressorBlock);
        NetlistPort* decFetchPort =
            new NetlistPort(FETCH_PORT_NAME, "1", BIT, OUT, *decompressorBlock);
        NetlistPort* decLockPort =
            new NetlistPort(LOCK_PORT_NAME, "1", BIT, IN, *decompressorBlock);
        NetlistPort* decFetchBlockPort = new NetlistPort(FETCHBLOCK_PORT_NAME,
            IMEMWIDTHFORMULA, BIT_VECTOR, IN, *decompressorBlock);
        NetlistPort* decClkPort =
            new NetlistPort(CLOCK_PORT_NAME, "1", BIT, IN, *decompressorBlock);
        NetlistPort* decRstPort =
            new NetlistPort(RESET_PORT_NAME, "1", BIT, IN, *decompressorBlock);
        NetlistPort* decIWordPort = new NetlistPort(INSTRUCTIONWORD_PORT_NAME,
            INSTRUCTIONWIDTH, BIT_VECTOR, OUT, *decompressorBlock);
        NetlistPort* decGlockPort = new NetlistPort(
            GLOBAL_LOCK_PORT_NAME, "1", BIT, OUT, *decompressorBlock);
        NetlistPort* decLockReqPort = new NetlistPort(
            LOCK_REQUEST_PORT_NAME, "1", BIT, IN, *decompressorBlock);

        // connect ifetch to decompressor
        netlist.connect(*ifetchFetchPort, *decFetchPort);
        netlist.connect(*ifetchGlockPort, *decLockPort);
        netlist.connect(*ifetchFetchBlockPort, *decFetchBlockPort);

        // connect toplevel to decompressor
        netlist.connect(*tlClkPort, *decClkPort);
        netlist.connect(*tlRstPort, *decRstPort);

        // add decoder block
        NetlistBlock* decoderBlock = new NetlistBlock(
            context_.coreEntityName() + "_" + DECODER_BLOCK_NAME,
            "inst_decoder");
        toplevelBlock.addSubBlock(decoderBlock);
        instructionDecoder_ = decoderBlock;
        NetlistPort* decodIWordPort = new NetlistPort(DECODER_INSTR_WORD_PORT,
            INSTRUCTIONWIDTH, BIT_VECTOR, IN, *decoderBlock);
        NetlistPort* decodPCLoadPort =
            new NetlistPort(DECODER_PC_LOAD_PORT, "1", BIT, OUT, *decoderBlock);
        NetlistPort* decodRALoadPort =
            new NetlistPort(DECODER_RA_LOAD_PORT, "1", BIT, OUT, *decoderBlock);
        NetlistPort* decodPCOpcodePort = new NetlistPort(DECODER_PC_OPCODE_PORT,
            Conversion::toString(opcodePortWidth(*gcu)), opcodePortWidth(*gcu),
            BIT_VECTOR, OUT, *decoderBlock);
        NetlistPort* decodLockPort = new NetlistPort(
            DECODER_LOCK_REQ_IN_PORT, "1", BIT, IN, *decoderBlock);
        NetlistPort* decodLockReqPort = new NetlistPort(
            DECODER_LOCK_REQ_OUT_PORT, "1", BIT, OUT, *decoderBlock);
        NetlistPort* decodClkPort =
            new NetlistPort(DECODER_CLOCK_PORT, "1", BIT, IN, *decoderBlock);
        NetlistPort* decodRstPort =
            new NetlistPort(DECODER_RESET_PORT, "1", BIT, IN, *decoderBlock);
        NetlistPort* decodLockedPort = new NetlistPort(
            DECODER_LOCK_STATUS_PORT, "1", BIT, OUT, *decoderBlock);

        // connect decoder to decompressor
        netlist.connect(*decIWordPort, *decodIWordPort);
        netlist.connect(*decGlockPort, *decodLockPort);
        netlist.connect(*decLockReqPort, *decodLockReqPort);

        // connect decoder to top-level
        netlist.connect(*decodClkPort, *tlClkPort);
        netlist.connect(*decodRstPort, *tlRstPort);

        // connect decoder to ifetch
        netlist.connect(*decodPCLoadPort, *ifetchPCLoadPort);
        netlist.connect(*decodRALoadPort, *ifetchRALoadPort);
        netlist.connect(*decodPCOpcodePort, *ifetchPCOpcodePort);

        // connect lock status signal port
        netlist.connect(*tlLockStatusPort, *decodLockedPort);

        Netlist::connectClocks(toplevelBlock);
        Netlist::connectResets(toplevelBlock);
    }

    GeneratableFUNetlistBlock* NetlistGenerator::addGeneratableFUsToNetlist(
        const IDF::FUGenerated& fug, NetlistBlock& coreBlock) {

        std::string fuName = "fu_" + fug.name();
        std::transform(fuName.begin(), fuName.end(), fuName.begin(), ::tolower);

        GeneratableFUNetlistBlock* block = new GeneratableFUNetlistBlock(
            fuName, fuName + "_generated", *this);
        coreBlock.addSubBlock(block);

        FunctionUnit* fu =
            context_.adf().functionUnitNavigator().item(fug.name());

        mapNetlistBlock(*fu, *block);

        // add clock port
        NetlistPort* clkPort = new NetlistPort("clk", "1", 1, BIT, IN, *block);
        mapClockPort(*block, *clkPort);
        // connect clock port
        NetlistPort& tlClkPort = this->clkPort(coreBlock);
        coreBlock.netlist().connect(*clkPort, tlClkPort);

        // add reset port
        NetlistPort* rstPort = new NetlistPort("rstx", "1", 1, BIT, IN, *block);
        mapResetPort(*block, *rstPort);
        // connect reset port
        NetlistPort& tlRstPort = this->rstPort(coreBlock);
        coreBlock.netlist().connect(*rstPort, tlRstPort);

        // add global lock port
        NetlistPort* glockPort =
            new NetlistPort("glock_in", "1", 1, BIT, IN, *block);
        mapGlobalLockPort(*block, *glockPort);

        // opcode port.
        int opcodeWidth = static_cast<int>(std::ceil(
            std::log2(fu->operationCount())));
        if (fu->operationCount() > 1) {
            NetlistPort* opcodePort = new NetlistPort("operation_in",
                std::to_string(opcodeWidth), BIT_VECTOR, IN, *block);
            mapFUOpcodePort(*block, *opcodePort);
        }

        // global lock request port
        NetlistPort* glockReqPort = new NetlistPort(
            "glockreq_out", "1", 1, BIT, OUT, *block);
        mapGlobalLockRequestPort(*block, *glockReqPort);

        // operand ports.
        for (int i = 0; i < fu->portCount(); ++i) {
            FUPort* adfPort = dynamic_cast<FUPort*>(fu->port(i));

            if (adfPort->isInput()) {
                block->addInOperand(adfPort, i);
            } else {
                block->addOutOperand(adfPort);
            }
        }

        return block;
    }

     GeneratableRFNetlistBlock* NetlistGenerator::addGeneratableRFsToNetlist(
        const IDF::RFGenerated& rfg, NetlistBlock& coreBlock) {

        std::string rfName = "rf_" + rfg.name();
        std::transform(rfName.begin(), rfName.end(), rfName.begin(), ::tolower);

        GeneratableRFNetlistBlock* block = new GeneratableRFNetlistBlock(
            rfName, rfName + "_generated", *this);
        coreBlock.addSubBlock(block);

        RegisterFile* rf =
            context_.adf().registerFileNavigator().item(rfg.name());

        mapNetlistBlock(*rf, *block);

        // add clock port
        NetlistPort* clkPort = new NetlistPort("clk", "1", 1, BIT, IN, *block);
        mapClockPort(*block, *clkPort);
        // connect clock port
        NetlistPort& tlClkPort = this->clkPort(coreBlock);
        coreBlock.netlist().connect(*clkPort, tlClkPort);

        // add reset port
        NetlistPort* rstPort = new NetlistPort("rstx", "1", 1, BIT, IN, *block);
        mapResetPort(*block, *rstPort);
        // connect reset port
        NetlistPort& tlRstPort = this->rstPort(coreBlock);
        coreBlock.netlist().connect(*rstPort, tlRstPort);

        // add global lock port
        NetlistPort* glockPort =
            new NetlistPort("glock_in", "1", 1, BIT, IN, *block);
        mapGlobalLockPort(*block, *glockPort);

        // operand ports.
        for (int i = 0; i < rf->portCount(); ++i) {
            RFPort* adfPort = dynamic_cast<RFPort*>(rf->port(i));

            if (adfPort->isInput()) {
                block->addInOperand(adfPort, i, rf->numberOfRegisters());
            } else {
                block->addOutOperand(adfPort, rf->numberOfRegisters());
            }
        }

        // add guard port
        if (rf->isUsedAsGuard()) {
            string guardPortName = "guard_out";
            string size;
            size = Conversion::toString(rf->numberOfRegisters());
            NetlistPort* guardPort = new NetlistPort(guardPortName, size,
                BIT_VECTOR, OUT, *block);
            mapRFGuardPort(*block, *guardPort);
        }

        return block;
    }


    /**
     * Adds the FU identified by the given FUImplementationLocation
     * instance to the netlist.
     *
     * @param location The FUImplementationLocation instance.
     * @param netlist The netlist block under where FU is added.
     * @exception IOException If the HDB that contains the implementation
     * cannot
     *                        be accessed.
     * @exception InvalidData If the MachineImplementation instance is
     * erroneous
     *                        or if HDB erroneous.
     */
    void
    NetlistGenerator::addFUToNetlist(
        const FUImplementationLocation& location, NetlistBlock& coreBlock,
        std::ostream& warningStream) {
        string hdbFile = location.hdbFile();
        int id = location.id();
        FUEntry* entry = NULL;

        try {
            HDBManager& manager = HDBRegistry::instance().hdb(hdbFile);
            entry = manager.fuByEntryID(id);
        } catch (const KeyNotFound& e) {
            throw InvalidData(__FILE__, __LINE__, __func__, e.errorMessage());
        }

        if (!entry->hasImplementation() || !entry->hasArchitecture()) {
            string errorMsg;
            if (!entry->hasImplementation()) {
                errorMsg = "FU entry " + Conversion::toString(id) +
                           " does not "
                           "have an implementation in HDB " +
                           hdbFile + ".";
            } else {
                errorMsg = "FU entry " + Conversion::toString(id) +
                           " does not "
                           "have architecture definition in HDB " +
                           hdbFile + ".";
            }
            throw InvalidData(__FILE__, __LINE__, __func__, errorMsg);
        }

        if (!context_.adf().functionUnitNavigator().hasItem(
                location.unitName())) {
            string errorMsg = "ADF does not have FU '" + location.unitName() +
                              "' which is referred to in the IDF.";
            throw InvalidData(__FILE__, __LINE__, __func__, errorMsg);
        }

        fuEntryMap_.insert(std::make_pair(location.unitName(), entry));
        FUImplementation& fuImplementation = entry->implementation();
        FUArchitecture& architecture = entry->architecture();
        const FunctionUnit* adfFU =
            context_.adf().functionUnitNavigator().item(location.unitName());
        TCEString instanceName = FU_NAME_PREFIX + location.unitName();
        TCEString moduleName = fuImplementation.moduleName();
        instanceName = checkInstanceName(instanceName, moduleName);
        NetlistBlock* block = new NetlistBlock(moduleName, instanceName);
        coreBlock.addSubBlock(block);
        mapNetlistBlock(*adfFU, *block);

        format architecturesDontMatch("Architectures of FU entry %1% in "
                                      "%2% and FU '%3%' don't match.");

        // add parameters
        for (int i = 0; i < fuImplementation.parameterCount(); i++) {
            FUImplementation::Parameter parameter =
                fuImplementation.parameter(i);
            string paramName = parameter.name;
            string paramType = parameter.type;
            string paramValue = parameter.value;
            if (paramValue == "" || isParameterizable(paramName, entry)) {
                bool parameterResolved = false;
                // find the port which uses this parameter
                for (int j = 0; j < fuImplementation.architecturePortCount();
                     j++) {
                    FUPortImplementation& port =
                        fuImplementation.architecturePort(j);

                    // Special case if the FU is an LSU and it has
                    // parametrizable
                    // address width. This sets the ADF port width according
                    // to
                    // address space width in ADF
                    if (adfFU->hasAddressSpace() && isLSU(*adfFU)) {
                        FUPort& adfPort = findCorrespondingPort(*adfFU,
                            architecture.architecture(),
                            port.architecturePort());
                        // Assume address port is the triggering port
                        if (adfPort.isTriggering() &&
                            port.widthFormula() == paramName) {
                            int ASWidth = calculateAddressWidth(adfFU);
                            block->setParameter(paramName, paramType,
                                Conversion::toString(ASWidth));
                            // Fix the FU port width
                            adfPort.setWidth(ASWidth);
                            parameterResolved = true;
                            break;
                        }
                    }
                    if (port.widthFormula() == paramName) {
                        try {
                            FUPort& adfPort = findCorrespondingPort(*adfFU,
                                architecture.architecture(),
                                port.architecturePort());
                            block->setParameter(paramName, paramType,
                                Conversion::toString(adfPort.width()));
                            parameterResolved = true;
                            break;
                        } catch (const InstanceNotFound&) {
                            architecturesDontMatch % id % hdbFile %
                                adfFU->name();
                            throw InvalidData(__FILE__, __LINE__, __func__,
                                architecturesDontMatch.str());
                        }
                    }
                }

                if (!parameterResolved) {
                    format errorMsg("Unable to resolve the value of "
                                    "parameter %1% of FU "
                                    "entry %2%.");
                    errorMsg % paramName % id;
                    throw InvalidData(
                        __FILE__, __LINE__, __func__, errorMsg.str());
                }

            } else {
                block->setParameter(paramName, paramType, paramValue);
            }
        }

        // add ports to the netlist block
        for (int i = 0; i < fuImplementation.architecturePortCount(); i++) {

            FUPortImplementation& port = fuImplementation.architecturePort(i);

            // add architectural port
            string architecturePort = port.architecturePort();

            FUPort* adfPort;
            try {
                adfPort = &findCorrespondingPort(
                    *adfFU, architecture.architecture(), architecturePort);
            } catch (const InstanceNotFound&) {
                architecturesDontMatch % id % hdbFile % adfFU->name();
                throw InvalidData(
                    __FILE__, __LINE__, __func__, architecturesDontMatch.str());
            }
            NetlistPort* newPort = new NetlistPort(port.name(),
                port.widthFormula(), adfPort->width(), BIT_VECTOR,
                translateDirection(
                    architecture.portDirection(architecturePort)),
                *block);
            mapNetlistPort(*adfPort, *newPort);

            // add load port
            if (port.loadPort() != "") {
                NetlistPort* loadPort =
                    new NetlistPort(port.loadPort(), "1", 1, BIT, IN, *block);
                mapLoadPort(*newPort, *loadPort);
            }

            // add guard port
            if (port.guardPort() != "") {
                NetlistPort* guardPort =
                    new NetlistPort(port.guardPort(), "1", 1, BIT, OUT, *block);
                mapFUGuardPort(*newPort, *guardPort);
            }
        }

        // add opcode port
        if (fuImplementation.opcodePort() != "") {
            NetlistPort* opcodePort = new NetlistPort(
                fuImplementation.opcodePort(),
                Conversion::toString(opcodePortWidth(*entry, warningStream)),
                opcodePortWidth(*entry, warningStream), BIT_VECTOR, IN, *block);
            mapFUOpcodePort(*block, *opcodePort);
        }

        // add external ports
        addFUExternalPortsToNetlist(
            fuImplementation, coreBlock, *block, *adfFU);

        // add clock port
        if (fuImplementation.clkPort() != "") {
            NetlistPort* clkPort = new NetlistPort(
                fuImplementation.clkPort(), "1", 1, BIT, IN, *block);
            mapClockPort(*block, *clkPort);
            // connect clock port
            NetlistPort& tlClkPort = this->clkPort(coreBlock);
            coreBlock.netlist().connect(*clkPort, tlClkPort);
        }

        // add reset port
        if (fuImplementation.rstPort() != "") {
            NetlistPort* rstPort = new NetlistPort(
                fuImplementation.rstPort(), "1", 1, BIT, IN, *block);
            mapResetPort(*block, *rstPort);
            // connect reset port
            NetlistPort& tlRstPort = this->rstPort(coreBlock);
            coreBlock.netlist().connect(*rstPort, tlRstPort);
        }

        // add global lock port
        if (fuImplementation.glockPort() != "") {
            NetlistPort* glockPort = new NetlistPort(
                fuImplementation.glockPort(), "1", 1, BIT, IN, *block);
            mapGlobalLockPort(*block, *glockPort);
        }

        // add global lock request port
        if (fuImplementation.glockReqPort() != "") {
            NetlistPort* glockReqPort = new NetlistPort(
                fuImplementation.glockReqPort(), "1", 1, BIT, OUT, *block);
            mapGlobalLockRequestPort(*block, *glockReqPort);
        }
    }

    /**
     * Adds external ports defined in HDB into netlist.
     *
     * Also attempts to recognize LSU memory bus ports and assign them into
     * corresponding NetlistGroup. Recognition is now done by seeking
     * external
     * ports having specific patterns (see inferLSUSignal()).
     */
    void NetlistGenerator::addFUExternalPortsToNetlist(
        const HDB::FUImplementation& fuImplementation, NetlistBlock& coreBlock,
        NetlistBlock& fuBlock, const FunctionUnit& adfFU) {

        // external ports recognized as LSU' data memory ports.
        set<std::pair<NetlistPort*, NetlistPort*>> lsuPorts;
        // Other unrecognized ports.
        set<std::pair<NetlistPort*, NetlistPort*>> otherPorts;

        // for each external port, a port is added to top-level block too
        for (int i = 0; i < fuImplementation.externalPortCount(); i++) {
            FUExternalPort& externalPort = fuImplementation.externalPort(i);
            // if external port uses parameter, it must be added as netlist
            // parameter too and create new width formula for the top-level
            // port
            // by replacing parameter names with the corresponding names in
            // netlist.
            string tlPortWidth = externalPort.widthFormula();
            for (int i = 0; i < externalPort.parameterDependencyCount(); i++) {
                string paramName = externalPort.parameterDependency(i);
                Parameter param = fuBlock.parameter(paramName);
                string nlParamName = fuBlock.instanceName() + "_" + paramName;
                if (!coreBlock.netlist().hasParameter(nlParamName)) {
                    coreBlock.netlist().setParameter(
                        nlParamName, param.type(), param.value());
                }
                fuBlock.setParameter(param.name(), param.type(), nlParamName);
                size_t replaceStart = tlPortWidth.find(param.name(), 0);
                if (replaceStart == std::string::npos) {
                    throw InvalidData(__FILE__, __LINE__, __func__,
                        (boost::format("FU external port parameter "
                                       "dependencies do not "
                                       "seem "
                                       "to be right: Tried to find "
                                       "parameter named '%s'"
                                       " from external port width formula "
                                       "'%s' in unit "
                                       "'%s'") %
                            param.name() % tlPortWidth % nlParamName)
                            .str());
                }
                size_t replaceLength = param.name().length();
                tlPortWidth = tlPortWidth.replace(
                    replaceStart, replaceLength, nlParamName);
            }

            NetlistPort* extPort = new NetlistPort(externalPort.name(),
                externalPort.widthFormula(), BIT_VECTOR,
                translateDirection(externalPort.direction()), fuBlock);
            string tlPortName =
                fuBlock.instanceName() + "_" + externalPort.name();
            if (isLSUDataPort(adfFU, externalPort.name())) {
                NetlistPort* tlPort = new NetlistPort(tlPortName, tlPortWidth,
                    BIT_VECTOR, translateDirection(externalPort.direction()),
                    coreBlock, inferLSUSignal(externalPort.name()));
                lsuPorts.insert(std::make_pair(extPort, tlPort));
            } else {
                NetlistPort* tlPort = new NetlistPort(tlPortName, tlPortWidth,
                    BIT_VECTOR, translateDirection(externalPort.direction()),
                    coreBlock);
                coreBlock.netlist().connect(*extPort, *tlPort);
            }
        }

        // Handle LSU data ports.
        TCEString asName("");
        if (adfFU.hasAddressSpace()) {
            asName = adfFU.addressSpace()->name();
        }

        NetlistPortGroup* dmemPortGroup = nullptr;
        for (auto portPair : lsuPorts) {
            dmemPortGroup =
                dmemPortGroup
                    ? dmemPortGroup
                    : (new MemoryBusInterface(
                          SignalGroupType::BITMASKED_SRAM_PORT, asName));
            dmemPortGroup->addPort(*portPair.second);
            coreBlock.netlist().connect(*portPair.first, *portPair.second);
        }
        if (dmemPortGroup != nullptr) {
            coreBlock.addPortGroup(dmemPortGroup);
            dmemPortGroup = nullptr;
        }
    }

    /**
     * Adds the RF identified by the given RFImplementationLocation
     * instance to the netlist.
     *
     * @param location The RFImplementationLocation instance.
     * @param netlist The netlist.
     * @exception IOException If the HDB that contains the implementation
     * cannot
     *                        be accessed.
     * @exception InvalidData If the MachineImplementation instance or HDB
     * is
     *                        erroneous.
     */
    void
    NetlistGenerator::addRFToNetlist(
        const ProGeOptions& options, const RFImplementationLocation& location,
        NetlistBlock& netlistBlock) {
        if (!context_.adf().registerFileNavigator().hasItem(
                location.unitName())) {
            string errorMsg = "ADF does not contain register file '" +
                              location.unitName() + "' referred to in IDF.";
            throw InvalidData(__FILE__, __LINE__, __func__, errorMsg);
        }
        RegisterFile* rf =
            context_.adf().registerFileNavigator().item(location.unitName());
        addBaseRFToNetlist(options, *rf, location, netlistBlock,
            RF_NAME_PREFIX);
    }

    /**
     * Adds the IU identified by the given RFImplementationLocation
     * instance to the netlist.
     *
     * @param location The RFImplementationLocation instance.
     * @param netlist The netlist.
     * @exception IOException If the HDB that contains the implementation
     * cannot
     *                        be accessed.
     * @exception InvalidData If the MachineImplementation instance or HDB
     * is
     *                        erroneous.
     */
    void
    NetlistGenerator::addIUToNetlist(
        const ProGeOptions& options, const RFImplementationLocation& location,
        NetlistBlock& netlistBlock) {
        if (!context_.adf().immediateUnitNavigator().hasItem(
                location.unitName())) {
            string errorMsg = "ADF does not contain immediate unit '" +
                              location.unitName() + "' referred to in IDF.";
            throw InvalidData(__FILE__, __LINE__, __func__, errorMsg);
        }
        ImmediateUnit* iu =
            context_.adf().immediateUnitNavigator().item(location.unitName());
        addBaseRFToNetlist(options, *iu, location, netlistBlock,
            IU_NAME_PREFIX);
    }

    /**
     * Adds the RF or IU identified by the given RFImplementationLocation
     * instance to the netlist.
     *
     * @param regFile The corresponding RF (or IU) in ADF.
     * @param location The RFImplementationLocation instance.
     * @param netlist The netlist.
     * @param blockNamePrefix Prefix to be added to the block name.
     * @exception IOException If the HDB that contains the implementation
     * cannot
     *                        be accessed.
     * @exception InvalidData If the RF entry in HDB does not have an
     *                        implementation or architecture or if the HDB
     * does
     *                        not have the entry defined in the
     *                        RFImplementationLocation instance at all.
     */
    void
    NetlistGenerator::addBaseRFToNetlist(
        const ProGeOptions& options,
        const TTAMachine::BaseRegisterFile& regFile,
        const RFImplementationLocation& location, NetlistBlock& targetBlock,
        const std::string& blockNamePrefix) {
        RFEntry* entry = NULL;
        try {
            HDBManager& manager =
                HDBRegistry::instance().hdb(location.hdbFile());
            entry = manager.rfByEntryID(location.id());
        } catch (const KeyNotFound& e) {
            throw InvalidData(__FILE__, __LINE__, __func__, e.errorMessage());
        }

        if (!entry->hasImplementation() || !entry->hasArchitecture()) {
            format text("RF entry %1% does not have an implementation or "
                        "architecture "
                        "defined in HDB %2%.");
            text % location.id() % location.hdbFile();
            throw InvalidData(__FILE__, __LINE__, __func__, text.str());
        }

        rfEntryMap_.insert(std::make_pair(location.unitName(), entry));
        RFImplementation& implementation = entry->implementation();
        RFArchitecture& architecture = entry->architecture();
        TCEString instanceName = blockNamePrefix + location.unitName();
        TCEString moduleName = implementation.moduleName();
        instanceName = checkInstanceName(instanceName, moduleName);
        NetlistBlock* block = new NetlistBlock(moduleName, instanceName);
        targetBlock.addSubBlock(block);
        mapNetlistBlock(regFile, *block);

        // Add parameters (generics) to the block.
        for (int i = 0; i < implementation.parameterCount(); i++) {
            RFImplementation::Parameter param = implementation.parameter(i);

            // Check if parameter matches size or width parameter reference
            // and set/override its parameter value according to
            // architecture.
            if (param.name == implementation.sizeParameter()) {
                block->setParameter(implementation.sizeParameter(), "integer",
                    Conversion::toString(regFile.numberOfRegisters()));
            } else if (param.name == implementation.widthParameter()) {
                block->setParameter(implementation.widthParameter(), "integer",
                    Conversion::toString(regFile.width()));
            } else if (param.value == "") {
                format errorMsg(
                    "Unable to resolve the value of parameter %1% of RF "
                    "entry %2%.");
                errorMsg % param.name % location.id();
                throw InvalidData(__FILE__, __LINE__, __func__, errorMsg.str());
            } else {
                block->setParameter(param.name, param.type, param.value);
            }
        }

        // add ports
        for (int i = 0; i < implementation.portCount(); i++) {
            RFPortImplementation& port = implementation.port(i);
            NetlistPort* newInputPort = NULL;
            NetlistPort* newOutputPort = NULL;
            if (!implementation.widthParameter().empty()) {
                switch (port.direction()) {
                    case HDB::BIDIR:
                        newInputPort = new NetlistPort(port.name()+"_in",
                            implementation.widthParameter(), regFile.width(),
                            BIT_VECTOR, IN, *block);
                        newOutputPort = new NetlistPort(port.name()+"_out",
                            implementation.widthParameter(), regFile.width(),
                            BIT_VECTOR, OUT, *block);
                        break;
                    case HDB::IN:
                        newInputPort = new NetlistPort(port.name(),
                            implementation.widthParameter(), regFile.width(),
                            BIT_VECTOR, IN,
                            *block);
                        break;
                    case HDB::OUT:
                        newOutputPort = new NetlistPort(port.name(),
                            implementation.widthParameter(), regFile.width(),
                            BIT_VECTOR, OUT, *block);
                        break;
                }
            } else {
                switch (port.direction()) {
                    case HDB::BIDIR:
                        newInputPort = new NetlistPort(port.name(),
                            Conversion::toString(architecture.width()),
                            BIT_VECTOR, IN, *block);
                        newOutputPort = new NetlistPort(port.name(),
                            Conversion::toString(architecture.width()),
                            BIT_VECTOR, OUT, *block);
                        break;
                    case HDB::IN:
                        newInputPort = new NetlistPort(port.name(),
                            Conversion::toString(architecture.width()),
                            BIT_VECTOR, IN, *block);
                        break;
                    case HDB::OUT:
                        newOutputPort = new NetlistPort(port.name(),
                            Conversion::toString(architecture.width()),
                            BIT_VECTOR, OUT, *block);
                        break;
                }
            }
            assert(newInputPort || newOutputPort);

            RFPort* rfPort = NULL;
            // map the port if it is not input port of IU (not visible in
            // ADF)
            if (dynamic_cast<const ImmediateUnit*>(&regFile) == NULL ||
                port.direction() != HDB::IN) {

                bool mapped = false;
                for (int i = 0; i < regFile.portCount(); i++) {
                    rfPort = regFile.port(i);
                    if (portCorrespondenceMap_.count(rfPort) == 0) {
                        if ((port.direction() == HDB::IN &&
                                rfPort->inputSocket() != NULL &&
                                rfPort->outputSocket() == NULL) ||
                            (port.direction() == HDB::OUT &&
                                rfPort->outputSocket() != NULL &&
                                rfPort->inputSocket() == NULL) ||
                            (port.direction() == HDB::BIDIR &&
                                rfPort->outputSocket() != NULL &&
                                rfPort->inputSocket() != NULL)) {
                            mapped = true;
                            break;
                        }
                    }
                }

                if (!mapped) {
                    format text("Unable to map port '%1%' of RF entry '%2%' in "
                                "HDB '%3%' "
                                "to any port in %4% '%5%'.");
                    text % port.name() % location.id() % location.hdbFile();
                    if (dynamic_cast<const ImmediateUnit*>(&regFile) != NULL) {
                        text % "immediate unit";
                    } else {
                        text % "register file";
                    }
                    text % regFile.name();
                    throw InvalidData(__FILE__, __LINE__, __func__, text.str());
                }
            }

            // if the given BaseRegisterFile is ImmediateUnit, keep track of
            // the
            // data ports that does not appear in ADF
            const ImmediateUnit* iu =
                dynamic_cast<const ImmediateUnit*>(&regFile);
            if (iu != NULL && newInputPort) {
                mapImmediateUnitWritePort(*iu, *newInputPort);
            }

            // add load port
            NetlistPort* loadPort =
                new NetlistPort(port.loadPort(), "1", BIT, IN, *block);


            // Map data (unless it is IU's write port) and load port
            if (rfPort != NULL) {
                if (newInputPort)
                    mapNetlistPort(*rfPort, *newInputPort);
                if (newOutputPort)
                    mapNetlistPort(*rfPort, *newOutputPort);
            }
            if (newInputPort)
                mapLoadPort(*newInputPort, *loadPort);
            if (newOutputPort)
                mapLoadPort(*newOutputPort, *loadPort);

            // add opcode port
            NetlistPort* opcodePort = NULL;
            if (!port.opcodePort().empty()) {
                opcodePort = new NetlistPort(port.opcodePort(),
                    port.opcodePortWidthFormula(),
                    MathTools::requiredBits(regFile.numberOfRegisters() - 1),
                    BIT_VECTOR, IN, *block);
                if (newInputPort)
                    mapRFOpcodePort(*newInputPort, *opcodePort);
                if (newOutputPort)
                    mapRFOpcodePort(*newOutputPort, *opcodePort);
            } else if (regFile.numberOfRegisters() == 1) {
                // Special case for single register RFs which do not need
                // opcode
                // port. For legacy support the opcode port is left out if
                // opcode
                // port field is empty in HDB.
                // Do nothing.
            } else {
                format text("RF entry '%1%' in HDB '%2%' does not have "
                            "opcode port required for RFs of size > 1.");
                text % location.id() % location.hdbFile();
                throw InvalidData(__FILE__, __LINE__, __func__, text.str());
            }
        }

        // add guard port
        if (architecture.hasGuardSupport()) {
            string guardPortName = implementation.guardPort();
            string size;
            if (!implementation.sizeParameter().empty()) {
                size = implementation.sizeParameter();
            } else {
                size = Conversion::toString(architecture.size());
            }
            NetlistPort* guardPort = new NetlistPort(guardPortName, size,
                regFile.numberOfRegisters(), BIT_VECTOR, OUT, *block);
            mapRFGuardPort(*block, *guardPort);
        }

        // Add external ports
        string component_type("");
        if (dynamic_cast<const ImmediateUnit*>(&regFile) != NULL) {
            component_type = IU_NAME_PREFIX;
        } else {
            component_type = RF_NAME_PREFIX;
        }
        for (int i = 0; i < implementation.externalPortCount(); i++) {
            RFExternalPort& externalPort = implementation.externalPort(i);
            // if external port uses parameter, it must be added as netlist
            // parameter too and create new width formula for the top-level
            // port
            // by replacing parameter names with the corresponding names in
            // netlist.
            string tlPortWidth = externalPort.widthFormula();
            for (int i = 0; i < externalPort.parameterDependencyCount(); i++) {
                string hdbParamName = externalPort.parameterDependency(i);
                Parameter param = block->parameter(hdbParamName);
                string nlParamName =
                    component_type + location.unitName() + "_" + hdbParamName;
                if (!targetBlock.netlist().hasParameter(nlParamName)) {
                    targetBlock.netlist().setParameter(
                        nlParamName, param.type(), param.value());
                }
                block->setParameter(param.name(), param.type(), nlParamName);
                size_t replaceStart = tlPortWidth.find(param.name(), 0);
                if (replaceStart == std::string::npos) {
                    throw InvalidData(__FILE__, __LINE__, __func__,
                        (boost::format("RF external port parameter "
                                       "dependencies do not "
                                       "seem "
                                       "to be right: Tried to find "
                                       "parameter named '%s'"
                                       " from external port width formula "
                                       "'%s' in unit "
                                       "'%s'") %
                            param.name() % tlPortWidth % nlParamName)
                            .str());
                }
                size_t replaceLength = param.name().length();
                tlPortWidth = tlPortWidth.replace(
                    replaceStart, replaceLength, nlParamName);
            }

            NetlistPort* extPort = new NetlistPort(externalPort.name(),
                externalPort.widthFormula(), BIT_VECTOR,
                translateDirection(externalPort.direction()), *block);
            // connect the external port to top level
            string tlPortName = component_type + location.unitName() + "_" +
                                externalPort.name();
            NetlistPort* tlPort =
                new NetlistPort(tlPortName, tlPortWidth, BIT_VECTOR,
                    translateDirection(externalPort.direction()), targetBlock);
            targetBlock.netlist().connect(*tlPort, *extPort);
        }

        // add clock port
        NetlistPort* clockPort =
            new NetlistPort(implementation.clkPort(), "1", BIT, IN, *block);
        mapClockPort(*block, *clockPort);
        // connect clock port
        NetlistPort& tlClockPort = clkPort(targetBlock);
        targetBlock.netlist().connect(tlClockPort, *clockPort);

        // add reset port
        NetlistPort* resetPort =
            new NetlistPort(implementation.rstPort(), "1", BIT, IN, *block);
        mapResetPort(*block, *resetPort);
        // connect reset port
        NetlistPort& tlResetPort = rstPort(targetBlock);
        targetBlock.netlist().connect(tlResetPort, *resetPort);

        // add glock port
        if (implementation.glockPort() != "") {
            NetlistPort* glockPort = new NetlistPort(
                implementation.glockPort(), "1", BIT, IN, *block);
            mapGlobalLockPort(*block, *glockPort);
        }
    }

    /**
     * Maps the given ADF port to the given netlist port.
     *
     * @param adfPort The port in ADF (Machine Object Model).
     * @param netlistPort The corresponding port in the netlist.
     */
    void NetlistGenerator::mapNetlistPort(
        const TTAMachine::Port& adfPort, NetlistPort& netlistPort) {

        if (portCorrespondenceMap_.count(&adfPort) != 0) {
            auto range = portCorrespondenceMap_.equal_range(&adfPort);
            for (auto i = range.first; i != range.second; ++i) {
                assert(i->second->direction() != netlistPort.direction());
            }
        }
        portCorrespondenceMap_.emplace(&adfPort, &netlistPort);
    }

    /**
     * Maps the given ADF Function Unit to the given netlist block.
     *
     * @param adfFU The Function Unit in ADF (Machine Object Model).
     * @param netlistBlock The corresponding block in the netlist.
     */
    void NetlistGenerator::mapNetlistBlock(
        const TTAMachine::Unit& unit, NetlistBlock& netlistBlock) {
        assert(!MapTools::containsKey(unitCorrespondenceMap_, &unit));
        unitCorrespondenceMap_.insert(
            std::pair<const Unit*, NetlistBlock*>(&unit, &netlistBlock));
    }

    /**
     * Returns true if the given ADF port is mapped to some netlist port.
     *
     * @param adfPort The port in ADF (Machine Object Model).
     */
    bool
    NetlistGenerator::netlistPortIsMapped(const TTAMachine::Port& adfPort) {
        return portCorrespondenceMap_.count(&adfPort) != 0;
    }

    /**
     * Maps the given load port for the given architectural port.
     *
     * @param port The architectural port in the netlist.
     * @param loadPort The load port of the architectural port.
     */
    void NetlistGenerator::mapLoadPort(
        const NetlistPort& port, NetlistPort& loadPort) {

        assert(!MapTools::containsKey(loadPortMap_, &port));
        loadPortMap_.insert(
            std::pair<const NetlistPort*, NetlistPort*>(&port, &loadPort));
    }

    /**
     * Maps the given opcode port for the given architectural RF port.
     *
     * @param port The architectural port in the netlist.
     * @param opcodePort The opcode port of the architectural port.
     */
    void NetlistGenerator::mapRFOpcodePort(
        const NetlistPort& port, NetlistPort& opcodePort) {

        assert(!MapTools::containsKey(rfOpcodePortMap_, &port));
        rfOpcodePortMap_.insert(
            std::pair<const NetlistPort*, NetlistPort*>(&port, &opcodePort));
    }

    /**
     * Maps the given opcode port for the given FU block.
     *
     * @param block The FU block.
     * @param opcodePort The opcode port of the FU block.
     */
    void NetlistGenerator::mapFUOpcodePort(
        const NetlistBlock& block, NetlistPort& port) {

        assert(!MapTools::containsKey(fuOpcodePorts_, &block));
        fuOpcodePorts_.insert(
            std::pair<const NetlistBlock*, NetlistPort*>(&block, &port));
    }

    /**
     * Maps the given clock port for the given block.
     *
     * @param block The netlist block.
     * @param clkPort The clock port of the block.
     */
    void NetlistGenerator::mapClockPort(
        const NetlistBlock& block, NetlistPort& clkPort) {

        assert(!MapTools::containsKey(clkPorts_, &block));
        clkPorts_.insert(
            std::pair<const NetlistBlock*, NetlistPort*>(&block, &clkPort));
    }

    /**
     * Maps the given reset port for the given block.
     *
     * @param block The netlist block.
     * @param resetPort The reset port of the block.
     */
    void NetlistGenerator::mapResetPort(
        const NetlistBlock& block, NetlistPort& resetPort) {

        assert(!MapTools::containsKey(rstPorts_, &block));
        rstPorts_.insert(
            std::pair<const NetlistBlock*, NetlistPort*>(&block, &resetPort));
    }

    /**
     * Maps the given global lock port for the given block.
     *
     * @param block The netlist block.
     * @param glockPort The global lock port of the block.
     */
    void NetlistGenerator::mapGlobalLockPort(
        const NetlistBlock& block, NetlistPort& glockPort) {

        assert(!MapTools::containsKey(glockPorts_, &block));
        glockPorts_.insert(
            std::pair<const NetlistBlock*, NetlistPort*>(&block, &glockPort));
    }

    /**
     * Maps the given global lock request port for the given block.
     *
     * @param block The netlist block.
     * @param glockReqPort The global lock request port of the block.
     */
    void NetlistGenerator::mapGlobalLockRequestPort(
        const NetlistBlock& block, NetlistPort& glockReqPort) {

        assert(!MapTools::containsKey(glockReqPorts_, &block));
        glockReqPorts_.insert(std::pair<const NetlistBlock*, NetlistPort*>(
            &block, &glockReqPort));
    }

    /**
     * Maps the given guard port for the given RF block.
     *
     * @param block The netlist block.
     * @param guardPort The guard port of the block.
     */
    void NetlistGenerator::mapRFGuardPort(
        const NetlistBlock& block, NetlistPort& guardPort) {

        assert(!MapTools::containsKey(rfGuardPorts_, &block));
        rfGuardPorts_.insert(
            std::pair<const NetlistBlock*, NetlistPort*>(&block, &guardPort));
    }

    /**
     * Maps the given guard port for the given FU data port.
     *
     * @param dataPort The data port.
     * @param guardPort The guard port.
     */
    void NetlistGenerator::mapFUGuardPort(
        const NetlistPort& dataPort, NetlistPort& guardPort) {

        assert(!MapTools::containsKey(fuGuardPortMap_, &dataPort));
        fuGuardPortMap_.insert(
            std::pair<const NetlistPort*, NetlistPort*>(&dataPort, &guardPort));
    }

    /**
     * Maps the given netlist port as a write port of the given immediate
     * unit.
     *
     * @param iu The immediate unit.
     * @param port The netlist port.
     */
    void NetlistGenerator::mapImmediateUnitWritePort(
        const TTAMachine::ImmediateUnit& iu, NetlistPort& port) {

        iuPortMap_.insert(
            std::pair<const ImmediateUnit*, NetlistPort*>(&iu, &port));
    }

    /**
     * Checks if the given parameter is defined parametrizable in the given
     * FUImplementation
     *
     * @param paramName Name of the parameter
     * @param fuImplementation FUImplementation to be tested
     */
    bool NetlistGenerator::isParameterizable(
        const string& paramName, const FUEntry* fuEntry) const {
        FUImplementation& fuImplementation = fuEntry->implementation();

        for (int i = 0; i < fuImplementation.architecturePortCount(); i++) {

            FUPortImplementation& port = fuImplementation.architecturePort(i);
            string widthFormula = port.widthFormula();
            if (widthFormula == paramName) {
                return fuEntry->architecture().hasParameterizedWidth(
                    port.architecturePort());
            }
        }
        return false;
    }

    /**
     * Calculates the address width of an address space in FU
     *
     * @exception Invalid data If the FU doesn't have an address space or
     * the
     * address space is invalid
     *
     */
    unsigned int NetlistGenerator::calculateAddressWidth(
        TTAMachine::FunctionUnit const* fu) const {
        if (fu->hasAddressSpace() && fu->addressSpace() != NULL) {
            AddressSpace* AS = fu->addressSpace();
            unsigned int highestAddr = AS->end();
            unsigned int lowestAddr = AS->start();
            if (highestAddr == 0 || lowestAddr >= highestAddr) {
                string errorMessage = "Invalid address space";
                throw InvalidData(
                    __FILE__, __LINE__, __func__, errorMessage.c_str());
            }
            return static_cast<unsigned int>(ceil(log(highestAddr) / log(2)));
        } else {
            string errorMessage = "Tried to resolve address space width "
                                  "from FU '" +
                                  fu->name() +
                                  "' that doesn't have address space";
            throw InvalidData(
                __FILE__, __LINE__, __func__, errorMessage.c_str());
        }
        // never reached
        return 0;
    }

    /**
     * Calculates the required opcode port width for the given FU.
     *
     * @param fu The FU.
     * @param warningStream Output stream where warnings are written
     * @return The required width of opcode port.
     * @exception InvalidData If the HDB is missing some information.
     */
    int
    NetlistGenerator::opcodePortWidth(
        const FUEntry& fu, std::ostream& warningStream) {
        assert(fu.hasImplementation());
        assert(fu.hasArchitecture());
        FUImplementation& implementation = fu.implementation();
        FunctionUnit& architecture = fu.architecture().architecture();
        int portWidth = 0;

        // operation codes are now numbered according to their alphabetical
        // order
        set<string> opcodeSet;
        for (int i = 0; i < architecture.operationCount(); i++) {
            HWOperation* operation = architecture.operation(i);
            opcodeSet.insert(operation->name());
        }

        int opcode = 0;
        for (set<string>::iterator iter = opcodeSet.begin();
             iter != opcodeSet.end(); iter++) {
            // there is an old opcode value in hdb
            if (opcodeSet.size() != 1 && implementation.hasOpcode(*iter)) {
                // old value differs from current guide line
                if (opcode != implementation.opcode(*iter)) {
                    warningStream << "Warning: Opcode defined in HDB for "
                                  << "operation " << *iter
                                  << " does not comply with the requirement "
                                  << "for numbering the operation codes "
                                  << "according to alphabetical order of "
                                  << "operations. Please fix the vhdl and "
                                  << "hdb entry." << std::endl;
                }
            }
            int requiredBits = MathTools::requiredBits(opcode);
            if (requiredBits > portWidth) {
                portWidth = requiredBits;
            }
            opcode++;
        }

        return portWidth;
    }

    /**
     * Calculates the required opcode port width for the given GCU.
     *
     * @param gcu The GCU.
     * @return The required width of opcode port.
     */
    int NetlistGenerator::opcodePortWidth(const TTAMachine::ControlUnit& gcu) {
        set<string> opcodeSet;
        for (int i = 0; i < gcu.operationCount(); i++) {
            HWOperation* operation = gcu.operation(i);
            opcodeSet.insert(operation->name());
        }

        // Assumes that operations of GCU are coded as
        // 0..operationCount()-1.
        if (opcodeSet.size() < 2) {
            return 1;
        } else {
            return MathTools::requiredBits(opcodeSet.size() - 1);
        }
    }

    /**
     * Finds the corresponding ports from the given function units.
     *
     * @param fuToSearch The FU that is searched for the corresponding port.
     * @param origFU The original FU.
     * @param portName Name of the port in the original FU.
     * @return The corresponding port in fuToSearch FU.
     * @exception InstanceNotFound If there is no corresponding port.
     */
    TTAMachine::FUPort&
    NetlistGenerator::findCorrespondingPort(
        const TTAMachine::FunctionUnit& fuToSearch,
        const TTAMachine::FunctionUnit& origFU, const std::string& portName) {
        FUPort* correspondingPort = NULL;
        assert(origFU.hasOperationPort(portName));
        const FUPort* origPort = origFU.operationPort(portName);
        for (int i = 0; i < origFU.operationCount(); i++) {
            HWOperation* origOperation = origFU.operation(i);
            string opName = origOperation->name();
            if (fuToSearch.hasOperation(opName)) {
                HWOperation* operation = fuToSearch.operation(opName);
                if (!origOperation->isBound(*origPort)) {
                    continue;
                }
                int origIO = origOperation->io(*origPort);
                FUPort* port = operation->port(origIO);
                if (port == NULL) {
                    throw InstanceNotFound(__FILE__, __LINE__, __func__);
                }
                if (correspondingPort == NULL) {
                    correspondingPort = port;
                } else if (correspondingPort != port) {
                    throw InstanceNotFound(__FILE__, __LINE__, __func__);
                }
            }
        }

        if (correspondingPort == NULL) {
            THROW_EXCEPTION(InstanceNotFound,
                std::string("Corresponding port for [") + portName +
                    "] defined in HDB was not found in FU of ADF: [" +
                    fuToSearch.name() + "].");
        }

        return *correspondingPort;
    }

    /**
     * Explores the given machine for the width of the instruction memory
     * address.
     *
     * @param machine The machine.
     * @return The width of the instruction memory address.
     * @exception IllegalMachine If the machine is erroneous.
     */
    int
    NetlistGenerator::instructionMemoryAddressWidth(
        const TTAMachine::Machine& machine) {
        AddressSpace& iMem = instructionMemory(machine);
        return MathTools::requiredBits(iMem.end());
    }

    /**
     * Explores the given machine for the width of the instruction memory.
     *
     * @param machine The machine.
     * @return The width of the instruction memory.
     * @exception IllegalMachine If the machine is erroneous.
     */
    int
    NetlistGenerator::instructionMemoryWidth(
        const TTAMachine::Machine& machine) {
        AddressSpace& iMem = instructionMemory(machine);
        return iMem.width();
    }

    /**
     * Returns the address space that represents the instruction memory of
     * the
     * given machine.
     *
     * @param machine The machine.
     * @return The address space.
     * @exception IllegalMachine If the machine does not have the
     * instruction
     *                           memory address space.
     */
    TTAMachine::AddressSpace&
    NetlistGenerator::instructionMemory(const TTAMachine::Machine& machine) {
        ControlUnit* cu = machine.controlUnit();
        if (cu == NULL) {
            string errorMsg = "The machine does not have a control unit.";
            throw IllegalMachine(__FILE__, __LINE__, __func__, errorMsg);
        }

        AddressSpace* iMem = cu->addressSpace();
        if (iMem == NULL) {
            string errorMsg =
                "The control unit does not have an address space.";
            throw IllegalMachine(__FILE__, __LINE__, __func__, errorMsg);
        }

        return *iMem;
    }

    ProGe::Direction
    NetlistGenerator::translateDirection(HDB::Direction direction) {
        switch (direction) {
        case HDB::IN:
            return IN;
        case HDB::OUT:
            return OUT;
        case HDB::BIDIR:
            return BIDIR;
        default:
            assert(false && "Unknown HDB::direction.");
            return IN;
        }
    }

    /**
     * Return fixed instance name if instance name is equal to module name.
     *
     * @param baseInstanceName Default name for a component instance
     * @param moduleName Name for the component module defined in HDB
     * @return Instance name differing from the module name.
     */
    TCEString NetlistGenerator::checkInstanceName(
        const TCEString& baseInstanceName, const TCEString& moduleName) const {

        TCEString newInstanceName;
        if (baseInstanceName.lower() == moduleName.lower()) {
            newInstanceName = baseInstanceName + INSTANCE_SUFFIX;
        } else {
            newInstanceName = baseInstanceName;
        }
        return newInstanceName;
    }

    /**
     * (Ugly) heursitics for identifying an LSU.
     *
     * If it has an address space and implements a memory operation, it is
     * an
     * LSU
     *
     * @param fu Function unit to be tested
     * @return Is the fu an LSU
     */
    bool NetlistGenerator::isLSU(const TTAMachine::FunctionUnit& fu) const {

        if (!fu.hasAddressSpace()) {
            return false;
        }

        const int opCount = 18;
        TCEString ops[opCount] = {
            "ldw", "ldh", "ldq", "ldw2", "ldw4",
            "stw", "sth", "stq", "stw2", "stw4",
            "ld8", "ldu8", "ld16","ldu16", "ld32",
            "st8", "st16", "st32"
        };

        for (auto& op : ops) {
            if (fu.hasOperationLowercase(op)) {
                return true;
            }
        }
        return false;
    }

    /**
     * (Ugly) heuristics for identifying an LSU data memory port.
     *
     * The identification is made by partial match of the port's name.
     *
     * @param portName The port name given in HDB.
     * @return True if the port is LSU data memory port.
     */
    bool NetlistGenerator::isLSUDataPort(
        const TTAMachine::FunctionUnit& adfFU, const std::string& portName) {

        if (!adfFU.hasAddressSpace()) {
            return false;
        }

        static const std::set<std::string> magicWords{
            "addr", "data", "mem_en", "wr_en", "wr_mask"};

        for (auto magicWord : magicWords) {
            if (portName.find(magicWord) != std::string::npos) {
                return true;
            }
        }

        return false;
    }

    Signal NetlistGenerator::inferLSUSignal(const std::string& portName) {
        size_t pos = 0;
        if (((pos = portName.find("addr")) != std::string::npos)) {
            return SignalType::ADDRESS;
        } else if (((pos = portName.find("data")) != std::string::npos)) {
            if ((portName.find("_in", pos + 4)) != std::string::npos) {
                return SignalType::READ_DATA;
            } else if ((portName.find("_out", pos + 4)) != std::string::npos) {
                return SignalType::WRITE_DATA;
            }
        } else if (((pos = portName.find("mem_en")) != std::string::npos)) {
            if ((portName.find("_x", pos + 6)) != std::string::npos) {
                return Signal(SignalType::READ_WRITE_REQUEST, ActiveState::LOW);
            } else {
                return SignalType::READ_WRITE_REQUEST;
            }
        } else if (((pos = portName.find("wr_mask")) != std::string::npos)) {
            if ((portName.find("_x", pos + 7)) != std::string::npos) {
                return Signal(SignalType::WRITE_BITMASK, ActiveState::LOW);
            } else {
                return SignalType::WRITE_BITMASK;
            }
        } else if (((pos = portName.find("wr_en")) != std::string::npos)) {
            if ((portName.find("_x", pos + 5)) != std::string::npos) {
                return Signal(SignalType::WRITEMODE, ActiveState::LOW);
            } else {
                return SignalType::WRITEMODE;
            }
        }
        return SignalType::UNDEFINED;
    }

} // namespace ProGe
