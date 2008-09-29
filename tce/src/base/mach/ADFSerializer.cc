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
 * @file ADFSerializer.cc
 *
 * Implementation of ADFSerializer class.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen-no.spam-tut.fi)
 * @note reviewed 15 Jun 2004 by pj, vpj, ml, ll
 * @note rating: red
 */

#include "ADFSerializer.hh"
#include "Bus.hh"
#include "Guard.hh"
#include "Segment.hh"
#include "Socket.hh"
#include "Bridge.hh"
#include "FunctionUnit.hh"
#include "HWOperation.hh"
#include "ExecutionPipeline.hh"
#include "FUPort.hh"
#include "SpecialRegisterPort.hh"
#include "RegisterFile.hh"
#include "ImmediateUnit.hh"
#include "AddressSpace.hh"
#include "ControlUnit.hh"
#include "Connection.hh"
#include "InstructionTemplate.hh"
#include "TemplateSlot.hh"
#include "ADFSerializerTextGenerator.hh"
#include "Environment.hh"

using std::string;
using boost::format;
using namespace TTAMachine;

// declaration of constant strings used in MDF file
const string MDF = "adf";
const string MDF_VERSION = "version";
// writes files according to ADF specs version 1.7.x
const string MDF_VERSION_NUMBER = "1.7";
const string BUS = "bus";
const string BUS_NAME = "name";
const string BUS_WIDTH = "width";
const string SHORT_IMMEDIATE = "short-immediate";
const string SHORT_IMM_WIDTH = "width";
const string SHORT_IMM_EXTENSION = "extension";
const string SIGN_EXTENSION = "sign";
const string ZERO_EXTENSION = "zero";

const string GUARD = "guard";
const string INVERTED_EXPR = "inverted-expr";
const string SIMPLE_EXPR = "simple-expr";
const string GUARD_BOOL = "bool";
const string GUARD_REGFILE_NAME = "name";
const string GUARD_REGFILE_INDEX = "index";
const string GUARD_UNIT = "unit";
const string GUARD_UNIT_NAME = "name";
const string GUARD_UNIT_PORT = "port";
const string ALWAYS_FALSE_GUARD = "always-false";
const string ALWAYS_TRUE_GUARD = "always-true";
const string SEGMENT = "segment";
const string SEGMENT_NAME = "name";
const string SEGMENT_WRITES_TO = "writes-to";

const string SOCKET = "socket";
const string SOCKET_NAME = "name";
const string SOCKET_READS_FROM = "reads-from";
const string SOCKET_WRITES_TO = "writes-to";
const string SOCKET_BUS = "bus";
const string SOCKET_SEGMENT = "segment";

const string BRIDGE = "bridge";
const string BRIDGE_NAME = "name";
const string BRIDGE_READS_FROM = "reads-from";
const string BRIDGE_WRITES_TO = "writes-to";

const string FUNCTION_UNIT = "function-unit";
const string FU_NAME = "name";
const string FU_ADDRESS_SPACE = "address-space";
const string FU_OPERATION = "operation";
const string FU_OP_NAME = "name";
const string FU_OP_BIND = "bind";
const string FU_OP_BIND_NAME = "name";
const string FU_OP_PIPELINE = "pipeline";
const string FU_OP_PL_RESOURCE = "resource";
const string FU_OP_PL_RES_NAME = "name";
const string FU_OP_PL_RES_START_CYCLE = "start-cycle";
const string FU_OP_PL_RES_CYCLES = "cycles";
const string FU_OP_PL_READS = "reads";
const string FU_OP_PL_READS_NAME = "name";
const string FU_OP_PL_WRITES = "writes";
const string FU_OP_PL_WRITES_NAME = "name";

const string REGISTER_FILE = "register-file";
const string RF_NAME = "name";
const string RF_TYPE = "type";
const string RF_TYPE_NORMAL = "normal";
const string RF_TYPE_VOLATILE = "volatile";
const string RF_TYPE_RESERVED = "reserved";
const string RF_SIZE = "size";
const string RF_WIDTH = "width";
const string RF_MAX_READS = "max-reads";
const string RF_MAX_WRITES = "max-writes";
const string RF_GUARD_LATENCY = "guard-latency";

const string IMMEDIATE_UNIT = "immediate-unit";
const string IU_NAME = "name";
const string IU_TYPE = "type";
const string IU_TYPE_NORMAL = "normal";
const string IU_TYPE_VOLATILE = "volatile";
const string IU_TYPE_RESERVED = "reserved";
const string IU_SIZE = "size";
const string IU_WIDTH = "width";
const string IU_MAX_READS = "max-reads";
const string IU_MAX_WRITES = "max-writes";
const string IU_EXTENSION = "extension";
const string IU_GUARD_LATENCY = "guard-latency";
const string IU_TEMPLATE = "template";
const string IU_TEMPLATE_NAME = "name";
const string IU_TEMPLATE_SLOT = "slot";
const string IU_TEMPLATE_SLOT_NAME = "name";
const string IU_TEMPLATE_SLOT_WIDTH = "width";
const string IU_TEMPLATE_BUS = "bus";

const string ADDRESS_SPACE = "address-space";
const string AS_NAME = "name";
const string AS_WIDTH = "width";
const string AS_MIN_ADDRESS = "min-address";
const string AS_MAX_ADDRESS = "max-address";

const string CONTROL_UNIT = "global-control-unit";
const string CU_NAME = "name";
const string CU_ADDRESS_SPACE = "address-space";
const string CU_DELAY_SLOTS = "delay-slots";
const string CU_GUARD_LATENCY = "guard-latency";
const string CU_RETURN_ADDRESS = "return-address";
const string CU_CTRL_OPERATION = "ctrl-operation";
const string CU_SPECIAL_PORT = "special-port";

const string PORT = "port";
const string PORT_NAME = "name";
const string PORT_CONNECTS_TO = "connects-to";

const string FU_PORT_WIDTH = "width";
const string FU_PORT_TRIGGERS = "triggers";
const string FU_PORT_SETS_OPCODE = "sets-opcode";

const string IMMEDIATE_SLOT = "immediate-slot";
const string IMMEDIATE_SLOT_NAME = "name";

const string ADF_SCHEMA_FILE = "mach/ADF_Schema.xsd";


/**
 * Constructor.
 */
ADFSerializer::ADFSerializer() : XMLSerializer() {
    setSchemaFile(Environment::schemaDirPath(ADF_SCHEMA_FILE));
    setUseSchema(true);
}


/**
 * Destructor.
 */
ADFSerializer::~ADFSerializer() {
}


/**
 * Writes the given ObjectState tree created by Machine::saveState to the
 * destination file.
 *
 * @param machineState ObjectState tree created by Machine::saveState.
 * @exception SerializerException If an error occurs while serializing.
 */
void
ADFSerializer::writeState(const ObjectState* machineState)
    throw (SerializerException) {

    ObjectState* converted = convertToMDFFormat(machineState);
    XMLSerializer::writeState(converted);
    delete converted;
}


/**
 * Reads the current MDF file set and creates an ObjectState tree which can
 * be given to Machine::loadState to create a machine.
 *
 * @return The newly created ObjectState tree.
 * @exception SerializerException If an error occurs while reading.
 */
ObjectState*
ADFSerializer::readState()
    throw (SerializerException) {

    ObjectState* mdfState = XMLSerializer::readState();
    ObjectState* machineState;
    try {
        machineState = convertToMachineFormat(mdfState);
    } catch (const SerializerException&) {
        delete mdfState;
        throw;
    }

    delete mdfState;
    return machineState;
}


/**
 * Serializes the given machine to mdf file.
 *
 * @param machine The machine to be serialized.
 * @exception SerializerException If an error occurs while serializing.
 */
void
ADFSerializer::writeMachine(const Machine& machine)
    throw (SerializerException) {

    ObjectState* machineState = machine.saveState();
    writeState(machineState);
    delete machineState;
}


/**
 * Reads the current MDF file set and creates a Machine according to it.
 *
 * @return The newly created machine.
 * @exception SerializerException If an error occurs while reading the MDF
 *                                file.
 * @exception ObjectStateLoadingException If an error occurs while creating
 *                                        Machine.
 */
Machine*
ADFSerializer::readMachine()
    throw (SerializerException, ObjectStateLoadingException) {

    ObjectState* machineState = readState();
    Machine* mach = new Machine();
    mach->loadState(machineState);
    delete machineState;
    return mach;
}


/**
 * Converts the given ObjectState tree created by Machine to correspond
 * with MDF format.
 *
 * @param machineState Root of the ObjectState tree.
 * @return The newly created ObjectState tree which matches with MDF format.
 */
ObjectState*
ADFSerializer::convertToMDFFormat(const ObjectState* machineState) {

    ObjectState* root = new ObjectState(MDF);
    root->setAttribute(MDF_VERSION, MDF_VERSION_NUMBER);

    // add buses
    for (int i = 0; i < machineState->childCount(); i++) {
        ObjectState* child = machineState->child(i);
        if (child->name() == Bus::OSNAME_BUS) {
            root->addChild(busToMDF(child));
        }
    }

    // add sockets
    for (int i = 0; i < machineState->childCount(); i++) {
        ObjectState* child = machineState->child(i);
        if (child->name() == Socket::OSNAME_SOCKET) {
            root->addChild(socketToMDF(child));
        }
    }

    // add bridges
    for (int i = 0; i < machineState->childCount(); i++) {
        ObjectState* child = machineState->child(i);
        if (child->name() == Bridge::OSNAME_BRIDGE) {
            root->addChild(bridgeToMDF(child));
        }
    }

    // add function units
    for (int i = 0; i < machineState->childCount(); i++) {
        ObjectState* child = machineState->child(i);
        if (child->name() == FunctionUnit::OSNAME_FU) {
            root->addChild(functionUnitToMDF(child));
        }
    }

    // add register files
    for (int i = 0; i < machineState->childCount(); i++) {
        ObjectState* child = machineState->child(i);
        if (child->name() == RegisterFile::OSNAME_REGISTER_FILE) {
            root->addChild(registerFileToMDF(child));
        }
    }

    // add address spaces
    for (int i = 0; i < machineState->childCount(); i++) {
        ObjectState* child = machineState->child(i);
        if (child->name() == AddressSpace::OSNAME_ADDRESS_SPACE) {
            root->addChild(addressSpaceToMDF(child));
        }
    }

    // add control unit
    for (int i = 0; i < machineState->childCount(); i++) {
        ObjectState* child = machineState->child(i);
        if (child->name() == ControlUnit::OSNAME_CONTROL_UNIT) {
            root->addChild(controlUnitToMDF(child));
        }
    }

    // add immediate units
    for (int i = 0; i < machineState->childCount(); i++) {
        ObjectState* child = machineState->child(i);
        if (child->name() == ImmediateUnit::OSNAME_IMMEDIATE_UNIT) {
            root->addChild(immediateUnitToMDF(child, machineState));
        }
    }

    // add immediate slots
    for (int i = 0; i < machineState->childCount(); i++) {
        ObjectState* child = machineState->child(i);
        if (child->name() == ImmediateSlot::OSNAME_IMMEDIATE_SLOT) {
            root->addChild(immediateSlotToMDF(child));
        }
    }

    return root;
}


