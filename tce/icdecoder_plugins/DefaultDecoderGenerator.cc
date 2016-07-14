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
 * @file DefaultDecoderGenerator.cc
 *
 * Implementation of DefaultDecoderGenerator class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @author Vinogradov Viacheslav(added Verilog generating) 2012
 * @note rating: red
 */
#include <string>
#include <fstream>
#include <iostream>
#include <set>
#include <boost/format.hpp>

#include "DefaultDecoderGenerator.hh"
#include "CentralizedControlICGenerator.hh"

#include "NetlistBlock.hh"
#include "NetlistGenerator.hh"
#include "NetlistPort.hh"
#include "Netlist.hh"
#include "VHDLNetlistWriter.hh"
#include "VerilogNetlistWriter.hh"

#include "Machine.hh"
#include "MachineInfo.hh"
#include "Bus.hh"
#include "Segment.hh"
#include "Socket.hh"
#include "Guard.hh"
#include "FUPort.hh"
#include "SpecialRegisterPort.hh"
#include "ControlUnit.hh"
#include "HWOperation.hh"

#include "BinaryEncoding.hh"
#include "LImmDstRegisterField.hh"
#include "MoveSlot.hh"
#include "SourceField.hh"
#include "DestinationField.hh"
#include "GuardField.hh"
#include "SocketEncoding.hh"
#include "ImmediateEncoding.hh"
#include "SocketCodeTable.hh"
#include "ImmediateSlotField.hh"
#include "ImmediateControlField.hh"
#include "GPRGuardEncoding.hh"
#include "FUGuardEncoding.hh"
#include "UnconditionalGuardEncoding.hh"
#include "FUPortCode.hh"
#include "RFPortCode.hh"
#include "IUPortCode.hh"

#include "FUEntry.hh"
#include "FUImplementation.hh"

#include "RFEntry.hh"
#include "RFImplementation.hh"

#include "FileSystem.hh"
#include "MathTools.hh"
#include "AssocTools.hh"
#include "StringTools.hh"
#include "TCEString.hh"
#include "Conversion.hh"

using namespace ProGe;
using namespace TTAMachine;
using namespace HDB;

using std::string;
using std::endl;
using std::set;
using boost::format;

const string LIMM_TAG_SIGNAL = "limm_tag";
const string GLOCK_PORT_NAME = "glock";
const string LOCK_REQ_PORT_NAME = "lock_req";


const string JUMP = "jump";
const string CALL = "call";

/**
 * The constructor.
 *
 * @param machine The machine.
 * @param bem The binary encoding map.
 * @param icGenerator The IC generator.
 */
DefaultDecoderGenerator::DefaultDecoderGenerator(
    const TTAMachine::Machine& machine,
    const BinaryEncoding& bem,
    const CentralizedControlICGenerator& icGenerator) : 
    machine_(machine), bem_(bem), icGenerator_(icGenerator),
    nlGenerator_(NULL), decoderBlock_(NULL), generateLockTrace_(false),
    lockTraceStartingCycle_(1), generateDebugger_(false) {
}

/**
 * SetHDL.
 *
 * @param language The HDL language.
 */
void
DefaultDecoderGenerator::SetHDL(ProGe::HDL language){
    language_=language;
}

/**
 * The destructor.
 */
DefaultDecoderGenerator::~DefaultDecoderGenerator() {
}

void
DefaultDecoderGenerator::setGenerateDebugger(bool generate) {
    generateDebugger_ = generate;
}

/**
 * Completes the decoder block in the given netlist by adding the
 * IC-interface and connecting the decoder to the interconnection
 * network and machine building units.
 *
 * @param nlGenerator The netlist generator that generated the netlist.
 * @param netlist The netlist that contains the decoder.
 */
void
DefaultDecoderGenerator::completeDecoderBlock(
    const ProGe::NetlistGenerator& nlGenerator,
    ProGe::Netlist& netlist) {

    nlGenerator_ = &nlGenerator;
    NetlistBlock& decoder = nlGenerator.instructionDecoder();
    decoderBlock_ = &decoder;

    entityNameStr_ = netlist.topLevelBlock().moduleName();

    // add ports for short immediates to decoder and connect them to IC
    Machine::BusNavigator busNav = machine_.busNavigator();
    for (int i = 0; i < busNav.count(); i++) {
        Bus* bus = busNav.item(i);
        if (bus->immediateWidth() > 0) {
            NetlistPort& icSimmPort = icGenerator_.simmDataPort(
                bus->name());
            NetlistPort& icSimmCntrlPort = icGenerator_.
                simmCntrlPort(bus->name());
            NetlistPort* decSimmPort = new NetlistPort(
                simmDataPort(bus->name()),
                Conversion::toString(simmPortWidth(*bus)),
                simmPortWidth(*bus), ProGe::BIT_VECTOR, HDB::OUT, decoder);
            NetlistPort* decSimmCntrlPort = new NetlistPort(
                simmControlPort(bus->name()), "1", 1, ProGe::BIT_VECTOR,
                HDB::OUT, decoder);
            netlist.connectPorts(*decSimmPort, icSimmPort);
            netlist.connectPorts(*decSimmCntrlPort, icSimmCntrlPort);
        }
    }

    // add socket control ports to decoder and connect them to IC
    Machine::SocketNavigator socketNav = machine_.socketNavigator();
    for (int i = 0; i < socketNav.count(); i++) {
        Socket* socket = socketNav.item(i);
        if (socket->portCount() == 0 || socket->segmentCount() == 0) {
            continue;
        }
        if (needsBusControl(*socket)) {
            NetlistPort* busCntrlPort = new NetlistPort(
                socketBusControlPort(socket->name()), 
                Conversion::toString(busControlWidth(*socket)),
                busControlWidth(*socket), ProGe::BIT_VECTOR, HDB::OUT,
                decoder);
            NetlistPort& icBusCntrlPort = icGenerator_.busCntrlPortOfSocket(
                socket->name());
            netlist.connectPorts(icBusCntrlPort, *busCntrlPort);
        }
        if (needsDataControl(*socket)) {
            NetlistPort* dataCntrlPort = new NetlistPort(
                socketDataControlPort(socket->name()), 
                Conversion::toString(dataControlWidth(*socket)),
                dataControlWidth(*socket), ProGe::BIT_VECTOR, HDB::OUT,
                decoder);
            NetlistPort& icDataCntrlPort = 
                icGenerator_.dataCntrlPortOfSocket(socket->name());
            netlist.connectPorts(icDataCntrlPort, *dataCntrlPort);
        }
    }
    
    // add FU control ports to decoder and connect them to the FUs
    Machine::FunctionUnitNavigator fuNav = 
        machine_.functionUnitNavigator();
    for (int i = 0; i < fuNav.count(); i++) {
        FunctionUnit* fu = fuNav.item(i);
        for (int i = 0; i < fu->portCount(); i++) {
            BaseFUPort* port = fu->port(i);
            NetlistPort& nlPort = nlGenerator.netlistPort(*port);
            if (nlPort.direction() == HDB::IN) {
                NetlistPort& loadPort = nlGenerator.loadPort(nlPort);
                NetlistPort* loadCntrlPort = new NetlistPort(
                    fuLoadCntrlPort(fu->name(), port->name()), "1", 1, 
                    ProGe::BIT, HDB::OUT, decoder);
                netlist.connectPorts(*loadCntrlPort, loadPort);
            }
        }
        
        if (opcodeWidth(*fu) > 0) {
            NetlistBlock* fuBlock = nlGenerator.netlistPort(*fu->port(0)).
                parentBlock();
            NetlistPort& opcodePort = nlGenerator.fuOpcodePort(*fuBlock);
            NetlistPort* opcodeCntrlPort = new NetlistPort(
                fuOpcodeCntrlPort(fu->name()),
                Conversion::toString(opcodeWidth(*fu)),
                opcodeWidth(*fu), ProGe::BIT_VECTOR, HDB::OUT, decoder);
            netlist.connectPorts(opcodePort, *opcodeCntrlPort);
        }
    }

    // add RF control ports to decoder and connect them to the RFs
    Machine::RegisterFileNavigator rfNav = 
        machine_.registerFileNavigator();
    for (int i = 0; i < rfNav.count(); i++) {
        RegisterFile* rf = rfNav.item(i);
        for (int i = 0; i < rf->portCount(); i++) {
            RFPort* port = rf->port(i);
            NetlistPort& nlPort = nlGenerator.netlistPort(*port);
            NetlistPort& loadPort = nlGenerator.loadPort(nlPort);

            NetlistPort* loadCntrlPort = new NetlistPort(
                rfLoadCntrlPort(rf->name(), port->name()), 
                loadPort.widthFormula(), 1, ProGe::BIT, HDB::OUT, decoder);

            netlist.connectPorts(loadPort, *loadCntrlPort);

            int opcodeWidth = rfOpcodeWidth(*rf);
            assert(!(!nlGenerator.hasOpcodePort(nlPort) &&
                opcodeWidth > 1));
            if (nlGenerator.hasOpcodePort(nlPort) && opcodeWidth >= 0) {
                NetlistPort& opcodePort = nlGenerator.rfOpcodePort(nlPort);
                NetlistPort* opcodeCntrlPort = new NetlistPort(
                    rfOpcodeCntrlPort(rf->name(), port->name()),
                    Conversion::toString(opcodeWidth), opcodeWidth,
                    ProGe::BIT_VECTOR, HDB::OUT, decoder);
                netlist.connectPorts(opcodePort, *opcodeCntrlPort);
            } 
        }
    }
    
    // add IU control ports to decoder and connect them to the IU's
    Machine::ImmediateUnitNavigator iuNav =
        machine_.immediateUnitNavigator();
    for (int i = 0; i < iuNav.count(); i++) {
        ImmediateUnit* iu = iuNav.item(i);

        // add control ports for read ports
        for (int i = 0; i < iu->portCount(); i++) {
            RFPort* port = iu->port(i);
            NetlistPort& iuDataPort = nlGenerator.netlistPort(*port);
            NetlistPort& loadPort = nlGenerator.loadPort(iuDataPort);
            NetlistPort* loadCntrlPort = new NetlistPort(
                iuReadLoadCntrlPort(iu->name(), port->name()), "1", 1,
                ProGe::BIT, HDB::OUT, decoder);
            netlist.connectPorts(loadPort, *loadCntrlPort);

            int opcodeWidth = rfOpcodeWidth(*iu);
            assert(!(!nlGenerator.hasOpcodePort(iuDataPort) &&
                opcodeWidth > 1));
            if (nlGenerator.hasOpcodePort(iuDataPort) && opcodeWidth >= 0) {
                NetlistPort& opcodePort = nlGenerator.rfOpcodePort(
                    iuDataPort);
                NetlistPort* opcodeCntrlPort = new NetlistPort(
                    iuReadOpcodeCntrlPort(iu->name(), port->name()), 
                    Conversion::toString(opcodeWidth), opcodeWidth, 
                    ProGe::BIT_VECTOR, HDB::OUT, decoder);
                netlist.connectPorts(opcodePort, *opcodeCntrlPort);
            }
        }

        // add IU data write port and control ports
        NetlistPort& iuDataPort = nlGenerator.immediateUnitWritePort(*iu);
        NetlistPort* decIUDataPort = new NetlistPort(
            iuWritePort(iu->name()), Conversion::toString(iu->width()),
            iu->width(), ProGe::BIT_VECTOR, HDB::OUT, decoder);
        netlist.connectPorts(iuDataPort, *decIUDataPort);
        NetlistPort& loadPort = nlGenerator.loadPort(iuDataPort);
        NetlistPort* loadCntrlPort = new NetlistPort(
            iuWriteLoadCntrlPort(iu->name()), "1", 1, ProGe::BIT, HDB::OUT,
            decoder);
        netlist.connectPorts(loadPort, *loadCntrlPort);

        int opcodeWidth = rfOpcodeWidth(*iu);
        if (nlGenerator.hasOpcodePort(iuDataPort) && opcodeWidth >= 0) {
            NetlistPort& opcodePort = nlGenerator.rfOpcodePort(iuDataPort);
            NetlistPort* opcodeCntrlPort = new NetlistPort(
                iuWriteOpcodeCntrlPort(iu->name()), 
                Conversion::toString(opcodeWidth), opcodeWidth,
                ProGe::BIT_VECTOR, HDB::OUT, decoder);
            netlist.connectPorts(opcodePort, *opcodeCntrlPort);
        }
    }
    
    // add guard ports to decoder and connect them to RF's and FU's
    std::set<PortGuard*> generatedPortGuards;
    std::set<RegisterGuard*> generatedRegGuards;
    for (int i = 0; i < busNav.count(); i++) {
        Bus* bus = busNav.item(i);
        for (int i = 0; i < bus->guardCount(); i++) {
            Guard* guard = bus->guard(i);
            PortGuard* portGuard = dynamic_cast<PortGuard*>(guard);
            RegisterGuard* regGuard = dynamic_cast<RegisterGuard*>(guard);
            if (portGuard != NULL) {
                if (containsSimilarGuard(
                        generatedPortGuards, *portGuard)) {
                    continue;
                }
                FUPort* port = portGuard->port();
                NetlistPort& nlPort = nlGenerator.netlistPort(*port);
                NetlistPort& nlGuardPort = nlGenerator.fuGuardPort(
                    nlPort);
                NetlistPort* decGuardPort = new NetlistPort(
                    guardPortName(*guard), "1", 1, ProGe::BIT, HDB::IN,
                    decoder);
                netlist.connectPorts(nlGuardPort, *decGuardPort);
                generatedPortGuards.insert(portGuard);
            } else if (regGuard != NULL) {
                if (containsSimilarGuard(
                        generatedRegGuards, *regGuard)) {
                    continue;
                }
                RegisterFile* rf = regGuard->registerFile();
                assert(rf->portCount() > 0);
                NetlistBlock* nlRf = nlGenerator.netlistPort(
                    *rf->port(0)).parentBlock();
                NetlistPort& nlGuardPort = nlGenerator.rfGuardPort(
                    *nlRf);
                NetlistPort* decGuardPort = new NetlistPort(
                    guardPortName(*guard), "1", 1, ProGe::BIT, HDB::IN,
                    decoder);
                netlist.connectPorts(
                    nlGuardPort, *decGuardPort,
                    regGuard->registerIndex(), 0, 1); 
                generatedRegGuards.insert(regGuard);
            }
        }
    }    

    addLockReqPortToDecoder();
    addGlockPortToDecoder();

    if (generateDebugger_) {
        /*NetlistPort* dbgResetPort = */ new NetlistPort(
            "db_tta_nreset", "1", 1, 
            ProGe::BIT, HDB::IN, decoder);
    }
}


/**
 * Adds the lock request input port to decoder and connects the global lock
 * request ports of FU's to it.
 */
void
DefaultDecoderGenerator::addLockReqPortToDecoder() {

    int lockReqWidth = glockRequestWidth();

    if (lockReqWidth == 0) {
        return;
    }

    Machine::FunctionUnitNavigator fuNav = machine_.functionUnitNavigator();
    Netlist& netlist = decoderBlock_->netlist();

    // add lock request port to decoder
    NetlistPort* lockReqPort = new NetlistPort(
        LOCK_REQ_PORT_NAME, Conversion::toString(lockReqWidth),
        lockReqWidth, ProGe::BIT_VECTOR, HDB::IN, *decoderBlock_);

    // connect the glock request ports of FUs to the lock request port
    int bitToConnect(0);
    for (int i = 0; i < fuNav.count(); i++) {
        FunctionUnit* fu = fuNav.item(i);
        if (fu->portCount() == 0) {
            continue;
        }
        NetlistBlock* fuBlock = 
            nlGenerator_->netlistPort(*fu->port(0)).parentBlock();
        if (nlGenerator_->hasGlockReqPort(*fuBlock)) {
            NetlistPort& glockReqPort = 
                nlGenerator_->glockReqPort(*fuBlock);
            netlist.connectPorts(
                *lockReqPort, glockReqPort, bitToConnect, 0, 1);
            bitToConnect++;
        }
    }
}


/**
 * Adds the global lock port to decoder and connects it to the glock ports
 * of units.
 */
void
DefaultDecoderGenerator::addGlockPortToDecoder() {

    Netlist& netlist = decoderBlock_->netlist();
    NetlistPort* decGlockPort = new NetlistPort(
        GLOCK_PORT_NAME, "1", 1, ProGe::BIT, HDB::OUT, *decoderBlock_);
    
    Machine::FunctionUnitNavigator fuNav = machine_.functionUnitNavigator();
    for (int i = 0; i < fuNav.count(); i++) {
        FunctionUnit* fu = fuNav.item(i);
        if (fu->portCount() == 0) {
            continue;
        }
        NetlistBlock* fuBlock = 
            nlGenerator_->netlistPort(*fu->port(0)).parentBlock();
        if (nlGenerator_->hasGlockPort(*fuBlock)) {
            NetlistPort& glockPort = nlGenerator_->glockPort(*fuBlock);
            netlist.connectPorts(*decGlockPort, glockPort);
        }
    }

    Machine::RegisterFileNavigator rfNav = machine_.registerFileNavigator();
    for (int i = 0; i < rfNav.count(); i++) {
        RegisterFile* rf = rfNav.item(i);
        if (rf->portCount() == 0) {
            continue;
        }
        NetlistBlock* rfBlock = 
            nlGenerator_->netlistPort(*rf->port(0)).parentBlock();
        if (nlGenerator_->hasGlockPort(*rfBlock)) {
            NetlistPort& glockPort = nlGenerator_->glockPort(*rfBlock);
            netlist.connectPorts(*decGlockPort, glockPort);
        }
    }

    Machine::ImmediateUnitNavigator iuNav = machine_.immediateUnitNavigator();
    for (int i = 0; i < iuNav.count(); i++) {
        ImmediateUnit* iu = iuNav.item(i);
        if (iu->portCount() == 0) {
            continue;
        }
        NetlistBlock* iuBlock = 
            nlGenerator_->netlistPort(*iu->port(0)).parentBlock();
        if (nlGenerator_->hasGlockPort(*iuBlock)) {
            NetlistPort& glockPort = nlGenerator_->glockPort(*iuBlock);
            netlist.connectPorts(*decGlockPort, glockPort);
        }
    }

    // If IC requests glock port.
    if (icGenerator_.hasGlockPort()) {
        netlist.connectPorts(*decGlockPort, icGenerator_.glockPort());
    }
}


/**
 * Returns the width of the global lock request port.
 *
 * @return The bit width.
 */
int
DefaultDecoderGenerator::glockRequestWidth() const {

    Machine::FunctionUnitNavigator fuNav = machine_.functionUnitNavigator();

    int lockReqWidth(0);
    for (int i = 0; i < fuNav.count(); i++) {
        FunctionUnit* fu = fuNav.item(i);
        if (fu->portCount() == 0) {
            continue;
        }
        NetlistBlock* fuBlock = 
            nlGenerator_->netlistPort(*fu->port(0)).parentBlock();
        if (nlGenerator_->hasGlockReqPort(*fuBlock)) {
            lockReqWidth++;
        }
    }
    if (generateDebugger_) {
        lockReqWidth++;
    }

    return lockReqWidth;
}


/**
 * Writes the instruction decoder to the given destination directory.
 *
 * @param dstDirectory The destination directory.
 * @exception IOException If an IO error occurs.
 */
void
DefaultDecoderGenerator::generateInstructionDecoder(
    const ProGe::NetlistGenerator& nlGenerator,
    const std::string& dstDirectory)
    throw (IOException) {

    nlGenerator_ = &nlGenerator;
    
    string iDecoderFile = dstDirectory
            + FileSystem::DIRECTORY_SEPARATOR
            + ((language_==Verilog)?"decoder.v":"decoder.vhdl");
    bool decCreated = FileSystem::createFile(iDecoderFile);
    if (!decCreated) {
        string errorMsg = "Unable to create file " + iDecoderFile;
        throw IOException(__FILE__, __LINE__, __func__, errorMsg);
    }        
    std::ofstream decoderStream(
        iDecoderFile.c_str(), std::ofstream::out);
    writeInstructionDecoder(decoderStream);
    decoderStream.close();
}


/**
 * Returns the required latency of the hardware implementation of the given
 * immediate unit.
 *
 * In TCE v1.0 all RFs (and thus IUs) must have latency 1.
 *
 * @param iu The immediate unit.
 */
int
DefaultDecoderGenerator::requiredRFLatency(
    const TTAMachine::ImmediateUnit& /*iu*/) const {

    return 1;
}


/**
 * Verifies that the decoder generator is compatible with the machine.
 *
 * @exception InvalidData If the decoder generator is incompatible.
 */
