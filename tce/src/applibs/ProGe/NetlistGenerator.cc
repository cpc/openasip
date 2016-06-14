/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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

#include <iostream>
#include <string>
#include <utility> // for make_pair()
#include <boost/format.hpp>
#include <set>

#include "NetlistGenerator.hh"
#include "Netlist.hh"
#include "NetlistBlock.hh"
#include "NetlistPort.hh"
#include "ICDecoderGeneratorPlugin.hh"
#include "ProcessorGenerator.hh"

#include "MachineImplementation.hh"
#include "FUImplementationLocation.hh"

#include "Machine.hh"
#include "FunctionUnit.hh"
#include "HWOperation.hh"
#include "FUPort.hh"
#include "ControlUnit.hh"
#include "ImmediateUnit.hh"
#include "SpecialRegisterPort.hh"

#include "HDBManager.hh"
#include "HDBRegistry.hh"
#include "FUEntry.hh"
#include "FUImplementation.hh"
#include "FUArchitecture.hh"
#include "FUPortImplementation.hh"
#include "FUExternalPort.hh"
#include "RFExternalPort.hh"
#include "RFEntry.hh"
#include "RFImplementation.hh"
#include "RFArchitecture.hh"
#include "RFPortImplementation.hh"

#include "BinaryEncoding.hh"

#include "MathTools.hh"
#include "MapTools.hh"
#include "Conversion.hh"

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
    "instructionword";
const std::string NetlistGenerator::DECODER_RESET_PORT = "rstx";
const std::string NetlistGenerator::DECODER_CLOCK_PORT = "clk";
const std::string NetlistGenerator::DECODER_RA_LOAD_PORT = "ra_load";
const std::string NetlistGenerator::DECODER_PC_LOAD_PORT = "pc_load";
const std::string NetlistGenerator::DECODER_PC_OPCODE_PORT = "pc_opcode";
const std::string NetlistGenerator::DECODER_LOCK_REQ_OUT_PORT = "lock_r";
const std::string NetlistGenerator::DECODER_LOCK_REQ_IN_PORT = "lock";

/**
 * Constructor. Records the input parameters for later operation.
 *
 * @param machine Architecture description of target machine.
 * @param implementation Implementation description of target machine.
 * @param plugin The IC/decoder generator plugin.
 */
NetlistGenerator::NetlistGenerator(
    const TTAMachine::Machine& machine,
    const IDF::MachineImplementation& implementation,
    ICDecoderGeneratorPlugin& plugin) :
    machine_(machine), machImplementation_(implementation), plugin_(plugin),
    instructionDecoder_(NULL), instructionFetch_(NULL), raInPort_(NULL),
    raOutPort_(NULL) {
}


/**
 * The destructor.
 */
NetlistGenerator::~NetlistGenerator() {
    MapTools::deleteAllValues(fuEntryMap_);
    MapTools::deleteAllValues(rfEntryMap_);
}


/**
 * Generates the netlist.
 *
 * @param imemWidthInMAUs Width of instruction memory in MAUs.
 * @param entityNameStr The name string used to make the netlist blocks 
 *                      uniquely named.
 * @return The newly generated netlist.
 * @exception IOException If some of the HDBs given in IDF cannot be
 *                        accessed.
 * @exception InvalidData If HDB or IDF is invalid.
 * @exception OutOfRange If the given width of instruction memory is not 
 *                       positive
 * @exception InstanceNotFound Something missing missing from HDB.
 */
Netlist*
NetlistGenerator::generate(
    int imemWidthInMAUs, 
    TCEString entityNameStr=TOPLEVEL_BLOCK_DEFAULT_NAME,
    std::ostream& warningStream=std::cerr) 
    throw (IOException, InvalidData, OutOfRange, InstanceNotFound) {

    entityNameStr_ = entityNameStr;

    if (imemWidthInMAUs < 1) {
        string errorMsg = "Instruction memory width in MAUs must be positive.";
        throw OutOfRange(__FILE__, __LINE__, __func__, errorMsg);
    }

    Netlist* netlist = new Netlist();
    
    // add toplevel block
    NetlistBlock* toplevelBlock = new NetlistBlock(
        entityNameStr, "toplevel", *netlist);

    // add GCU to the netlist
    addGCUToNetlist(*toplevelBlock, imemWidthInMAUs);

    // add function units to the netlist
    for (int i = 0; i < machImplementation_.fuImplementationCount(); i++) {
        IDF::FUImplementationLocation& location =
            machImplementation_.fuImplementation(i);
        addFUToNetlist(location, *netlist, warningStream);
    }

    // add register files to the netlist
    for (int i = 0; i < machImplementation_.rfImplementationCount(); i++) {
        RFImplementationLocation& location =
            machImplementation_.rfImplementation(i);
        addRFToNetlist(location, *netlist);
    }

    // add immediate units to the netlist
    for (int i = 0; i < machImplementation_.iuImplementationCount(); i++) {
        RFImplementationLocation& location =
            machImplementation_.iuImplementation(i);
        addIUToNetlist(location, *netlist);
    }

    plugin_.completeNetlist(*netlist, *this);

    return netlist;
}


/**
 * Returns the netlist port which is corresponding to the given port in the
 * machine object model.
 *
 * @param port The port in the machine object model
 * @return The corresponding port in the netlist.
 * @exception InstanceNotFound If the netlist does not contain the port.
 */