/**
 * Creates a new ObjectState tree which matches one-to-one with MDF specs
 * syntax.
 *
 * The tree is created according to the given ObjectState tree created by
 * Bus::saveState.
 *
 * @param busState Root node of the ObjectState tree created by
 *                 Bus::saveState.
 * @return The newly created ObjectState tree.
 */
ObjectState*
ADFSerializer::busToMDF(const ObjectState* busState) {

    ObjectState* root = new ObjectState(BUS);
    root->setAttribute(BUS_NAME, busState->stringAttribute(Bus::OSKEY_NAME));

    // add width element
    ObjectState* widthChild = new ObjectState(BUS_WIDTH);
    widthChild->setValue(busState->stringAttribute(Bus::OSKEY_WIDTH));
    root->addChild(widthChild);

    // add guards
    for (int i = 0; i < busState->childCount(); i++) {
        ObjectState* child = busState->child(i);

        if (child->name() == PortGuard::OSNAME_PORT_GUARD ||
            child->name() == RegisterGuard::OSNAME_REGISTER_GUARD) {
            string guardType;
            string guardUnit;
            string guardTarget;
            string unitNameAttrib;
            string targetNameAttrib;
            if (child->name() == PortGuard::OSNAME_PORT_GUARD) {
                guardType = GUARD_UNIT;
                guardUnit = GUARD_UNIT_NAME;
                guardTarget = GUARD_UNIT_PORT;
                unitNameAttrib = PortGuard::OSKEY_FU;
                targetNameAttrib = PortGuard::OSKEY_PORT;
            } else if (child->name() ==
                       RegisterGuard::OSNAME_REGISTER_GUARD) {
                guardType = GUARD_BOOL;
                guardUnit = GUARD_REGFILE_NAME;
                guardTarget = GUARD_REGFILE_INDEX;
                unitNameAttrib = RegisterGuard::OSKEY_REGFILE;
                targetNameAttrib = RegisterGuard::OSKEY_INDEX;
            }

            ObjectState* guardChild = new ObjectState(GUARD);
            root->addChild(guardChild);
            ObjectState* conditionalChild;
            if (child->intAttribute(Guard::OSKEY_INVERTED)) {
                conditionalChild = new ObjectState(INVERTED_EXPR);
            } else {
                conditionalChild = new ObjectState(SIMPLE_EXPR);
            }
            guardChild->addChild(conditionalChild);
            ObjectState* typeChild = new ObjectState(guardType);
            conditionalChild->addChild(typeChild);
            ObjectState* unitChild = new ObjectState(guardUnit);
            typeChild->addChild(unitChild);
            unitChild->setValue(child->stringAttribute(unitNameAttrib));
            ObjectState* targetChild = new ObjectState(guardTarget);
            typeChild->addChild(targetChild);
            targetChild->setValue(child->stringAttribute(targetNameAttrib));

        } else if (child->name() ==
                   UnconditionalGuard::OSNAME_UNCONDITIONAL_GUARD) {

            // unconditional guards
            ObjectState* guardChild = new ObjectState(GUARD);
            root->addChild(guardChild);
            if (child->boolAttribute(Guard::OSKEY_INVERTED)) {
                guardChild->addChild(new ObjectState(ALWAYS_FALSE_GUARD));
            } else {
                guardChild->addChild(new ObjectState(ALWAYS_TRUE_GUARD));
            }
        }
    }

    // add segments
    for (int i = 0; i < busState->childCount(); i++) {
        ObjectState* child = busState->child(i);

        if (child->name() == Segment::OSNAME_SEGMENT) {
            ObjectState* segment = new ObjectState(SEGMENT);
            root->addChild(segment);
            segment->setAttribute(
                SEGMENT_NAME, child->stringAttribute(Segment::OSKEY_NAME));
            ObjectState* writesTo = new ObjectState(SEGMENT_WRITES_TO);
            segment->addChild(writesTo);
            if (child->hasAttribute(Segment::OSKEY_DESTINATION)) {
                writesTo->setValue(
                    child->stringAttribute(Segment::OSKEY_DESTINATION));
            }
        }
    }

    // add short immediate element
    ObjectState* sImmChild = new ObjectState(SHORT_IMMEDIATE);
    root->addChild(sImmChild);
    ObjectState* extensionChild = new ObjectState(SHORT_IMM_EXTENSION);
    string extension = busState->stringAttribute(Bus::OSKEY_EXTENSION);
    if (extension == Bus::OSVALUE_SIGN) {
        extensionChild->setValue(SIGN_EXTENSION);
    } else if (extension == Bus::OSVALUE_ZERO) {
        extensionChild->setValue(ZERO_EXTENSION);
    } else {
        string procName = "MDFSerialiser::busToMDF";
        string errorMsg = "Unknown extension attribute value in ObjectState "
            "instance.";
        Application::writeToErrorLog(__FILE__, __LINE__, procName, errorMsg);
        Application::abortProgram();
    }

    sImmChild->addChild(extensionChild);
    ObjectState* sImmWidthChild = new ObjectState(SHORT_IMM_WIDTH);
    sImmChild->addChild(sImmWidthChild);
    sImmWidthChild->setValue(busState->stringAttribute(Bus::OSKEY_IMMWIDTH));

   return root;
}


/**
 * Creates a new ObjectState tree which matches one-to-one with MDF specs
 * syntax.
 *
 * The tree is created according to the given ObjectState tree created by
 * Socket::saveState.
 *
 * @param socketState Root node of the ObjectState tree created by
 *                    Socket::saveState.
 * @return The newly created ObjectState tree.
 */
ObjectState*
ADFSerializer::socketToMDF(const ObjectState* socketState) {

    ObjectState* socket = new ObjectState(SOCKET);
    socket->setAttribute(
        SOCKET_NAME, socketState->stringAttribute(Socket::OSKEY_NAME));

    bool input;
    if (socketState->stringAttribute(Socket::OSKEY_DIRECTION) ==
        Socket::OSVALUE_INPUT) {
        input = true;
    } else if (socketState->stringAttribute(Socket::OSKEY_DIRECTION) ==
               Socket::OSVALUE_OUTPUT) {
        input = false;
    } else {
        return socket;
    }

    for (int i = 0; i < socketState->childCount(); i++) {
        ObjectState* child = socketState->child(i);
        ObjectState* conn;
        if (input) {
            conn = new ObjectState(SOCKET_READS_FROM);
        } else {
            conn = new ObjectState(SOCKET_WRITES_TO);
        }
        socket->addChild(conn);
        ObjectState* bus = new ObjectState(SOCKET_BUS);
        conn->addChild(bus);
        bus->setValue(child->stringAttribute(Connection::OSKEY_BUS));

        ObjectState* segment = new ObjectState(SOCKET_SEGMENT);
        conn->addChild(segment);
        if (child->hasAttribute(Connection::OSKEY_SEGMENT)) {
            segment->setValue(
                child->stringAttribute(Connection::OSKEY_SEGMENT));
        }
    }

    return socket;
}


/**
 * Creates a new ObjectState tree which matches one-to-one with MDF specs
 * syntax.
 *
 * The tree is created according to the given ObjectState tree created by
 * Bridge::saveState.
 *
 * @param bridgeState Root node of the ObjectState tree created by
 *                    Bridge::saveState.
 * @return The newly created ObjectState tree.
 */
ObjectState*
ADFSerializer::bridgeToMDF(const ObjectState* bridgeState) {

    ObjectState* bridge = new ObjectState(BRIDGE);
    bridge->setAttribute(
        BRIDGE_NAME, bridgeState->stringAttribute(Bridge::OSKEY_NAME));

    ObjectState* readsFrom = new ObjectState(BRIDGE_READS_FROM);
    bridge->addChild(readsFrom);
    readsFrom->setValue(
        bridgeState->stringAttribute(Bridge::OSKEY_SOURCE_BUS));

    ObjectState* writesTo = new ObjectState(BRIDGE_WRITES_TO);
    bridge->addChild(writesTo);
    writesTo->setValue(
        bridgeState->stringAttribute(Bridge::OSKEY_DESTINATION_BUS));

    return bridge;
}


/**
 * Creates a new ObjectState tree which matches one-to-one with MDF specs
 * syntax.
 *
 * The tree is created according to the given ObjectState tree created by
 * FunctionUnit::saveState.
 *
 * @param fuState Root node of the ObjectState tree created by
 *                FunctionUnit::saveState.
 * @return The newly created ObjectState tree.
 */
ObjectState*
ADFSerializer::functionUnitToMDF(const ObjectState* fuState) {

    ObjectState* fu = new ObjectState(FUNCTION_UNIT);
    fu->setAttribute(
        FU_NAME, fuState->stringAttribute(FunctionUnit::OSKEY_NAME));

    // add ports and operations
    for (int i = 0; i < fuState->childCount(); i++) {
        ObjectState* child = fuState->child(i);
        if (child->name() == FUPort::OSNAME_FUPORT) {
            fu->addChild(mdfFUPort(child));
        } else if (child->name() == HWOperation::OSNAME_OPERATION) {
            fu->addChild(mdfOperation(child));
        }
    }

    // add address space
    ObjectState* addressSpace = new ObjectState(FU_ADDRESS_SPACE);
    fu->addChild(addressSpace);

    // set address space name if it exists
    if (fuState->hasAttribute(FunctionUnit::OSKEY_AS)) {
        addressSpace->setValue(
            fuState->stringAttribute(FunctionUnit::OSKEY_AS));
    }

    return fu;
}