void
DefaultDecoderGenerator::verifyCompatibility() const
    throw (InvalidData) {

    // check that the GCU does not have other operations than JUMP and CALL.
    ControlUnit* gcu = machine_.controlUnit();
    assert(gcu != NULL);
    for (int i = 0; i < gcu->operationCount(); i++) {
        HWOperation* operation = gcu->operation(i);
        if (!StringTools::ciEqual(operation->name(), JUMP) &&
            !StringTools::ciEqual(operation->name(), CALL)) {
            format errorMsg(
                "Decoder generator does not support operation %1% in GCU.");
            errorMsg % operation->name();
            throw InvalidData(__FILE__, __LINE__, __func__, errorMsg.str());
        }
    }

    // check that there are 3 delay slots in the transport pipeline
    if (gcu->delaySlots() != 3) {
        throw InvalidData(
            __FILE__, __LINE__, __func__,
            TCEString("Decoder generator supports only 4-stage transport ")+
            "pipeline of GCU. Given machine has " + 
            Conversion::toString(gcu->delaySlots()+1) + " stages");
    }

    // check that global guard latency is 1
    if (gcu->globalGuardLatency() != 1) {
        string errorMsg = 
            TCEString("Decoder generator supports only ") +
            "global guard latency of 1. Given machine has " +
            Conversion::toString(gcu->globalGuardLatency());
        throw InvalidData(__FILE__, __LINE__, __func__, errorMsg);
    }
}

/**
 * Controls whenever global lock trace dump process will be generated.
 *
 * @param generate Generate lock trace process if true.
 */
void
DefaultDecoderGenerator::setGenerateLockTrace(bool generate) {
    generateLockTrace_ = generate;
}

/**
 * Sets starting cycle to begin global lock tracing.
 *
 * @param startCycle nth cycle to begin tracing.
 */
void
DefaultDecoderGenerator::setLockTraceStartingCycle(unsigned int startCycle) {
    lockTraceStartingCycle_ = startCycle;
}

/**
 * Writes the instruction decoder to the given stream.
 *
 * @param stream The stream.
 */
void
DefaultDecoderGenerator::writeInstructionDecoder(std::ostream& stream) const {
    if (language_ == VHDL) {
        stream << "library IEEE;" << endl;
        stream << "use IEEE.std_logic_1164.all;" << endl;
        stream << "use IEEE.std_logic_arith.all;" << endl;
        if (generateLockTrace_) {
            stream << "use STD.textio.all;" << endl;
        }
        stream << "use work." << entityNameStr_ << "_globals.all;" << endl;
        stream << "use work." << entityNameStr_ << "_gcu_opcodes.all;" << endl << endl;
        
        string entityName = entityNameStr_ + "_decoder";
        stream << "entity " << entityName << " is" << endl << endl;
        
        // create generic and port declarations
        VHDLNetlistWriter::writeGenericDeclaration(
            *decoderBlock_, 1, indentation(1), stream);
        VHDLNetlistWriter::writePortDeclaration(
            *decoderBlock_, 1, indentation(1), stream);
        
        stream << endl;
        stream << "end " << entityName << ";" << endl << endl;
        string architectureName = "rtl_andor";
        stream << "architecture " << architectureName << " of " << entityName
               << " is" << endl << endl;
        
        writeSourceDestinationAndGuardSignals(stream);
        stream << endl;
        writeImmediateSlotSignals(stream);
        stream << endl;
        writeLongImmediateTagSignal(stream);
        stream << endl;
        writeSquashSignals(stream);
        stream << endl;
        writeSocketCntrlSignals(stream);
        stream << endl;
        writeFUCntrlSignals(stream);
        stream << endl;
        writeRFCntrlSignals(stream);
        
        stream << endl << "begin" << endl << endl;
        
        if (generateLockTrace_) {
            writeLockDumpCode(stream);
            stream << endl;
        }

        writeInstructionDismembering(stream);
        stream << endl;
        writeControlRegisterMappings(stream);
        stream << endl;
        writeSquashSignalGenerationProcesses(stream);
        stream << endl;
        writeLongImmediateWriteProcess(stream);
        stream << endl;
        writeRFSRAMDecodingProcess(stream);
        stream << endl;
        writeMainDecodingProcess(stream);
        stream << endl;
        
        int lockReqWidth = glockRequestWidth();
        stream << indentation(1) << NetlistGenerator::DECODER_LOCK_REQ_OUT_PORT
           << " <= ";
        if (lockReqWidth > 0) {
            for (int i = 0; i < lockReqWidth; i++) {
                stream << LOCK_REQ_PORT_NAME << "(" << i << ")";
                if (i+1 < lockReqWidth) {
                    stream << " or ";
                }
            }
            stream << ";" << endl;
        } else {
          stream << "'0';" << endl;
        }
        
        stream << indentation(1) << GLOCK_PORT_NAME << " <= "
               << NetlistGenerator::DECODER_LOCK_REQ_IN_PORT << ";" << endl
               << endl;
        
        stream << endl << "end " << architectureName << ";" << endl;
    } else { //language_ == Verilog
        const std::string DS = FileSystem::DIRECTORY_SEPARATOR;
        string entityName = entityNameStr_ + "_decoder";
        stream << "`timescale 1ns/1ns" << endl
               << "module " << entityName << endl
               << "#(" << endl
               << "`include \""
               << entityNameStr_ << "_globals_pkg.vh\"" << endl
               << "," << endl
               << "`include \""  << "gcu_opcodes_pkg.vh\"" << endl
               << ")" << endl;
        
        VerilogNetlistWriter::writePortDeclaration(
            *decoderBlock_, 1, indentation(1), stream);

        // create generic and port declarations
        VerilogNetlistWriter::writeGenericDeclaration(
            *decoderBlock_, 1, indentation(1), stream);

        stream << endl;
        
        writeSourceDestinationAndGuardSignals(stream);
        stream << endl;
        writeImmediateSlotSignals(stream);
        stream << endl;
        writeLongImmediateTagSignal(stream);
        stream << endl;
        writeSquashSignals(stream);
        stream << endl;
        writeSocketCntrlSignals(stream);
        stream << endl;
        writeFUCntrlSignals(stream);
        stream << endl;
        writeRFCntrlSignals(stream);
        stream << endl;
        
        if (generateLockTrace_) {
            writeLockDumpCode(stream);
            stream << endl;
        }

        writeInstructionDismembering(stream);
        stream << endl;
        writeControlRegisterMappings(stream);
        stream << endl;
        writeSquashSignalGenerationProcesses(stream);
        stream << endl;
        writeLongImmediateWriteProcess(stream);
        stream << endl;
        writeRFSRAMDecodingProcess(stream);
        stream << endl;
        writeMainDecodingProcess(stream);
        stream << endl;
        
        int lockReqWidth = glockRequestWidth();
        stream << indentation(1) << "assign " << NetlistGenerator::DECODER_LOCK_REQ_OUT_PORT
           << "=";
        if (lockReqWidth > 0) {
            for (int i = 0; i < lockReqWidth; i++) {
                stream << LOCK_REQ_PORT_NAME << "[" << i << "]";
                if (i+1 < lockReqWidth) {
                    stream << " | ";
                }
            }
            stream << ";" << endl;
        } else {
          stream << "1'b0;" << endl;
        }
        
        stream << indentation(1) << "assign " << GLOCK_PORT_NAME << "="
               << NetlistGenerator::DECODER_LOCK_REQ_IN_PORT << ";" << endl
               << endl;
        
        stream << endl << "endmodule" << endl;    
    }
}

/**
 * Writes process that captures state of global lock per clock cycle.
 *
 * The captured contents are dumped into an output file.
 *
 * @param stream The stream to write.
 */
void
DefaultDecoderGenerator::writeLockDumpCode(std::ostream& stream) const {
    if (language_==VHDL){
        stream << indentation(1)
               << "-- Dump the status of global lock into a file once "
                  "in clock cycle"
               << endl
               << indentation(1)
               << "-- setting DUMP false will disable dumping"
               << endl << endl;

        stream << indentation(1)
               << "-- Do not synthesize this process!"
               << endl
               << indentation(1)
               << "-- pragma synthesis_off"
               << endl << endl;

        stream << indentation(1)
               << "file_output : process" << endl;
        stream << indentation(2)
               << "file fileout : text;" << endl << endl
               << indentation(2)
               << "variable lineout : line;" << endl
               << indentation(2)
               << "variable start : boolean := true;" << endl
               << indentation(2)
               << "variable count : integer := 0;" << endl
               << indentation(2)
               << "constant SEPARATOR : string := \" | \";" << endl
               << indentation(2)
               << "constant DUMP : boolean := true;" << endl
               << indentation(2)
               << "constant DUMPFILE : string := \"lock.dump\";" << endl;

        stream << indentation(1)
               << "begin" << endl;

        stream << indentation(2)
               << "if DUMP = true then" << endl;

        stream << indentation(3)
               << "if start = true then" << endl;

        stream << indentation(4)
               << "file_open(fileout, DUMPFILE, write_mode);" << endl
               << indentation(4)
               << "start := false;" << endl;

        stream << indentation(3)
               << "end if;" << endl;

        stream << indentation(3)
               << "wait for PERIOD;" << endl;

        stream << indentation(3)
               << "if count > " << (lockTraceStartingCycle_ - 1)
               << " then" << endl;

        stream << indentation(4)
               << "write(lineout, count-" << lockTraceStartingCycle_
               << ", right, 12);" << endl
               << indentation(4)
               << "write(lineout, SEPARATOR);" << endl
               << indentation(4)
               << "write(lineout, conv_integer(unsigned'(\"\" & "
               << NetlistGenerator::DECODER_LOCK_REQ_IN_PORT
               << ")), right, 12);" << endl
               << indentation(4)
               << "write(lineout, SEPARATOR);" << endl
               << indentation(4)
               << "writeline(fileout, lineout);" << endl;

        stream << indentation(3)
               << "end if;" << endl;
        stream << indentation(3)
               << "count := count + 1;" << endl;

        stream << indentation(2)
               << "end if;" << endl;

        stream << indentation(1)
               << "end process file_output;" << endl;

        stream << indentation(1)
               << "-- pragma synthesis_on"
               << endl;

    } else { // language_==Verilog
        stream << indentation(1)
               << "// Dump the status of global lock into a file once "
               << "in clock cycle"
               << endl
               << indentation(1)
               << "// setting DUMP false will disable dumping"
               << endl << endl
               << indentation(1) << "// Do not synthesize!" << endl
               << indentation(1) << "//synthesis translate_off" << endl
               << indentation(1) << "integer fileout;" << endl << endl
               << indentation(1) << "integer count=0;" << endl << endl
               << indentation(1) << "`define DUMPFILE \"lock.dump\""
               << endl << endl

               << indentation(1) << "initial" << endl
               << indentation(1) << "begin" << endl
               << indentation(2) << "fileout = $fopen(`DUMPFILE,\"w\");"
               << endl
               << indentation(2) << "$fclose(fileout);" << endl
               << indentation(2) << "forever" << endl
               << indentation(2) << "begin" << endl
               << indentation(3) << "#PERIOD;" << endl
               << indentation(3) << "if ( count > "
               << (lockTraceStartingCycle_ - 1) << ")" << endl;

        stream << indentation(3) << "begin" << endl
               << indentation(4) << "fileout = $fopen(`DUMPFILE,\"a\");"
               << endl
               << indentation(4) << "$fwrite(fileout," << "\""
               << " %11d |  %11d | \\n\"" << ", count - "
               << lockTraceStartingCycle_ << ", "
               << NetlistGenerator::DECODER_LOCK_REQ_IN_PORT << ");"
               << endl
               << indentation(4) << "$fclose(fileout);" << endl
               << indentation(3) << "end" << endl
               << indentation(3) << "count = count + 1;" << endl
               << indentation(2) << "end" << endl
               << indentation(1) << "end" << endl
               << indentation(1) << "//synthesis translate_on" << endl;
    }
}

/**
 * Writes the signals for source, destination and guard fields to the
 * given stream.
 *
 * @param stream The stream.
 */
void
DefaultDecoderGenerator::writeSourceDestinationAndGuardSignals(
    std::ostream& stream) const {

    if(language_==VHDL){
        stream << indentation(1) 
               << "-- signals for source, destination and guard fields" 
               << endl;
        for (int i = 0; i < bem_.moveSlotCount(); i++) {
            MoveSlot& slot = bem_.moveSlot(i);
            if (slot.hasSourceField() && slot.sourceField().width() != 0) {
                SourceField& srcField = slot.sourceField();
                stream << indentation(1) << "signal " 
                       << srcFieldSignal(slot.name()) 
                       << " : std_logic_vector(" << srcField.width() - 1 
                       << " downto 0);" << endl;
            }
            if (slot.hasDestinationField() && slot.destinationField().width() != 0) {
                DestinationField& dstField = slot.destinationField();
                stream << indentation(1) << "signal " 
                       << dstFieldSignal(slot.name()) << " : std_logic_vector("
                       << dstField.width() - 1 << " downto 0);" << endl;
            }
            if (slot.hasGuardField()) {
                GuardField& grdField = slot.guardField();
                stream << indentation(1) << "signal " 
                       << guardFieldSignal(slot.name()) 
                       << " : std_logic_vector(" << grdField.width() - 1 
                       << " downto 0);" << endl;
            }
        }
    } else {
        stream << indentation(1) 
               << "// signals for source, destination and guard fields" 
               << endl;
        for (int i = 0; i < bem_.moveSlotCount(); i++) {
            MoveSlot& slot = bem_.moveSlot(i);
            if (slot.hasSourceField() && slot.sourceField().width() != 0) {
                SourceField& srcField = slot.sourceField();
                stream << indentation(1) << "reg[" 
                       << srcField.width() - 1 << ":0] "
                       << srcFieldSignal(slot.name()) << ";"
                       << endl;
            }
            if (slot.hasDestinationField() && slot.destinationField().width() != 0) {
                DestinationField& dstField = slot.destinationField();
                stream << indentation(1) << "reg[" 
                       << dstField.width() - 1 << ":0] "
                       << dstFieldSignal(slot.name()) << ";"
                       << endl;
            }
            if (slot.hasGuardField()) {
                GuardField& grdField = slot.guardField();
                stream << indentation(1) << "reg[" 
                       << grdField.width() - 1 << ":0] "
                       << guardFieldSignal(slot.name()) << ";" 
                       << endl;
            }
        }    
    }
}


/**
 * Writes the signals for dedicated immediate slots to the given stream.
 *
 * @param stream The stream.
 */
void 
DefaultDecoderGenerator::writeImmediateSlotSignals(
    std::ostream& stream) const {
    if(language_==VHDL){
        stream << indentation(1) 
               << "-- signals for dedicated immediate slots" << endl;
        for (int i = 0; i < bem_.immediateSlotCount(); i++) {
            ImmediateSlotField& slot = bem_.immediateSlot(i);
            stream << indentation(1) << "signal " 
                   << immSlotSignal(slot.name()) << " : std_logic_vector(" 
                   << slot.width() - 1 << " downto 0);" << endl;
        }
    } else {
        stream << indentation(1) 
               << "// signals for dedicated immediate slots" << endl;
        for (int i = 0; i < bem_.immediateSlotCount(); i++) {
            ImmediateSlotField& slot = bem_.immediateSlot(i);
            stream << indentation(1) << "reg[" 
                   << slot.width() - 1 << ":0] "
                   << immSlotSignal(slot.name()) << ";"
                   << endl;    
        }
    }
}

    
/**
 * Writes the signal for long immediate tag to the given stream.
 *
 * @param stream The stream.
 */
void 
DefaultDecoderGenerator::writeLongImmediateTagSignal(
    std::ostream& stream) const {
    if(language_==VHDL){
        if (bem_.hasImmediateControlField()) {
            stream << indentation(1) << "-- signal for long immediate tag" 
                   << endl;
            stream << indentation(1) << "signal " << LIMM_TAG_SIGNAL 
                   << " : std_logic_vector(" 
                   << bem_.immediateControlField().width() - 1 
                   << " downto 0);" << endl;
        }
    } else {
        if (bem_.hasImmediateControlField()) {
            stream << indentation(1) << "// signal for long immediate tag" 
                   << endl;
            stream << indentation(1) << "reg["
                   << bem_.immediateControlField().width() - 1 
                   << ":0] "
                   << LIMM_TAG_SIGNAL << ";"
                   << endl;
        }    
    }
}
    

/**
 * Writes the squash signals of guards to the given stream.
 *
 * @param stream The stream.
 */
void
DefaultDecoderGenerator::writeSquashSignals(
    std::ostream& stream) const {

    TCEString comment = language_ == VHDL ? "-- " : "// ";
    Machine::BusNavigator busNav = machine_.busNavigator();
    stream << indentation(1) << comment << "squash signals" << endl;
    for (int i = 0; i < busNav.count(); i++) {
        Bus* bus = busNav.item(i);
        stream << indentation(1);
        if (language_ == VHDL) {
            stream << "signal " << squashSignal(bus->name())
                   << " : std_logic;" << endl;
        } else {
            assert(bem_.hasMoveSlot(bus->name()));
            MoveSlot& slot = bem_.moveSlot(bus->name());
            if (slot.hasGuardField()) {
                stream << "reg ";
            } else {
                // Declare guard signal as wire for constant assignment
                stream << "wire ";
            }
            stream << squashSignal(bus->name()) << ";" << endl;
        }
    }
}


/**
 * Writes the socket control signals to the given stream.
 *
 * @param stream The stream to write.
 */
void
DefaultDecoderGenerator::writeSocketCntrlSignals(
    std::ostream& stream) const {
    if(language_==VHDL){
        stream << indentation(1) << "-- socket control signals" << endl;
        Machine::SocketNavigator socketNav = machine_.socketNavigator();
        for (int i = 0; i < socketNav.count(); i++) {
            Socket* socket = socketNav.item(i);
            if (socket->portCount() == 0 || socket->segmentCount() == 0) {
                continue;
            }

            if (needsBusControl(*socket)) {
                stream << indentation(1) << "signal " 
                       << socketBusCntrlSignalName(
                           socket->name()) << " : std_logic_vector(" 
                       << busControlWidth(*socket) - 1 << " downto 0);" 
                       << endl;
            }
            if (needsDataControl(*socket)) {
                stream << indentation(1) << "signal " 
                       << socketDataCntrlSignalName(socket->name()) 
                       << " : std_logic_vector(" 
                       << dataControlWidth(*socket) - 1 << " downto 0);" 
                       << endl;
            }
        }

        // write signals for short immediate sockets (not visible in ADF)
        Machine::BusNavigator busNav = machine_.busNavigator();
        for (int i = 0; i < busNav.count(); i++) {
            Bus* bus = busNav.item(i);
            if (bus->immediateWidth() > 0) {
                stream << indentation(1) << "signal "
                       << simmDataSignalName(bus->name())
                       << " : std_logic_vector(" << simmPortWidth(*bus) - 1
                       << " downto 0);" << endl;
                stream << indentation(1) << "signal "
                       << simmCntrlSignalName(bus->name())
                       << " : std_logic_vector(0 downto 0);" << endl;
            }
        }
    } else {
        stream << indentation(1) << "// socket control signals" << endl;
        Machine::SocketNavigator socketNav = machine_.socketNavigator();
        for (int i = 0; i < socketNav.count(); i++) {
            Socket* socket = socketNav.item(i);
            if (socket->portCount() == 0 || socket->segmentCount() == 0) {
                continue;
            }

            if (needsBusControl(*socket)) {
                stream << indentation(1) << "reg[" 
                       << busControlWidth(*socket) - 1 << ":0] " 
                       << socketBusCntrlSignalName(socket->name())
                       << ";" << endl;
            }
            if (needsDataControl(*socket)) {
                stream << indentation(1) << "reg[" 
                       << dataControlWidth(*socket) - 1 << ":0] " 
                       << socketDataCntrlSignalName(socket->name()) 
                       << ";" << endl;
            }
        }

        // write signals for short immediate sockets (not visible in ADF)
        Machine::BusNavigator busNav = machine_.busNavigator();
        for (int i = 0; i < busNav.count(); i++) {
            Bus* bus = busNav.item(i);
            if (bus->immediateWidth() > 0) {
                stream << indentation(1) << "reg["
                       << simmPortWidth(*bus) - 1 << ":0] "
                       << simmDataSignalName(bus->name())
                       << ";" << endl;
                       
                stream << indentation(1) << "reg[0:0] "
                       << simmCntrlSignalName(bus->name())
                       << ";" << endl;
            }
        }    
    }
}


/**
 * Writes the FU control signals to the given stream.
 *
 * @param stream The stream.
 */
void
DefaultDecoderGenerator::writeFUCntrlSignals(
    std::ostream& stream) const {

    if(language_==VHDL)
        stream << indentation(1) << "-- FU control signals" << endl;
    else
        stream << indentation(1) << "// FU control signals" << endl;
    
    Machine::FunctionUnitNavigator fuNav = machine_.
        functionUnitNavigator();
    for (int i = 0; i < fuNav.count(); i++) {
        FunctionUnit* fu = fuNav.item(i);
        writeFUCntrlSignals(*fu, stream);
    }

    if (machine_.controlUnit() != NULL) {
        ControlUnit* gcu = machine_.controlUnit();
        writeFUCntrlSignals(*gcu, stream);
    }            
}