NetlistPort&
NetlistGenerator::netlistPort(const TTAMachine::Port& port) const
    throw (InstanceNotFound) {

    try {
        return *MapTools::valueForKey<NetlistPort*>(
            portCorrespondenceMap_, &port);
    } catch (const Exception&) {
        throw InstanceNotFound(__FILE__, __LINE__, __func__,
            (boost::format(
                "Cannot find netlist port '%s' in parent unit: '%s'") %
             port.name() %
             port.parentUnit()->name()).str());
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
NetlistGenerator::loadPort(const NetlistPort& port) const
    throw (InstanceNotFound) {

    try {
        return *MapTools::valueForKey<NetlistPort*>(
            loadPortMap_, &port);
    } catch (const Exception&) {
        throw InstanceNotFound(
            __FILE__, __LINE__, __func__,
            (boost::format(
                "Cannot find load port '%s' in instance name: '%s', "
                "module name: '%s'") %
             port.name() %
             port.parentBlock()->instanceName() %
             port.parentBlock()->moduleName()).str());
    }
}


/**
 * Returns true if the given RF port has opcode port. Otherwise, returns false.
 */
bool
NetlistGenerator::hasOpcodePort(const NetlistPort& port) const {
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
NetlistGenerator::rfOpcodePort(const NetlistPort& port) const
    throw (InstanceNotFound) {

    try {
        return *MapTools::valueForKey<NetlistPort*>(
            rfOpcodePortMap_, &port);
    } catch (const Exception&) {
        throw InstanceNotFound(__FILE__, __LINE__, __func__,
            (boost::format(
                "Cannot find rf opcode port '%s' in instance name: '%s', "
                "module name: '%s'") %
             port.name() %
             port.parentBlock()->instanceName() %
             port.parentBlock()->moduleName()).str());
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
NetlistGenerator::rfGuardPort(const NetlistBlock& rfBlock) const
    throw (InstanceNotFound) {

    try {
        return *MapTools::valueForKey<NetlistPort*>(rfGuardPorts_, &rfBlock);
    } catch (const Exception&) {
        throw InstanceNotFound(
            __FILE__, __LINE__, __func__,
            (boost::format(
                "Cannot find guard port in register file instance name: '%s',"
                "module name: '%s'") % 
             rfBlock.instanceName() % rfBlock.moduleName()).str());
    }
}


/**
 * Returns the opcode port of the given FU.
 *
 * @param fuBlock The FU.
 * @return The opcode port.
 * @exception InstanceNotFound If the given FU does not have an opcode port.
 */
NetlistPort&
NetlistGenerator::fuOpcodePort(const NetlistBlock& fuBlock) const
    throw (InstanceNotFound) {

    try {
        return *MapTools::valueForKey<NetlistPort*>(fuOpcodePorts_, &fuBlock);
    } catch (const Exception&) {
        throw InstanceNotFound(__FILE__, __LINE__, __func__,
            (boost::format(
                "Cannot find opcode port in function unit instance name: '%s',"
                "module name: '%s'") % 
             fuBlock.instanceName() % fuBlock.moduleName()).str());
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
NetlistGenerator::fuGuardPort(const NetlistPort& fuPort) const
    throw (InstanceNotFound) {

    try {
        return *MapTools::valueForKey<NetlistPort*>(fuGuardPortMap_, &fuPort);
    } catch (const Exception&) {
        throw InstanceNotFound(__FILE__, __LINE__, __func__,
            (boost::format(
                "Cannot find guard port '%s' in instance name: '%s', "
                "module name: '%s'") %
             fuPort.name() %
             fuPort.parentBlock()->instanceName() %
             fuPort.parentBlock()->moduleName()).str());
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
NetlistGenerator::clkPort(const NetlistBlock& block) const
    throw (InstanceNotFound) {

    try {
        return *MapTools::valueForKey<NetlistPort*>(clkPorts_, &block);
    } catch (const Exception&) {
        throw InstanceNotFound(__FILE__, __LINE__, __func__,
            (boost::format(
                "Cannot find clock port in netlist block instance name: "
                "'%s', module name: '%s'") % 
             block.instanceName() % block.moduleName()).str());
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
NetlistGenerator::rstPort(const NetlistBlock& block) const
    throw (InstanceNotFound) {

    try {
        return *MapTools::valueForKey<NetlistPort*>(rstPorts_, &block);
    } catch (const Exception&) {
        throw InstanceNotFound(__FILE__, __LINE__, __func__,
            (boost::format(
                "Cannot find reset port in netlist block instance name: "
                "'%s', module name: '%s'") % 
             block.instanceName() % block.moduleName()).str());
    }
}


/**
 * Tells whether the given netlist block has a global lock port.
 *
 * @param block The netlist block.
 * @return True if the block has a global lock port, otherwise false.
 */
bool
NetlistGenerator::hasGlockPort(const NetlistBlock& block) const {
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
NetlistGenerator::glockPort(const NetlistBlock& block) const
    throw (InstanceNotFound) {

    try {
        return *MapTools::valueForKey<NetlistPort*>(glockPorts_, &block);
    } catch (const Exception&) {
        throw InstanceNotFound(__FILE__, __LINE__, __func__,
            (boost::format(
                "Cannot find global lock port in netlist block instance name:"
                " '%s', module name: '%s'") % 
             block.instanceName() % block.moduleName()).str());
    }
}


/**
 * Tells whether the given netlist block has global lock request port.
 *
 * @param block The netlist block.
 * @return True if the block has a global lock request port, otherwise false.
 */
bool
NetlistGenerator::hasGlockReqPort(const NetlistBlock& block) const {
    return MapTools::containsKey(glockReqPorts_, &block);
}


/**
 * Returns the global lock request port of the given block in the netlist.
 *
 * @param block The block in the netlist.
 * @return The global lock request port of the given block.
 * @exception InstanceNotFound If the netlist does not contain the port.
 */
NetlistPort&
NetlistGenerator::glockReqPort(const NetlistBlock& block) const
    throw (InstanceNotFound) {

    try {
        return *MapTools::valueForKey<NetlistPort*>(glockReqPorts_, &block);
    } catch (const Exception&) {
        throw InstanceNotFound(__FILE__, __LINE__, __func__,
            (boost::format(
                "Cannot find global lock request port in netlist block "
                "instance name: '%s', module name: '%s'") % 
             block.instanceName() % block.moduleName()).str());
    }
}


/**
 * Returns a netlist port that is the write port of the given immediate unit.
 *
 * @param iu The immediate unit.
 * @return The netlist port.
 * @exception InstanceNotFound If the port is not found.
 */
NetlistPort&
NetlistGenerator::immediateUnitWritePort(
    const TTAMachine::ImmediateUnit& iu) const 
    throw (InstanceNotFound) {

    try {
        return *MapTools::valueForKey<NetlistPort*>(iuPortMap_, &iu);
    } catch (const Exception&) {
        throw InstanceNotFound(__FILE__, __LINE__, __func__,
            (boost::format(
                "Cannot find immediate unit '%s' write port.") %
             iu.name()).str());
    }
}


/**
 * Returns the return address in port of GCU.
 *
 * @return The return address in port.
 * @exception InstanceNotFound If GCU does not have the port.
 */
NetlistPort&
NetlistGenerator::gcuReturnAddressInPort() const
    throw (InstanceNotFound) {

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
NetlistGenerator::gcuReturnAddressOutPort() const
    throw (InstanceNotFound) {

    if (raOutPort_ != NULL) {
        return *raOutPort_;
    } else {
        throw InstanceNotFound(__FILE__, __LINE__, __func__,
            "Cannot find return address of out port of GCU");
    }
}


/**
 * Returns the instruction decoder block in the netlist.
 *
 * @return The instruction decoder block.
 * @exception InstanceNotFound If there is no instruction decoder in the
 *                             netlist.
 */
NetlistBlock&
NetlistGenerator::instructionDecoder() const
    throw (InstanceNotFound) {

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
NetlistGenerator::instructionFetch() const
        throw (InstanceNotFound) {
    if (instructionFetch_ == NULL) {
        throw InstanceNotFound(__FILE__, __LINE__, __func__,
            "Cannot find instruction fetch block from the netlist.");
    } else {
        return *instructionFetch_;
    }
}

/**
 * Returns the FU entry which was selected to represent the given FU.
 *
 * @param fuName Name of the FU in ADF.
 * @return The FU entry.
 * @exception InstanceNotFound If the netlist is not created yet or if there was
 *                             no FU entry for the given FU.
 */
HDB::FUEntry&
NetlistGenerator::fuEntry(const std::string& fuName) const
    throw (InstanceNotFound) {

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
 * @exception InstanceNotFound If the netlist is not created yet or if there
 *                             was no RF entry for the given RF.
 */
HDB::RFEntry&
NetlistGenerator::rfEntry(const std::string& rfName) const
    throw (InstanceNotFound) {

    try {
        return *MapTools::valueForKey<RFEntry*>(rfEntryMap_, rfName);
    } catch (const Exception&) {
        throw InstanceNotFound(__FILE__, __LINE__, __func__,
            "Cannot find register file entry from the netlist.");
    }
}

/**
 * Adds the global control unit to the netlist as sub-block of the given
 * top-level block.
 *
 * @param toplevelBlock The top-level block of the netlist.
 * @param imemWidthInMAUs Width of the instruction memory in MAUs.
 */
void
NetlistGenerator::addGCUToNetlist(
    NetlistBlock& toplevelBlock, int imemWidthInMAUs) {

    if (imemWidthInMAUs != 1) {
        throw InvalidData(__FILE__, __LINE__, __func__,
                          "Imem width is currently fixed to 1 MAU");
    }
    Netlist& netlist = toplevelBlock.netlist();

    NetlistPort* tlClkPort = new NetlistPort(
        CLOCK_PORT_NAME, "1", BIT, HDB::IN, toplevelBlock);
    NetlistPort* tlRstPort = new NetlistPort(
        RESET_PORT_NAME, "1", BIT, HDB::IN, toplevelBlock);
    NetlistPort* tlBusyPort = new NetlistPort(
        BUSY_PORT_NAME, "1", BIT, HDB::IN, toplevelBlock);
    NetlistPort* tlReadEnablePort = new NetlistPort(
        READ_ENABLE_PORT_NAME, "1", BIT, HDB::OUT, toplevelBlock);
    NetlistPort* tlAddressPort = new NetlistPort(
        ADDRESS_PORT_NAME, IMEMADDRWIDTH, BIT_VECTOR, HDB::OUT,
        toplevelBlock);
    NetlistPort* tlDataPort = new NetlistPort(
        DATA_PORT_NAME, IMEMWIDTHFORMULA, BIT_VECTOR, HDB::IN, toplevelBlock);

    mapClockPort(toplevelBlock, *tlClkPort);
    mapResetPort(toplevelBlock, *tlRstPort);

    // add ifetch block
    instructionFetch_ = new NetlistBlock(
        entityNameStr_ + "_" + IFETCH_BLOCK_NAME, "inst_fetch", netlist);
    toplevelBlock.addSubBlock(instructionFetch_);
    NetlistPort* ifetchClkPort = new NetlistPort(
        CLOCK_PORT_NAME, "1", BIT, HDB::IN, *instructionFetch_);
    NetlistPort* ifetchRstPort = new NetlistPort(
        RESET_PORT_NAME, "1", BIT, HDB::IN, *instructionFetch_);
    NetlistPort* ifetchRAOutPort = new NetlistPort(
        RA_OUT_PORT_NAME, IMEMADDRWIDTH, BIT_VECTOR, HDB::OUT, *instructionFetch_);
    raOutPort_ = ifetchRAOutPort;
    NetlistPort* ifetchRAInPort = new NetlistPort(
        RA_IN_PORT_NAME, IMEMADDRWIDTH, BIT_VECTOR, HDB::IN, *instructionFetch_);
    raInPort_ = ifetchRAInPort;
    NetlistPort* ifetchBusyPort = new NetlistPort(
        BUSY_PORT_NAME, "1", BIT, HDB::IN, *instructionFetch_);
    NetlistPort* ifetchReadEnablePort = new NetlistPort(
        READ_ENABLE_PORT_NAME, "1", BIT, HDB::OUT, *instructionFetch_);
    NetlistPort* ifetchAddressPort = new NetlistPort(
        ADDRESS_PORT_NAME, IMEMADDRWIDTH, BIT_VECTOR, HDB::OUT,
        *instructionFetch_);
    NetlistPort* ifetchDataPort = new NetlistPort(
        DATA_PORT_NAME, IMEMWIDTHFORMULA, BIT_VECTOR, HDB::IN, *instructionFetch_);
    NetlistPort* ifetchPCInPort = new NetlistPort(
        PC_IN_PORT_NAME, IMEMADDRWIDTH, BIT_VECTOR, HDB::IN, *instructionFetch_);
    NetlistPort* ifetchPCLoadPort = new NetlistPort(
        PC_LOAD_PORT_NAME, "1", BIT, HDB::IN, *instructionFetch_);
    NetlistPort* ifetchRALoadPort = new NetlistPort(
        RA_LOAD_PORT_NAME, "1", BIT, HDB::IN, *instructionFetch_);
    NetlistPort* ifetchPCOpcodePort = new NetlistPort(
        PC_OPCODE_PORT_NAME, "1", 1, BIT_VECTOR, HDB::IN, *instructionFetch_);
    NetlistPort* ifetchFetchPort = new NetlistPort(
        FETCH_PORT_NAME, "1", BIT, HDB::IN, *instructionFetch_);
    NetlistPort* ifetchGlockPort = new NetlistPort(
        GLOBAL_LOCK_PORT_NAME, "1", BIT, HDB::OUT, *instructionFetch_);
    NetlistPort* ifetchFetchBlockPort = new NetlistPort(
        FETCHBLOCK_PORT_NAME,
        IMEMWIDTHFORMULA, BIT_VECTOR, HDB::OUT, *instructionFetch_);

    // connect ifetch to toplevel
    netlist.connectPorts(*tlClkPort, *ifetchClkPort);
    netlist.connectPorts(*tlRstPort, *ifetchRstPort);
    netlist.connectPorts(*tlBusyPort, *ifetchBusyPort);
    netlist.connectPorts(*tlReadEnablePort, *ifetchReadEnablePort);
    netlist.connectPorts(*tlAddressPort, *ifetchAddressPort);
    netlist.connectPorts(*tlDataPort, *ifetchDataPort);

    // map PC port
    ControlUnit* gcu = machine_.controlUnit();
    if (gcu->hasOperation(CALL)) {
        HWOperation* callOp = gcu->operation(CALL);
        FUPort* pcPort = callOp->port(1);
        mapNetlistPort(*pcPort, *ifetchPCInPort);
    } else if (gcu->hasOperation(JUMP)) {
        HWOperation* jumpOp = gcu->operation(JUMP);
        FUPort* pcPort = jumpOp->port(1);
        mapNetlistPort(*pcPort, *ifetchPCInPort);
    }

    // add decompressor block
    NetlistBlock* decompressorBlock = new NetlistBlock(
        entityNameStr_ + "_" + DECOMPRESSOR_BLOCK_NAME, "decomp", netlist);
    toplevelBlock.addSubBlock(decompressorBlock);
    NetlistPort* decFetchPort = new NetlistPort(
        FETCH_PORT_NAME, "1", BIT, HDB::OUT, *decompressorBlock);
    NetlistPort* decLockPort = new NetlistPort(
        LOCK_PORT_NAME, "1", BIT, HDB::IN, *decompressorBlock);
    NetlistPort* decFetchBlockPort = new NetlistPort(
        FETCHBLOCK_PORT_NAME,
        IMEMWIDTHFORMULA, BIT_VECTOR, HDB::IN, *decompressorBlock);
    NetlistPort* decClkPort = new NetlistPort(
        CLOCK_PORT_NAME, "1", BIT, HDB::IN, *decompressorBlock);
    NetlistPort* decRstPort = new NetlistPort(
        RESET_PORT_NAME, "1", BIT, HDB::IN, *decompressorBlock);
    NetlistPort* decIWordPort = new NetlistPort(
        INSTRUCTIONWORD_PORT_NAME,
        INSTRUCTIONWIDTH, BIT_VECTOR, HDB::OUT, *decompressorBlock);
    NetlistPort* decGlockPort = new NetlistPort(
        GLOBAL_LOCK_PORT_NAME, "1", BIT, HDB::OUT, *decompressorBlock);
    NetlistPort* decLockReqPort = new NetlistPort(
        LOCK_REQUEST_PORT_NAME, "1", BIT, HDB::IN, *decompressorBlock);

    // connect ifetch to decompressor
    netlist.connectPorts(*ifetchFetchPort, *decFetchPort);
    netlist.connectPorts(*ifetchGlockPort, *decLockPort);
    netlist.connectPorts(
        *ifetchFetchBlockPort, *decFetchBlockPort);

    // connect toplevel to decompressor
    netlist.connectPorts(*tlClkPort, *decClkPort);
    netlist.connectPorts(*tlRstPort, *decRstPort);

    // add decoder block
    NetlistBlock* decoderBlock = new NetlistBlock(
        entityNameStr_ + "_" + DECODER_BLOCK_NAME, "inst_decoder", netlist);
    toplevelBlock.addSubBlock(decoderBlock);
    instructionDecoder_ = decoderBlock;
    NetlistPort* decodIWordPort = new NetlistPort(
        DECODER_INSTR_WORD_PORT,
        INSTRUCTIONWIDTH, BIT_VECTOR, HDB::IN, *decoderBlock);
    NetlistPort* decodPCLoadPort = new NetlistPort(
        DECODER_PC_LOAD_PORT, "1", BIT, HDB::OUT, *decoderBlock);
    NetlistPort* decodRALoadPort = new NetlistPort(
        DECODER_RA_LOAD_PORT, "1", BIT, HDB::OUT, *decoderBlock);
    NetlistPort* decodPCOpcodePort = new NetlistPort(
        DECODER_PC_OPCODE_PORT, "1", 1, BIT_VECTOR, HDB::OUT, *decoderBlock);
    NetlistPort* decodLockPort = new NetlistPort(
        DECODER_LOCK_REQ_IN_PORT, "1", BIT, HDB::IN, *decoderBlock);
    NetlistPort* decodLockReqPort = new NetlistPort(
        DECODER_LOCK_REQ_OUT_PORT, "1", BIT, HDB::OUT, *decoderBlock);
    NetlistPort* decodClkPort = new NetlistPort(
        DECODER_CLOCK_PORT, "1", BIT, HDB::IN, *decoderBlock);
    NetlistPort* decodRstPort = new NetlistPort(
        DECODER_RESET_PORT, "1", BIT, HDB::IN, *decoderBlock);

    // connect decoder to decompressor
    netlist.connectPorts(*decIWordPort, *decodIWordPort);
    netlist.connectPorts(*decGlockPort, *decodLockPort);
    netlist.connectPorts(*decLockReqPort, *decodLockReqPort);

    // connect decoder to top-level
    netlist.connectPorts(*decodClkPort, *tlClkPort);
    netlist.connectPorts(*decodRstPort, *tlRstPort);

    // connect decoder to ifetch
    netlist.connectPorts(*decodPCLoadPort, *ifetchPCLoadPort);
    netlist.connectPorts(*decodRALoadPort, *ifetchRALoadPort);
    netlist.connectPorts(*decodPCOpcodePort, *ifetchPCOpcodePort);
}


/**
 * Adds the FU identified by the given FUImplementationLocation
 * instance to the netlist.
 *
 * @param location The FUImplementationLocation instance.
 * @param netlist The netlist.
 * @exception IOException If the HDB that contains the implementation cannot
 *                        be accessed.
 * @exception InvalidData If the MachineImplementation instance is erroneous
 *                        or if HDB erroneous.
 */
void
NetlistGenerator::addFUToNetlist(
    const FUImplementationLocation& location,
    Netlist& netlist, std::ostream& warningStream)
    throw (IOException, InvalidData) {

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
            errorMsg = "FU entry " + Conversion::toString(id) + " does not "
                "have an implementation in HDB " + hdbFile + ".";
        } else {
            errorMsg = "FU entry " + Conversion::toString(id) + " does not "
                "have architecture definition in HDB " + hdbFile + ".";
        }
        throw InvalidData(__FILE__, __LINE__, __func__, errorMsg);
    }

    if (!machine_.functionUnitNavigator().hasItem(location.unitName())) {
        string errorMsg = "ADF does not have FU '" + location.unitName() +
            "' which is referred to in the IDF.";
        throw InvalidData(__FILE__, __LINE__, __func__, errorMsg);
    }

    fuEntryMap_.insert(std::make_pair(location.unitName(), entry));
    FUImplementation& fuImplementation = entry->implementation();
    FUArchitecture& architecture = entry->architecture();
    const FunctionUnit* adfFU = machine_.functionUnitNavigator().item(
        location.unitName());
    TCEString instanceName = FU_NAME_PREFIX + location.unitName();
    TCEString moduleName = fuImplementation.moduleName();
    instanceName = checkInstanceName(instanceName, moduleName);
    NetlistBlock* block = new NetlistBlock(
        moduleName, instanceName, netlist);
    netlist.topLevelBlock().addSubBlock(block);

    format architecturesDontMatch(
        "Architectures of FU entry %1% in %2% and FU '%3%' don't match.");

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
                
                // Special case if the FU is an LSU and it has parametrizable
                // address width. This sets the ADF port width according to
                // address space width in ADF
                if (adfFU->hasAddressSpace() && isLSU(*adfFU)) {
                    FUPort& adfPort = findCorrespondingPort(
                        *adfFU, architecture.architecture(),
                        port.architecturePort());
                    // Assume address port is the triggering port
                    if (adfPort.isTriggering() 
                        && port.widthFormula() == paramName) {
                        int ASWidth = calculateAddressWidth(adfFU);
                        block->setParameter(
                            paramName, paramType, 
                            Conversion::toString(ASWidth));
                        // Fix the FU port width
                        adfPort.setWidth(ASWidth);
                        parameterResolved = true;
                        break;
                    }
                }
                if (port.widthFormula() == paramName) {
                    try {
                        FUPort& adfPort = findCorrespondingPort(
                            *adfFU, architecture.architecture(),
                            port.architecturePort());
                        block->setParameter(
                            paramName, paramType,
                            Conversion::toString(adfPort.width()));
                        parameterResolved = true;
                        break;
                    } catch (const InstanceNotFound&) {
                        architecturesDontMatch % id % hdbFile %
                            adfFU->name();
                        throw InvalidData(
                            __FILE__, __LINE__, __func__,
                            architecturesDontMatch.str());
                    }
                }
            }

            if (!parameterResolved) {
                format errorMsg(
                    "Unable to resolve the value of parameter %1% of FU "
                    "entry %2%.");
                errorMsg % paramName % id;
                throw InvalidData(
                    __FILE__, __LINE__, __func__, errorMsg.str());
            }

        } else {
            block->setParameter(paramName, paramType, paramValue);
        }
    }

    NetlistBlock& topLevelBlock = netlist.topLevelBlock();

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
        NetlistPort* newPort = new NetlistPort(
            port.name(), port.widthFormula(), adfPort->width(), BIT_VECTOR,
            architecture.portDirection(architecturePort), *block);
        mapNetlistPort(*adfPort, *newPort);

        // add load port
        if (port.loadPort() != "") {
            NetlistPort* loadPort = new NetlistPort(
                port.loadPort(), "1", 1, BIT, HDB::IN, *block);
            mapLoadPort(*newPort, *loadPort);
        }

        // add guard port
        if (port.guardPort() != "") {
            NetlistPort* guardPort = new NetlistPort(
                port.guardPort(), "1", 1, BIT, HDB::OUT, *block);
            mapFUGuardPort(*newPort, *guardPort);
        }
    }

    // add opcode port
    if (fuImplementation.opcodePort() != "") {
        NetlistPort* opcodePort = new NetlistPort(
            fuImplementation.opcodePort(),
            Conversion::toString(opcodePortWidth(*entry, warningStream)),
            opcodePortWidth(*entry, warningStream), 
            BIT_VECTOR, HDB::IN, *block);
        mapFUOpcodePort(*block, *opcodePort);
    }

    // add external ports
    // for each external port, a port is added to top-level block too
    for (int i = 0; i < fuImplementation.externalPortCount(); i++) {
        FUExternalPort& externalPort = fuImplementation.externalPort(i);
        // if external port uses parameter, it must be added as netlist 
        // parameter too and create new width formula for the top-level port
        // by replacing parameter names with the corresponding names in
        // netlist.
        string tlPortWidth = externalPort.widthFormula();
        for (int i = 0; i < externalPort.parameterDependencyCount(); i++) {
            string paramName = externalPort.parameterDependency(i);
            Netlist::Parameter param = block->parameter(paramName);
            string nlParamName = "fu_" + location.unitName() + "_" +
                paramName;
            if (!netlist.hasParameter(nlParamName)) {
                netlist.setParameter(
                    nlParamName, param.type, param.value);
            }
            block->setParameter(param.name, param.type, nlParamName);
            size_t replaceStart = tlPortWidth.find(param.name, 0);
            if (replaceStart == std::string::npos) {
                throw InvalidData(__FILE__, __LINE__, __func__,
                        (boost::format(
                        "FU external port parameter dependencies do not seem "
                        "to be right: Tried to find parameter named '%s'"
                        " from external port width formula '%s' in unit '%s'") 
                        % param.name % tlPortWidth % nlParamName).str());
            }
            size_t replaceLength = param.name.length();
            tlPortWidth = tlPortWidth.replace(
                replaceStart, replaceLength, nlParamName);
        }

        NetlistPort* extPort = new NetlistPort(
            externalPort.name(), externalPort.widthFormula(), BIT_VECTOR,
            externalPort.direction(), *block);
        // connect the external port to top level
        string tlPortName = "fu_" + location.unitName() + "_" +
            externalPort.name();
        NetlistPort* tlPort = new NetlistPort(
            tlPortName, tlPortWidth, BIT_VECTOR, externalPort.direction(),
            topLevelBlock);
        netlist.connectPorts(*tlPort, *extPort);
    }

    // add clock port
    if (fuImplementation.clkPort() != "") {
        NetlistPort* clkPort = new NetlistPort(
            fuImplementation.clkPort(), "1", 1, BIT, HDB::IN, *block);
        mapClockPort(*block, *clkPort);
        // connect clock port
        NetlistPort& tlClkPort = this->clkPort(topLevelBlock);
        netlist.connectPorts(*clkPort, tlClkPort);
    }

    // add reset port
    if (fuImplementation.rstPort() != "") {
        NetlistPort* rstPort = new NetlistPort(
            fuImplementation.rstPort(), "1", 1, BIT, HDB::IN, *block);
        mapResetPort(*block, *rstPort);
        // connect reset port
        NetlistPort& tlRstPort = this->rstPort(topLevelBlock);
        netlist.connectPorts(*rstPort, tlRstPort);
    }

    // add global lock port
    if (fuImplementation.glockPort() != "") {
        NetlistPort* glockPort = new NetlistPort(
            fuImplementation.glockPort(), "1", 1, BIT, HDB::IN, *block);
        mapGlobalLockPort(*block, *glockPort);
    }

    // add global lock request port
    if (fuImplementation.glockReqPort() != "") {
        NetlistPort* glockReqPort = new NetlistPort(
            fuImplementation.glockReqPort(), "1", 1, BIT, HDB::OUT, *block);
        mapGlobalLockRequestPort(*block, *glockReqPort);
    }
}


/**
 * Adds the RF identified by the given RFImplementationLocation
 * instance to the netlist.
 *
 * @param location The RFImplementationLocation instance.
 * @param netlist The netlist.
 * @exception IOException If the HDB that contains the implementation cannot
 *                        be accessed.
 * @exception InvalidData If the MachineImplementation instance or HDB is
 *                        erroneous.
 */
void
NetlistGenerator::addRFToNetlist(
    const RFImplementationLocation& location,
    Netlist& netlist)
    throw (IOException, InvalidData) {

    if (!machine_.registerFileNavigator().hasItem(location.unitName())) {
        string errorMsg = "ADF does not contain register file '" +
            location.unitName() + "' referred to in IDF.";
        throw InvalidData(__FILE__, __LINE__, __func__, errorMsg);
    }
    RegisterFile* rf = machine_.registerFileNavigator().item(
        location.unitName());
    addBaseRFToNetlist(*rf, location, netlist, RF_NAME_PREFIX);
}


/**
 * Adds the IU identified by the given RFImplementationLocation
 * instance to the netlist.
 *
 * @param location The RFImplementationLocation instance.
 * @param netlist The netlist.
 * @exception IOException If the HDB that contains the implementation cannot
 *                        be accessed.
 * @exception InvalidData If the MachineImplementation instance or HDB is
 *                        erroneous.
 */
void
NetlistGenerator::addIUToNetlist(
    const RFImplementationLocation& location,
    Netlist& netlist)
    throw (IOException, InvalidData) {

    if (!machine_.immediateUnitNavigator().hasItem(location.unitName())) {
        string errorMsg = "ADF does not contain immediate unit '" +
            location.unitName() + "' referred to in IDF.";
        throw InvalidData(__FILE__, __LINE__, __func__, errorMsg);
    }
    ImmediateUnit* iu = machine_.immediateUnitNavigator().item(
        location.unitName());
    addBaseRFToNetlist(*iu, location, netlist, IU_NAME_PREFIX);
}


/**
 * Adds the RF or IU identified by the given RFImplementationLocation
 * instance to the netlist.
 *
 * @param regFile The corresponding RF (or IU) in ADF.
 * @param location The RFImplementationLocation instance.
 * @param netlist The netlist.
 * @param blockNamePrefix Prefix to be added to the block name.
 * @exception IOException If the HDB that contains the implementation cannot
 *                        be accessed.
 * @exception InvalidData If the RF entry in HDB does not have an
 *                        implementation or architecture or if the HDB does
 *                        not have the entry defined in the
 *                        RFImplementationLocation instance at all.
 */
void
NetlistGenerator::addBaseRFToNetlist(
    const TTAMachine::BaseRegisterFile& regFile,
    const RFImplementationLocation& location,
    Netlist& netlist,
    const std::string& blockNamePrefix)
    throw (IOException, InvalidData) {

    RFEntry* entry = NULL;
    try {
        HDBManager& manager = HDBRegistry::instance().hdb(location.hdbFile());
        entry = manager.rfByEntryID(location.id());
    } catch (const KeyNotFound& e) {
        throw InvalidData(__FILE__, __LINE__, __func__, e.errorMessage());
    }

    if (!entry->hasImplementation() || !entry->hasArchitecture()) {
        format text(
            "RF entry %1% does not have an implementation or architecture "
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
    NetlistBlock* block = new NetlistBlock(
        moduleName, instanceName, netlist);
    NetlistBlock& topLevelBlock = netlist.topLevelBlock();
    topLevelBlock.addSubBlock(block);

    // Add parameters (generics) to the block.
    for (int i = 0; i< implementation.parameterCount(); i++) {
        RFImplementation::Parameter param = implementation.parameter(i);

        // Check if parameter matches size or width parameter reference
        // and set/override its parameter value according to architecture.
        if (param.name == implementation.sizeParameter()) {
            block->setParameter(
                implementation.sizeParameter(), "integer",
                Conversion::toString(regFile.numberOfRegisters()));
        } else if (param.name == implementation.widthParameter()) {
            block->setParameter(
                implementation.widthParameter(), "integer",
                Conversion::toString(regFile.width()));
        } else if (param.value == "") {
            format errorMsg(
                "Unable to resolve the value of parameter %1% of RF "
                "entry %2%.");
            errorMsg % param.name % location.id();
            throw InvalidData(
                __FILE__, __LINE__, __func__, errorMsg.str());
        } else {
            block->setParameter(param.name, param.type, param.value);
        }
    }

    // add ports
    for (int i = 0; i < implementation.portCount(); i++) {
        RFPortImplementation& port = implementation.port(i);
        NetlistPort* newPort = NULL;
        if (!implementation.widthParameter().empty()) {
            newPort = new NetlistPort(
                port.name(), implementation.widthParameter(),
                regFile.width(), BIT_VECTOR, port.direction(), *block);
        } else {
            newPort = new NetlistPort(
                port.name(), Conversion::toString(architecture.width()),
                BIT_VECTOR, port.direction(), *block);
        }

        // map the port if it is not input port of IU (not visible in ADF)
        if (dynamic_cast<const ImmediateUnit*>(&regFile) == NULL ||
            port.direction() != HDB::IN) {

            bool mapped = false;
            for (int i = 0; i < regFile.portCount(); i++) {
                RFPort* rfPort = regFile.port(i);
                if (!MapTools::containsKey(portCorrespondenceMap_, rfPort)) {
                    if ((port.direction() == HDB::IN &&
                        rfPort->inputSocket() != NULL &&
                        rfPort->outputSocket() == NULL) ||
                        (port.direction() == HDB::OUT &&
                        rfPort->outputSocket() != NULL &&
                        rfPort->inputSocket() == NULL) ||
                        (port.direction() == HDB::BIDIR &&
                        rfPort->outputSocket() != NULL &&
                        rfPort->inputSocket() != NULL)) {
                        assert(
                            !MapTools::containsValue(
                                portCorrespondenceMap_, newPort));
                        mapNetlistPort(*rfPort, *newPort);
                        mapped = true;
                        break;
                    }
                }
            }

            if (!mapped) {
                format text(
                    "Unable to map port '%1%' of RF entry '%2%' in HDB '%3%' "
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

        // if the given BaseRegisterFile is ImmediateUnit, keep track of the
        // data ports that does not appear in ADF
        const ImmediateUnit* iu = dynamic_cast<const ImmediateUnit*>(
            &regFile);
        if (iu != NULL && newPort->direction() == HDB::IN) {
            mapImmediateUnitWritePort(*iu, *newPort);
        }

        // add load port
        NetlistPort* loadPort = new NetlistPort(
            port.loadPort(), "1", BIT, HDB::IN, *block);
        mapLoadPort(*newPort, *loadPort);

        // add opcode port
        NetlistPort* opcodePort = NULL;
        if (!port.opcodePort().empty()) {
            opcodePort = new NetlistPort(
                port.opcodePort(), port.opcodePortWidthFormula(),
                MathTools::requiredBits(regFile.numberOfRegisters() - 1),
                BIT_VECTOR, HDB::IN, *block);
            mapRFOpcodePort(*newPort, *opcodePort);
        } else if (regFile.numberOfRegisters() == 1) {
            // Special case for single register RFs which do not need opcode
            // port. For legacy support the opcode port is left out if opcode
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
        NetlistPort* guardPort = new NetlistPort(
            guardPortName, size, regFile.numberOfRegisters(), BIT_VECTOR,
            HDB::OUT, *block);
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
        // parameter too and create new width formula for the top-level port
        // by replacing parameter names with the corresponding names in
        // netlist.
        string tlPortWidth = externalPort.widthFormula();
        for (int i = 0; i < externalPort.parameterDependencyCount(); i++) {
            string hdbParamName = externalPort.parameterDependency(i);
            Netlist::Parameter param = block->parameter(hdbParamName);
            string nlParamName = component_type + location.unitName() + "_" +
                hdbParamName;
            if (!netlist.hasParameter(nlParamName)) {
                netlist.setParameter(nlParamName, param.type, param.value);
            }
            block->setParameter(param.name, param.type, nlParamName);
            size_t replaceStart = tlPortWidth.find(param.name, 0);
            if (replaceStart == std::string::npos) {
                throw InvalidData(__FILE__, __LINE__, __func__,
                    (boost::format(
                        "RF external port parameter dependencies do not seem "
                        "to be right: Tried to find parameter named '%s'"
                        " from external port width formula '%s' in unit '%s'")
                        % param.name % tlPortWidth % nlParamName).str());
            }
            size_t replaceLength = param.name.length();
            tlPortWidth = tlPortWidth.replace(
                replaceStart, replaceLength, nlParamName);
        }

        NetlistPort* extPort = new NetlistPort(
            externalPort.name(), externalPort.widthFormula(), BIT_VECTOR,
            externalPort.direction(), *block);
        // connect the external port to top level
        string tlPortName = component_type + location.unitName() + "_" +
            externalPort.name();
        NetlistPort* tlPort = new NetlistPort(
            tlPortName, tlPortWidth, BIT_VECTOR, externalPort.direction(),
            topLevelBlock);
        netlist.connectPorts(*tlPort, *extPort);
    }

    // add clock port
    NetlistPort* clockPort = new NetlistPort(
        implementation.clkPort(), "1", BIT, HDB::IN, *block);
    mapClockPort(*block, *clockPort);
    // connect clock port
    NetlistPort& tlClockPort = clkPort(topLevelBlock);
    netlist.connectPorts(tlClockPort, *clockPort);

    // add reset port
    NetlistPort* resetPort = new NetlistPort(
        implementation.rstPort(), "1", BIT, HDB::IN, *block);
    mapResetPort(*block, *resetPort);
    // connect reset port
    NetlistPort& tlResetPort = rstPort(topLevelBlock);
    netlist.connectPorts(tlResetPort, *resetPort);

    // add glock port
    if (implementation.glockPort() != "") {
        NetlistPort* glockPort = new NetlistPort(
            implementation.glockPort(), "1", BIT, HDB::IN, *block);
        mapGlobalLockPort(*block, *glockPort);
    }
}


/**
 * Maps the given ADF port to the given netlist port.
 *
 * @param adfPort The port in ADF (Machine Object Model).
 * @param netlistPort The corresponding port in the netlist.
 */
void
NetlistGenerator::mapNetlistPort(
    const TTAMachine::Port& adfPort,
    NetlistPort& netlistPort) {

    assert(!MapTools::containsKey(portCorrespondenceMap_, &adfPort));
    portCorrespondenceMap_.insert(
        std::pair<const Port*, NetlistPort*>(&adfPort, &netlistPort));
}


/**
 * Maps the given load port for the given architectural port.
 *
 * @param port The architectural port in the netlist.
 * @param loadPort The load port of the architectural port.
 */
void
NetlistGenerator::mapLoadPort(
    const NetlistPort& port,
    NetlistPort& loadPort) {

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
void
NetlistGenerator::mapRFOpcodePort(
    const NetlistPort& port,
    NetlistPort& opcodePort) {

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
void
NetlistGenerator::mapFUOpcodePort(
    const NetlistBlock& block,
    NetlistPort& port) {

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
void
NetlistGenerator::mapClockPort(
    const NetlistBlock& block,
    NetlistPort& clkPort) {

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
void
NetlistGenerator::mapResetPort(
    const NetlistBlock& block,
    NetlistPort& resetPort) {

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
void
NetlistGenerator::mapGlobalLockPort(
    const NetlistBlock& block,
    NetlistPort& glockPort) {

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
void
NetlistGenerator::mapGlobalLockRequestPort(
    const NetlistBlock& block,
    NetlistPort& glockReqPort) {

    assert(!MapTools::containsKey(glockReqPorts_, &block));
    glockReqPorts_.insert(
        std::pair<const NetlistBlock*, NetlistPort*>(&block, &glockReqPort));
}


/**
 * Maps the given guard port for the given RF block.
 *
 * @param block The netlist block.
 * @param guardPort The guard port of the block.
 */
void
NetlistGenerator::mapRFGuardPort(
    const NetlistBlock& block,
    NetlistPort& guardPort) {

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
void
NetlistGenerator::mapFUGuardPort(
    const NetlistPort& dataPort,
    NetlistPort& guardPort) {

    assert(!MapTools::containsKey(fuGuardPortMap_, &dataPort));
    fuGuardPortMap_.insert(
        std::pair<const NetlistPort*, NetlistPort*>(&dataPort, &guardPort));
}


/**
 * Maps the given netlist port as a write port of the given immediate unit.
 *
 * @param iu The immediate unit.
 * @param port The netlist port.
 */
void
NetlistGenerator::mapImmediateUnitWritePort(
    const TTAMachine::ImmediateUnit& iu,
    NetlistPort& port) {

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
bool
NetlistGenerator::isParameterizable(
     const string& paramName, const FUEntry* fuEntry) const {
    HDB::FUImplementation& fuImplementation = fuEntry->implementation();

    for (int i = 0; i < fuImplementation.architecturePortCount(); i++) {

        FUPortImplementation& port = fuImplementation.architecturePort(i);
        string widthFormula = port.widthFormula();
        if (widthFormula == paramName) {
            return fuEntry->
                architecture().hasParameterizedWidth(port.architecturePort());
        }
    }
    return false;
}

/**
 * Calculates the address width of an address space in FU
 *
 * @exception Invalid data If the FU doesn't have an address space or the
 * address space is invalid
 *
 */
unsigned int
NetlistGenerator::calculateAddressWidth(
    TTAMachine::FunctionUnit const* fu)const {
    if (fu->hasAddressSpace() && fu->addressSpace() != NULL) {
        AddressSpace* AS = fu->addressSpace();
        unsigned int highestAddr = AS->end();
        unsigned int lowestAddr = AS->start();
        if (highestAddr == 0 || lowestAddr >= highestAddr) {
            string errorMessage = "Invalid address space";
            throw InvalidData(__FILE__, __LINE__, __func__,
                              errorMessage.c_str());
        }
        return static_cast<unsigned int>(
            ceil(log(highestAddr) / log(2)));
    } else {
        string errorMessage = "Tried to resolve address space width "
            "from FU '" + fu->name() + "' that doesn't have address space";
        throw InvalidData(__FILE__, __LINE__, __func__,
                          errorMessage.c_str());
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
    const HDB::FUEntry& fu, std::ostream& warningStream)
    throw (InvalidData) {

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
    const TTAMachine::FunctionUnit& origFU,
    const std::string& portName)
    throw (InstanceNotFound) {

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
        throw InstanceNotFound(__FILE__, __LINE__, __func__);
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
    const TTAMachine::Machine& machine)
    throw (IllegalMachine) {

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
    const TTAMachine::Machine& machine)
    throw (IllegalMachine) {

    AddressSpace& iMem = instructionMemory(machine);
    return iMem.width();
}


/**
 * Returns the address space that represents the instruction memory of the
 * given machine.
 *
 * @param machine The machine.
 * @return The address space.
 * @exception IllegalMachine If the machine does not have the instruction
 *                           memory address space.
 */
TTAMachine::AddressSpace&
NetlistGenerator::instructionMemory(const TTAMachine::Machine& machine)
    throw (IllegalMachine) {

    ControlUnit* cu = machine.controlUnit();
    if (cu == NULL) {
        string errorMsg = "The machine does not have a control unit.";
        throw IllegalMachine(__FILE__, __LINE__, __func__, errorMsg);
    }

    AddressSpace* iMem = cu->addressSpace();
    if (iMem == NULL) {
        string errorMsg = "The control unit does not have an address space.";
        throw IllegalMachine(__FILE__, __LINE__, __func__, errorMsg);
    }

    return *iMem;
}

/**
 * Return fixed instance name if instance name is equal to module name.
 *
 * @param baseInstanceName Default name for a component instance
 * @param moduleName Name for the component module defined in HDB
 * @return Instance name differing from the module name.
 */
TCEString
NetlistGenerator::checkInstanceName(
    const TCEString& baseInstanceName, 
    const TCEString& moduleName) const {
    
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
 * If it has an address space and implements a memory operation, it is an LSU
 *
 * @param fu Function unit to be tested
 * @return Is the fu an LSU
 */
bool
NetlistGenerator::isLSU(const TTAMachine::FunctionUnit& fu) const {

    if (!fu.hasAddressSpace()) {
        return false;
    }
    
    const int opCount = 10;
    TCEString ops[opCount] = 
        {"ldw","ldh","ldq","ldw2","ldw4","stw","sth","stq","stw2","stw4"};
    bool foundOp = false;
    for (int i = 0; i < opCount; i++) {
        if (fu.hasOperationLowercase(ops[i])) {
            foundOp = true;
            break;
        }
    }
    return foundOp;
}

} // namespace ProGe