/**
 * Creates a new ObjectState tree which matches one-to-one with MDF specs
 * syntax.
 *
 * The tree is created according to the given ObjectState tree created by
 * RegisterFile::saveState.
 *
 * @param rfState Root node of the ObjectState tree created by
 *                RegisterFile::saveState.
 * @return The newly created ObjectState tree.
 */
ObjectState*
ADFSerializer::registerFileToMDF(const ObjectState* rfState) {

    ObjectState* regFile = new ObjectState(REGISTER_FILE);
    regFile->setAttribute(
        RF_NAME, rfState->stringAttribute(RegisterFile::OSKEY_NAME));

    ObjectState* type = new ObjectState(RF_TYPE);
    regFile->addChild(type);
    string typeValue = rfState->stringAttribute(RegisterFile::OSKEY_TYPE);
    if (typeValue == RegisterFile::OSVALUE_NORMAL) {
        type->setValue(RF_TYPE_NORMAL);
    } else if (typeValue == RegisterFile::OSVALUE_RESERVED) {
        type->setValue(RF_TYPE_RESERVED);
    } else if (typeValue == RegisterFile::OSVALUE_VOLATILE) {
        type->setValue(RF_TYPE_VOLATILE);
    } else {
        string procName = "ADFSerializer::registerFileToMDF";
        string errorMsg = "Unknown register file type in ObjectState "
            "instance.";
        Application::writeToErrorLog(__FILE__, __LINE__, procName, errorMsg);
        Application::abortProgram();
    }

    ObjectState* size = new ObjectState(RF_SIZE);
    regFile->addChild(size);
    size->setValue(rfState->stringAttribute(RegisterFile::OSKEY_SIZE));

    ObjectState* width = new ObjectState(RF_WIDTH);
    regFile->addChild(width);
    width->setValue(rfState->stringAttribute(RegisterFile::OSKEY_WIDTH));

    ObjectState* maxReads = new ObjectState(RF_MAX_READS);
    regFile->addChild(maxReads);
    maxReads->setValue(
        rfState->stringAttribute(RegisterFile::OSKEY_MAX_READS));

    ObjectState* maxWrites = new ObjectState(RF_MAX_WRITES);
    regFile->addChild(maxWrites);
    maxWrites->setValue(
        rfState->stringAttribute(RegisterFile::OSKEY_MAX_WRITES));

    if (rfState->intAttribute(RegisterFile::OSKEY_GUARD_LATENCY) > 0) {
        ObjectState* guardLatency = new ObjectState(RF_GUARD_LATENCY);
        regFile->addChild(guardLatency);
        guardLatency->setValue(
            rfState->stringAttribute(RegisterFile::OSKEY_GUARD_LATENCY));
    }

    // add ports
    for (int i = 0; i < rfState->childCount(); i++) {
        ObjectState* child = rfState->child(i);
        regFile->addChild(mdfPort(child));
    }    

    return regFile;
}


/**
 * Creates a new ObjectState tree which matches one-to-one with MDF specs
 * syntax.
 *
 * The tree is created according to the given ObjectState tree created by
 * ImmediateUnit::saveState. The whole ObjectState tree created by
 * Machine::saveState must be given too because instruction templates are
 * needed too to create the new ObjectState tree.
 *
 * @param iuState Root node of the ObjectState tree created by
 *                ImmediateUnit::saveState.
 * @param machineState The ObjectState tree created by Machine::saveState.
 * @return The newly created ObjectState tree.
 */
ObjectState*
ADFSerializer::immediateUnitToMDF(const ObjectState* iuState,
                                  const ObjectState* machineState) {

    ObjectState* iUnit = new ObjectState(IMMEDIATE_UNIT);
    string iuName = iuState->stringAttribute(ImmediateUnit::OSKEY_NAME);
    iUnit->setAttribute(IU_NAME, iuName);

    // type
    ObjectState* type = new ObjectState(IU_TYPE);
    iUnit->addChild(type);
    string typeValue = iuState->stringAttribute(RegisterFile::OSKEY_TYPE);
    if (typeValue == ImmediateUnit::OSVALUE_NORMAL) {
        type->setValue(IU_TYPE_NORMAL);
    } else if (typeValue == ImmediateUnit::OSVALUE_RESERVED) {
        type->setValue(IU_TYPE_RESERVED);
    } else if (typeValue == ImmediateUnit::OSVALUE_VOLATILE) {
        type->setValue(IU_TYPE_VOLATILE);
    } else {
        string procName = "ADFSerializer::immediateUnitToMDF";
        string errorMsg = "Unknown immediate unit type in ObjectState "
            "instance.";
        Application::writeToErrorLog(__FILE__, __LINE__, procName, errorMsg);
        Application::abortProgram();
    }
    
    // add size element
    ObjectState* size = new ObjectState(IU_SIZE);
    iUnit->addChild(size);
    size->setValue(iuState->stringAttribute(ImmediateUnit::OSKEY_SIZE));

    // add width element
    ObjectState* width = new ObjectState(IU_WIDTH);
    iUnit->addChild(width);
    width->setValue(iuState->stringAttribute(ImmediateUnit::OSKEY_WIDTH));

    // add max reads
    ObjectState* maxReads = new ObjectState(RF_MAX_READS);
    iUnit->addChild(maxReads);
    maxReads->setValue(
        iuState->stringAttribute(RegisterFile::OSKEY_MAX_READS));

    // add max writes
    ObjectState* maxWrites = new ObjectState(IU_MAX_WRITES);
    iUnit->addChild(maxWrites);
    maxWrites->setValue(
        iuState->stringAttribute(RegisterFile::OSKEY_MAX_WRITES));

    // add guard latency
    if (iuState->intAttribute(ImmediateUnit::OSKEY_GUARD_LATENCY) > 0) {
        ObjectState* guardLatency = new ObjectState(IU_GUARD_LATENCY);
        iUnit->addChild(guardLatency);
        guardLatency->setValue(
            iuState->stringAttribute(ImmediateUnit::OSKEY_GUARD_LATENCY));
    }
    
    // add extension element
    ObjectState* extension = new ObjectState(IU_EXTENSION);
    iUnit->addChild(extension);
    extension->setValue(
        iuState->stringAttribute(ImmediateUnit::OSKEY_EXTENSION));

    // add cycles element | removed
    //ObjectState* cycles = new ObjectState(IU_CYCLES);
    //iUnit->addChild(cycles);
    //cycles->setValue(iuState->stringAttribute(ImmediateUnit::OSKEY_LATENCY));

    // add ports
    for (int i = 0; i < iuState->childCount(); i++) {
        ObjectState* child = iuState->child(i);
        if (child->name() == RFPort::OSNAME_RFPORT) {
            iUnit->addChild(mdfPort(child));
        }
    }

    // add instruction templates
    for (int iTempIndex = 0; iTempIndex < machineState->childCount();
         iTempIndex++) {
        ObjectState* iTemp = machineState->child(iTempIndex);
        if (iTemp->name() ==
            InstructionTemplate::OSNAME_INSTRUCTION_TEMPLATE) {
            instructionTemplateToMDF(iTemp, iUnit);
        }
    }

    return iUnit;
}


/**
 * Creates a new ObjectState tree which matches one-to-one with MDF specs
 * syntax.
 *
 * The tree is created according to the given ObjectState tree created by
 * AddressSpace::saveState.
 *
 * @param asState Root node of the ObjectState tree created by
 *                AddressSPace::saveState.
 * @return The newly created ObjectState tree.
 */
ObjectState*
ADFSerializer::addressSpaceToMDF(const ObjectState* asState) {

    ObjectState* as = new ObjectState(ADDRESS_SPACE);
    as->setAttribute(
        AS_NAME, asState->stringAttribute(AddressSpace::OSKEY_NAME));

    // add width element
    ObjectState* width = new ObjectState(AS_WIDTH);
    as->addChild(width);
    width->setValue(asState->stringAttribute(AddressSpace::OSKEY_WIDTH));

    // add min-address element
    ObjectState* minAddress = new ObjectState(AS_MIN_ADDRESS);
    as->addChild(minAddress);
    minAddress->setValue(
        asState->stringAttribute(AddressSpace::OSKEY_MIN_ADDRESS));

    // add max-address element
    ObjectState* maxAddress = new ObjectState(AS_MAX_ADDRESS);
    as->addChild(maxAddress);
    maxAddress->setValue(
        asState->stringAttribute(AddressSpace::OSKEY_MAX_ADDRESS));

    return as;
}


/**
 * Creates a new ObjectState tree which matches one-to-one with MDF specs
 * syntax.
 *
 * The tree is created according to the given ObjectState tree created by
 * ControlUnit::saveState.
 *
 * @param cuState Root node of the ObjectState tree created by
 *                ControlUnit::saveState.
 * @return The newly created ObjectState tree.
 */
ObjectState*
ADFSerializer::controlUnitToMDF(const ObjectState* cuState) {

    ObjectState* cUnit = new ObjectState(CONTROL_UNIT);
    cUnit->setAttribute(
        CU_NAME, cuState->stringAttribute(ControlUnit::OSKEY_NAME));

    // add operand ports
    for (int i = 0; i < cuState->childCount(); i++) {
        ObjectState* child = cuState->child(i);
        if (child->name() == FUPort::OSNAME_FUPORT) {
            cUnit->addChild(mdfFUPort(child));
        }
    }

    // add special ports
    for (int i = 0; i < cuState->childCount(); i++) {
        ObjectState* child = cuState->child(i);
        if (child->name() == SpecialRegisterPort::OSNAME_SPECIAL_REG_PORT) {
            cUnit->addChild(mdfSRPort(child));
        }
    }

    // add return-address element
    ObjectState* raElement = new ObjectState(CU_RETURN_ADDRESS);
    cUnit->addChild(raElement);
    if (cuState->hasAttribute(ControlUnit::OSKEY_RA_PORT)) {
        raElement->setValue(
            cuState->stringAttribute(ControlUnit::OSKEY_RA_PORT));
    }

    // add operations
    for (int i = 0; i < cuState->childCount(); i++) {
        ObjectState* child = cuState->child(i);
        if (child->name() == HWOperation::OSNAME_OPERATION) {
            ObjectState* ctrlOperation = mdfOperation(child);
            ctrlOperation->setName(CU_CTRL_OPERATION);
            cUnit->addChild(ctrlOperation);
        }
    }

    // add address space element
    ObjectState* as = new ObjectState(CU_ADDRESS_SPACE);
    cUnit->addChild(as);
    if (cuState->hasAttribute(FunctionUnit::OSKEY_AS)) {
        as->setValue(cuState->stringAttribute(FunctionUnit::OSKEY_AS));
    }

    // add delay-slots element
    ObjectState* dSlots = new ObjectState(CU_DELAY_SLOTS);
    cUnit->addChild(dSlots);
    dSlots->setValue(
        cuState->stringAttribute(ControlUnit::OSKEY_DELAY_SLOTS));

    // add guard latency element
    ObjectState* gLatency = new ObjectState(CU_GUARD_LATENCY);
    cUnit->addChild(gLatency);
    gLatency->setValue(
        cuState->stringAttribute(ControlUnit::OSKEY_GUARD_LATENCY));

    return cUnit;
}