/**
 * Writes the control signals of the given FU to the given stream.
 *
 * @param fu The FU.
 * @param stream The stream to write.
 */
void
DefaultDecoderGenerator::writeFUCntrlSignals(
    const TTAMachine::FunctionUnit& fu,
    std::ostream& stream) const {

    for (int i = 0; i < fu.portCount(); i++) {
        BaseFUPort* port = fu.port(i);
        
        if (port->inputSocket() != NULL) {
            // if input port
            if(language_==VHDL){
                stream << indentation(1) << "signal "
                       << fuLoadSignalName(fu.name(), port->name()) 
                       << " : std_logic;" << endl;
            } else {
                stream << indentation(1) << "reg "
                       << fuLoadSignalName(fu.name(), port->name()) 
                       << ";" << endl;
            }
        }
    }
    
    // write opcode signal if the FU needs opcode
    int opcWidth = opcodeWidth(fu);

    // gcu is special case since we have hard code ifetch
    if (dynamic_cast<const ControlUnit*>(&fu) != NULL) {
        opcWidth = 1;
    }
    
    if (opcWidth > 0) {
        if(language_==VHDL){
            stream << indentation(1) << "signal "
                   << fuOpcodeSignalName(fu.name())
                   << " : std_logic_vector(" << opcWidth - 1 << " downto 0);"
                   << endl;
        } else {
            stream << indentation(1) << "reg["
                   << opcWidth - 1 << ":0] "
                   << fuOpcodeSignalName(fu.name())
                   << ";" << endl;       
        }
    }
}   

    
/**
 * Writes the RF control signals to the given stream.
 *
 * @param stream The stream.
 */
void
DefaultDecoderGenerator::writeRFCntrlSignals(
    std::ostream& stream) const {
    if(language_==VHDL)
        stream << indentation(1) << "-- RF control signals" << endl;
    else
        stream << indentation(1) << "// RF control signals" << endl;
    
    Machine::RegisterFileNavigator rfNav = machine_.
        registerFileNavigator();
    for (int i = 0; i < rfNav.count(); i++) {
        RegisterFile* rf = rfNav.item(i);

        for (int i = 0; i < rf->portCount(); i++) {
            RFPort* port = rf->port(i);
            bool async_signal = sacEnabled(rf->name())
                            && port->outputSocket() != NULL;

            // load signal
            if(language_==VHDL){
                stream << indentation(1) << "signal " << rfLoadSignalName(
                rf->name(), port->name(), async_signal) << " : std_logic;"
                << endl;
            } else {
                stream << indentation(1) << "reg " << rfLoadSignalName(
                rf->name(), port->name(), async_signal) << ";" << endl;
            }
                
            // opcode signal
            if (0 < rfOpcodeWidth(*rf)) {
                if(language_==VHDL){
                    stream << indentation(1) << "signal " 
                        << rfOpcodeSignalName(rf->name(), port->name(),
                            async_signal)
                        << " : std_logic_vector(" << rfOpcodeWidth(*rf) - 1 
                        << " downto 0);" << endl;
                } else {
                    stream << indentation(1) << "reg[" 
                        << rfOpcodeWidth(*rf) - 1 << ":0] " 
                        << rfOpcodeSignalName(rf->name(), port->name(),
                            async_signal)
                        << ";" << endl;
                }
            }
        }
    }
    if (language_ == VHDL) {
        // No need for declarations for vhdl.
    } else { // language_ == Verilog
        Machine::ImmediateUnitNavigator iuNav =
            machine_.immediateUnitNavigator();
        for (int i = 0; i < iuNav.count(); i++) {
            ImmediateUnit* iu = iuNav.item(i);
            for (int i = 0; i < iu->portCount(); i++) {
                RFPort* port = iu->port(i);
                if (port->isOutput()) {
                    stream << indentation(1) << "reg " <<
                        iuReadLoadCntrlSignal(iu->name(), port->name())
                           << ";" << endl;

                    if (0 < rfOpcodeWidth(*iu)) {
                        stream << indentation(1) << "reg["
                            << rfOpcodeWidth(*iu) - 1 << ":0] "
                            << iuReadOpcodeCntrlSignal(iu->name(),
                                port->name())
                            << ";" << endl;
                    }
                }
            }
            stream << indentation(1) << "reg[" << iu->width()-1 << ":0] "
                   << iuWriteSignal(iu->name())
                   << ";" << endl;
            stream << indentation(1) << "reg "
                   << iuWriteLoadCntrlSignal(iu->name())
                   << ";" << endl;
            if (0 < rfOpcodeWidth(*iu)) {
                stream << indentation(1) << "reg["
                       << rfOpcodeWidth(*iu) - 1 << ":0] "
                       << iuWriteOpcodeCntrlSignal(iu->name())
                       << ";" << endl;
            }
        }
    } // language_ == Verilog
}


/**
 * Writes dismembering of instruction word to signals to the given stream.
 *
 * @param stream The stream.
 */
void 
DefaultDecoderGenerator::writeInstructionDismembering(
    std::ostream& stream) const {
    if(language_==VHDL){
        stream << indentation(1) << "-- dismembering of instruction" << endl;
        stream << indentation(1) << "process ("
               << NetlistGenerator::DECODER_INSTR_WORD_PORT << ")" << endl;
        stream << indentation(1) << "begin --process" << endl;
            
        for (int i = 0; i < bem_.moveSlotCount(); i++) {
            MoveSlot& slot = bem_.moveSlot(i);
            int slotPosition = slot.bitPosition();
            if (slot.hasSourceField() && slot.sourceField().width() != 0) {
                SourceField& srcField = slot.sourceField();
                stream << indentation(2) << srcFieldSignal(slot.name()) 
                       << " <= " << NetlistGenerator::DECODER_INSTR_WORD_PORT
                       << "(" << slotPosition + srcField.bitPosition() + 
                    srcField.width() - 1 << " downto " 
                       << slotPosition + srcField.bitPosition() << ");" 
                       << endl;
            }
            if (slot.hasDestinationField() && slot.destinationField().width() != 0) {
                DestinationField& dstField = slot.destinationField();
                stream << indentation(2) << dstFieldSignal(slot.name()) 
                       << " <= " << NetlistGenerator::DECODER_INSTR_WORD_PORT
                       << "(" << slotPosition + dstField.bitPosition() + 
                    dstField.width() - 1 << " downto " 
                       << slotPosition + dstField.bitPosition() << ");" 
                       << endl;
            }
            if (slot.hasGuardField()) {
                GuardField& grdField = slot.guardField();
                stream << indentation(2) << guardFieldSignal(slot.name()) 
                       << " <= " << NetlistGenerator::DECODER_INSTR_WORD_PORT
                       << "(" << slotPosition + grdField.bitPosition() + 
                    grdField.width() - 1 << " downto " 
                       << slotPosition + grdField.bitPosition() << ");" 
                       << endl;
            }
        }
        stream << endl;
        for (int i = 0; i < bem_.immediateSlotCount(); i++) {
            ImmediateSlotField& slot = bem_.immediateSlot(i);
            stream << indentation(2) << immSlotSignal(slot.name()) << " <= "
                   << NetlistGenerator::DECODER_INSTR_WORD_PORT << "(" 
                   << slot.bitPosition() + slot.width() - 1 << " downto " 
                   << slot.bitPosition() << ");" << endl;
        }
        if (bem_.hasImmediateControlField()) {
            ImmediateControlField& icField = bem_.immediateControlField();
            stream << indentation(2) << LIMM_TAG_SIGNAL << " <= " 
                   << NetlistGenerator::DECODER_INSTR_WORD_PORT << "(" 
                   << icField.bitPosition() + icField.width() - 1 
                   << " downto " << icField.bitPosition() << ");" << endl;
        }
        stream << indentation(1) << "end process;" << endl;
    } else {
        stream << indentation(1) << "// dismembering of instruction" << endl;
        stream << indentation(1) << "always@(*)" << endl;
        stream << indentation(1) << "begin //process" << endl;
            
        for (int i = 0; i < bem_.moveSlotCount(); i++) {
            MoveSlot& slot = bem_.moveSlot(i);
            int slotPosition = slot.bitPosition();
            if (slot.hasSourceField() && slot.sourceField().width() != 0) {
                SourceField& srcField = slot.sourceField();
                stream << indentation(2) << srcFieldSignal(slot.name()) 
                       << " = " << NetlistGenerator::DECODER_INSTR_WORD_PORT
                       << "[" << slotPosition + srcField.bitPosition() + 
                    srcField.width() - 1 << " : " 
                       << slotPosition + srcField.bitPosition() << "];" 
                       << endl;
            }
            if (slot.hasDestinationField() && slot.destinationField().width() != 0) {
                DestinationField& dstField = slot.destinationField();
                stream << indentation(2) << dstFieldSignal(slot.name()) 
                       << " = " << NetlistGenerator::DECODER_INSTR_WORD_PORT
                       << "[" << slotPosition + dstField.bitPosition() + 
                    dstField.width() - 1 << " : " 
                       << slotPosition + dstField.bitPosition() << "];" 
                       << endl;
            }
            if (slot.hasGuardField()) {
                GuardField& grdField = slot.guardField();
                stream << indentation(2) << guardFieldSignal(slot.name()) 
                       << " = " << NetlistGenerator::DECODER_INSTR_WORD_PORT
                       << "[" << slotPosition + grdField.bitPosition() + 
                    grdField.width() - 1 << " : " 
                       << slotPosition + grdField.bitPosition() << "];" 
                       << endl;
            }
        }
        stream << endl;
        for (int i = 0; i < bem_.immediateSlotCount(); i++) {
            ImmediateSlotField& slot = bem_.immediateSlot(i);
            stream << indentation(2) << immSlotSignal(slot.name()) << " = "
                   << NetlistGenerator::DECODER_INSTR_WORD_PORT << "[" 
                   << slot.bitPosition() + slot.width() - 1 << " : " 
                   << slot.bitPosition() << "];" << endl;
        }
        if (bem_.hasImmediateControlField()) {
            ImmediateControlField& icField = bem_.immediateControlField();
            stream << indentation(2) << LIMM_TAG_SIGNAL << " = " 
                   << NetlistGenerator::DECODER_INSTR_WORD_PORT << "[" 
                   << icField.bitPosition() + icField.width() - 1 
                   << " : " << icField.bitPosition() << "];" << endl;
        }
        stream << indentation(1) << "end" << endl;    
    }
}


/**
 * Writes the generation processes of squash signals to the given stream.
 *
 * @param stream The stream.
 */
void
DefaultDecoderGenerator::writeSquashSignalGenerationProcesses(
    std::ostream& stream) const {

    Machine::BusNavigator busNav = machine_.busNavigator();
    for (int i = 0; i < busNav.count(); i++) {
        Bus* bus = busNav.item(i);
        writeSquashSignalGenerationProcess(*bus, stream);
    }
}


/**
 * Writes the generation process of squash signal for the given bus.
 *
 * @param bus The bus.
 * @param stream The stream to write.
 */
void 
DefaultDecoderGenerator::writeSquashSignalGenerationProcess(
    const TTAMachine::Bus& bus,
    std::ostream& stream) const {
    if(language_==VHDL){
        assert(bem_.hasMoveSlot(bus.name()));
        MoveSlot& slot = bem_.moveSlot(bus.name());
        if (slot.hasGuardField()) {
            GuardField& grdField = slot.guardField();
            stream << indentation(1) << "-- generate signal " 
                   << squashSignal(slot.name()) << endl;
            stream << indentation(1) << "process (";
            std::set<string> guardPorts;
            for (int i = 0; i < bus.guardCount(); i++) {
                Guard* guard = bus.guard(i);
                string guardPort = guardPortName(*guard);
                if (guardPort != "" &&
                    !AssocTools::containsKey(guardPorts, guardPort)) {
                    stream << guardPortName(*guard) << ", ";
                    guardPorts.insert(guardPort);
                }
            }
            stream << guardFieldSignal(slot.name());

            std::set<InstructionTemplate*> affectingInstTemplates =
                MachineInfo::templatesUsingSlot(machine_, bus.name());

            if (affectingInstTemplates.size() > 0) {
                stream << ", " << LIMM_TAG_SIGNAL;
            }

            stream << ")" << endl;
            stream << indentation(2) << "variable sel : integer;" << endl;
            stream << indentation(1) << "begin --process" << endl;
            int indLevel = 2;
            if (affectingInstTemplates.size() > 0) {
                stream << indentation(indLevel) << "if (";
                for (set<InstructionTemplate*>::const_iterator iter = 
                         affectingInstTemplates.begin();
                     iter != affectingInstTemplates.end(); iter++) {
                    if (iter != affectingInstTemplates.begin()) {
                        stream << " or ";
                    }
                    ImmediateControlField& icField = 
                        bem_.immediateControlField();
                    InstructionTemplate* affectingTemp = *iter;
                    stream << "conv_integer(unsigned(" << LIMM_TAG_SIGNAL
                           << ")) = "
                           << icField.templateEncoding(affectingTemp->name());
                }
                stream << ") then" << endl;
                stream << indentation(indLevel+1) << squashSignal(bus.name())
                       << " <= '1';" << endl;
                stream << indentation(indLevel) << "else" << endl;
                indLevel++;
            }
            stream << indentation(indLevel) << "sel := conv_integer(unsigned(" 
                   << guardFieldSignal(slot.name()) << "));" << endl;
            stream << indentation(indLevel) << "case sel is" << endl;
            indLevel++;
            for (int i = 0; i < grdField.gprGuardEncodingCount(); i++) {
                GPRGuardEncoding& enc = grdField.gprGuardEncoding(i);
                RegisterGuard& regGuard = findGuard(enc);
                writeSquashSignalSubstitution(VHDL,
                    bus, enc, regGuard, stream, indLevel);
            }

            for (int i = 0; i < grdField.fuGuardEncodingCount(); i++) {
                FUGuardEncoding& enc = grdField.fuGuardEncoding(i);
                PortGuard& portGuard = findGuard(enc);
                writeSquashSignalSubstitution(VHDL,
                    bus, enc, portGuard, stream, indLevel);
            }

            if (grdField.hasUnconditionalGuardEncoding(true)) {
                UnconditionalGuardEncoding& enc = 
                    grdField.unconditionalGuardEncoding(true);
                stream << indentation(indLevel) << "when " << enc.encoding()
                       << " => "
                       << endl;
                stream << indentation(indLevel+1) << squashSignal(slot.name()) 
                       << " <= '1';" << endl;
            }

            stream << indentation(indLevel) << "when others =>" << endl;
            stream << indentation(indLevel+1) << squashSignal(slot.name()) 
                   << " <= '0';" << endl;
            stream << indentation(indLevel-1) << "end case;" << endl;
        
            if (affectingInstTemplates.size() > 0) {
                stream << indentation(2) << "end if;" << endl;
            }
            stream << indentation(1) << "end process;" << endl << endl;
        } else if (MachineInfo::templatesUsesSlot(machine_, bus.name())) {
            // In absence of guards generate squash signal if
            // slot is affected by any long immediate instruction template.
            stream << indentation(1) << "-- generate signal "
                   << squashSignal(slot.name()) << endl;
            stream << indentation(1) << "process (";

            std::set<InstructionTemplate*> affectingInstTemplates =
                MachineInfo::templatesUsingSlot(machine_, bus.name());

            stream << LIMM_TAG_SIGNAL;
            stream << ")" << endl;
            stream << indentation(1) << "begin --process" << endl;
            int indLevel = 2;
            if (affectingInstTemplates.size() > 0) {
                stream << indentation(indLevel) << "if (";
                for (set<InstructionTemplate*>::const_iterator iter =
                         affectingInstTemplates.begin();
                     iter != affectingInstTemplates.end(); iter++) {
                    if (iter != affectingInstTemplates.begin()) {
                        stream << " or ";
                    }
                    ImmediateControlField& icField =
                        bem_.immediateControlField();
                    InstructionTemplate* affectingTemp = *iter;
                    stream << "conv_integer(unsigned(" << LIMM_TAG_SIGNAL
                           << ")) = "
                           << icField.templateEncoding(affectingTemp->name());
                }
                stream << ") then" << endl;
                stream << indentation(indLevel+1) << squashSignal(bus.name())
                       << " <= '1';" << endl;
                stream << indentation(2) << "else" << endl;
                stream << indentation(indLevel+1) << squashSignal(bus.name())
                       << " <= '0';" << endl;
                stream << indentation(2) << "end if;" << endl;
                stream << indentation(1) << "end process;" << endl << endl;
            }
        } else {
            // the bus contains always true guard so squash has static value
            // Synthesis software should optimize it away
            string squashName = squashSignal(bus.name());
            stream << indentation(1) << "-- generate signal " << squashName
                   << endl;
            stream << indentation(1) << squashName << " <= '0';" << endl;
        }
    } else {
        assert(bem_.hasMoveSlot(bus.name()));
        MoveSlot& slot = bem_.moveSlot(bus.name());
        if (slot.hasGuardField()) {
            GuardField& grdField = slot.guardField();
            stream << indentation(1) << "// generate signal "
                   << squashSignal(slot.name()) << endl;
            stream << indentation(1) << "always@(";
            std::set<string> guardPorts;
            for (int i = 0; i < bus.guardCount(); i++) {
                Guard* guard = bus.guard(i);
                string guardPort = guardPortName(*guard);
                if (guardPort != "" &&
                    !AssocTools::containsKey(guardPorts, guardPort)) {
                    stream << guardPortName(*guard) << ", ";
                    guardPorts.insert(guardPort);
                }
            }
            stream << guardFieldSignal(slot.name());

            std::set<InstructionTemplate*> affectingInstTemplates;
            Machine::InstructionTemplateNavigator itNav = 
                machine_.instructionTemplateNavigator();
            for (int i = 0; i < itNav.count(); i++) {
                InstructionTemplate* iTemp = itNav.item(i);
                if (iTemp->usesSlot(bus.name())) {
                    affectingInstTemplates.insert(iTemp);
                }
            }

            if (affectingInstTemplates.size() > 0) {
                stream << ", " << LIMM_TAG_SIGNAL;
            }

            stream << ")" << endl;
            stream << indentation(1) << "begin" << endl;
            int indLevel = 2;
            if (affectingInstTemplates.size() > 0) {
                stream << indentation(indLevel) << "if (";
                for (set<InstructionTemplate*>::const_iterator iter = 
                         affectingInstTemplates.begin();
                     iter != affectingInstTemplates.end(); iter++) {
                    if (iter != affectingInstTemplates.begin()) {
                        stream << " || ";
                    }
                    ImmediateControlField& icField = 
                        bem_.immediateControlField();
                    InstructionTemplate* affectingTemp = *iter;
                    stream <<  LIMM_TAG_SIGNAL
                           << " == "
                           << icField.templateEncoding(affectingTemp->name());
                }
                stream << ")" << endl;
                stream << indentation(indLevel+1) << squashSignal(bus.name())
                       << " <= 1'b1;" << endl;
                stream << indentation(indLevel) << "else" << endl;
                indLevel++;
            }
            stream << indentation(indLevel) << "case("
                   << guardFieldSignal(slot.name()) << ")" << endl;
            indLevel++;
            for (int i = 0; i < grdField.gprGuardEncodingCount(); i++) {
                GPRGuardEncoding& enc = grdField.gprGuardEncoding(i);
                RegisterGuard& regGuard = findGuard(enc);
                writeSquashSignalSubstitution(Verilog,
                    bus, enc, regGuard, stream, indLevel);
            }

            for (int i = 0; i < grdField.fuGuardEncodingCount(); i++) {
                FUGuardEncoding& enc = grdField.fuGuardEncoding(i);
                PortGuard& portGuard = findGuard(enc);
                writeSquashSignalSubstitution(Verilog,
                    bus, enc, portGuard, stream, indLevel);
            }

            if (grdField.hasUnconditionalGuardEncoding(true)) {
                UnconditionalGuardEncoding& enc = 
                    grdField.unconditionalGuardEncoding(true);
                stream << indentation(indLevel) << enc.encoding() << " :"
                       << endl;
                stream << indentation(indLevel+1) << squashSignal(slot.name()) 
                       << " <= 1'b1;" << endl;
            }

            stream << indentation(indLevel) << "default:" << endl;
            stream << indentation(indLevel+1) << squashSignal(slot.name()) 
                   << " <= 1'b0;" << endl;
            stream << indentation(indLevel-1) << "endcase" << endl;
            stream << indentation(1) << "end" << endl << endl;
        } else {
            // the bus contains always true guard so squash has static value
            // Synthesis software should optimize it away
            string squashName = squashSignal(bus.name());
            stream << indentation(1) << "// generate signal " << squashName
                   << endl;
            stream << indentation(1) << "assign " << squashName
                   << " = 1'b0;" << endl;
        }
    }
}


/**
 * Writes the process that writes long immediates to immediate units.
 *
 * @param stream The stream to write.
 */
void
DefaultDecoderGenerator::writeLongImmediateWriteProcess(
    std::ostream& stream) const {
    
    Machine::InstructionTemplateNavigator itNav = 
        machine_.instructionTemplateNavigator();
    if (itNav.count() == 0 || (itNav.count() == 1 &&
        itNav.item(0)->isEmpty())) {
        return;
    }

    string resetPort = NetlistGenerator::DECODER_RESET_PORT;
    string clockPort = NetlistGenerator::DECODER_CLOCK_PORT;
    if (language_ == VHDL) {
        stream << indentation(1) << "--long immediate write process" << endl;
        stream << indentation(1) << "process ("
               << clockPort << ", "  << resetPort << ")" << endl;
        stream << indentation(1) << "begin --process" << endl;
        
        // reset
        stream << indentation(2) << "if (" << resetPort << " = '0') then" 
               << endl;
        Machine::ImmediateUnitNavigator iuNav = 
            machine_.immediateUnitNavigator();

        for (int i = 0; i < iuNav.count(); i++) {
            ImmediateUnit* iu = iuNav.item(i);
            stream << indentation(3) << iuWriteLoadCntrlPort(iu->name())
                   << " <= '0';" << endl;
            stream << indentation(3) << iuWritePort(iu->name()) 
                   << " <= (others => '0');" << endl;
            if (rfOpcodeWidth(*iu) != 0)
            stream << indentation(3) << iuWriteOpcodeCntrlPort(iu->name())
                   << " <= (others => '0');" << endl;
        }
        // else
        stream << indentation(2)  << "elsif (clk'event and clk = '1') then" << endl;
        // global lock test
        stream << indentation(3)  << "if lock = '0' then" << endl;
        for (int i = 0; i < itNav.count(); i++) {
            InstructionTemplate* iTemp = itNav.item(i);
            int indLevel = 4;
            if (bem_.hasImmediateControlField()) {
                indLevel = 5;
                if (i == 0) {
                    stream << indentation(4) << "if ("
                           << instructionTemplateCondition(VHDL, iTemp->name())
                           << ") then" << endl;
                } else if (i+1 < itNav.count()) {
                    stream << indentation(4) << "elsif ("
                           << instructionTemplateCondition(VHDL, iTemp->name())
                           << ") then" << endl;
                } else {
                    stream << indentation(4) << "else" << endl;
                }
            }
            writeInstructionTemplateProcedures(VHDL, *iTemp, indLevel, stream);
        }

        if (bem_.hasImmediateControlField()) {
            stream << indentation(4) << "end if;" << endl;
        }
        // global lock test endif
        stream << indentation(3) << "end if;" << endl;
        // reset endif
        stream << indentation(2) << "end if;" << endl;
        stream << indentation(1) << "end process;" << endl;
    } else { // language_ == Verilog
        stream << indentation(1) << "//long immediate write process" << endl
               << indentation(1) << "always@(posedge "
               << clockPort << " or negedge "  << resetPort << ")" << endl
                // reset
               << indentation(2) << "if (" << resetPort << " == 0)"
               << endl
               << indentation(2) << "begin" << endl;
        Machine::ImmediateUnitNavigator iuNav = 
            machine_.immediateUnitNavigator();

        for (int i = 0; i < iuNav.count(); i++) {
            ImmediateUnit* iu = iuNav.item(i);
            stream << indentation(3) << iuWriteLoadCntrlSignal(iu->name())
                   << " <= 1'b0;" << endl
                   << indentation(3) << iuWriteSignal(iu->name())
                   << " <= 0;" << endl;
            if (rfOpcodeWidth(*iu) != 0)
            stream << indentation(3) << iuWriteOpcodeCntrlSignal(iu->name())
                   << " <= 0;" << endl;
        }
        stream  << indentation(2) << "end" << endl
                << indentation(2) << "else" << endl
                << indentation(2) << "begin" << endl
                << indentation(3) << "if (lock == 0)" << endl
                << indentation(3) << "begin" << endl;
        for (int i = 0; i < itNav.count(); i++) {
            InstructionTemplate* iTemp = itNav.item(i);
            int indLevel = 4;
            if (bem_.hasImmediateControlField()) {
                indLevel = 5;
                if (i == 0) {
                    stream << indentation(4) << "if ("
                           << instructionTemplateCondition(
                               Verilog, iTemp->name())
                           << ")" << endl;
                } else if (i+1 < itNav.count()) {
                    stream << indentation(4) << "else if ("
                           << instructionTemplateCondition(
                               Verilog, iTemp->name())
                           << ")" << endl;
                } else {
                    stream << indentation(4) << "else" << endl;
                }
            }
            stream << indentation(4) << "begin" << endl;
            writeInstructionTemplateProcedures(
                Verilog, *iTemp, indLevel, stream);
            stream << indentation(4) << "end" << endl;
        }
        stream << indentation(3) << "end" << endl
               << indentation(2) << "end" << endl;
    }
}


/**
 * Writes the procedures required if the instruction is of the given
 * instruction template.
 *
 * @param iTemp The instruction template.
 * @param indLevel The indentation level.
 * @param stream The stream to write.
 */
void
DefaultDecoderGenerator::writeInstructionTemplateProcedures(
    const ProGe::HDL language,
    const TTAMachine::InstructionTemplate& iTemp,
    int indLevel,
    std::ostream& stream) const {

    Machine::ImmediateUnitNavigator iuNav = 
        machine_.immediateUnitNavigator();
    if (language == VHDL) {
        if (iTemp.isEmpty()) {
            for (int i = 0; i < iuNav.count(); i++) {
                ImmediateUnit* iu = iuNav.item(i);
                stream << indentation(indLevel)
                       << iuWriteLoadCntrlPort(iu->name()) << " <= '0';" << endl;

                stream << indentation(indLevel)
                       << iuWritePort(iu->name())
                       << "(" << (iu->width() - 1) << " downto 0"
                       << ") <= sxt(\"0\", " << iu->width() << ");" << endl;
            }
        } else {
            for (int i = 0; i < iuNav.count(); i++) {
                ImmediateUnit* iu = iuNav.item(i);
                if (iTemp.isOneOfDestinations(*iu)) {
                    int msb = iu->width() - 1; 
                    int lsb = iTemp.supportedWidth(*iu) - 
                        iTemp.supportedWidth(iTemp.slotOfDestination(*iu, 0));
                    for (int j = 0; j < iTemp.numberOfSlots(*iu); j++) {
                        string slot = iTemp.slotOfDestination(*iu, j);
                        if (j != 0) {
                            msb = lsb-1;
                            lsb = msb - iTemp.supportedWidth(slot) + 1;
                        }
                        
                        int immPartWidth = msb - lsb + 1;
                        stream << indentation(indLevel)
                               << iuWritePort(iu->name())
                               << "(" << msb << " downto " << lsb << ") <= ";
                        if (j == 0) {
                            if (iu->extensionMode() == Machine::SIGN) {
                                stream << "sxt(";
                            } else {
                                stream << "ext(";
                            }
                        }

                        if (machine_.busNavigator().hasItem(slot)) {
                            MoveSlot& mSlot = bem_.moveSlot(slot);
                            stream << NetlistGenerator::DECODER_INSTR_WORD_PORT
                                   << "(" << mSlot.bitPosition() + 
                                iTemp.supportedWidth(slot) - 1
                                   << " downto " << mSlot.bitPosition() << ")";
                        } else {
                            ImmediateSlotField& iSlot = 
                                bem_.immediateSlot(slot);
                            stream << NetlistGenerator::DECODER_INSTR_WORD_PORT
                                   << "(" << iSlot.bitPosition() + 
                                iTemp.supportedWidth(slot) - 1
                                   << " downto " << iSlot.bitPosition() << ")";
                        }

                        if (j == 0) {
                            stream << ", " << immPartWidth << ");" << endl;
                        } else {
                            stream << ";" << endl;
                        }

                    }
                    if (iu->numberOfRegisters() > 1) {
                        LImmDstRegisterField& field = 
                            bem_.longImmDstRegisterField(
                            iTemp.name(), iu->name());
                        stream << indentation(indLevel) 
                               << iuWriteOpcodeCntrlPort(iu->name()) << " <= "
                               << "ext("
                               << NetlistGenerator::DECODER_INSTR_WORD_PORT
                               << "("
                               << field.bitPosition() + rfOpcodeWidth(*iu) - 1
                               << " downto " << field.bitPosition() << "), "
                               << iuWriteOpcodeCntrlPort(iu->name())
                               << "'length);" << endl;
                    }
                    stream << indentation(indLevel)
                           << iuWriteLoadCntrlPort(iu->name()) << " <= '1';"
                           << endl;
                } else {
                    stream << indentation(indLevel)
                           << iuWriteLoadCntrlPort(iu->name()) << " <= '0';"
                           << endl;
                }
            }
        }
    } else { // language == Verilog
        if (iTemp.isEmpty()) {
            for (int i = 0; i < iuNav.count(); i++) {
                ImmediateUnit* iu = iuNav.item(i);
                stream << indentation(indLevel)
                       << iuWriteLoadCntrlSignal(iu->name())
                       << " <= 1'b0;" << endl;

                stream << indentation(indLevel)
                       << iuWriteSignal(iu->name())
                       << "[" << (iu->width() - 1) << " : 0"
                       << "] <= {" << iu->width() <<"{1'b0}};" << endl;
            }
        } else {
            for (int i = 0; i < iuNav.count(); i++) {
                ImmediateUnit* iu = iuNav.item(i);
                if (iTemp.isOneOfDestinations(*iu)) {
                    int msb = iu->width() - 1; 
                    int lsb = iTemp.supportedWidth(*iu) - 
                        iTemp.supportedWidth(iTemp.slotOfDestination(*iu, 0));
                    for (int j = 0; j < iTemp.numberOfSlots(*iu); j++) {
                        string slot = iTemp.slotOfDestination(*iu, j);
                        if (j != 0) {
                            msb = lsb-1;
                            lsb = msb - iTemp.supportedWidth(slot) + 1;
                        }

                        stream << indentation(indLevel)
                               << iuWriteSignal(iu->name())
                               << "[" << msb << " : " << lsb << "] <= ";
                        if (j == 0) {
                            if (iu->extensionMode() == Machine::SIGN) {
                                stream << "$signed(";
                            } else {
                                stream << "$unsigned(";
                            }
                        }

                        if (machine_.busNavigator().hasItem(slot)) {
                            MoveSlot& mSlot = bem_.moveSlot(slot);
                            stream << NetlistGenerator::DECODER_INSTR_WORD_PORT
                                   << "[" << mSlot.bitPosition() + 
                                iTemp.supportedWidth(slot) - 1
                                   << " : " << mSlot.bitPosition() << "]";
                        } else {
                            ImmediateSlotField& iSlot = 
                                bem_.immediateSlot(slot);
                            stream << NetlistGenerator::DECODER_INSTR_WORD_PORT
                                   << "[" << iSlot.bitPosition() + 
                                iTemp.supportedWidth(slot) - 1
                                   << " : " << iSlot.bitPosition() << "]";
                        }

                        if (j == 0) {
                            stream << ");" << endl;
                        } else {
                            stream << ";" << endl;
                        }
                    }
                    if (iu->numberOfRegisters() > 1) {
                        LImmDstRegisterField& field = 
                            bem_.longImmDstRegisterField(
                            iTemp.name(), iu->name());
                        stream << indentation(indLevel) 
                               << iuWriteOpcodeCntrlSignal(iu->name())
                               << " <= " << "$unsigned("
                               << NetlistGenerator::DECODER_INSTR_WORD_PORT
                               << "["
                               << field.bitPosition() + rfOpcodeWidth(*iu) - 1
                               << " : " << field.bitPosition() << "]);"
                               << endl;
                    }
                    stream << indentation(indLevel)
                           << iuWriteLoadCntrlSignal(iu->name()) << " <= 1'b1;"
                           << endl;
                } else {
                    stream << indentation(indLevel)
                           << iuWriteLoadCntrlSignal(iu->name()) << " <= 1'b0;"
                           << endl;
                }
            }
        }
    }
}

/**
 * Writes separate combinational decoding process for SRAM register files.
 *
 * @param stream The stream to write.
 */
void
DefaultDecoderGenerator::writeRFSRAMDecodingProcess(std::ostream& stream)
const {
    
    set<const RegisterFile*> sramRFset;
    set<const RegisterFile*>::const_iterator sramrf_it;

    // Write only when there is SRAM RFs.
    bool hasSramRFs = false;
    const Machine::RegisterFileNavigator& rfNav =
                    machine_.registerFileNavigator();
    for (int i = 0; i < rfNav.count(); i++) {
        if(sacEnabled(rfNav.item(i)->name())) {
            hasSramRFs = true;
            sramRFset.insert(rfNav.item(i));
        }
    }

    if(!hasSramRFs) {
        return;
    }

    if (language_ == VHDL) {
        string resetPort = NetlistGenerator::DECODER_RESET_PORT;

        // Begin process //
        stream << indentation(1)
               << "-- separate SRAM RF read decoding process" << endl;
        stream << indentation(1) << "process (" << resetPort;

        // Sensitivity list //
        BusSet connectedToSramRFs;
        for (sramrf_it = sramRFset.begin(); sramrf_it != sramRFset.end();
                        sramrf_it++) {
            const RegisterFile& rf = **sramrf_it;
            assert(sacEnabled(rf.name()));
            for (int ip = 0; ip < rf.portCount(); ip++) {
                const RFPort& port = *rf.port(ip);
                if (port.outputSocket() != NULL) {
                    BusSet tmp = connectedBuses(*port.outputSocket());
                    connectedToSramRFs.insert(tmp.begin(), tmp.end());
                }
            }
        }

        BusSet::const_iterator busSet_it;
        for (busSet_it = connectedToSramRFs.begin();
             busSet_it != connectedToSramRFs.end();
             busSet_it++) {
            string busName = (*busSet_it)->name();
            stream << ", " << srcFieldSignal(busName)
                   << ", " << squashSignal(busName);
        }
        stream << ")" << endl;
        stream << indentation(1) << "begin" << endl;

        // Signal resets //
        stream << indentation(2) << "if (" << resetPort << " = '0') then"
               << endl;
        for (sramrf_it = sramRFset.begin(); sramrf_it != sramRFset.end();
                        sramrf_it++)  {
            const RegisterFile& rf = **sramrf_it;
            assert(sacEnabled(rf.name()));
            for (int i = 0; i < rf.portCount(); i++) {
                const RFPort& port = *rf.port(i);
                if (port.outputSocket() == NULL) {
                    continue;
                }

                stream << indentation(3)
                       << rfLoadSignalName(rf.name(), port.name(), true)
                       << " <= '0';" << endl;
                if (0 < rfOpcodeWidth(rf)) {
                    stream << indentation(3)
                           << rfOpcodeSignalName(rf.name(), port.name(), true)
                           << " <= (others => '0');" << endl;
                }
            }
        }

        // Write decoding rules //
        stream << endl << indentation(2) << "else" << endl;
        for (sramrf_it = sramRFset.begin(); sramrf_it != sramRFset.end();
                                sramrf_it++)  {
            const RegisterFile& rf = **sramrf_it;
            assert(sacEnabled(rf.name()));
            for(int i = 0; i < rf.portCount(); i++) {
                const RFPort& port = *rf.port(i);
                if (port.outputSocket() != NULL) {
                    writeControlRulesOfRFReadPort(port, stream);
                }
            }
        }
        stream << indentation(2) << "end if;" << endl;

        // End process //
        stream << indentation(1) << "end process;" << endl;

    } else { // language_ == VERILOG
        // Begin process //
        string resetPort = NetlistGenerator::DECODER_RESET_PORT;

        stream << indentation(1)
               << "// separate SRAM RF read decoding process" << endl;
        stream << indentation(1)
               << "always@(";

        // Sensitivity list //
        BusSet connectedToSramRFs;
        for (sramrf_it = sramRFset.begin(); sramrf_it != sramRFset.end();
                        sramrf_it++) {
            const RegisterFile& rf = **sramrf_it;
            assert(sacEnabled(rf.name()));
            for (int ip = 0; ip < rf.portCount(); ip++) {
                const RFPort& port = *rf.port(ip);
                if (port.outputSocket() != NULL) {
                    BusSet tmp = connectedBuses(*port.outputSocket());
                    connectedToSramRFs.insert(tmp.begin(), tmp.end());
                }
            }
        }

        BusSet::const_iterator busSet_it;
        for (busSet_it = connectedToSramRFs.begin();
             busSet_it != connectedToSramRFs.end();
             busSet_it++) {
            string busName = (*busSet_it)->name();
            stream << ", " << srcFieldSignal(busName)
                   << ", " << squashSignal(busName);
        }
        stream << ")" << endl;
        stream << indentation(1) << "begin" << endl;

        // Signal resets //
        stream << indentation(2) << "if (" << resetPort << " == 0)" << endl
               << indentation(2) << "begin" << endl;

        for (sramrf_it = sramRFset.begin(); sramrf_it != sramRFset.end();
                        sramrf_it++)  {
            const RegisterFile& rf = **sramrf_it;
            assert(sacEnabled(rf.name()));
            for (int i = 0; i < rf.portCount(); i++) {
                const RFPort& port = *rf.port(i);
                if (port.outputSocket() == NULL) {
                    continue;
                }

                stream << indentation(3)
                       << rfLoadSignalName(rf.name(), port.name(), true)
                       << " <= 1'b0;" << endl;
                if (0 < rfOpcodeWidth(rf)) {
                    stream << indentation(3)
                           << rfOpcodeSignalName(rf.name(), port.name(), true)
                           << " <= 0;" << endl;
                }
            }
        }
        stream << indentation(2) << "end" << endl;

        // Decoding rules //
        stream << indentation(2) << "else" << endl;
        stream << indentation(2) << "begin" << endl;
        for (sramrf_it = sramRFset.begin(); sramrf_it != sramRFset.end();
                                sramrf_it++)  {
            const RegisterFile& rf = **sramrf_it;
            assert(sacEnabled(rf.name()));
            for(int i = 0; i < rf.portCount(); i++) {
                const RFPort& port = *rf.port(i);
                if (port.outputSocket() != NULL) {
                    writeControlRulesOfRFReadPort(port, stream);
                }
            }
        }
        stream << indentation(2) << "end" << endl;

        // End process //
        stream << indentation(1) << "end // process" << endl;
    }
}

/**
 * Writes the main decoding process to the given stream.
 *
 * @param stream The stream to write.
 */
void
DefaultDecoderGenerator::writeMainDecodingProcess(
    std::ostream& stream) const {
    if(language_==VHDL){
        string resetPort = NetlistGenerator::DECODER_RESET_PORT;
        string clockPort = NetlistGenerator::DECODER_CLOCK_PORT;

        stream << indentation(1) << "-- main decoding process" << endl;
        stream << indentation(1) << "process (" << clockPort << ", " 
               << resetPort << ")" << endl;
        stream << indentation(1) << "begin" << endl;

        // if reset is active
        stream << indentation(2) << "if (" << resetPort << " = '0') then" 
               << endl;
        writeResettingOfControlRegisters(stream);
        stream << endl << indentation(2) 
           << "elsif (clk'event and clk = '1') then"
           << " -- rising clock edge" << endl;
        if (generateDebugger_){
            string softResetPort = "db_tta_nreset";
            stream << indentation(3) << "if (" << softResetPort
                   << " = '0') then"
                   << endl;
            writeResettingOfControlRegisters(stream);
            stream << indentation(3) << "elsif ("
                   << NetlistGenerator::DECODER_LOCK_REQ_IN_PORT 
                   << " = '0') then" << endl << endl;
        } else {
            stream << indentation(3) << "if ("
                   << NetlistGenerator::DECODER_LOCK_REQ_IN_PORT
                   << " = '0') then" << endl << endl;
        }

        writeInstructionDecoding(stream);
        stream << indentation(3) << "end if;" << endl;
        stream << indentation(2) << "end if;" << endl;
        stream << indentation(1) << "end process;" << endl;
    } else {
        string resetPort = NetlistGenerator::DECODER_RESET_PORT;
        string clockPort = NetlistGenerator::DECODER_CLOCK_PORT;

        stream << indentation(1) << "// main decoding process" << endl
               << indentation(1) << "always@(posedge " << clockPort
               << " or negedge " << resetPort << ")" << endl
                // if reset is active
               << indentation(2) << "if (" << resetPort << " == 0)" << endl
               << indentation(2) <<"begin" << endl;
        writeResettingOfControlRegisters(stream);
        stream << indentation(2) << "end"  << endl
               << indentation(2) << "else" << endl
               << indentation(3) << "begin"<< endl
               << indentation(3) << "if ("
               << NetlistGenerator::DECODER_LOCK_REQ_IN_PORT
               << " == 0)" << endl << endl
               << indentation(3) << "begin"<< endl;
        writeInstructionDecoding(stream);
        stream << indentation(3) << "end" << endl
               << indentation(2) << "end" << endl;
    }
}
    
    
/**
 * Writes resetting of all the control registers to the given stream.
 *
 * @param stream The stream.
 */