/**
 * Creates a new ObjectState instance that matches with the syntax of 
 * immediate-slot element in ADF file.
 *
 * @param isState ObjectState instance representing ImmediateSlot instance.
 * @return The newly created ObjectState instance.
 */
ObjectState*
ADFSerializer::immediateSlotToMDF(const ObjectState* isState) {
    ObjectState* immediateSlot = new ObjectState(IMMEDIATE_SLOT);
    immediateSlot->setAttribute(
        IMMEDIATE_SLOT_NAME, 
        isState->stringAttribute(ImmediateSlot::OSKEY_NAME));
    return immediateSlot;
}


/**
 * Creates a new ObjectState tree which can be read by Machine::loadState
 * and which corresponds with the given ObjectState tree.
 *
 * @param mdfState An ObjectState tree whose structure matches with MDF file
 *                 structure.
 * @return The newly created ObjectState tree.
 * @exception SerializerException If an error occurs while converting.
 */
ObjectState*
ADFSerializer::convertToMachineFormat(const ObjectState* mdfState)
    throw (SerializerException) {

    ObjectState* machine = new ObjectState(Machine::OSNAME_MACHINE);

    try {
        for (int i = 0; i < mdfState->childCount(); i++) {
            ObjectState* child = mdfState->child(i);
            if (child->name() == BUS) {
                machine->addChild(busToMachine(child));
            } else if (child->name() == SOCKET) {
                machine->addChild(socketToMachine(child));
            } else if (child->name() == BRIDGE) {
                machine->addChild(bridgeToMachine(child));
            } else if (child->name() == FUNCTION_UNIT) {
                machine->addChild(functionUnitToMachine(child));
            } else if (child->name() == REGISTER_FILE) {
                machine->addChild(registerFileToMachine(child));
            } else if (child->name() == ADDRESS_SPACE) {
                machine->addChild(addressSpaceToMachine(child));
            } else if (child->name() == CONTROL_UNIT) {
                machine->addChild(controlUnitToMachine(child));
            } else if (child->name() == IMMEDIATE_UNIT) {
                machine->addChild(immediateUnitToMachine(child, machine));
            } else if (child->name() == IMMEDIATE_SLOT) {
                machine->addChild(immediateSlotToMachine(child));
            }
        }
    } catch (const SerializerException&) {
        delete machine;
        throw;
    }

    return machine;
}


/**
 * Creates a new ObjectState tree which can be read by Bus::loadState and
 * which corresponds with the given ObjectState tree declaring a bus by
 * MDF syntax.
 *
 * @param busState The ObjectState tree declaring a bus by MDF syntax.
 * @return The newly created ObjectState tree.
 */
ObjectState*
ADFSerializer::busToMachine(const ObjectState* busState) {

    ObjectState* bus = new ObjectState(Bus::OSNAME_BUS);

    // set name
    bus->setAttribute(
        Component::OSKEY_NAME, busState->stringAttribute(BUS_NAME));

    // set width
    ObjectState* width = busState->childByName(BUS_WIDTH);
    bus->setAttribute(Bus::OSKEY_WIDTH, width->stringValue());

    // set immediate width and extension
    ObjectState* shortImm = busState->childByName(SHORT_IMMEDIATE);
    ObjectState* shortImmWidth = shortImm->childByName(SHORT_IMM_WIDTH);
    bus->setAttribute(Bus::OSKEY_IMMWIDTH, shortImmWidth->stringValue());
    ObjectState* shortImmExt = shortImm->childByName(SHORT_IMM_EXTENSION);
    string extension = shortImmExt->stringValue();
    if (extension == SIGN_EXTENSION) {
        bus->setAttribute(Bus::OSKEY_EXTENSION, Bus::OSVALUE_SIGN);
    } else if (extension == ZERO_EXTENSION) {
        bus->setAttribute(Bus::OSKEY_EXTENSION, Bus::OSVALUE_ZERO);
    } else {
        string procName = "ADFSerializer::busToMachine";
        string errorMsg = "Unknown extension value in ObjectState.";
        Application::writeToErrorLog(__FILE__, __LINE__, procName, errorMsg);
        Application::abortProgram();
    }

    // add segments and guards
    for (int i = 0; i < busState->childCount(); i++) {
        ObjectState* child = busState->child(i);
        if (child->name() == SEGMENT) {
            ObjectState* segment = new ObjectState(Segment::OSNAME_SEGMENT);
            bus->addChild(segment);
            segment->setAttribute(
                Segment::OSKEY_NAME, child->stringAttribute(SEGMENT_NAME));
            ObjectState* writesTo = child->childByName(SEGMENT_WRITES_TO);
            string destination = writesTo->stringValue();
            if (destination != "") {
                segment->setAttribute(
                    Segment::OSKEY_DESTINATION, writesTo->stringValue());
            }
        } else if (child->name() == GUARD) {
            ObjectState* guardType = child->child(0);
            if (guardType->name() == ALWAYS_TRUE_GUARD) {
                ObjectState* ucGuard = new ObjectState(
                    UnconditionalGuard::OSNAME_UNCONDITIONAL_GUARD);
                bus->addChild(ucGuard);
                ucGuard->setAttribute(Guard::OSKEY_INVERTED, false);
            } else if (guardType->name() == ALWAYS_FALSE_GUARD) {
                ObjectState* ucGuard = new ObjectState(
                    UnconditionalGuard::OSNAME_UNCONDITIONAL_GUARD);
                bus->addChild(ucGuard);
                ucGuard->setAttribute(Guard::OSKEY_INVERTED, true);
            } else if (guardType->name() == SIMPLE_EXPR ||
                       guardType->name() == INVERTED_EXPR) {
                ObjectState* boolOrUnit = guardType->child(0);
                string objectStateName;
                string unitNameElem;
                string unitNameAttrib;
                string targetNameElem;
                string targetNameAttrib;
                if (boolOrUnit->name() == GUARD_BOOL) {
                    objectStateName = RegisterGuard::OSNAME_REGISTER_GUARD;
                    unitNameElem = GUARD_REGFILE_NAME;
                    unitNameAttrib = RegisterGuard::OSKEY_REGFILE;
                    targetNameElem = GUARD_REGFILE_INDEX;
                    targetNameAttrib = RegisterGuard::OSKEY_INDEX;
                } else if (boolOrUnit->name() == GUARD_UNIT) {
                    objectStateName = PortGuard::OSNAME_PORT_GUARD;
                    unitNameElem = GUARD_UNIT_NAME;
                    unitNameAttrib = PortGuard::OSKEY_FU;
                    targetNameElem = GUARD_UNIT_PORT;
                    targetNameAttrib = PortGuard::OSKEY_PORT;
                } else {
                    string procName = "ADFSerializer::busToMachine";
                    string errorMsg = "Unknown ObjectState name.";
                    Application::writeToErrorLog(
                        __FILE__, __LINE__, procName, errorMsg);
                    Application::abortProgram();
                }

                ObjectState* guard = new ObjectState(objectStateName);
                bus->addChild(guard);
                if (guardType->name() == SIMPLE_EXPR) {
                    guard->setAttribute(Guard::OSKEY_INVERTED, false);
                } else {
                    guard->setAttribute(Guard::OSKEY_INVERTED, true);
                }
                ObjectState* unit = boolOrUnit->childByName(unitNameElem);
                guard->setAttribute(unitNameAttrib, unit->stringValue());
                ObjectState* target = boolOrUnit->childByName(
                    targetNameElem);
                guard->setAttribute(targetNameAttrib, target->stringValue());
            }
        }
    }

    return bus;
}


/**
 * Creates a new ObjectState tree which can be read by Socket::loadState and
 * which corresponds with the given ObjectState tree declaring a socket by
 * MDF syntax.
 *
 * @param socketState The ObjectState tree declaring a socket by MDF syntax.
 * @return The newly created ObjectState tree.
 */
ObjectState*
ADFSerializer::socketToMachine(const ObjectState* socketState) {

    ObjectState* socket = new ObjectState(Socket::OSNAME_SOCKET);

    // set name
    socket->setAttribute(
        Component::OSKEY_NAME, socketState->stringAttribute(SOCKET_NAME));

    // set direction
    if (socketState->hasChild(SOCKET_READS_FROM) ||
        socketState->hasChild(SOCKET_WRITES_TO)) {

        if (socketState->hasChild(SOCKET_READS_FROM)) {
            socket->setAttribute(
                Socket::OSKEY_DIRECTION, Socket::OSVALUE_INPUT);
        } else {
            socket->setAttribute(
                Socket::OSKEY_DIRECTION, Socket::OSVALUE_OUTPUT);
        }

        // add connections
        for (int i = 0; i < socketState->childCount(); i++) {
            ObjectState* srcOrDest = socketState->child(i);
            ObjectState* conn = new ObjectState(
                Connection::OSNAME_CONNECTION);
            socket->addChild(conn);
            conn->setAttribute(
                Connection::OSKEY_SOCKET,
                socketState->stringAttribute(SOCKET_NAME));
            ObjectState* bus = srcOrDest->childByName(SOCKET_BUS);
            conn->setAttribute(Connection::OSKEY_BUS, bus->stringValue());
            ObjectState* segment = srcOrDest->childByName(SOCKET_SEGMENT);
            conn->setAttribute(
                Connection::OSKEY_SEGMENT, segment->stringValue());
        }

    } else {
        socket->setAttribute(
            Socket::OSKEY_DIRECTION, Socket::OSVALUE_UNKNOWN);
    }

    return socket;
}