void
DefaultDecoderGenerator::writeResettingOfControlRegisters(
    std::ostream& stream) const {
    if(language_==VHDL){
        // reset socket control registers
        Machine::SocketNavigator socketNav = machine_.socketNavigator();
        for (int i = 0; i < socketNav.count(); i++) {
            Socket* socket = socketNav.item(i);
            if (needsBusControl(*socket)) {
                stream << indentation(3) 
                       << socketBusCntrlSignalName(socket->name())
                       << " <= (others => '0');" << endl;
            }
            if (needsDataControl(*socket)) {
                stream << indentation(3) 
                       << socketDataCntrlSignalName(socket->name())
                       << " <= (others => '0');" << endl;
            }
        }

        stream << endl;

        // reset short immediate registers
        Machine::BusNavigator busNav = machine_.busNavigator();
        for (int i = 0; i < busNav.count(); i++) {
            Bus* bus = busNav.item(i);
            if (bus->immediateWidth() > 0) {
                stream << indentation(3) << simmCntrlSignalName(bus->name())
                       << " <= (others => '0');" << endl;
                stream << indentation(3) << simmDataSignalName(bus->name())
                       << " <= (others => '0');" << endl;
            }
        }

        stream << endl;
        // reset FU control registers
        Machine::FunctionUnitNavigator fuNav = machine_.
            functionUnitNavigator();
        for (int i = 0; i < fuNav.count(); i++) {
            FunctionUnit* fu = fuNav.item(i);
            if (opcodeWidth(*fu) > 0) {
                stream << indentation(3) << fuOpcodeSignalName(fu->name()) 
                       << " <= (others => '0');" << endl;
            }
            
            for (int i = 0; i < fu->portCount(); i++) {
                BaseFUPort* port = fu->port(i);
                if (port->inputSocket() != NULL) {
                    stream << indentation(3) 
                           << fuLoadSignalName(fu->name(), port->name()) 
                           << " <= '0';" << endl;
                }
            }
        }

        // reset GCU control registers
        ControlUnit* gcu = machine_.controlUnit();
        stream << indentation(3) << fuOpcodeSignalName(gcu->name())
               << " <= (others => '0');" << endl;
        for (int i = 0; i < gcu->portCount(); i++) {
            BaseFUPort* port = gcu->port(i);
            if (port->inputSocket() != NULL) {
                stream << indentation(3)
                       << fuLoadSignalName(gcu->name(), port->name())
                       << " <= '0';" << endl;
            }
        }

        stream << endl;

        // reset RF control registers
        Machine::RegisterFileNavigator rfNav = machine_.
            registerFileNavigator();
        for (int i = 0; i < rfNav.count(); i++) {
            RegisterFile* rf = rfNav.item(i);

            for (int i = 0; i < rf->portCount(); i++) {
                RFPort* port = rf->port(i);
                // Skip read ports of RFs with separate address cycle address
                // flag enabled.
                if (sacEnabled(rf->name()) && port->outputSocket() != NULL) {
                    continue;
                }

                stream << indentation(3) 
                       << rfLoadSignalName(rf->name(), port->name())
                       << " <= '0';" << endl;
                if (0 < rfOpcodeWidth(*rf)) {
                    stream << indentation(3) 
                           << rfOpcodeSignalName(rf->name(), port->name()) 
                           << " <= (others => '0');" << endl;
                }
            }
        }

        stream << endl;
        
        // reset IU control registers
        Machine::ImmediateUnitNavigator iuNav = machine_.
            immediateUnitNavigator();
        for (int i = 0; i < iuNav.count(); i++) {
            ImmediateUnit* iu = iuNav.item(i);
            for (int i = 0; i < iu->portCount(); i++) {
                RFPort* port = iu->port(i);
                stream << indentation(3) 
                       << iuReadLoadCntrlPort(iu->name(), port->name())
                       << " <= '0';" << endl;
                if (0 < rfOpcodeWidth(*iu)) {
                    stream << indentation(3) 
                           << iuReadOpcodeCntrlPort(iu->name(), port->name()) 
                           << " <= (others => '0');" << endl;
                }
            }
        }
    } else {
        // reset socket control registers
        Machine::SocketNavigator socketNav = machine_.socketNavigator();
        for (int i = 0; i < socketNav.count(); i++) {
            Socket* socket = socketNav.item(i);
            if (needsBusControl(*socket)) {
                stream << indentation(3) 
                       << socketBusCntrlSignalName(socket->name())
                       << " <= 0;" << endl;
            }
            if (needsDataControl(*socket)) {
                stream << indentation(3) 
                       << socketDataCntrlSignalName(socket->name())
                       << " <= 0;" << endl;
            }
        }

        stream << endl;

        // reset short immediate registers
        Machine::BusNavigator busNav = machine_.busNavigator();
        for (int i = 0; i < busNav.count(); i++) {
            Bus* bus = busNav.item(i);
            if (bus->immediateWidth() > 0) {
                stream << indentation(3) << simmCntrlSignalName(bus->name())
                       << " <= 0;" << endl;
                stream << indentation(3) << simmDataSignalName(bus->name())
                       << " <= 0;" << endl;
            }
        }

        stream << endl;
            
        // reset FU control registers
        Machine::FunctionUnitNavigator fuNav = machine_.
            functionUnitNavigator();
        for (int i = 0; i < fuNav.count(); i++) {
            FunctionUnit* fu = fuNav.item(i);
            if (opcodeWidth(*fu) > 0) {
                stream << indentation(3) << fuOpcodeSignalName(fu->name()) 
                       << " <= 0;" << endl;
            }
            
            for (int i = 0; i < fu->portCount(); i++) {
                BaseFUPort* port = fu->port(i);
                if (port->inputSocket() != NULL) {
                    stream << indentation(3) 
                           << fuLoadSignalName(fu->name(), port->name()) 
                           << " <= 1'b0;" << endl;
                }
            }
        }

        // reset GCU control registers
        ControlUnit* gcu = machine_.controlUnit();
        stream << indentation(3) << fuOpcodeSignalName(gcu->name())
               << " <= 0;" << endl;
        for (int i = 0; i < gcu->portCount(); i++) {
            BaseFUPort* port = gcu->port(i);
            if (port->inputSocket() != NULL) {
                stream << indentation(3)
                       << fuLoadSignalName(gcu->name(), port->name())
                       << " <= 1'b0;" << endl;
            }
        }

        stream << endl;

        // reset RF control registers
        Machine::RegisterFileNavigator rfNav = machine_.
            registerFileNavigator();
        for (int i = 0; i < rfNav.count(); i++) {
            RegisterFile* rf = rfNav.item(i);
            for (int i = 0; i < rf->portCount(); i++) {
                RFPort* port = rf->port(i);
                // Skip read ports of RFs with separate address cycle address
                // flag enabled.
                if (sacEnabled(rf->name()) && port->outputSocket() != NULL) {
                    continue;
                }

                stream << indentation(3) 
                       << rfLoadSignalName(rf->name(), port->name())
                       << " <= 1'b0;" << endl;
                if (0 < rfOpcodeWidth(*rf)) {
                    stream << indentation(3) 
                           << rfOpcodeSignalName(rf->name(), port->name()) 
                           << " <= 0;" << endl;
                }
            }
        }

        stream << endl;
        
        // reset IU control registers
        Machine::ImmediateUnitNavigator iuNav = machine_.
            immediateUnitNavigator();
        for (int i = 0; i < iuNav.count(); i++) {
            ImmediateUnit* iu = iuNav.item(i);
            for (int i = 0; i < iu->portCount(); i++) {
                RFPort* port = iu->port(i);
                stream << indentation(3) 
                       << iuReadLoadCntrlSignal(iu->name(), port->name())
                       << " <= 1'b0;" << endl;
                if (0 < rfOpcodeWidth(*iu)) {
                    stream << indentation(3) 
                           << iuReadOpcodeCntrlSignal(iu->name(), port->name())
                           << " <= 0;" << endl;
                }
            }
        }
    }
}


/**
 * Writes the instruction decoding section to the main process in decoder.
 *
 * @param stream The stream to write.
 */
void
DefaultDecoderGenerator::writeInstructionDecoding(
    std::ostream& stream) const {

    writeRulesForSourceControlSignals(stream);
    stream << endl;
    writeRulesForDestinationControlSignals(stream);
}
    

/**
 * Writes the rules for source control signals to the instruction
 * decoding section.
 *
 * @param stream The stream to write.
 */
void
DefaultDecoderGenerator::writeRulesForSourceControlSignals(
    std::ostream& stream) const {
    
    stream << indentation(4) << ((language_==VHDL)?"--":"//")
           << "bus control signals for output sockets"
           << endl;

    Machine::SocketNavigator socketNav = machine_.socketNavigator();
    for (int i = 0; i < socketNav.count(); i++) {
        Socket* socket = socketNav.item(i);
        if (socket->direction() == Socket::OUTPUT && socket->segmentCount() > 0 
            && socket->portCount() > 0) {
            writeBusControlRulesOfOutputSocket(*socket, stream);
        }
    }

    stream << endl << indentation(4)
           << ((language_==VHDL)?"--":"//")
           << "bus control signals for short immediate sockets" << endl;
    Machine::BusNavigator busNav = machine_.busNavigator();
    for (int i = 0; i < busNav.count(); i++) {
        Bus* bus = busNav.item(i);
        if (bus->immediateWidth() > 0) {
            writeBusControlRulesOfSImmSocketOfBus(*bus, stream);
        }
    }

    stream << endl << indentation(4) 
           << ((language_==VHDL)?"--":"//")
           << "data control signals for output sockets connected to FUs"
           << endl;
    Machine::FunctionUnitNavigator fuNav = machine_.functionUnitNavigator();
    for (int i = 0; i < fuNav.count(); i++) {
        FunctionUnit* fu = fuNav.item(i);
        for (int i = 0; i < fu->portCount(); i++) {
            BaseFUPort* port = fu->port(i);
            if (port->outputSocket() != NULL &&
                port->outputSocket()->portCount() > 1) {
                writeControlRulesOfFUOutputPort(*port, stream);
            }
        }
    }

    ControlUnit* gcu = machine_.controlUnit();
    for (int i = 0; i < gcu->portCount(); i++) {
        BaseFUPort* port = gcu->port(i);
        if (port->outputSocket() != NULL &&
            port->outputSocket()->portCount() > 1) {
            writeControlRulesOfFUOutputPort(*port, stream);
        }
    }

    stream << endl << indentation(4)
           << ((language_==VHDL)?"--":"//")
           << "control signals for RF read ports" << endl;
    Machine::RegisterFileNavigator rfNav = machine_.registerFileNavigator();
    for (int i = 0; i < rfNav.count(); i++) {
        RegisterFile* rf = rfNav.item(i);
        // Skip RFs with separate address cycle flag enabled.
        if (sacEnabled(rf->name())) {
            continue;
        }
        for (int i = 0; i < rf->portCount(); i++) {
            RFPort* port = rf->port(i);
            if (port->outputSocket() != NULL) {
                writeControlRulesOfRFReadPort(*port, stream);
            }
        }
    }

    stream << endl << indentation(4)
           << ((language_==VHDL)?"--":"//")
           << "control signals for IU read ports" << endl;
    Machine::ImmediateUnitNavigator iuNav = 
        machine_.immediateUnitNavigator();
    for (int i = 0; i < iuNav.count(); i++) {
        ImmediateUnit* iu = iuNav.item(i);
        for (int i = 0; i < iu->portCount(); i++) {
            RFPort* port = iu->port(i);
            if (port->outputSocket() != NULL) {
                writeControlRulesOfRFReadPort(*port, stream);
            }
        }
    }
}


/**
 * Writes the rules for destination control signals to the instruction
 * decoding section.
 *
 * @param stream The stream to write.
 */
void
DefaultDecoderGenerator::writeRulesForDestinationControlSignals(
    std::ostream& stream) const {

    stream << indentation(4)
           << ((language_==VHDL)?"--":"//")
           << "control signals for FU inputs" << endl;
    Machine::FunctionUnitNavigator fuNav = machine_.functionUnitNavigator();
    for (int i = 0; i < fuNav.count(); i++) {
        FunctionUnit* fu = fuNav.item(i);
        for (int i = 0; i < fu->portCount(); i++) {
            BaseFUPort* port = fu->port(i);
            if (port->inputSocket() != NULL) {
                writeControlRulesOfFUInputPort(*port, stream);
            }
        }
    }

    ControlUnit* gcu = machine_.controlUnit();
    for (int i = 0; i < gcu->portCount(); i++) {
        BaseFUPort* port = gcu->port(i);
        if (port->inputSocket() != NULL) {
            writeControlRulesOfFUInputPort(*port, stream);
        }
    }

    stream << endl << indentation(4)
           << ((language_==VHDL)?"--":"//")
           << "control signals for RF inputs"
           << endl;
    Machine::RegisterFileNavigator rfNav = machine_.registerFileNavigator();
    for (int i = 0; i < rfNav.count(); i++) {
        RegisterFile* rf = rfNav.item(i);
        for (int i = 0; i < rf->portCount(); i++) {
            RFPort* port = rf->port(i);
            if (port->inputSocket() != NULL) {
                writeControlRulesOfRFWritePort(*port, stream);
            }
        }
    }
}   


/**
 * Writes the control signal rules of the given output socket.
 *
 * @param socket The socket.
 * @param stream The stream to write.
 */
void
DefaultDecoderGenerator::writeBusControlRulesOfOutputSocket(
    const TTAMachine::Socket& socket,
    std::ostream& stream) const {

    assert(socket.direction() == Socket::OUTPUT);
    if(language_==VHDL){
        // collect to a set all the buses the socket is connected to
        BusSet connectedBuses = DefaultDecoderGenerator::connectedBuses(socket);
        for (BusSet::const_iterator iter = connectedBuses.begin(); 
             iter != connectedBuses.end(); iter++) {
            Bus* bus = *iter;
            MoveSlot& slot = bem_.moveSlot(bus->name());
            SourceField& srcField = slot.sourceField();
            stream << indentation(4) << "if (" 
                   << squashSignal(bus->name()) << " = '0' and "
                   << socketEncodingCondition(VHDL, srcField, socket.name())
                   << ") then" << endl;
            string busCntrlPin = busCntrlSignalPinOfSocket(socket, *bus);
            stream << indentation(5) << busCntrlPin << " <= '1';" << endl;
            
            stream << indentation(4) << "else" << endl;
            stream << indentation(5) << busCntrlPin << " <= '0';" << endl;
            stream << indentation(4) << "end if;" << endl;
        }
    } else{
        // collect to a set all the buses the socket is connected to
        BusSet connectedBuses = DefaultDecoderGenerator::connectedBuses(socket);
        for (BusSet::const_iterator iter = connectedBuses.begin(); 
             iter != connectedBuses.end(); iter++) {
            Bus* bus = *iter;
            MoveSlot& slot = bem_.moveSlot(bus->name());
            SourceField& srcField = slot.sourceField();
            stream << indentation(4) << "if (" 
                   << squashSignal(bus->name()) << " == 0 && "
                   << socketEncodingCondition(Verilog, srcField, socket.name()) << ")"
                   << endl;
            string busCntrlPin = busCntrlSignalPinOfSocket(socket, *bus);
            stream << indentation(5) << busCntrlPin << " <= 1'b1;" << endl
                   << indentation(4) << "else" << endl
                   << indentation(5) << busCntrlPin << " <= 1'b0;" << endl << endl;
        }
    }
}      


/**
 * Writes the control signal rules for the socket that transports the short
 * immediate to the given bus.
 *
 * @param bus The bus.
 * @param stream The stream to write.
 */
void
DefaultDecoderGenerator::writeBusControlRulesOfSImmSocketOfBus(
    const TTAMachine::Bus& bus,
    std::ostream& stream) const {

    assert(bus.immediateWidth() > 0);
    MoveSlot& slot = bem_.moveSlot(bus.name());
    SourceField& srcField = slot.sourceField();
    assert(srcField.hasImmediateEncoding());
    ImmediateEncoding& enc = srcField.immediateEncoding();
    
    if(language_==VHDL){
        stream << indentation(4) << "if (" << squashSignal(bus.name())
               << " = '0'";
        if (enc.encodingWidth() > 0) {
            stream << " and " << "conv_integer(unsigned("
                   << srcFieldSignal(bus.name()) << "("
                   << enc.encodingPosition() + enc.encodingWidth() - 1
                   << " downto " << enc.encodingPosition() << "))) = "
                   << enc.encoding();
        }
        stream << ") then" << endl;
        stream << indentation(5) << simmCntrlSignalName(bus.name())
               << "(0) <= '1';" << endl;
        stream << indentation(5) << simmDataSignalName(bus.name()) << " <= ";
        if (bus.signExtends()) {
            stream << "sxt(";
        } else {
            stream << "ext(";
        }
        stream << srcFieldSignal(bus.name()) << "("
               << enc.immediatePosition() + enc.immediateWidth() - 1
               << " downto " << enc.immediatePosition() << "), "
               << simmDataSignalName(bus.name()) << "'length);" << endl;
        stream << indentation(4) << "else" << endl;
        stream << indentation(5) << simmCntrlSignalName(bus.name())
               << "(0) <= '0';" << endl;
        stream << indentation(4) << "end if;" << endl;
    } else {
        stream << indentation(4) << "if (" 
               << squashSignal(bus.name()) << " == 0";
        if (enc.encodingWidth() > 0) {
            stream << " && "
                << srcFieldSignal(bus.name()) << "["
                << enc.encodingPosition() + enc.encodingWidth() - 1 << " : "
                << enc.encodingPosition() << "] == " << enc.encoding();
        }
        stream << ")" << endl
               << indentation(4) << "begin" << endl
               << indentation(5) << simmCntrlSignalName(bus.name())
               << "[0] <= 1'b1;" << endl
               << indentation(5) << simmDataSignalName(bus.name()) << " <= ";
        
        if (bus.signExtends()) {
            stream << "$signed(";
        } else {
            stream << "$unsigned(";
        }
        stream << srcFieldSignal(bus.name()) << "["
               << enc.immediatePosition() + enc.immediateWidth() - 1
               << " : " << enc.immediatePosition() << "]);" << endl
               << indentation(4) << "end"   << endl
               << indentation(4) << "else"  << endl
               << indentation(4) << "begin" << endl
               << indentation(5) << simmCntrlSignalName(bus.name())
               << "[0] <= 1'b0;" << endl
               << indentation(4) << "end" << endl;
    }
}


/**
 * Writes the data control signal rules of the socket connected to the given
 * FU output port.
 *
 * @param port The port.
 * @param stream The stream to write.
 */
void
DefaultDecoderGenerator::writeControlRulesOfFUOutputPort(
    const TTAMachine::BaseFUPort& port,
    std::ostream& stream) const {

    Socket* socket = port.outputSocket();
    assert(socket != NULL);
    BusSet connectedBuses = DefaultDecoderGenerator::connectedBuses(*socket);
    if(language_==VHDL){
        for (BusSet::const_iterator iter = connectedBuses.begin();
             iter != connectedBuses.end(); iter++) {
            Bus* bus = *iter;
            MoveSlot& slot = bem_.moveSlot(bus->name());
            SourceField& srcField = slot.sourceField();
            SocketEncoding& enc = srcField.socketEncoding(socket->name());
            stream << indentation(4) << "if (" << squashSignal(bus->name()) 
                   << " = '0' and "
                   << socketEncodingCondition(VHDL, srcField, socket->name());
            if (enc.hasSocketCodes()) {
                SocketCodeTable& scTable = enc.socketCodes();
                FUPortCode& code = scTable.fuPortCode(
                    port.parentUnit()->name(), port.name());
                stream << " and " << portCodeCondition(VHDL, enc, code)
                       << ") then" << endl;
            } else {
                stream << ") then" << endl;
            }
            stream << indentation(5) << socketDataCntrlSignalName(socket->name())
                   << " <= " << "conv_std_logic_vector(" 
                   << icGenerator_.outputSocketDataControlValue(*socket, port) 
                   << ", " << socketDataCntrlSignalName(socket->name()) 
                   << "'length);" << endl; 
            stream << indentation(4) << "end if;" << endl;
        }
    } else {
        for (BusSet::const_iterator iter = connectedBuses.begin();
             iter != connectedBuses.end(); iter++) {
            Bus* bus = *iter;
            MoveSlot& slot = bem_.moveSlot(bus->name());
            SourceField& srcField = slot.sourceField();
            SocketEncoding& enc = srcField.socketEncoding(socket->name());
            stream << indentation(4) << "if ("
                   << squashSignal(bus->name()) << " == 0 && "
                   << socketEncodingCondition(Verilog, srcField, socket->name());
            if (enc.hasSocketCodes()) {
                SocketCodeTable& scTable = enc.socketCodes();
                FUPortCode& code = scTable.fuPortCode(
                    port.parentUnit()->name(), port.name());
                stream << " && " << portCodeCondition(Verilog, enc, code) << ")"
                       << endl;
            } else {
                stream << ")" << endl;
            }
            stream << indentation(5) << socketDataCntrlSignalName(socket->name())
                   << " <= "
                   << icGenerator_.outputSocketDataControlValue(*socket, port) 
                   << ";" << endl;
        }
    }
}


/**
 * Writes the control signal rules related to the given RF read port.
 *
 * @param port The RF read port.
 * @param stream The stream to write.
 */
void
DefaultDecoderGenerator::writeControlRulesOfRFReadPort(
    const TTAMachine::RFPort& port,
    std::ostream& stream) const {

    assert(port.outputSocket() != NULL);
    Socket* socket = port.outputSocket();
    BaseRegisterFile* rf = port.parentUnit();
    bool async_signal = sacEnabled(rf->name());

    // collect to a set all the buses the socket is connected to
    BusSet connectedBuses = DefaultDecoderGenerator::connectedBuses(*socket);
    if (language_ == VHDL) {
        for (BusSet::const_iterator iter = connectedBuses.begin();
             iter != connectedBuses.end();iter++) {
            BusSet::const_iterator nextIter = iter;
            nextIter++;
            if (iter == connectedBuses.begin()) {
                stream << indentation(4) << "if (";
            } else {
                stream << indentation(4) << "elsif (";
            }

            Bus* bus = *iter;
            MoveSlot& slot = bem_.moveSlot(bus->name());
            SourceField& srcField = slot.sourceField();
            SocketEncoding& enc = srcField.socketEncoding(socket->name());
            SocketCodeTable* scTable = NULL;
            if (enc.hasSocketCodes()) {
                scTable = &enc.socketCodes();
            }

            PortCode* code = NULL;
            if (scTable != NULL) {
                if (dynamic_cast<ImmediateUnit*>(rf) != NULL) {
                    code = &scTable->iuPortCode(rf->name());
                } else {
                    code = &scTable->rfPortCode(rf->name());
                }
            }
            stream << squashSignal(bus->name()) << " = '0' and "
                   << socketEncodingCondition(VHDL, srcField, socket->name());
            if (code != NULL && code->hasEncoding()) {
                stream << " and " << portCodeCondition(VHDL, enc, *code);
            }
            stream << ") then" << endl;
            
            string loadSignalName;
            string opcodeSignalName;
            if (dynamic_cast<ImmediateUnit*>(rf) != NULL) {
                loadSignalName =
                    iuReadLoadCntrlPort(rf->name(), port.name());
                opcodeSignalName =
                    iuReadOpcodeCntrlPort(rf->name(), port.name());
            } else {
                loadSignalName = rfLoadSignalName(rf->name(), port.name(),
                    async_signal);
                opcodeSignalName = rfOpcodeSignalName(rf->name(), port.name(),
                    async_signal);
            }
            
            stream << indentation(5) << loadSignalName << " <= '1';" << endl;
            if (code != NULL) {
                stream << indentation(5) << opcodeSignalName << " <= ext(" 
                       << rfOpcodeFromSrcOrDstField(VHDL, enc, *code) << ", "
                       << opcodeSignalName << "'length);" << endl;
            }
            
            if (needsDataControl(*socket)) {
                stream << indentation(5) 
                       << socketDataCntrlSignalName(socket->name())
                       << " <= conv_std_logic_vector(" 
                       << icGenerator_.outputSocketDataControlValue(*socket, port)
                       << ", " << socketDataCntrlSignalName(socket->name())
                       << "'length);" << endl;
            }                                                 
        }

        stream << indentation(4) << "else" << endl;
        stream << indentation(5);
        if (dynamic_cast<ImmediateUnit*>(rf) != NULL) {
            stream << iuReadLoadCntrlPort(rf->name(), port.name());
        } else {
            stream << rfLoadSignalName(rf->name(), port.name(), async_signal);
        }
        stream << " <= '0';" << endl;
        stream << indentation(4) << "end if;" << endl;
    } else { // language_ == Verilog
        for (BusSet::const_iterator iter = connectedBuses.begin();
             iter != connectedBuses.end();iter++) {
            BusSet::const_iterator nextIter = iter;
            nextIter++;
            if (iter == connectedBuses.begin()) {
                stream << indentation(4) << "if (";
            } else {
                stream << indentation(4) << "else if(";
            }
            
            Bus* bus = *iter;
            MoveSlot& slot = bem_.moveSlot(bus->name());
            SourceField& srcField = slot.sourceField();
            SocketEncoding& enc = srcField.socketEncoding(socket->name());
            SocketCodeTable* scTable = NULL;
            if (enc.hasSocketCodes()) {
                scTable = &enc.socketCodes();
            }

            PortCode* code = NULL;
            if (scTable != NULL) {
                if (dynamic_cast<ImmediateUnit*>(rf) != NULL) {
                    code = &scTable->iuPortCode(rf->name());
                } else {
                    code = &scTable->rfPortCode(rf->name());
                }
            }
            stream << squashSignal(bus->name()) << " == 0 && "
                   << socketEncodingCondition(Verilog, srcField, socket->name());
            if (code != NULL && code->hasEncoding()) {
                stream << " && " << portCodeCondition(Verilog, enc, *code);
            }
            stream << ")" << endl
                   << indentation(4) << "begin" << endl;
            
            string loadSignalName;
            string opcodeSignalName;
            if (dynamic_cast<ImmediateUnit*>(rf) != NULL) {
                loadSignalName =
                    iuReadLoadCntrlSignal(rf->name(), port.name());
                opcodeSignalName =
                    iuReadOpcodeCntrlSignal(rf->name(), port.name());
            } else {
                loadSignalName = rfLoadSignalName(rf->name(), port.name(),
                    async_signal);
                opcodeSignalName = rfOpcodeSignalName(rf->name(), port.name(),
                    async_signal);
            }
            
            stream << indentation(5) << loadSignalName << " <= 1'b1;" << endl;
            if (code != NULL) {
                stream << indentation(5) << opcodeSignalName
                       << " <= $unsigned("
                       << rfOpcodeFromSrcOrDstField(Verilog, enc, *code)
                       << ");" << endl;
            }
            
            if (needsDataControl(*socket)) {
                stream << indentation(5) 
                       << socketDataCntrlSignalName(socket->name())
                       << " <= " 
                       << icGenerator_.outputSocketDataControlValue(*socket, port)
                       << ";" << endl;
            }
            stream << indentation(4) << "end" << endl;
        }
        stream << indentation(4) << "else"  << endl
               << indentation(4) << "begin" << endl
               << indentation(5);
        if (dynamic_cast<ImmediateUnit*>(rf) != NULL) {
            stream << iuReadLoadCntrlSignal(rf->name(), port.name());
        } else {
            stream << rfLoadSignalName(rf->name(), port.name(), async_signal);
        }
        stream << " <= 1'b0;" << endl
               << indentation(4) << "end" << endl;
    }
}


/**
 * Writes the rules for control signals related to the given FU input port.
 *
 * @param port The port.
 * @param stream The stream to write.
 */
void
DefaultDecoderGenerator::writeControlRulesOfFUInputPort(
    const TTAMachine::BaseFUPort& port,
    std::ostream& stream) const {

    FunctionUnit* fu = port.parentUnit();
    Socket* socket = port.inputSocket();
    assert(socket != NULL);
    BusSet buses = connectedBuses(*socket);
    stream << indentation(4) << "if (";
    if(language_==VHDL){
        for (BusSet::const_iterator iter = buses.begin(); iter != buses.end();
             iter++) {
            Bus* bus = *iter;
            MoveSlot& moveSlot = bem_.moveSlot(bus->name());
            DestinationField& dstField = moveSlot.destinationField();
            SocketEncoding& enc = dstField.socketEncoding(socket->name());
            stream << squashSignal(bus->name()) << " = '0' and "
                   << socketEncodingCondition(VHDL, dstField, socket->name());
            if (enc.hasSocketCodes()) {
                SocketCodeTable& scTable = enc.socketCodes();
                if (port.isOpcodeSetting()) {
                    stream << ") then" << endl;
                        
                    ControlUnit* gcu = dynamic_cast<ControlUnit*>(fu);

                    // Check
                    bool ordered=true;
                    for (int i = 0; i < fu->operationCount(); i++) {
                        HWOperation* operation = fu->operation(i);
                        FUPortCode& code = scTable.fuPortCode(
                            fu->name(), port.name(), operation->name());
                        if ((int)(code.encoding()) != (int)(opcode(*operation))) {
                            ordered=false;
                            break;
                        }
                    }
                    if (!ordered) {
                        stream << indentation(5) << "if (";
                        for (int i = 0; i < fu->operationCount(); i++) {
                            HWOperation* operation = fu->operation(i);
                            FUPortCode& code = scTable.fuPortCode(
                                fu->name(), port.name(), operation->name());
                            stream << portCodeCondition(VHDL, enc, code) << ") then"
                                   << endl;
                            stream << indentation(6) 
                                   << fuLoadSignalName(fu->name(), port.name())
                                   << " <= '1';" << endl;
                            if (gcu != NULL) {
                                if (operation->name() == JUMP) {
                                    stream << indentation(6)
                                           << fuOpcodeSignalName(fu->name())
                                           << " <= IFE_JUMP;" << endl;
                                } else if (operation->name() == CALL) {
                                    stream << indentation(6)
                                           << fuOpcodeSignalName(fu->name())
                                           << " <= IFE_CALL;" << endl;
                                }
                            }
                            else {
                                stream << indentation(6) 
                                       << fuOpcodeSignalName(fu->name())
                                       << " <= conv_std_logic_vector(" 
                                       << opcode(*operation) << ", " 
                                       << fuOpcodeSignalName(fu->name())
                                       << "'length);"
                                       << endl;
                            }
                            if (i+1 < fu->operationCount()) {
                                stream << indentation(5) << "elsif (";
                            }
                        }
                        stream << indentation(5) << "else" << endl;
                        stream << indentation(6) 
                               << fuLoadSignalName(fu->name(), port.name())
                               << " <= '0';"
                               << endl;
                        stream << indentation(5) << "end if;" << endl;
                    } else {
                        FUPortCode& code = scTable.fuPortCode(
                            fu->name(), port.name(), fu->operation(0)->name());
                        SlotField* parent = enc.parent();
                        string signalName;
                        if (dynamic_cast<SourceField*>(parent) != NULL) {
                            signalName = srcFieldSignal(parent->parent()->name());
                        } else {
                            signalName = dstFieldSignal(parent->parent()->name());
                        }
                    
                        int codeStart;
                        if (parent->componentIDPosition() == BinaryEncoding::RIGHT) {
                            codeStart = enc.socketIDWidth() + code.indexWidth();
                        } else {
                            codeStart = code.indexWidth();
                        }
                        int codeEnd = codeStart + code.encodingWidth() - 1;
                        assert(codeEnd >= codeStart);
                        stream << indentation(5) 
                               << fuLoadSignalName(fu->name(), port.name())
                               << " <= '1';" << endl;
                        stream << indentation(5)
                               << fuOpcodeSignalName(fu->name())
                               << " <= " << signalName
                               << "(" << Conversion::toString(codeEnd) << " downto "
                               << Conversion::toString(codeStart) << ")"
                               << ";" << endl;
                    }
                } else {
                    FUPortCode& code = scTable.fuPortCode(
                        fu->name(), port.name());
                    stream << " and " << portCodeCondition(VHDL, enc, code) << ") then"
                           << endl;
                    stream << indentation(5) 
                           << fuLoadSignalName(fu->name(), port.name()) 
                           << " <= '1';" << endl;
                }

            } else {
                stream << ") then" << endl;
                stream << indentation(5) 
                       << fuLoadSignalName(fu->name(), port.name()) << " <= '1';"
                       << endl;
                ControlUnit* gcu = dynamic_cast<ControlUnit*>(fu);
                if (gcu != NULL) {
                    if (gcu->hasOperation(JUMP)) {
                        HWOperation* jumpOp = gcu->operation(JUMP);
                        if (&port == jumpOp->port(1)) {
                            stream << indentation(5) 
                                   << fuOpcodeSignalName(fu->name())
                                   << " <= IFE_JUMP;" << endl;
                        }
                    }
                    if (gcu->hasOperation(CALL)) {
                        HWOperation* callOp = gcu->operation(CALL);
                        if (&port == callOp->port(1)) {
                            stream << indentation(5)
                                   << fuOpcodeSignalName(fu->name())
                                   << " <= IFE_CALL;" << endl;
                        }
                    }
                }
            }
            if (needsBusControl(*socket)) {
                stream << indentation(5)
                       << socketBusCntrlSignalName(socket->name())
                       << " <= conv_std_logic_vector(" 
                       << icGenerator_.inputSocketControlValue(
                           *socket, *bus->segment(0)) << ", " 
                       << socketBusCntrlSignalName(socket->name()) << "'length);"
                       << endl;
            }
            
            BusSet::const_iterator nextIter = iter;
            nextIter++;
            if (nextIter != buses.end()) {
                stream << indentation(4) << "elsif (";
            };
        }

        stream << indentation(4) << "else" << endl;
        stream << indentation(5) << fuLoadSignalName(fu->name(), port.name())
               << " <= '0';" << endl;
        stream << indentation(4) << "end if;" << endl;
    } else {
        for (BusSet::const_iterator iter = buses.begin(); iter != buses.end();
             iter++) {
            Bus* bus = *iter;
            MoveSlot& moveSlot = bem_.moveSlot(bus->name());
            DestinationField& dstField = moveSlot.destinationField();
            SocketEncoding& enc = dstField.socketEncoding(socket->name());
            stream << squashSignal(bus->name()) << " == 0 && "
                   << socketEncodingCondition(Verilog, dstField, socket->name());
            if (enc.hasSocketCodes()) {
                SocketCodeTable& scTable = enc.socketCodes();
                if (port.isOpcodeSetting()) {
                    stream << ")" << endl
                           << indentation(4) << "begin" << endl 
                           << indentation(5) << "if (";
                    for (int i = 0; i < fu->operationCount(); i++) {
                        HWOperation* operation = fu->operation(i);
                        FUPortCode& code = scTable.fuPortCode(
                            fu->name(), port.name(), operation->name());
                        stream << portCodeCondition(Verilog, enc, code) << ")"
                               << endl
                               << indentation(5) << "begin" << endl
                               << indentation(6) 
                               << fuLoadSignalName(fu->name(), port.name())
                               << " <= 1'b1;" << endl;
                        ControlUnit* gcu = dynamic_cast<ControlUnit*>(fu);
                        if (gcu != NULL) {
                            if (operation->name() == JUMP) {
                                stream << indentation(5) 
                                       << fuOpcodeSignalName(fu->name())
                                       << " <= IFE_JUMP;" << endl;
                            } else if (operation->name() == CALL) {
                                stream << indentation(5)
                                       << fuOpcodeSignalName(fu->name())
                                       << " <= IFE_CALL;" << endl;
                            }
                        } else {                            
                            stream << indentation(6) 
                                   << fuOpcodeSignalName(fu->name())
                                   << " <= "
                                   << opcode(*operation)
                                   << ";"
                                   << endl;
                        }
                        if (i+1 < fu->operationCount()) {
                            stream << indentation(5) << "end" << endl
                                   << indentation(5) << "else if (";
                        }
                    }
                    stream << indentation(5) << "end"   << endl
                           << indentation(5) << "else"  << endl
                           << indentation(6) 
                           << fuLoadSignalName(fu->name(), port.name())
                           << " <= 1'b0;"
                           << endl;
                } else {
                    FUPortCode& code = scTable.fuPortCode(
                        fu->name(), port.name());
                    stream << " && " << portCodeCondition(Verilog, enc, code) << ")"
                           << endl
                           << indentation(4) << "begin" << endl
                           << indentation(5) 
                           << fuLoadSignalName(fu->name(), port.name()) 
                           << " <= 1'b1;" << endl;
                }
            } else {
                stream << ")" << endl
                       << indentation(4) << "begin" << endl
                       << indentation(5) 
                       << fuLoadSignalName(fu->name(), port.name()) << " <= 1'b1;"
                       << endl;
                ControlUnit* gcu = dynamic_cast<ControlUnit*>(fu);
                if (gcu != NULL) {
                    if (gcu->hasOperation(JUMP)) {
                        HWOperation* jumpOp = gcu->operation(JUMP);
                        if (&port == jumpOp->port(1)) {
                            stream << indentation(5) 
                                   << fuOpcodeSignalName(fu->name())
                                   << " <= IFE_JUMP;" << endl;
                        }
                    }
                    if (gcu->hasOperation(CALL)) {
                        HWOperation* callOp = gcu->operation(CALL);
                        if (&port == callOp->port(1)) {
                            stream << indentation(5)
                                   << fuOpcodeSignalName(fu->name())
                                   << " <= IFE_CALL;" << endl;
                        }
                    }
                }
            }
            if (needsBusControl(*socket)) {
                stream << indentation(5)
                       << socketBusCntrlSignalName(socket->name())
                       << " <= " 
                       << icGenerator_.inputSocketControlValue(
                           *socket, *bus->segment(0)) << ";" 
                       << endl;
            }
            
            BusSet::const_iterator nextIter = iter;
            nextIter++;
            stream << indentation(4) << "end" << endl;
            if (nextIter != buses.end()) {
                stream << indentation(4) << "else if(";
            };
        }

        stream << indentation(4) << "else" << endl;
        stream << indentation(5) << fuLoadSignalName(fu->name(), port.name())
               << " <= 1'b0;" << endl;
    }
}


/**
 * Writes the rules for control signals related to the given RF write port.
 *
 * @param port The port.
 * @param stream The stream to write.
 */
void
DefaultDecoderGenerator::writeControlRulesOfRFWritePort(
    const TTAMachine::RFPort& port,
    std::ostream& stream) const {

    BaseRegisterFile* rf = port.parentUnit();
    Socket* socket = port.inputSocket();
    assert(socket != NULL);
    BusSet buses = connectedBuses(*socket);
    stream << indentation(4) << "if (";
    if(language_==VHDL){
        for (BusSet::const_iterator iter = buses.begin(); iter != buses.end();
             iter++) {
            Bus* bus = *iter;
            MoveSlot& moveSlot = bem_.moveSlot(bus->name());
            DestinationField& dstField = moveSlot.destinationField();
            SocketEncoding& enc = dstField.socketEncoding(socket->name());
            stream << squashSignal(bus->name()) << " = '0' and "
                   << socketEncodingCondition(VHDL, dstField, socket->name());
            if (enc.hasSocketCodes()) {
                SocketCodeTable& scTable = enc.socketCodes();
                RFPortCode& code = scTable.rfPortCode(rf->name());
                if (code.hasEncoding()) {
                    stream << " and " << portCodeCondition(VHDL, enc, code);

                }
                stream << ") then" << endl;
                stream << indentation(5)
                       << rfLoadSignalName(rf->name(), port.name()) << " <= '1';"
                       << endl;
                stream << indentation(5) 
                       << rfOpcodeSignalName(rf->name(), port.name()) << " <= "
                       << rfOpcodeFromSrcOrDstField(VHDL, enc, code) << ";" << endl;
            } else {
                stream << ") then" << endl;
                stream << indentation(5)
                       << rfLoadSignalName(rf->name(), port.name()) << " <= '1';"
                       << endl;
            }

            if (needsBusControl(*socket)) {
                stream << indentation(5)
                       << socketBusCntrlSignalName(socket->name())
                       << " <= conv_std_logic_vector(" 
                       << icGenerator_.inputSocketControlValue(
                           *socket, *bus->segment(0)) << ", " 
                       << socketBusCntrlSignalName(socket->name()) << "'length);"
                       << endl;
            }
            
            BusSet::const_iterator nextIter = iter;
            nextIter++;
            if (nextIter != buses.end()) {
                stream << indentation(4) << "elsif (";
            }
        }
        stream << indentation(4) << "else" << endl;
        stream << indentation(5) << rfLoadSignalName(rf->name(), port.name())
               << " <= '0';" << endl;
        stream << indentation(4) << "end if;" << endl;
    } else {
        for (BusSet::const_iterator iter = buses.begin(); iter != buses.end();
             iter++) {
            Bus* bus = *iter;
            MoveSlot& moveSlot = bem_.moveSlot(bus->name());
            DestinationField& dstField = moveSlot.destinationField();
            SocketEncoding& enc = dstField.socketEncoding(socket->name());
            stream << squashSignal(bus->name()) << " == 0 && "
                   << socketEncodingCondition(Verilog, dstField, socket->name());
            if (enc.hasSocketCodes()) {
                SocketCodeTable& scTable = enc.socketCodes();
                RFPortCode& code = scTable.rfPortCode(rf->name());
                if (code.hasEncoding()) {
                    stream << " && " << portCodeCondition(Verilog, enc, code);

                }
                stream << ")" << endl
                       << indentation(4) << "begin" << endl
                       << indentation(5)
                       << rfLoadSignalName(rf->name(), port.name()) << " <= 1'b1;"
                       << endl
                       << indentation(5) 
                       << rfOpcodeSignalName(rf->name(), port.name()) << " <= "
                       << rfOpcodeFromSrcOrDstField(Verilog, enc, code) << ";" << endl;
            } else {
                stream << ")" << endl
                       << indentation(4) << "begin" << endl
                       << indentation(5)
                       << rfLoadSignalName(rf->name(), port.name()) << " <= 1'b1;"
                       << endl;
            }

            if (needsBusControl(*socket)) {
                stream << indentation(5)
                       << socketBusCntrlSignalName(socket->name())
                       << " <= " 
                       << icGenerator_.inputSocketControlValue(
                           *socket, *bus->segment(0)) << ";"
                       << endl;
            }
            
            BusSet::const_iterator nextIter = iter;
            nextIter++;
            stream << indentation(4) << "end" << endl;
            if (nextIter != buses.end()) {
                stream << indentation(4) << "else if (";
            }
        }
        stream << indentation(4) << "else" << endl
               << indentation(5) << rfLoadSignalName(rf->name(), port.name())
               << " <= 1'b0;" << endl;
    }
}