/**
 * Creates a new ObjectState tree which can be read by Bridge::loadState.
 *
 * The ObjectState tree is created according to the given ObjectState tree
 * which matches with the syntax of bridge declaration in MDF file.
 *
 * @param bridgeState The ObjectState tree which matches with the syntax of
 *                    bridge declaration in MDF file.
 * @return The newly created ObjectState tree.
 */
ObjectState*
ADFSerializer::bridgeToMachine(const ObjectState* bridgeState) {

    ObjectState* bridge = new ObjectState(Bridge::OSNAME_BRIDGE);
    bridge->setAttribute(
        Component::OSKEY_NAME, bridgeState->stringAttribute(BRIDGE_NAME));
    ObjectState* readsFrom = bridgeState->childByName(BRIDGE_READS_FROM);
    bridge->setAttribute(Bridge::OSKEY_SOURCE_BUS, readsFrom->stringValue());
    ObjectState* writesTo = bridgeState->childByName(BRIDGE_WRITES_TO);
    bridge->setAttribute(
        Bridge::OSKEY_DESTINATION_BUS, writesTo->stringValue());
    return bridge;
}


/**
 * Creates a new ObjectState tree which can be read by
 * FunctionUnit::loadState.
 *
 * The ObjectState tree is created according to the given ObjectState tree
 * which matches with the syntax of function unit declaration in MDF file.
 *
 * @param fuState The ObjectState tree which matches with the syntax of
 *                    function unit declaration in MDF file.
 * @return The newly created ObjectState tree.
 */
ObjectState*
ADFSerializer::functionUnitToMachine(const ObjectState* fuState) {

    ObjectState* fu = new ObjectState(FunctionUnit::OSNAME_FU);

    // set name
    fu->setAttribute(
        Component::OSKEY_NAME, fuState->stringAttribute(FU_NAME));

    // set address space
    ObjectState* as = fuState->childByName(FU_ADDRESS_SPACE);
    string asName = as->stringValue();
    if (asName != "") {
        fu->setAttribute(FunctionUnit::OSKEY_AS, asName);
    }

    // set ports and operations
    for (int i = 0; i < fuState->childCount(); i++) {
        ObjectState* child = fuState->child(i);
        if (child->name() == PORT) {
            fu->addChild(machineFUPort(child));
        } else if (child->name() == FU_OPERATION) {
            fu->addChild(machineOperation(child));
        }
    }

    return fu;
}


/**
 * Creates a new ObjectState tree which can be read by
 * RegisterFile::loadState.
 *
 * The ObjectState tree is created according to the given ObjectState tree
 * which matches with the syntax of register file declaration in MDF file.
 *
 * @param rfState The ObjectState tree which matches with the syntax of
 *                register file declaration in MDF file.
 * @return The newly created ObjectState tree.
 */
ObjectState*
ADFSerializer::registerFileToMachine(const ObjectState* rfState) {

    ObjectState* regFile =
        new ObjectState(RegisterFile::OSNAME_REGISTER_FILE);

    // set name
    regFile->setAttribute(
        Component::OSKEY_NAME, rfState->stringAttribute(RF_NAME));

    // set type
    ObjectState* typeChild = rfState->childByName(RF_TYPE);
    string type = typeChild->stringValue();
    if (type == RF_TYPE_NORMAL) {
        regFile->setAttribute(
            RegisterFile::OSKEY_TYPE, RegisterFile::OSVALUE_NORMAL);
    } else if (type == RF_TYPE_RESERVED) {
        regFile->setAttribute(
            RegisterFile::OSKEY_TYPE, RegisterFile::OSVALUE_RESERVED);
    } else if (type == RF_TYPE_VOLATILE) {
        regFile->setAttribute(
            RegisterFile::OSKEY_TYPE, RegisterFile::OSVALUE_VOLATILE);
    } else {
        string procName = "ADFSerializer::registerFileToMachine";
        string errorMsg = "Unknown register file type in ObjectState "
            "instance.";
        Application::writeToErrorLog(__FILE__, __LINE__, procName, errorMsg);
        Application::abortProgram();
    }

    // set size
    ObjectState* size = rfState->childByName(RF_SIZE);
    regFile->setAttribute(BaseRegisterFile::OSKEY_SIZE, size->stringValue());

    // set width
    ObjectState* width = rfState->childByName(RF_WIDTH);
    regFile->setAttribute(
        BaseRegisterFile::OSKEY_WIDTH, width->stringValue());

    // set max reads
    ObjectState* maxReads = rfState->childByName(RF_MAX_READS);
    regFile->setAttribute(
        RegisterFile::OSKEY_MAX_READS, maxReads->stringValue());

    // set max writes
    ObjectState* maxWrites = rfState->childByName(RF_MAX_WRITES);
    regFile->setAttribute(
        RegisterFile::OSKEY_MAX_WRITES, maxWrites->stringValue());
    
    // set guard latency
    if (rfState->hasChild(RF_GUARD_LATENCY)) {
        ObjectState* latencyChild = rfState->childByName(RF_GUARD_LATENCY);
        regFile->setAttribute(
            RegisterFile::OSKEY_GUARD_LATENCY, latencyChild->stringValue());
    } else {
        regFile->setAttribute(RegisterFile::OSKEY_GUARD_LATENCY, 0);
    }

    // set ports
    for (int i = 0; i < rfState->childCount(); i++) {
        ObjectState* child = rfState->child(i);
        if (child->name() == PORT) {
            regFile->addChild(machineRFPort(child));
        }
    }

    return regFile;
}


/**
 * Creates a new ObjectState tree which can be read by
 * AddressSpace::loadState.
 *
 * The ObjectState tree is created according to the given ObjectState tree
 * which matches with the syntax of address space declaration in MDF file.
 *
 * @param asState The ObjectState tree which matches with the syntax of
 *                address space declaration in MDF file.
 * @return The newly created ObjectState tree.
 */
ObjectState*
ADFSerializer::addressSpaceToMachine(const ObjectState* asState) {

    ObjectState* as = new ObjectState(AddressSpace::OSNAME_ADDRESS_SPACE);

    // set name
    as->setAttribute(
        Component::OSKEY_NAME, asState->stringAttribute(AS_NAME));

    // set width
    ObjectState* width = asState->childByName(AS_WIDTH);
    as->setAttribute(AddressSpace::OSKEY_WIDTH, width->stringValue());

    // set min address
    ObjectState* minAddress = asState->childByName(AS_MIN_ADDRESS);
    as->setAttribute(
        AddressSpace::OSKEY_MIN_ADDRESS, minAddress->stringValue());

    // set max address
    ObjectState* maxAddress = asState->childByName(AS_MAX_ADDRESS);
    as->setAttribute(
        AddressSpace::OSKEY_MAX_ADDRESS, maxAddress->stringValue());

    return as;
}


/**
 * Creates a new ObjectState tree which can be read by
 * ControlUnit::loadState.
 *
 * The ObjectState tree is created according to the given ObjectState tree
 * which matches with the syntax of control unit declaration in MDF file.
 *
 * @param cuState The ObjectState tree which matches with the syntax of
 *                control unit declaration in MDF file.
 * @return The newly created ObjectState tree.
 */
ObjectState*
ADFSerializer::controlUnitToMachine(const ObjectState* cuState) {

    ObjectState* cu = new ObjectState(ControlUnit::OSNAME_CONTROL_UNIT);

    // set name
    cu->setAttribute(
        Component::OSKEY_NAME, cuState->stringAttribute(CU_NAME));

    // set address space
    ObjectState* as = cuState->childByName(CU_ADDRESS_SPACE);
    string asName = as->stringValue();
    if (asName != "") {
        cu->setAttribute(FunctionUnit::OSKEY_AS, as->stringValue());
    }

    // set delay slots
    ObjectState* delaySlots = cuState->childByName(CU_DELAY_SLOTS);
    cu->setAttribute(
        ControlUnit::OSKEY_DELAY_SLOTS, delaySlots->stringValue());

    // set the guard latency
    ObjectState* gLatency = cuState->childByName(CU_GUARD_LATENCY);
    cu->setAttribute(
        ControlUnit::OSKEY_GUARD_LATENCY, gLatency->stringValue());
    
    // set ports
    for (int i = 0; i < cuState->childCount(); i++) {
        ObjectState* child = cuState->child(i);
        if (child->name() == PORT) {
            cu->addChild(machineFUPort(child));
        } else if (child->name() == CU_SPECIAL_PORT) {
            cu->addChild(machineSRPort(child));
        }
    }

    // set return address port binding
    ObjectState* raElem = cuState->childByName(CU_RETURN_ADDRESS);
    string raPortName = raElem->stringValue();
    if (raPortName != "") {
        cu->setAttribute(ControlUnit::OSKEY_RA_PORT, raPortName);
    }

    // set operations
    for (int i = 0; i < cuState->childCount(); i++) {
        ObjectState* child = cuState->child(i);
        if (child->name() == CU_CTRL_OPERATION) {
            cu->addChild(machineOperation(child));
        }
    }

    return cu;
}


/**
 * Creates a new ObjectState tree which can be read by
 * ImmediateUnit::loadState.
 *
 * The ObjectState tree is created according to the given ObjectState tree
 * which matches with the syntax of immediate unit declaration in MDF file.
 * Adds also instruction template ObjectStates to the given ObjectState tree
 * which contains the whole machine.
 *
 * @param iuState The ObjectState tree which matches with the syntax of
 *                immediate unit declaration in MDF file.
 * @param machineState The ObjectState tree containing tree instruction
 *                     templates as child objects.
 * @return The newly created ObjectState tree.
 * @exception SerializerException If an error occurs while processing.
 */