/**
 * Writes the mappings of control registers to control ports to the given
 * stream.
 *
 * @param stream The stream.
 */
void
DefaultDecoderGenerator::writeControlRegisterMappings(
    std::ostream& stream) const {
    if(language_==VHDL){
        stream << indentation(1) << "-- map control registers to outputs" 
               << endl;

        // map FU control signals
        Machine::FunctionUnitNavigator fuNav = machine_.
            functionUnitNavigator();
        for (int i = 0; i < fuNav.count(); i++) {
            FunctionUnit* fu = fuNav.item(i);
            for (int i = 0; i < fu->portCount(); i++) {
                BaseFUPort* port = fu->port(i);
                if (port->inputSocket() != NULL) {
                    // map load signal
                    stream << indentation(1) 
                           << fuLoadCntrlPort(fu->name(), port->name()) 
                           << " <= " 
                           << fuLoadSignalName(fu->name(), port->name())
                           << ";" << endl;
                }
            }
                
            // map opcode signal
            if (fu->operationCount() > 1) {
                stream << indentation(1) << fuOpcodeCntrlPort(fu->name()) 
                       << " <= " << fuOpcodeSignalName(fu->name()) << ";" 
                       << endl;
            }
            stream << endl;
        }

        // map pc_load and ra_load signals
        ControlUnit* gcu = machine_.controlUnit();
        if (gcu->hasReturnAddressPort()) {
            SpecialRegisterPort* raPort = gcu->returnAddressPort();
            stream << indentation(1) << NetlistGenerator::DECODER_RA_LOAD_PORT
                   << " <= " << fuLoadSignalName(gcu->name(), raPort->name())
                   << ";" << endl;
        }

        // find the PC port
        FUPort* pcPort = NULL;
        if (gcu->hasOperation(CALL)) {
            HWOperation* callOp = gcu->operation(CALL);
            pcPort = callOp->port(1);
        } else if (gcu->hasOperation(JUMP)) {
            HWOperation* jumpOp = gcu->operation(JUMP);
            pcPort = jumpOp->port(1);
        }
        if (pcPort != NULL) {
            stream << indentation(1) << NetlistGenerator::DECODER_PC_LOAD_PORT
                   << " <= " << fuLoadSignalName(gcu->name(), pcPort->name())
                   << ";" << endl;
        }

        // map pc_opcode signal
        if (gcu->hasOperation(JUMP) || gcu->hasOperation(CALL)) {
            stream << indentation(1) << NetlistGenerator::DECODER_PC_OPCODE_PORT
                   << " <= " << fuOpcodeSignalName(gcu->name()) << ";" << endl;
        }
            
        // map RF control signals
        Machine::RegisterFileNavigator rfNav = machine_.
            registerFileNavigator();
        for (int i = 0; i < rfNav.count(); i++) {
            RegisterFile* rf = rfNav.item(i);

            for (int i = 0; i < rf->portCount(); i++) {
                RFPort* port = rf->port(i);
                bool async_signal = sacEnabled(rf->name())
                                && port->outputSocket() != NULL;

                // map load signal
                stream << indentation(1) 
                       << rfLoadCntrlPort(rf->name(), port->name()) << " <= " 
                       << rfLoadSignalName(rf->name(), port->name(),
                           async_signal) << ";"
                       << endl;

                // map opcode signal
                bool OpcodePortExists = decoderBlock_->portByName(
                    rfOpcodeCntrlPort(rf->name(), port->name()));
                if (OpcodePortExists) {
                    stream << indentation(1)
                           << rfOpcodeCntrlPort(rf->name(), port->name())
                           << " <= "
                           << (rfOpcodeWidth(*rf) == 0 ? "\"0\"" :
                               rfOpcodeSignalName(rf->name(), port->name(),
                                                  async_signal))
                           << ";"
                           << endl;
                }
            }
        }

        // map IU write/read opcode signals (only 0 downto 0) to 0
        // TODO: mapping of these ports should probably be elsewhere
        Machine::ImmediateUnitNavigator iuNav =
            machine_.immediateUnitNavigator();
        for (int i = 0; i < iuNav.count(); i++) {
            ImmediateUnit* iu = iuNav.item(i);
            for (int i = 0; i < iu->portCount(); i++) {
                RFPort* port = iu->port(i);
                bool readOpcodePortExists = decoderBlock_->portByName(
                    iuReadOpcodeCntrlPort(iu->name(), port->name()));
                bool writeOpcodePortExists = decoderBlock_->portByName(
                    iuWriteOpcodeCntrlPort(iu->name()));
                assert(readOpcodePortExists == writeOpcodePortExists);
                if (rfOpcodeWidth(*iu) == 0 and readOpcodePortExists and
                    writeOpcodePortExists) {

                    stream << indentation(1)
                           << iuReadOpcodeCntrlPort(iu->name(), port->name())
                           << " <= \"0\";"
                           << endl;

                    stream << indentation(1)
                           << iuWriteOpcodeCntrlPort(iu->name())
                           << " <= \"0\";"
                           << endl;
                }
            }
        }


        // map socket control signals
        Machine::SocketNavigator socketNav = machine_.socketNavigator();
        for (int i = 0; i < socketNav.count(); i++) {
            Socket* socket = socketNav.item(i);
            if (socket->portCount() == 0 || socket->segmentCount() == 0) {
                continue;
            }

            if (needsBusControl(*socket)) {
                stream << indentation(1) << socketBusControlPort(socket->name())
                       << " <= " << socketBusCntrlSignalName(socket->name())
                       << ";" << endl;
            }
            if (needsDataControl(*socket)) {
                stream << indentation(1) << socketDataControlPort(socket->name())
                       << " <= " << socketDataCntrlSignalName(socket->name()) 
                       << ";" << endl;
            }
        }

        // map short immediate signals
        Machine::BusNavigator busNav = machine_.busNavigator();
        for (int i = 0; i < busNav.count(); i++) {
            Bus* bus = busNav.item(i);
            if (bus->immediateWidth() > 0) {
                stream << indentation(1) << simmControlPort(bus->name())
                       << " <= " << simmCntrlSignalName(bus->name()) << ";"
                       << endl;
                stream << indentation(1) << simmDataPort(bus->name()) << " <= "
                       << simmDataSignalName(bus->name()) << ";" << endl;
            }
        }
    } else {
        stream << indentation(1) << "// map control registers to outputs" 
               << endl;

        // map FU control signals
        Machine::FunctionUnitNavigator fuNav = machine_.
            functionUnitNavigator();
        for (int i = 0; i < fuNav.count(); i++) {
            FunctionUnit* fu = fuNav.item(i);
            for (int i = 0; i < fu->portCount(); i++) {
                BaseFUPort* port = fu->port(i);
                if (port->inputSocket() != NULL) {
                    // map load signal
                    stream << indentation(1) 
                           << "assign "
                           << fuLoadCntrlPort(fu->name(), port->name()) 
                           << " = " 
                           << fuLoadSignalName(fu->name(), port->name())
                           << ";" << endl;
                }
            }
                
            // map opcode signal
            if (fu->operationCount() > 1) {
                stream << indentation(1)
                       << "assign "
                       << fuOpcodeCntrlPort(fu->name()) 
                       << " = " << fuOpcodeSignalName(fu->name()) << ";" 
                       << endl;
            }
            stream << endl;
        }

        // map pc_load and ra_load signals
        ControlUnit* gcu = machine_.controlUnit();
        if (gcu->hasReturnAddressPort()) {
            SpecialRegisterPort* raPort = gcu->returnAddressPort();
            stream << indentation(1)
                   << "assign "
                   << NetlistGenerator::DECODER_RA_LOAD_PORT
                   << " = " << fuLoadSignalName(gcu->name(), raPort->name())
                   << ";" << endl;
        }

        // find the PC port
        FUPort* pcPort = NULL;
        if (gcu->hasOperation(CALL)) {
            HWOperation* callOp = gcu->operation(CALL);
            pcPort = callOp->port(1);
        } else if (gcu->hasOperation(JUMP)) {
            HWOperation* jumpOp = gcu->operation(JUMP);
            pcPort = jumpOp->port(1);
        }
        if (pcPort != NULL) {
            stream << indentation(1)
                   << "assign "
                   << NetlistGenerator::DECODER_PC_LOAD_PORT
                   << " = " << fuLoadSignalName(gcu->name(), pcPort->name())
                   << ";" << endl;
        }

        // map pc_opcode signal
        if (gcu->hasOperation(JUMP) || gcu->hasOperation(CALL)) {
            stream << indentation(1) 
                   << "assign "
                   << NetlistGenerator::DECODER_PC_OPCODE_PORT
                   << " = " << fuOpcodeSignalName(gcu->name()) << ";" << endl;
        }
            
        // map RF control signals
        Machine::RegisterFileNavigator rfNav = machine_.
            registerFileNavigator();
        for (int i = 0; i < rfNav.count(); i++) {
            RegisterFile* rf = rfNav.item(i);
            for (int i = 0; i < rf->portCount(); i++) {
                RFPort* port = rf->port(i);
                bool async_signal = sacEnabled(rf->name())
                                && port->outputSocket() != NULL;

                // map load signal
                stream << indentation(1) 
                       << "assign "
                       << rfLoadCntrlPort(rf->name(), port->name()) << " = " 
                       << rfLoadSignalName(rf->name(), port->name(),
                           async_signal) << ";"
                       << endl;
                // map opcode signal
                stream << indentation(1)
                       << "assign "                
                       << rfOpcodeCntrlPort(rf->name(), port->name()) 
                       << " = " 
                       << (rfOpcodeWidth(*rf) == 0 ? "\"0\"" : 
                            rfOpcodeSignalName(rf->name(), port->name(),
                                async_signal))
                       << ";"
                       << endl;
            }
        }

        Machine::ImmediateUnitNavigator iuNav = 
            machine_.immediateUnitNavigator();
        for (int i = 0; i < iuNav.count(); i++) {
            ImmediateUnit* iu = iuNav.item(i);
            for (int i = 0; i < iu->portCount(); i++) {
                RFPort* port = iu->port(i);

                stream << indentation(1) << "assign "
                       << iuReadLoadCntrlPort(iu->name(), port->name())
                       << " = "
                       << iuReadLoadCntrlSignal(iu->name(), port->name())
                       << ";" << endl;
                if (rfOpcodeWidth(*iu) == 0) {
                    stream << indentation(1) << "assign "
                           << iuReadOpcodeCntrlPort(iu->name(), port->name())
                           << " = 1'b0;"
                           << endl;

                } else {
                    stream << indentation(1) << "assign "
                           << iuReadOpcodeCntrlPort(iu->name(), port->name())
                           << " = "
                           << iuReadOpcodeCntrlSignal(iu->name(), port->name())
                           << ";" << endl;

                }
            } // Loop for IU ports
            stream << indentation(1) << "assign "
                   << iuWritePort(iu->name()) << " = "
                   << iuWriteSignal(iu->name()) << ";" << endl;
            stream << indentation(1) << "assign "
                   << iuWriteLoadCntrlPort(iu->name()) << " = "
                   << iuWriteLoadCntrlSignal(iu->name()) << ";" << endl;
            if (rfOpcodeWidth(*iu) == 0) {
                stream << indentation(1) << "assign "
                       << iuWriteOpcodeCntrlPort(iu->name())
                       << " = 1'b0;" << endl;
            } else {
                stream << indentation(1) << "assign "
                       << iuWriteOpcodeCntrlPort(iu->name())
                       << " = "
                       << iuWriteOpcodeCntrlSignal(iu->name())
                       << ";" << endl;
            }
        } // Loop for IUs
       

        // map socket control signals
        Machine::SocketNavigator socketNav = machine_.socketNavigator();
        for (int i = 0; i < socketNav.count(); i++) {
            Socket* socket = socketNav.item(i);
            if (socket->portCount() == 0 || socket->segmentCount() == 0) {
                continue;
            }

            if (needsBusControl(*socket)) {
                stream << indentation(1)
                       << "assign "
                       << socketBusControlPort(socket->name())
                       << " = " << socketBusCntrlSignalName(socket->name())
                       << ";" << endl;
            }
            if (needsDataControl(*socket)) {
                stream << indentation(1)
                       << "assign "
                       << socketDataControlPort(socket->name())
                       << " = " << socketDataCntrlSignalName(socket->name()) 
                       << ";" << endl;
            }
        }

        // map short immediate signals
        Machine::BusNavigator busNav = machine_.busNavigator();
        for (int i = 0; i < busNav.count(); i++) {
            Bus* bus = busNav.item(i);
            if (bus->immediateWidth() > 0) {
                stream << indentation(1)
                       << "assign "
                       << simmControlPort(bus->name())
                       << " = " << simmCntrlSignalName(bus->name()) << ";"
                       << endl
                       << indentation(1)
                       << "assign "
                       << simmDataPort(bus->name()) << " = "
                       << simmDataSignalName(bus->name()) << ";" << endl;
            }
        }
    }
}


/**
 * Writes substitution of guard value to the squash signal of the given 
 * bus.
 *
 * @param bus The bus.
 * @param enc The guard encoding.
 * @param guard The guard.
 * @param stream The stream to write.
 * @param indLevel The indentation level.
 */
void
DefaultDecoderGenerator::writeSquashSignalSubstitution(
    const ProGe::HDL language,
    const TTAMachine::Bus& bus,
    const GuardEncoding& enc,
    const TTAMachine::Guard& guard,
    std::ostream& stream,
    int indLevel) {
    if(language==VHDL){
        stream << indentation(indLevel) << "when " << enc.encoding() 
               << " =>" << endl;
        stream << indentation(indLevel+1) << squashSignal(bus.name()) << " <= ";
        if (!guard.isInverted()) {
            stream << "not ";
        }
        stream << guardPortName(guard) << ";" << endl;
    } else {
        stream << indentation(indLevel)   << enc.encoding() << " : "
               << squashSignal(bus.name()) << " = ";
        if (!guard.isInverted()) {
            stream << " !";
        }
        stream << guardPortName(guard) << ";" << endl;
    }
}


/**
 * Tells whether the given guard set contains similar guard to the given
 * one. Similar means the guard refers to the same FU port.
 *
 * @param guardSet The guard set.
 * @param guard The guard.
 * @return True if the set contains similar guard, otherwise false.
 */
bool
DefaultDecoderGenerator::containsSimilarGuard(
    const std::set<TTAMachine::PortGuard*>& guardSet, 
    const TTAMachine::PortGuard& guard) {
    
    for (std::set<PortGuard*>::const_iterator iter = guardSet.begin();
         iter != guardSet.end(); iter++) {
        PortGuard* containedGuard = *iter;
        if (containedGuard->port() == guard.port()) {
            return true;
        }
    }
    
    return false;
}


/**
 * Tells whether the given guard set contains similar guard to the given
 * one. Similar means the guard refers to the same register.
 *
 * @param guardSet The guard set.
 * @param guard The guard.
 * @return True if the set contains similar guard, otherwise false.
 */
bool
DefaultDecoderGenerator::containsSimilarGuard(
    const std::set<TTAMachine::RegisterGuard*>& guardSet,
    const TTAMachine::RegisterGuard& guard) {
    
    for (std::set<RegisterGuard*>::const_iterator iter = guardSet.begin();
         iter != guardSet.end(); iter++) {
        RegisterGuard* containedGuard = *iter;
        if (containedGuard->registerFile() == guard.registerFile() &&
            containedGuard->registerIndex() == guard.registerIndex()) {
            return true;
        }
    }
    
    return false;
}


/**
 * Tells whether the given socket needs controlling from which bus data is
 * read or to which it is written.
 *
 * @param socket The socket.
 * @return True if the socket needs controlling, otherwise false.
 */
bool
DefaultDecoderGenerator::needsBusControl(const TTAMachine::Socket& socket) {
    if (socket.segmentCount() == 0 || socket.portCount() == 0) {
        return false;
    }
    if (socket.segmentCount() > 1 || 
        (socket.segmentCount() == 1 && 
         socket.direction() == Socket::OUTPUT)) {
        return true;
    } else {
        return false;
    }
}


/**
 * Tells whether the given output socket needs controlling from which
 * port the data should be written to a bus.
 */
bool
DefaultDecoderGenerator::needsDataControl(const TTAMachine::Socket& socket) {
    if (socket.direction() == Socket::OUTPUT && socket.portCount() > 1) {
        return true;
    } else {
        return false;
    }
}


/**
 * Finds the guard that is referred to by the given register guard 
 * encoding.
 *
 * @param encoding The encoding.
 * @return The guard.
 */
TTAMachine::RegisterGuard&
DefaultDecoderGenerator::findGuard(const GPRGuardEncoding& encoding) const {
    
    GuardField* parent = encoding.parent();
    string busName = parent->parent()->name();
    
    Machine::BusNavigator busNav = machine_.busNavigator();
    Bus* bus = busNav.item(busName);
    for (int i = 0; i < bus->guardCount(); i++) {
        Guard* guard = bus->guard(i);
        RegisterGuard* regGuard = dynamic_cast<RegisterGuard*>(guard);
        if (regGuard != NULL) {
            if (encoding.registerFile() == regGuard->registerFile()->name()
                && encoding.registerIndex() == regGuard->registerIndex() &&
                encoding.isGuardInverted() == regGuard->isInverted()) {
                return *regGuard;
            }
        }
    }
    
    assert(false);
}


/**
 * Finds the guard that is referred to by the given port guard encoding.
 *
 * @param encoding The encoding.
 * @return The guard.
 */
TTAMachine::PortGuard&
DefaultDecoderGenerator::findGuard(const FUGuardEncoding& encoding) const {
    
    GuardField* parent = encoding.parent();
    string busName = parent->parent()->name();
    
    Machine::BusNavigator busNav = machine_.busNavigator();
    Bus* bus = busNav.item(busName);
    for (int i = 0; i < bus->guardCount(); i++) {
        Guard* guard = bus->guard(i);
        PortGuard* portGuard = dynamic_cast<PortGuard*>(guard);
        if (portGuard != NULL) {
            if (encoding.functionUnit() == 
                portGuard->port()->parentUnit()->name() && 
                encoding.port() == portGuard->port()->name() && 
                encoding.isGuardInverted() == portGuard->isInverted()) {
                return *portGuard;
            }
        }
    }
    
    assert(false);
}        


/**
 * Returns the name of the data port for short immediate of the given bus.
 *
 * @param busName Name of the bus.
 * @return The name of the port.
 */
std::string
DefaultDecoderGenerator::simmDataPort(const std::string& busName) {
    return "simm_" + busName;
}


/**
 * Returns the name of the control port for short immediate of the given
 * bus.
 *
 * @param busName Name of the bus.
 * @return The name of the port.
 */
std::string
DefaultDecoderGenerator::simmControlPort(const std::string& busName) {
    return "simm_cntrl_" + busName;
}


/**
 * Returns the required width of the short immediate port of the given bus.
 *
 * @param bus The bus.
 * @return The width of the port.
 */
int
DefaultDecoderGenerator::simmPortWidth(const TTAMachine::Bus& bus) {
    if (bus.signExtends()) {
        return bus.width();
    } else if (bus.zeroExtends()) {
        return bus.immediateWidth();
    } else {
        assert(false && "Unknown extension policy.");
        return -1;
    }
}


/**
 * Returns the name of the short immediate data signal.
 *
 * @param busName Name of the bus that transports the short immediate.
 * @return The name of the signal.
 */
std::string
DefaultDecoderGenerator::simmDataSignalName(const std::string& busName) {
    return "simm_" + busName + "_reg";
}


/**
 * Returns the name of the short immediate control signal of the given bus.
 *
 * @param busName Name of the bus.
 * @return The name of the signal.
 */
std::string
DefaultDecoderGenerator::simmCntrlSignalName(const std::string& busName) {
    return "simm_cntrl_" + busName + "_reg";
}


/**
 * Returns the name of the load control port for the given FU port.
 *
 * @param fuName Name of the FU.
 * @param portName Name of the FU data port.
 * @return The name of the load control port in decoder.
 */
std::string
DefaultDecoderGenerator::fuLoadCntrlPort(
    const std::string& fuName,
    const std::string& portName) {

    return "fu_" + fuName + "_" + portName + "_load";
}


/**
 * Returns the name for the signal of load control port of the given FU
 * port.
 *
 * @param fuName Name of the FU.
 * @param portName Name of the FU data port.
 * @return The name of the signal.
 */
std::string
DefaultDecoderGenerator::fuLoadSignalName(
    const std::string& fuName,
    const std::string& portName) {
    
    return fuLoadCntrlPort(fuName, portName) + "_reg";
}


/**
 * Returns the name of the opcode control port for the given FU.
 *
 * @param fu Name of the FU.
 * @return The name of the opcode control port in decoder.
 */
std::string
DefaultDecoderGenerator::fuOpcodeCntrlPort(const std::string& fu) {
    return "fu_" + fu + "_opc";
}


/**
 * Returns the name for the signal of opcode control port of the given FU.
 *
 * @param fu Name of the FU.
 * @return The name of the opcode control signal.
 */
std::string
DefaultDecoderGenerator::fuOpcodeSignalName(const std::string& fu) {
    return fuOpcodeCntrlPort(fu) + "_reg";
}


/**
 * Returns the name of the load control port of the given RF data port.
 *
 * @param rfName Name of the RF.
 * @param portName Name of the RF data port.
 * @return The name of the load control port in decoder.
 */
std::string
DefaultDecoderGenerator::rfLoadCntrlPort(
    const std::string& rfName,
    const std::string& portName) {
    
    return "rf_" + rfName + "_" + portName + "_load";
}


/**
 * Returns the name for the load control signal for the given RF port.
 *
 * @param rfName Name of the RF.
 * @param portName Name of the RF data port.
 * @param async Flag to generate name for asynchronous signal. Default value
 *              is false.
 * @return The name of the signal.
 */
std::string
DefaultDecoderGenerator::rfLoadSignalName(
    const std::string& rfName,
    const std::string& portName,
    bool async) {
    
    if (async) {
        return rfLoadCntrlPort(rfName, portName) + "_wire";
    } else {
        return rfLoadCntrlPort(rfName, portName) + "_reg";
    }
}


/**
 * Returns the name for the signal of opcode control port of the given 
 * RF port.
 *
 * @param rfName Name of the register file.
 * @param portName Name of the RF data port.
 * @param async Flag to generate name for asynchronous signal. Default value
 *              is false.
 * @return The name of the signal.
 */
std::string
DefaultDecoderGenerator::rfOpcodeSignalName(
    const std::string& rfName, 
    const std::string& portName,
    bool async) {
    
    if (async) {
        return rfOpcodeCntrlPort(rfName, portName) + "_wire";
    } else {
        return rfOpcodeCntrlPort(rfName, portName) + "_reg";
    }
}   

/**
 * Returns the name of the opcode control port of the given RF port.
 *
 * @param rfName Name of the RF.
 * @param portName Name of the RF data port.
 * @return The opcode control port in decoder.
 */
std::string
DefaultDecoderGenerator::rfOpcodeCntrlPort(
    const std::string& rfName,
    const std::string& portName) {
    
    return "rf_" + rfName + "_" + portName + "_opc";
}


/**
 * Returns the name of the opcode control port of the given IU read port 
 * in decoder.
 *
 * @param unitName Name of the IU.
 * @param portName Name of the read port.
 * @return The name of the opcode control port.
 */
std::string
DefaultDecoderGenerator::iuReadOpcodeCntrlPort(
    const std::string& unitName,
    const std::string& portName) {
    
    return "iu_" + unitName + "_" + portName + "_read_opc";
}


/**
 * Returns the name of the opcode control signal of the given IU read port
 * in decoder.
 *
 * @param unitName Name of the IU.
 * @param portName Name of the read port.
 * @return The name of the opcode control port.
 */
std::string
DefaultDecoderGenerator::iuReadOpcodeCntrlSignal(
    const std::string& unitName,
    const std::string& portName) {

    return iuReadOpcodeCntrlPort(unitName, portName) + "_reg";
}


/**
 * Returns the name of the load control port of the given IU read port in
 * in decoder.
 *
 * @param unitName Name of the IU.
 * @param portName Name of the read port.
 * @return The name of the load control port.
 */
std::string
DefaultDecoderGenerator::iuReadLoadCntrlPort(
    const std::string& unitName,
    const std::string& portName) {

    return "iu_" + unitName + "_" + portName + "_read_load";
}


/**
 * Returns the name of the load control signal of the given IU read port in
 * in decoder.
 *
 * @param unitName Name of the IU.
 * @param portName Name of the read port.
 * @return The name of the load control port.
 */
std::string
DefaultDecoderGenerator::iuReadLoadCntrlSignal(
    const std::string& unitName,
    const std::string& portName) {

    return iuReadLoadCntrlPort(unitName, portName) + "_reg";
}


/**
 * Returns the name of the IU write port of the given IU in decoder.
 *
 * @param iuName Name of the IU.
 * @return The name of the port.
 */
std::string
DefaultDecoderGenerator::iuWritePort(const std::string& iuName) {
    return "iu_" + iuName + "_write";
}


/**
 * Returns the name of the IU write signal of the given IU in decoder.
 *
 * @param iuName Name of the IU.
 * @return The name of the port.
 */
std::string
DefaultDecoderGenerator::iuWriteSignal(const std::string& iuName) {
    return iuWritePort(iuName) + "_reg";
}


/**
 * Returns the name of the opcode control port of the write port of the given
 * IU in decoder.
 *
 * @param unitName Name of the IU.
 * @return The name of the opcode control port.
 */
std::string
DefaultDecoderGenerator::iuWriteOpcodeCntrlPort(const std::string& unitName) {
    return iuWritePort(unitName) + "_opc";
}


/**
 * Returns the name of the opcode control signal of the write port of the given
 * IU in decoder.
 *
 * @param unitName Name of the IU.
 * @return The name of the opcode control port.
 */
std::string
DefaultDecoderGenerator::iuWriteOpcodeCntrlSignal(
    const std::string& unitName) {
    return iuWriteOpcodeCntrlPort(unitName) + "_reg";
}


/**
 * Returns the name of the load control port of the write port of the given
 * IU in decoder.
 *
 * @param unitName Name of the IU.
 * @return The name of the load control port.
 */
std::string
DefaultDecoderGenerator::iuWriteLoadCntrlPort(const std::string& unitName) {
    return iuWritePort(unitName) + "_load";
}


/**
 * Returns the name of the load control Signal of the write port of the given
 * IU in decoder.
 *
 * @param unitName Name of the IU.
 * @return The name of the load control port.
 */
std::string
DefaultDecoderGenerator::iuWriteLoadCntrlSignal(const std::string& unitName) {
    return iuWriteLoadCntrlPort(unitName) + "_reg";
}


/**
 * Returns the name of the guard port in decoder for the given guard.
 *
 * @param guard The guard.
 * @return The name of the port.
 */
std::string
DefaultDecoderGenerator::guardPortName(const TTAMachine::Guard& guard) {
    const PortGuard* portGuard = dynamic_cast<const PortGuard*>(&guard);
    const RegisterGuard* regGuard = dynamic_cast<const RegisterGuard*>(
        &guard);
    if (portGuard != NULL) {
        FUPort* port = portGuard->port();
        FunctionUnit* fu = port->parentUnit();
        return "fu_guard_" + fu->name() + "_" + port->name();
    } else if (regGuard != NULL) {
        RegisterFile* rf = regGuard->registerFile();
        return "rf_guard_" + rf->name() + "_" + 
            Conversion::toString(regGuard->registerIndex());
    } else {
        return "";
    }
}


/**
 * Returns the name of the bus connection control port of the given 
 * socket.
 *
 * @param name Name of the socket.
 * @return The name of the control port.
 */
std::string
DefaultDecoderGenerator::socketBusControlPort(const std::string& name) {
    return "socket_" + name + "_bus_cntrl";
}


/**
 * Returns the name of the data control port of the given socket.
 *
 * @param name Name of the socket.
 * @return The name of the control port.
 */
std::string
DefaultDecoderGenerator::socketDataControlPort(const std::string& name) {
    return "socket_" + name + "_data_cntrl";
}


/**
 * Returns the name of the signal for the source field of the given bus.
 *
 * @param busName Name of the bus.
 * @return The name of the signal.
 */
std::string
DefaultDecoderGenerator::srcFieldSignal(const std::string& busName) {
    return "src_" + busName;
}
 
   
/**
 * Returns the name of the signal for the destination field of the given
 * bus.
 *
 * @param busName Name of the bus.
 * @return The name of the signal.
 */
std::string
DefaultDecoderGenerator::dstFieldSignal(const std::string& busName) {
    return "dst_" + busName;
}


/**
 * Returns the name of the signal for the guard field of the given bus.
 *
 * @param busName Name of the bus.
 * @return The name of the signal.
 */
std::string
DefaultDecoderGenerator::guardFieldSignal(const std::string& busName) {
    return "grd_" + busName;
}


/**
 * Returns the name of the signal for the given immediate slot.
 *
 * @param immSlot Name of the immediate slot.
 * @return The name of the signal.
 */
std::string
DefaultDecoderGenerator::immSlotSignal(const std::string& immSlot) {
    return "limmslot_" + immSlot;
}


/**
 * Returns the name of the squash signal for the given bus.
 *
 * @param busName Name of the bus.
 * @return The name of the signal.
 */
std::string
DefaultDecoderGenerator::squashSignal(const std::string& busName) {
    return "squash_" + busName;
}


/**
 * Returns the name of the bus connection control signal for the socket
 * of the given name.
 *
 * @param name Name of the socket.
 * @return The name of the control signal.
 */
std::string
DefaultDecoderGenerator::socketBusCntrlSignalName(const std::string& name) {
    return socketBusControlPort(name) + "_reg";
}


/**
 * Returns the name of the data control signal for the socket of the
 * given name.
 *
 * @param name Name of the socket.
 * @return The name of the control signal.
 */
std::string
DefaultDecoderGenerator::socketDataCntrlSignalName(const std::string& name) {
    return socketDataControlPort(name) + "_reg";
}


/**
 * Returns the real name of the GCU data port that has the given name in ADF.
 *
 * @param nameInADF Name of the port in ADF.
 */
std::string
DefaultDecoderGenerator::gcuDataPort(const std::string& nameInADF) {
    return nameInADF;
}


/**
 * Returns the width of the opcode of the given FU.
 *
 * @param fu The FU.
 * @return The width of the operation code.
 */
int
DefaultDecoderGenerator::opcodeWidth(
    const TTAMachine::FunctionUnit& fu) const {

    if (&fu == machine_.controlUnit()) {
        return 1;
    } else {
        try {
            FUEntry& entry = nlGenerator_->fuEntry(fu.name());
            FUImplementation& impl = entry.implementation();
            return impl.maxOpcodeWidth();
        } catch (const Exception& e) {
            assert(false);
        }
    }
}


/**
 * Returns the number of bits required to control the bus connections of
 * the given socket.
 *
 * @param socket The socket.
 * @return The control width.
 */
int
DefaultDecoderGenerator::busControlWidth(const TTAMachine::Socket& socket) {
    if (socket.direction() == Socket::INPUT) {
        return MathTools::bitLength(socket.segmentCount() - 1);
    } else {
        return socket.segmentCount();
    }
}


/**
 * Returns the number of bits required to control from which port the
 * data is written to the bus.
 *
 * @param socket The socket.
 * @return The control width.
 */
int
DefaultDecoderGenerator::dataControlWidth(const TTAMachine::Socket& socket) {
    if (socket.direction() == Socket::OUTPUT) {
        return MathTools::bitLength(socket.portCount() - 1);
    } else {
        return 0;
    }
}


/**
 * Returns the width of the opcode port in the given RF.
 *
 * @param rf The register file.
 * @return The bit width of the opcode port.
 */
int
DefaultDecoderGenerator::rfOpcodeWidth(const TTAMachine::BaseRegisterFile& rf) {
    return MathTools::bitLength(rf.numberOfRegisters() - 1);
}


/**
 * Returns a set of buses connected to the given socket.
 *
 * @param socket The socket.
 * @return The bus set.
 */
DefaultDecoderGenerator::BusSet
DefaultDecoderGenerator::connectedBuses(const TTAMachine::Socket& socket) {
    BusSet set;
    int segmentCount = socket.segmentCount();
    for (int i = 0; i < segmentCount; i++) {
        set.insert(socket.segment(i)->parentBus());
    }
    return set;
}


/**
 * Returns the condition when data is transferred by the given socket in the
 * given source field.
 *
 * @param srcField The source field.
 * @param socketName Name of the socket.
 * @return The conditional clause.
 */
std::string
DefaultDecoderGenerator::socketEncodingCondition(
    const ProGe::HDL language,
    const SlotField& slotField,
    const std::string& socketName) {
    
    string signalName;
    if (dynamic_cast<const SourceField*>(&slotField) != NULL) {
        signalName = srcFieldSignal(slotField.parent()->name());
    } else {
        signalName = dstFieldSignal(slotField.parent()->name());
    }
    SocketEncoding& enc = slotField.socketEncoding(socketName);
    int start = enc.socketIDPosition();
    int end = start + enc.socketIDWidth() - 1;
    if (language == VHDL) {
        if (enc.socketIDWidth() == 0) {
            return "true";
        } else {
            return "conv_integer(unsigned(" + signalName + "("
                + Conversion::toString(end) + " downto "
                + Conversion::toString(start)
                + "))) = " + Conversion::toString(enc.encoding());
        }
    } else {
        if (enc.socketIDWidth() == 0) {
            return "1";
        } else {
            return signalName + "["
                + Conversion::toString(end) + " : "
                + Conversion::toString(start)
                + "] == " + Conversion::toString(enc.encoding());
        }
    }
}


/**
 * Returns the condition when given port code of the given socket encoding is
 * true.
 *
 * @param socketEnc The socket encoding;
 * @param code The RF port code.
 * @return The condition.
 */
std::string
DefaultDecoderGenerator::portCodeCondition(
    const ProGe::HDL language,
    const SocketEncoding& socketEnc,
    const PortCode& code) {

    // empty encoding is always true
    if (!code.encodingWidth()) {
        if (language==VHDL) {
            return "true";
        } else {
            return "1"; 
        }
    }
    SlotField* parent = socketEnc.parent();
    string signalName;
    if (dynamic_cast<SourceField*>(parent) != NULL) {
        signalName = srcFieldSignal(parent->parent()->name());
    } else {
        signalName = dstFieldSignal(parent->parent()->name());
    }

    int codeStart;
    if (parent->componentIDPosition() == BinaryEncoding::RIGHT) {
        codeStart = socketEnc.socketIDWidth() + code.indexWidth();
    } else {
        codeStart = code.indexWidth();
    }
    int codeEnd = codeStart + code.encodingWidth() - 1;
    assert(codeEnd >= codeStart);

    if(language==VHDL)
        return "conv_integer(unsigned(" + signalName + "(" + 
            Conversion::toString(codeEnd) + " downto " + 
            Conversion::toString(codeStart) + "))) = " + 
            Conversion::toString(code.encoding());
    else
    return signalName + "[" + 
        Conversion::toString(codeEnd) + " : " + 
        Conversion::toString(codeStart) + "] == " + 
        Conversion::toString(code.encoding());    
}


/**
 * Returns the condition when the instruction is of the given template.
 *
 * @param iTempName Name of the instruction template.
 * @return The condition.
 */
std::string
DefaultDecoderGenerator::instructionTemplateCondition(
    const ProGe::HDL language,
    const std::string& iTempName) const {

    assert(bem_.hasImmediateControlField());
    ImmediateControlField& field = bem_.immediateControlField();
    assert(field.hasTemplateEncoding(iTempName));
    if(language==VHDL)
        return "conv_integer(unsigned(" + LIMM_TAG_SIGNAL + ")) = "
            + Conversion::toString(field.templateEncoding(iTempName));
    else
        return LIMM_TAG_SIGNAL + " == "
            + Conversion::toString(field.templateEncoding(iTempName));
}


/**
 * Returns the part of the source or destination field signal that contains the
 * opcode of the given RF port code.
 *
 * @param socketEnc The socket encoding.
 * @param code The RF port code.
 * @return The part of the source or destination field signal.
 */
std::string
DefaultDecoderGenerator::rfOpcodeFromSrcOrDstField(
    const ProGe::HDL language,
    const SocketEncoding& socketEnc,
    const PortCode& code) {

    SlotField* slotField = socketEnc.parent();
    string signalName;
    if (dynamic_cast<SourceField*>(slotField) != NULL) {
        signalName = srcFieldSignal(slotField->parent()->name());
    } else {
        signalName = dstFieldSignal(slotField->parent()->name());
    }

    int opcStart;
    SocketCodeTable& table = socketEnc.socketCodes();
    if (slotField->componentIDPosition() == BinaryEncoding::RIGHT) {
        opcStart = slotField->width() - table.width();
    } else {
        opcStart = 0;
    }
    int opcEnd = opcStart + code.indexWidth() - 1;
    if(language==VHDL)
        return signalName + "(" + Conversion::toString(opcEnd) + " downto " + 
            Conversion::toString(opcStart) + ")";
    else
        return signalName + "[" + Conversion::toString(opcEnd) + " : " + 
            Conversion::toString(opcStart) + "]";
}


/**
 * Returns the signal pin that controls the bus of the given output socket.
 *
 * @param socket The socket.
 * @param bus The bus.
 * @return The signal pin.
 */
std::string
DefaultDecoderGenerator::busCntrlSignalPinOfSocket(
    const TTAMachine::Socket& socket,
    const TTAMachine::Bus& bus) const {

    assert(socket.direction() == Socket::OUTPUT);
    int pin = icGenerator_.outputSocketCntrlPinForSegment(
        socket, *bus.segment(0));
    return socketBusCntrlSignalName(socket.name())
        + ((language_==VHDL)?"(":"[")
        + Conversion::toString(pin)
        + ((language_==VHDL)?")":"]");
}


/**
 * Returns the opcode of the given operation.
 *
 * @param operation The operation.
 * @return The opcode.
 */
int
DefaultDecoderGenerator::opcode(
    const TTAMachine::HWOperation& operation) const {

    string fuName = operation.parentUnit()->name();
    if (fuName == machine_.controlUnit()->name()) {
        if (StringTools::ciEqual(operation.name(), CALL)) {
            return 0;
        } else if (StringTools::ciEqual(operation.name(), JUMP)) {
            return 1;
        }
        assert(false);
    } else {
        try {
            FUEntry& entry = nlGenerator_->fuEntry(fuName);
            FUImplementation& impl = entry.implementation();
            return impl.opcode(operation.name());
        } catch (const Exception&) {
            std::cerr << "Implementation for FU: " << fuName << 
                " required for operation: " << operation.name() <<
                " not found!" << std::endl;
            assert(false);
        }
    }
    assert(false && "should not get here");
    return -1; // 
}

/**
 * Generates an indentation of the given level.
 *
 * @param level The level.
 */
std::string
DefaultDecoderGenerator::indentation(unsigned int level) {
    string indentation("");
    for (unsigned int i = 0; i < level; i++) {
        indentation += "  ";
    }
    return indentation;
}

/**
 * Queries if given register file by name has separate address cycle (SAC)
 * flag enabled.
 *
 * @param rfName Name of register file in ADF.
 * @return Status of the SAC flag.
 */
bool
DefaultDecoderGenerator::sacEnabled(const string& rfName) const
{
    assert(nlGenerator_ != NULL);
    const RFEntry& rfEntry = nlGenerator_->rfEntry(rfName);
    return rfEntry.implementation().separateAddressCycleParameter();
}