ObjectState*
ADFSerializer::immediateUnitToMachine(
    const ObjectState* iuState,
    ObjectState* machineState)
    throw (SerializerException) {

    ObjectState* iu = new ObjectState(ImmediateUnit::OSNAME_IMMEDIATE_UNIT);

    // set name
    iu->setAttribute(
        Component::OSKEY_NAME, iuState->stringAttribute(IU_NAME));

    // set type
    ObjectState* typeChild = iuState->childByName(IU_TYPE);
    string type = typeChild->stringValue();
    if (type == IU_TYPE_NORMAL) {
        iu->setAttribute(
            ImmediateUnit::OSKEY_TYPE, ImmediateUnit::OSVALUE_NORMAL);
    } else if (type == IU_TYPE_RESERVED) {
        iu->setAttribute(
            ImmediateUnit::OSKEY_TYPE, ImmediateUnit::OSVALUE_RESERVED);
    } else if (type == IU_TYPE_VOLATILE) {
        iu->setAttribute(
            ImmediateUnit::OSKEY_TYPE, ImmediateUnit::OSVALUE_VOLATILE);
    } else {
        string procName = "ADFSerializer::registerFileToMachine";
        string errorMsg = "Unknown register file type in ObjectState "
            "instance.";
        Application::writeToErrorLog(__FILE__, __LINE__, procName, errorMsg);
        Application::abortProgram();
    }

    // set size
    ObjectState* size = iuState->childByName(IU_SIZE);
    iu->setAttribute(BaseRegisterFile::OSKEY_SIZE, size->stringValue());

    // set width
    ObjectState* width = iuState->childByName(IU_WIDTH);
    iu->setAttribute(BaseRegisterFile::OSKEY_WIDTH, width->stringValue());

    // set max reads
    ObjectState* maxReads = iuState->childByName(IU_MAX_READS);
    iu->setAttribute(
        ImmediateUnit::OSKEY_MAX_READS, maxReads->stringValue());

    // set max writes
    ObjectState* maxWrites = iuState->childByName(IU_MAX_WRITES);
    iu->setAttribute(
        ImmediateUnit::OSKEY_MAX_WRITES, maxWrites->stringValue());
    
    // set guard latency
    if (iuState->hasChild(IU_GUARD_LATENCY)) {
        ObjectState* latencyChild = iuState->childByName(IU_GUARD_LATENCY);
        iu->setAttribute(
            ImmediateUnit::OSKEY_GUARD_LATENCY, latencyChild->stringValue());
    } else {
        iu->setAttribute(ImmediateUnit::OSKEY_GUARD_LATENCY, 0);
    }

    setIUExtensionMode(iuState, iu);

    // set ports and instruction templates
    for (int i = 0; i < iuState->childCount(); i++) {
        ObjectState* child = iuState->child(i);
        if (child->name() == PORT) {
            iu->addChild(machineRFPort(child));
        } else if (child->name() == IU_TEMPLATE) {
            try {
                instructionTemplateToMachine(
                    child, machineState, iuState->stringAttribute(IU_NAME));
            } catch (const SerializerException&) {
                delete iu;
                throw;
            }
        }
    }

    // if machine has an empty instruction template, it must be found in the
    // immediate unit declaration
    if (hasEmptyInstructionTemplate(machineState)) {
        bool emptyFound = false;
        for (int i = 0; i < iuState->childCount(); i++) {
            ObjectState* child = iuState->child(i);
            if (child->name() == IU_TEMPLATE && child->childCount() == 0) {
                emptyFound = true;
                break;
            }
        }
        if (!emptyFound) {
            delete iu;
            ADFSerializerTextGenerator textGen;
            format errorMsg = textGen.text(
                ADFSerializerTextGenerator::TXT_EMPTY_IT_NOT_DECLARED);
            errorMsg % iuState->stringAttribute(IU_NAME);
            string procName = "ADFSerializer::immediateUnitToMachine";
            throw SerializerException(
                __FILE__, __LINE__, procName, errorMsg.str());
        }
    }

    return iu;
}


/**
 * Creates an ObjectState instance that can be loaded by ImmediateSlot 
 * instance.
 *
 * @param isState An ObjectState instance that represents an immediate-slot
 *                element of ADF file.
 * @return The newly created ObjectState instance.
 */
ObjectState*
ADFSerializer::immediateSlotToMachine(const ObjectState* isState) {
    ObjectState* immediateSlot = new ObjectState(
        ImmediateSlot::OSNAME_IMMEDIATE_SLOT);
    immediateSlot->setAttribute(
        ImmediateSlot::OSKEY_NAME, 
        isState->stringAttribute(IMMEDIATE_SLOT_NAME));
    return immediateSlot;
}


/**
 * Creates an ObjectState instance that represents a special register port
 * in machine object model format.
 *
 * The ObjectState instance is created according to the given ObjectState
 * instance that represents a special register port in mdf format.
 *
 * @param mdfSRPortState ObjectState instance representing a special
 *                       register port in mdf format.
 * @return The newly created ObjectState instance representing a special
 *         register port in machine object model format.
 */
ObjectState*
ADFSerializer::machineSRPort(const ObjectState* mdfSRPortState) {

    assert(mdfSRPortState->name() == CU_SPECIAL_PORT);
    ObjectState* machPort =
        new ObjectState(SpecialRegisterPort::OSNAME_SPECIAL_REG_PORT);
    machPort->setAttribute(
        SpecialRegisterPort::OSKEY_NAME,
        mdfSRPortState->stringAttribute(PORT_NAME));

    ObjectState* width = mdfSRPortState->childByName(FU_PORT_WIDTH);
    machPort->setAttribute(
        SpecialRegisterPort::OSKEY_WIDTH, width->stringValue());

    int connIndex = 0;
    for (int i = 0; i < mdfSRPortState->childCount(); i++) {
        ObjectState* child = mdfSRPortState->child(i);
        if (child->name() == PORT_CONNECTS_TO) {
            connIndex++;
            if (connIndex == 1) {
                machPort->setAttribute(
                    Port::OSKEY_FIRST_SOCKET, child->stringValue());
            } else if (connIndex == 2) {
                machPort->setAttribute(
                    Port::OSKEY_SECOND_SOCKET, child->stringValue());
            } else {
                string procName = "ADFSerializer::machineSRPort";
                string errorMsg = "Too many connects-to elements in SR "
                    "port.";
                Application::writeToErrorLog(
                    __FILE__, __LINE__, procName, errorMsg);
                Application::abortProgram();
            }
        }
    }

    return machPort;
}


/**
 * Creates an ObjectState instance that represents a special register port
 * in mdf format.
 *
 * The ObjectState instance is created according to the given ObjectState
 * instance that represents a special register port in machine object model
 * format.
 *
 * @param machineSRPortState ObjectState instance representing a special
 *                           register port in machine object model format.
 * @return The newly created ObjectState instance representing a special
 *         register port in mdf format.
 */
ObjectState*
ADFSerializer::mdfSRPort(const ObjectState* machineSRPortState) {

    assert(machineSRPortState->name() ==
           SpecialRegisterPort::OSNAME_SPECIAL_REG_PORT);

    ObjectState* mdfSRPort = new ObjectState(CU_SPECIAL_PORT);
    string name =
        machineSRPortState->stringAttribute(SpecialRegisterPort::OSKEY_NAME);
    mdfSRPort->setAttribute(PORT_NAME, name);

    if (machineSRPortState->hasAttribute(FUPort::OSKEY_FIRST_SOCKET)) {
        ObjectState* conn = new ObjectState(PORT_CONNECTS_TO);
        mdfSRPort->addChild(conn);
        conn->setValue(machineSRPortState->stringAttribute(
                           FUPort::OSKEY_FIRST_SOCKET));
    }

    if (machineSRPortState->hasAttribute(FUPort::OSKEY_SECOND_SOCKET)) {
        ObjectState* conn = new ObjectState(PORT_CONNECTS_TO);
        mdfSRPort->addChild(conn);
        conn->setValue(machineSRPortState->stringAttribute(
                           FUPort::OSKEY_SECOND_SOCKET));
    }

    ObjectState* width = new ObjectState(FU_PORT_WIDTH);
    mdfSRPort->addChild(width);
    width->setValue(
        machineSRPortState->stringAttribute(
            SpecialRegisterPort::OSKEY_WIDTH));

    return mdfSRPort;
}


/**
 * Sets extension mode of immediate unit to the given ObjectState tree
 * representing immediate unit in machine object model format.
 *
 * @param mdfIUState ObjectState instance representing immediate unit in
 *                   mdf format.
 * @param momIUState ObjectState instance representing immediate unit in
 *                   machine object model format.
 */
void
ADFSerializer::setIUExtensionMode(
    const ObjectState* mdfIUState,
    ObjectState* momIUState) {

    ObjectState* extension = mdfIUState->childByName(IU_EXTENSION);
    string extensionValue = extension->stringValue();
    if (extensionValue == SIGN_EXTENSION) {
        momIUState->setAttribute(
            ImmediateUnit::OSKEY_EXTENSION, ImmediateUnit::OSVALUE_SIGN);
    } else if (extensionValue == ZERO_EXTENSION) {
        momIUState->setAttribute(
            ImmediateUnit::OSKEY_EXTENSION, ImmediateUnit::OSVALUE_ZERO);
    } else {
        string procName = "ADFSerializer::setIUExtensionMode";
        string errorMsg = "Unknown extension attribute value in ObjectState "
            "instance.";
        Application::writeToErrorLog(__FILE__, __LINE__, procName, errorMsg);
        Application::abortProgram();
    }
}


/**
 * Creates an ObjectState instance that represents a register file port in
 * machine object model format.
 *
 * The ObjectState instance is created according to the given ObjectState
 * instance that represents a port in mdf format. The given instance must
 * have an attribute called 'name' and up to two 'connects-to' children.
 *
 * @param mdfPortState ObjectState instance representing a normal port in
 *                     mdf format.
 * @return The newly created ObjectState instance representing a normal
 *         port in machine object model format.
 */
ObjectState*
ADFSerializer::machineRFPort(const ObjectState* mdfPortState) {

    ObjectState* port = new ObjectState(RFPort::OSNAME_RFPORT);
    port->setAttribute(
        RFPort::OSKEY_NAME, mdfPortState->stringAttribute(PORT_NAME));

    for (int i = 0; i < mdfPortState->childCount(); i++) {
        ObjectState* conn = mdfPortState->child(i);
        if (i == 0) {
            port->setAttribute(
                Port::OSKEY_FIRST_SOCKET, conn->stringValue());
        } else {
            port->setAttribute(
                Port::OSKEY_SECOND_SOCKET, conn->stringValue());
        }
    }

    return port;
}


/**
 * Creates an ObjectState instance that represents a normal port in mdf
 * format.
 *
 * The ObjectState instance is created according to the given ObjectState
 * instance that represents a port in machine object model format.
 *
 * @param machinePortState ObjectState instance representing a normal port in
 *                         machine object model format.
 * @return The newly created ObjectState instance representing a normal
 *         port in mdf format.
 */
ObjectState*
ADFSerializer::mdfPort(const ObjectState* machinePortState) {

    ObjectState* mdfPort = new ObjectState(PORT);
    mdfPort->setAttribute(PORT_NAME, machinePortState->stringAttribute(
                              Port::OSKEY_NAME));
    if (machinePortState->hasAttribute(Port::OSKEY_FIRST_SOCKET)) {
        ObjectState* conn = new ObjectState(PORT_CONNECTS_TO);
        mdfPort->addChild(conn);
        conn->setValue(machinePortState->stringAttribute(
                           Port::OSKEY_FIRST_SOCKET));
    }
    if (machinePortState->hasAttribute(Port::OSKEY_SECOND_SOCKET)) {
        ObjectState* conn = new ObjectState(PORT_CONNECTS_TO);
        mdfPort->addChild(conn);
        conn->setValue(machinePortState->stringAttribute(
                           Port::OSKEY_SECOND_SOCKET));
    }
    return mdfPort;
}


/**
 * Creates an ObjectState instance that represent a function unit port in
 * machine object model format.
 *
 * The ObjectState instance is created according to the given ObjectState
 * instance that represents a function unit port in mdf format. The given
 * instance must have an attribute called 'name' and up to two 'connects-to'
 * children. It has also 'width' child and may also have 'triggers' and
 * 'setsOpcode' children.
 *
 * @param mdfFUPortState ObjectState instance representing a function port in
 *                       mdf format.
 * @return The newly created ObjectState instance representing a function
 *         unit port in machine object model format.
 */
ObjectState*
ADFSerializer::machineFUPort(const ObjectState* mdfFUPortState) {

    ObjectState* fuPort = new ObjectState(FUPort::OSNAME_FUPORT);
    fuPort->setAttribute(Port::OSKEY_NAME, mdfFUPortState->stringAttribute(
                             PORT_NAME));

    // set socket connections
    for (int i = 0; i < mdfFUPortState->childCount(); i++) {
        ObjectState* portChild = mdfFUPortState->child(i);
        if (portChild->name() == PORT_CONNECTS_TO) {
            if (!fuPort->hasAttribute(Port::OSKEY_FIRST_SOCKET)) {
                fuPort->setAttribute(
                    Port::OSKEY_FIRST_SOCKET, portChild->stringValue());
            } else {
                fuPort->setAttribute(
                    Port::OSKEY_SECOND_SOCKET, portChild->stringValue());
            }
        }
    }

    // set other attributes
    ObjectState* width = mdfFUPortState->childByName(FU_PORT_WIDTH);
    fuPort->setAttribute(FUPort::OSKEY_WIDTH, width->stringValue());
    fuPort->setAttribute(
        FUPort::OSKEY_TRIGGERING, mdfFUPortState->hasChild(
            FU_PORT_TRIGGERS));
    fuPort->setAttribute(
        FUPort::OSKEY_OPCODE_SETTING, mdfFUPortState->hasChild(
            FU_PORT_SETS_OPCODE));

    return fuPort;
}


/**
 * Creates an ObjectState instance that represents a function unit port in
 * mdf format.
 *
 * The ObjectState instance is created according to the given ObjectState
 * instance that represents a function unit port in machine object model
 * format.
 *
 * @param machineFUPortState ObjectState instance representing a function
 *                           unit port in machine object model format.
 * @return The newly created ObjectState instance representing a function
 *         unit port in mdf format.
 */
ObjectState*
ADFSerializer::mdfFUPort(const ObjectState* machineFUPortState) {

    ObjectState* mdfFUPort = mdfPort(machineFUPortState);
    ObjectState* width = new ObjectState(FU_PORT_WIDTH);
    width->setValue(machineFUPortState->stringAttribute(
                        FUPort::OSKEY_WIDTH));
    mdfFUPort->addChild(width);
    if (machineFUPortState->intAttribute(FUPort::OSKEY_TRIGGERING)) {
        mdfFUPort->addChild(new ObjectState(FU_PORT_TRIGGERS));
    }
    if (machineFUPortState->intAttribute(FUPort::OSKEY_OPCODE_SETTING)) {
        mdfFUPort->addChild(new ObjectState(FU_PORT_SETS_OPCODE));
    }
    return mdfFUPort;
}


/**
 * Creates an ObjectState instance that represents an operation in machine
 * object model format.
 *
 * The ObjectState instance is created according to the given ObjectState
 * instance that represents an operation in mdf format.
 *
 * @param mdfOperationState ObjectState instance representing an operation
 *                          in mdf format.
 * @return The newly created ObjectState instance representing an operation
 *         in machine object model format.
 */
ObjectState*
ADFSerializer::machineOperation(const ObjectState* mdfOperationState) {

    ObjectState* machineOperation = new ObjectState(
        HWOperation::OSNAME_OPERATION);
    machineOperation->setAttribute(
        HWOperation::OSKEY_NAME,
        mdfOperationState->childByName(FU_OP_NAME)->stringValue());

    // set operand bindings
    for (int i = 0; i < mdfOperationState->childCount(); i++) {
        ObjectState* child = mdfOperationState->child(i);
        if (child->name() == FU_OP_BIND) {
            ObjectState* binding = new ObjectState(
                HWOperation::OSNAME_OPERAND_BINDING);
            machineOperation->addChild(binding);
            binding->setAttribute(
                HWOperation::OSKEY_OPERAND,
                child->stringAttribute(FU_OP_BIND_NAME));
            binding->setAttribute(
                HWOperation::OSKEY_PORT, child->stringValue());
        }
    }

    // set pipeline
    ObjectState* mdfPipeline = mdfOperationState->childByName(
        FU_OP_PIPELINE);
    machineOperation->addChild(machinePipeline(mdfPipeline));

    return machineOperation;
}


/**
 * Creates an ObjectState instance that represents an operation in mdf
 * format.
 *
 * The ObjectState instance is created according to the given ObjectState
 * instance that represents an operation in machine object model format.
 *
 * @param machineOperationState ObjectState instance representing an
 *                              operation in machine object model format.
 * @return The newly created ObjectState instance representing an operation
 *         in mdf format.
 */
ObjectState*
ADFSerializer::mdfOperation(const ObjectState* machineOperationState) {

    ObjectState* mdfOperation = new ObjectState(FU_OPERATION);
    ObjectState* name = new ObjectState(FU_OP_NAME);
    mdfOperation->addChild(name);
    name->setValue(machineOperationState->stringAttribute(
                       HWOperation::OSKEY_NAME));

    // add bind elements
    for (int i = 0; i < machineOperationState->childCount(); i++) {
        ObjectState* child = machineOperationState->child(i);
        if (child->name() == HWOperation::OSNAME_OPERAND_BINDING) {
            ObjectState* bind = new ObjectState(FU_OP_BIND);
            mdfOperation->addChild(bind);
            bind->setAttribute(
                FU_OP_BIND_NAME, child->stringAttribute(
                    HWOperation::OSKEY_OPERAND));
            bind->setValue(child->stringAttribute(HWOperation::OSKEY_PORT));
        }
    }

    ObjectState* machPipeline = machineOperationState->childByName(
        ExecutionPipeline::OSNAME_PIPELINE);
    mdfOperation->addChild(mdfPipeline(machPipeline));
    return mdfOperation;
}


/**
 * Creates an ObjectState instance that represents a pipeline in machine
 * object model format.
 *
 * The ObjectState instance is created according to the given ObjectState
 * instance that represents a pipeline in mdf format.
 *
 * @param mdfPipelineState ObjectState instance representing a pipeline in
 *                         mdf format.
 * @return The newly created ObjectState instance representing a pipeline in
 *         machine object model format.
 */
ObjectState*
ADFSerializer::machinePipeline(const ObjectState* mdfPipelineState) {

    ObjectState* machinePipeline = new ObjectState(
        ExecutionPipeline::OSNAME_PIPELINE);

    for (int i = 0; i < mdfPipelineState->childCount(); i++) {
        ObjectState* mdfResource = mdfPipelineState->child(i);
        ObjectState* machineResource = NULL;
        if (mdfResource->name() == FU_OP_PL_RESOURCE) {
            machineResource = new ObjectState(
                ExecutionPipeline::OSNAME_RESOURCE_USAGE);
            machineResource->setAttribute(
                ExecutionPipeline::OSKEY_RESOURCE_NAME,
                mdfResource->stringAttribute(FU_OP_PL_RES_NAME));
        } else if (mdfResource->name() == FU_OP_PL_READS) {
            machineResource = new ObjectState(
                ExecutionPipeline::OSNAME_OPERAND_READ);
            machineResource->setAttribute(
                ExecutionPipeline::OSKEY_OPERAND,
                mdfResource->stringAttribute(FU_OP_PL_READS_NAME));
        } else if (mdfResource->name() == FU_OP_PL_WRITES) {
            machineResource = new ObjectState(
                ExecutionPipeline::OSNAME_OPERAND_WRITE);
            machineResource->setAttribute(
                ExecutionPipeline::OSKEY_OPERAND,
                mdfResource->stringAttribute(FU_OP_PL_WRITES_NAME));
        } else {
            const string procName = "ADFSerializer::machinePipeline";
            const string errorMsg = "Given pipeline model is invalid.";
            Application::writeToErrorLog(
                __FILE__, __LINE__, procName, errorMsg);
            Application::abortProgram();
        }

        machinePipeline->addChild(machineResource);
        machineResource->setAttribute(
            ExecutionPipeline::OSKEY_START_CYCLE,
            mdfResource->childByName(FU_OP_PL_RES_START_CYCLE)->
            stringValue());
        machineResource->setAttribute(
            ExecutionPipeline::OSKEY_CYCLES,
            mdfResource->childByName(FU_OP_PL_RES_CYCLES)->stringValue());
    }

    return machinePipeline;
}


/**
 * Creates an ObjectState instance that represents a pipeline in mdf format.
 *
 * The ObjectState instance is created according to the given ObjectState
 * instance that represents a pipeline in machine object model format.
 *
 * @param machinePipelineState ObjectState instance representing a pipeline
 *                             in machine object model format.
 * @return The newly created ObjectState instance representing a pipeline in
 *         mdf format.
 */
ObjectState*
ADFSerializer::mdfPipeline(const ObjectState* machinePipelineState) {

    ObjectState* mdfPipeline = new ObjectState(FU_OP_PIPELINE);

    for (int i = 0; i < machinePipelineState->childCount(); i++) {
        ObjectState* resChild = machinePipelineState->child(i);
        ObjectState* mdfChild = NULL;
        if (resChild->name() == ExecutionPipeline::OSNAME_RESOURCE_USAGE) {
            mdfChild = new ObjectState(FU_OP_PL_RESOURCE);
            mdfChild->setAttribute(
                FU_OP_PL_RES_NAME,
                resChild->stringAttribute(
                    ExecutionPipeline::OSKEY_RESOURCE_NAME));
        } else if (resChild->name() ==
                   ExecutionPipeline::OSNAME_OPERAND_READ) {
            mdfChild = new ObjectState(FU_OP_PL_READS);
            mdfChild->setAttribute(
                FU_OP_PL_READS_NAME,
                resChild->stringAttribute(ExecutionPipeline::OSKEY_OPERAND));
        } else if (resChild->name() ==
                   ExecutionPipeline::OSNAME_OPERAND_WRITE) {
            mdfChild = new ObjectState(FU_OP_PL_WRITES);
            mdfChild->setAttribute(
                FU_OP_PL_WRITES_NAME,
                resChild->stringAttribute(ExecutionPipeline::OSKEY_OPERAND));
        } else {
            assert(false);
        }

        mdfPipeline->addChild(mdfChild);
        ObjectState* startCycle = new ObjectState(FU_OP_PL_RES_START_CYCLE);
        mdfChild->addChild(startCycle);
        startCycle->setValue(
            resChild->stringAttribute(ExecutionPipeline::OSKEY_START_CYCLE));
        ObjectState* cycles = new ObjectState(FU_OP_PL_RES_CYCLES);
        mdfChild->addChild(cycles);
        cycles->setValue(
            resChild->stringAttribute(ExecutionPipeline::OSKEY_CYCLES));
    }

    return mdfPipeline;
}


/**
 * Includes the given instruction template information to ObjectState tree
 * representing immediate unit in mdf format.
 *
 * @param momITState ObjectState instance representing an instruction
 *                   template in machine object model format.
 * @param mdfIUState ObjectState instance representing an immediate unit in
 *                   mdf format.
 */
void
ADFSerializer::instructionTemplateToMDF(
    const ObjectState* momITState,
    ObjectState* mdfIUState) {

    string iuName = mdfIUState->stringAttribute(IU_NAME);
    string iTempName = momITState->stringAttribute(Component::OSKEY_NAME);
    for (int slotIndex = 0; slotIndex < momITState->childCount();
         slotIndex++) {
        ObjectState* slot = momITState->child(slotIndex);
        if (slot->stringAttribute(TemplateSlot::OSKEY_DESTINATION) ==
            iuName) {

            ObjectState* correctTemplate = NULL;

            for (int iuChildIndex = 0;
                 iuChildIndex < mdfIUState->childCount(); iuChildIndex++) {

                ObjectState* iuChild = mdfIUState->child(iuChildIndex);
                if (iuChild->name() == IU_TEMPLATE &&
                    iuChild->stringAttribute(IU_TEMPLATE_NAME) ==
                    iTempName) {
                    correctTemplate = iuChild;
                }
            }

            if (correctTemplate == NULL) {
                correctTemplate = new ObjectState(IU_TEMPLATE);
                mdfIUState->addChild(correctTemplate);
                correctTemplate->setAttribute(IU_TEMPLATE_NAME, iTempName);
            }
            
            ObjectState* slotElem =
                new ObjectState(IU_TEMPLATE_SLOT);
            correctTemplate->addChild(slotElem);
            ObjectState* nameElem =
                new ObjectState(IU_TEMPLATE_SLOT_NAME);
            slotElem->addChild(nameElem);
            nameElem->setValue(
                slot->stringAttribute(TemplateSlot::OSKEY_SLOT));
            ObjectState* widthElem =
                new ObjectState(IU_TEMPLATE_SLOT_WIDTH);
            slotElem->addChild(widthElem);
            widthElem->setValue(
                slot->stringAttribute(
                    TemplateSlot::OSKEY_WIDTH));
        }
    }


    if (momITState->childCount() == 0) {
        ObjectState* emptyTemplate = new ObjectState(IU_TEMPLATE);
        mdfIUState->addChild(emptyTemplate);
        emptyTemplate->setAttribute(IU_TEMPLATE_NAME, iTempName);
    }
}


/**
 * Adds/modifies instruction templates to the given ObjectState instance
 * representing machine in machine object model format.
 *
 * Instruction templates are read from the given ObjectState instance
 * representing a template element inside immediate unit declaration of
 * mdf.
 *
 * @param mdfITState ObjectState instance representing a template element.
 * @param momMachineState ObjectState instance representing a machine in
 *                        machine object model format.
 * @param iuName Name of the immediate unit which contains the template
 *               element.
 * @exception SerializerException If an error is occurred while processing.
 */
void
ADFSerializer::instructionTemplateToMachine(
    const ObjectState* mdfITState,
    ObjectState* momMachineState,
    const std::string& iuName)
    throw (SerializerException) {

    const string procName = "ADFSerializer::instructionTemplateToMachine";

    string templateName = mdfITState->stringAttribute(IU_TEMPLATE_NAME);
    ObjectState* iTemp = NULL;
    bool iTempFound = false;

    for (int iTempIndex = 0; iTempIndex < momMachineState->childCount();
         iTempIndex++) {
        iTemp = momMachineState->child(iTempIndex);
        if (iTemp->name() ==
            InstructionTemplate::OSNAME_INSTRUCTION_TEMPLATE &&
            iTemp->stringAttribute(Component::OSKEY_NAME) ==
            templateName) {
            iTempFound = true;
            break;
        }
    }

    if (!iTempFound) {
        iTemp = new ObjectState(
            InstructionTemplate::OSNAME_INSTRUCTION_TEMPLATE);
        momMachineState->addChild(iTemp);
        iTemp->setAttribute(Component::OSKEY_NAME, templateName);
    }

    ADFSerializerTextGenerator textGen;

    if ((mdfITState->childCount() == 0 && iTempFound &&
         iTemp->childCount() > 0) ||
        (mdfITState->childCount() > 0 && iTempFound &&
         iTemp->childCount() == 0)) {
        format errorMsg = textGen.text(
            ADFSerializerTextGenerator::TXT_IT_EMPTY_AND_NON_EMPTY);
        errorMsg % templateName;
        throw SerializerException(
            __FILE__, __LINE__, procName, errorMsg.str());
    }

    for (int slotIndex = 0; slotIndex < mdfITState->childCount();
         slotIndex++) {
        ObjectState* slot = mdfITState->child(slotIndex);
        string slotName = slot->childByName(IU_TEMPLATE_SLOT_NAME)->
            stringValue();
        string width = slot->childByName(IU_TEMPLATE_SLOT_WIDTH)->
            stringValue();

        ObjectState* iTempSlot = momTemplateSlot(iTemp, slotName);
        if (iTempSlot == NULL) {
            iTempSlot = new ObjectState(TemplateSlot::OSNAME_TEMPLATE_SLOT);
            iTempSlot->setAttribute(TemplateSlot::OSKEY_SLOT, slotName);
            iTempSlot->setAttribute(TemplateSlot::OSKEY_WIDTH, width);
            iTempSlot->setAttribute(TemplateSlot::OSKEY_DESTINATION, iuName);
            iTemp->addChild(iTempSlot);
        } else {
            format errorMsg;
            if (iTempSlot->stringAttribute(
                    TemplateSlot::OSKEY_DESTINATION) == iuName) {
                errorMsg = textGen.text(
                    ADFSerializerTextGenerator::TXT_SAME_TEMPLATE_SLOT);
                errorMsg % slotName % iuName % templateName;
            } else {
                errorMsg = textGen.text(
                    ADFSerializerTextGenerator::
                    TXT_MULTIPLE_DESTINATIONS_IN_TEMPLATE_SLOT);
                errorMsg % slotName % templateName % iuName %
                    iTempSlot->stringAttribute(
                        TemplateSlot::OSKEY_DESTINATION);
            }
            throw SerializerException(
                __FILE__, __LINE__, procName, errorMsg.str());
        }
    }
}


/**
 * Looks for an ObjectState instance which represents a template slot in
 * machine object model format.
 *
 * The ObjectState instance is searched from inside the given ObjectState
 * instance that represents an instruction template in machine object model
 * format. The instance is seached by the given slot name. Returns NULL if
 * such an instance is not found.
 *
 * @param momITState An ObjectState instance representing an instruction
 *                   template in machine object model format.
 * @param busName The name of the slot to look the template slot for.
 * @return ObjectState instance representing a template slot in machine
 *         object model format or NULL.
 */
ObjectState*
ADFSerializer::momTemplateSlot(
    const ObjectState* momITState,
    const std::string& slotName) {

    for (int i = 0; i < momITState->childCount(); i++) {
        ObjectState* slot = momITState->child(i);
        if (slot->stringAttribute(TemplateSlot::OSKEY_SLOT) == slotName) {
            return slot;
        }
    }

    return NULL;
}


/**
 * Checks whether the machine represented by the given ObjectState tree
 * contains an empty instruction template.
 *
 * @param momMachineState The ObjectState tree.
 * @return True if it contains an empty instruction template, otherwise
 *         false.
 */
bool
ADFSerializer::hasEmptyInstructionTemplate(
    const ObjectState* momMachineState) {

    for (int i = 0; i < momMachineState->childCount(); i++) {
        ObjectState* child = momMachineState->child(i);
        if (child->name() ==
            InstructionTemplate::OSNAME_INSTRUCTION_TEMPLATE &&
            child->childCount() == 0) {
            return true;
        }
    }

    return false;
}
