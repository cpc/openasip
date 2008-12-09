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
 * @file UniversalMachine.cc
 *
 * Implementation of UniversalMachine class.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen-no.spam-tut.fi)
 * @author Pekka J‰‰skel‰inen 2006 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: yellow
 */

#include <string>

#include "UniversalMachine.hh"
#include "UnboundedRegisterFile.hh"
#include "UniversalFunctionUnit.hh"
#include "Segment.hh"
#include "Port.hh"
#include "SpecialRegisterPort.hh"
#include "Guard.hh"
#include "ControlUnit.hh"
#include "HWOperation.hh"
#include "FUPort.hh"
#include "ExecutionPipeline.hh"
#include "OperationPool.hh"

using std::string;
using namespace TTAMachine;

const unsigned int DATA_MEM_SIZE_UINT = DATA_MEMORY_SIZE;

/**
 * Constructor.
 *
 * Creates a complete universal machine with all the required components.
 */
UniversalMachine::UniversalMachine() :
    Machine(), isBuilt_(false) {
    construct();
}


/**
 * Deprecated constructor.
 *
 * Creates a complete universal machine with all the required components.
 *
 * @param opPool The operation pool used by universal function unit.
 * @deprecated Use the one without opPool as argument, as OperationPool is
 * going to be singleton anyways.
 */
UniversalMachine::UniversalMachine(OperationPool&) :
    Machine(), isBuilt_(false) {
    construct();
}

/**
 * Actually constructs the resources in the universal machine.
 */
void
UniversalMachine::construct() {

    try {
        // add bus
        Bus* bus = new Bus(UM_BUS_NAME, 64, 32, Machine::ZERO);
        addBus(*bus);

        // add a segment to the bus
        Segment* segment = new Segment(UM_SEGMENT_NAME, *bus);

        // add sockets
        Socket* inputSocket = new Socket(UM_INPUT_SOCKET_NAME);
        Socket* outputSocket =  new Socket(UM_OUTPUT_SOCKET_NAME);
        addSocket(*inputSocket);
        addSocket(*outputSocket);
        inputSocket->attachBus(*segment);
        inputSocket->setDirection(Socket::INPUT);
        outputSocket->attachBus(*segment);
        outputSocket->setDirection(Socket::OUTPUT);

        // add a boolean register file
        RegisterFile* boolean = new RegisterFile(
            UM_BOOLEAN_RF_NAME, 1, 1, 1, 1, 0, RegisterFile::NORMAL);
        addRegisterFile(*boolean);
        (new RFPort(UM_BOOLEAN_RF_WRITE_PORT, *boolean))->
            attachSocket(*inputSocket);
        (new RFPort(UM_BOOLEAN_RF_READ_PORT, *boolean))->
            attachSocket(*outputSocket);

        // add boolean register guards
        new RegisterGuard(false, *boolean, 0, *bus);
        new RegisterGuard(true, *boolean, 0, *bus);

        // add universal register file for integers
        UnboundedRegisterFile* integerURF = new UnboundedRegisterFile(
            UM_INTEGER_URF_NAME, 32, RegisterFile::NORMAL);
        addRegisterFile(*integerURF);
        (new RFPort(UM_INTEGER_URF_WRITE_PORT, *integerURF))->
            attachSocket(*inputSocket);
        (new RFPort(UM_INTEGER_URF_READ_PORT, *integerURF))->
            attachSocket(*outputSocket);

        // add universal register file for doubles
        UnboundedRegisterFile* doubleURF = new UnboundedRegisterFile(
            UM_DOUBLE_URF_NAME, 64, RegisterFile::NORMAL);
        addRegisterFile(*doubleURF);
        (new RFPort(UM_DOUBLE_URF_WRITE_PORT, *doubleURF))->
            attachSocket(*inputSocket);
        (new RFPort(UM_DOUBLE_URF_READ_PORT, *doubleURF))->
            attachSocket(*outputSocket);

        // add a special register file for function return value
        RegisterFile* specialRF = new RegisterFile(
            UM_SPECIAL_RF_NAME, 1, 32, 1, 1, 0, RegisterFile::NORMAL);
        addRegisterFile(*specialRF);
        (new RFPort(UM_SPECIAL_RF_WRITE_PORT, *specialRF))->
            attachSocket(*inputSocket);
        (new RFPort(UM_SPECIAL_RF_READ_PORT, *specialRF))->
            attachSocket(*outputSocket);

        // add address space for instruction memory 
        AddressSpace* iMem = new AddressSpace(
            UM_IMEM_NAME, 8, 0, 0xFFFFFF, *this);
        // add address space for data memory
        AddressSpace* dMem = new AddressSpace(
            UM_DMEM_NAME, 8, 0, DATA_MEM_SIZE_UINT - 1, *this);

        // add universal function unit
        UniversalFunctionUnit* uFU = new UniversalFunctionUnit(
            UM_UNIVERSAL_FU_NAME, opPool);
        addFunctionUnit(*uFU);
        uFU->setAddressSpace(dMem);

        // add control unit
        ControlUnit* cu = new ControlUnit(UM_GCU_NAME, 0, 1);
        setGlobalControl(*cu);
        cu->setAddressSpace(iMem);
        HWOperation* callOp = new HWOperation("CALL", *cu);
        HWOperation* jumpOp = new HWOperation("JUMP", *cu);

        FUPort* port1 = new FUPort("port1", 32, *cu, true, true);
        port1->attachSocket(*inputSocket);
        FUPort* port2 = new FUPort("port2", 32, *cu, false, false);
        port2->attachSocket(*inputSocket);
        SpecialRegisterPort* raPort = new SpecialRegisterPort("ra", 32, *cu);
        raPort->attachSocket(*inputSocket);
        raPort->attachSocket(*outputSocket);
        cu->setReturnAddressPort(*raPort);

        // create pipelines and operand bindings of GCU operations
        callOp->bindPort(1, *port1);
        callOp->pipeline()->addPortRead(1, 0, 1);
        jumpOp->bindPort(1, *port1);
        jumpOp->pipeline()->addPortRead(1, 0, 1);
    } catch (const Exception& exception) {
        const string procName = "UniversalMachine::UniversalMachine";
        Application::writeToErrorLog(
            __FILE__, __LINE__, procName, exception.errorMessage());
        Application::abortProgram();
    }

    isBuilt_ = true;
}


/**
 * Destructor.
 */
UniversalMachine::~UniversalMachine() {
}


/**
 * Returns the universal function unit of the machine.
 *
 * @return The universal function unit.
 */
UniversalFunctionUnit&
UniversalMachine::universalFunctionUnit() const {
    FunctionUnitNavigator fuNav = functionUnitNavigator();
    assert(fuNav.hasItem(UM_UNIVERSAL_FU_NAME));
    FunctionUnit* fu = fuNav.item(UM_UNIVERSAL_FU_NAME);
    UniversalFunctionUnit* ufu = dynamic_cast<UniversalFunctionUnit*>(fu);
    assert(ufu != NULL);
    return *ufu;
}


/**
 * Returns the boolean register file of the universal machine.
 *
 * @return The boolean register file.
 */
RegisterFile&
UniversalMachine::booleanRegisterFile() const {
    RegisterFileNavigator rfNav = registerFileNavigator();
    assert(rfNav.hasItem(UM_BOOLEAN_RF_NAME));
    return *rfNav.item(UM_BOOLEAN_RF_NAME);
}


/**
 * Returns the unbounded register file for integers.
 *
 * @return The unbounded register file for integers.
 */
UnboundedRegisterFile&
UniversalMachine::integerRegisterFile() const {
    RegisterFileNavigator rfNav = registerFileNavigator();
    assert(rfNav.hasItem(UM_INTEGER_URF_NAME));
    RegisterFile* rf = rfNav.item(UM_INTEGER_URF_NAME);
    UnboundedRegisterFile* urf = dynamic_cast<UnboundedRegisterFile*>(rf);
    assert(urf != NULL);
    return *urf;
}


/**
 * Returns the unbounded register file for double precision floating point 
 * numbers.
 *
 * @return The unbounded register file for doubles.
 */
UnboundedRegisterFile&
UniversalMachine::doubleRegisterFile() const {
    RegisterFileNavigator rfNav = registerFileNavigator();
    assert(rfNav.hasItem(UM_DOUBLE_URF_NAME));
    RegisterFile* rf = rfNav.item(UM_DOUBLE_URF_NAME);
    UnboundedRegisterFile* urf = dynamic_cast<UnboundedRegisterFile*>(rf);
    assert(urf != NULL);
    return *urf;
}


/**
 * Returns the special register file.
 *
 * @return The special register file.
 */
RegisterFile&
UniversalMachine::specialRegisterFile() const {
    RegisterFileNavigator rfNav = registerFileNavigator();
    assert(rfNav.hasItem(UM_SPECIAL_RF_NAME));
    return *rfNav.item(UM_SPECIAL_RF_NAME);
}


/**
 * Returns the address space of instruction memory.
 *
 * @return The address space of instruction memory.
 */
AddressSpace&
UniversalMachine::instructionAddressSpace() const {
    AddressSpaceNavigator asNav = addressSpaceNavigator();
    assert(asNav.hasItem(UM_IMEM_NAME));
    return *asNav.item(UM_IMEM_NAME);
}


/**
 * Returns the address space of data memory.
 *
 * @return The address space of data memory.
 */
AddressSpace&
UniversalMachine::dataAddressSpace() const {
    AddressSpaceNavigator asNav = addressSpaceNavigator();
    assert(asNav.hasItem(UM_DMEM_NAME));
    return *asNav.item(UM_DMEM_NAME);
}


/**
 * Returns the only transport bus of universal machine.
 *
 * @return The bus.
 */
Bus&
UniversalMachine::universalBus() const {
    BusNavigator busNav = busNavigator();
    assert(busNav.hasItem(UM_BUS_NAME));
    return *busNav.item(UM_BUS_NAME);
}


/**
 * Adds a bus to the machine. 
 *
 * Aborts the program if tried to add a bus after the UniversalMachine is 
 * constructed. DO NOT CALL THIS METHOD.
 *
 * @param bus Bus being added.
 * @exception ComponentAlreadyExists If a bus by the same name already exists
 *                                   in the machine.
 */
void
UniversalMachine::addBus(Bus& bus)
    throw (ComponentAlreadyExists) {

    if (!isBuilt_) {
        Machine::addBus(bus);
    } else {
        const string procName = "UniversalMachine::addBus";
        const string errorMsg = "Tried to add a bus to UniversalMachine!";
        Application::writeToErrorLog(__FILE__, __LINE__, procName, errorMsg);
        Application::abortProgram();
    }
}


/**
 * Adds a socket to the machine. 
 *
 * Aborts the program if tried to add a socket after the UniversalMachine 
 * is constructed. DO NOT CALL THIS METHOD.
 *
 * @param socket The socket being added.
 * @exception ComponentAlreadyExists If a socket by the same name already
 *                                   exists in the machine.
 */
void
UniversalMachine::addSocket(Socket& socket)
    throw (ComponentAlreadyExists) {

    if (!isBuilt_) {
        Machine::addSocket(socket);
    } else {
        const string procName = "UniversalMachine::addSocket";
        const string errorMsg = "Tried to add a socket to UniversalMachine!";
        Application::writeToErrorLog(__FILE__, __LINE__, procName, errorMsg);
        Application::abortProgram();
    }
}


/**
 * Adds a function unit to the machine. Aborts the program if tried to
 * add a function unit after the UniversalMachine is constructed. DO
 * NOT CALL THIS METHOD.
 *
 * @param unit The function unit being added.
 * @exception ComponentAlreadyExists If a function unit by the same name
 *                                   already exists in the machine.
 */
void
UniversalMachine::addFunctionUnit(FunctionUnit& unit)
    throw (ComponentAlreadyExists) {

    if (!isBuilt_) {
        Machine::addFunctionUnit(unit);
    } else {
        const string procName = "UniversalMachine::addFunctionUnit";
        const string errorMsg =
            "Tried to add a function unit to UniversalMachine!";
        Application::writeToErrorLog(__FILE__, __LINE__, procName, errorMsg);
        Application::abortProgram();
    }
}


/**
 * Aborts the program. It is not allowed to add immediate units to
 * UniversalMachine. DO NOT CALL THIS METHOD.
 *
 * @param unit Never used.
 * @exception ComponentAlreadyExists Never thrown.
 */
void
UniversalMachine::addImmediateUnit(ImmediateUnit&)
    throw (ComponentAlreadyExists) {

    const string procName = "UniversalMachine::addImmediateUnit";
    const string errorMsg =
        "Tried to add an immediate unit to UniversalMachine!";
    Application::writeToErrorLog(__FILE__, __LINE__, procName, errorMsg);
    Application::abortProgram();
}


/**
 * Adds a register file to the machine. Aborts the program if tried to
 * add a register file after the UniversalMachine is constructed. DO NOT CALL
 * THIS METHOD.
 *
 * @param unit The register file being added.
 * @exception ComponentAlreadyExists If a register file by the same name
 *                                   already exists in the machine.
 */
void
UniversalMachine::addRegisterFile(RegisterFile& unit)
    throw (ComponentAlreadyExists) {

    if (!isBuilt_) {
        Machine::addRegisterFile(unit);
    } else {
        const string procName = "UniversalMachine::addRegisterFile";
        const string errorMsg =
            "Tried to add a register file to UniversalMachine!";
        Application::writeToErrorLog(__FILE__, __LINE__, procName, errorMsg);
        Application::abortProgram();
    }
}


/**
 * Adds an address space to the machine. Aborts the program if tried
 * to add an address space after the UniversalMachine is
 * constructed. DO NOT CALL THIS METHOD.
 *
 * @param as The address space being added.
 * @exception ComponentAlreadyExists If an address space by the same name
 *                                   already exists in the machine.
 */
void
UniversalMachine::addAddressSpace(AddressSpace& as)
    throw (ComponentAlreadyExists) {

    if (!isBuilt_) {
        Machine::addAddressSpace(as);
    } else {
        const string procName = "UniversalMachine::addAddressSpace";
        const string errorMsg =
            "Tried to add an address space to UniversalMachine!";
        Application::writeToErrorLog(__FILE__, __LINE__, procName, errorMsg);
        Application::abortProgram();
    }
}


/**
 * Aborts the program. UniversalMachine can not contain bridges. DO NOT CALL
 * THIS METHOD.
 *
 * @param bridge Never used.
 * @exception ComponentAlreadyExists Never thrown.
 */
void
UniversalMachine::addBridge(Bridge&)
    throw (ComponentAlreadyExists) {

    const string procName = "UniversalMachine::addBridge";
    const string errorMsg = "Tried to add a bridge to UniversalMachine!";
    Application::writeToErrorLog(__FILE__, __LINE__, procName, errorMsg);
    Application::abortProgram();
}


/**
 * Aborts the program. UniversalMachine can not contain instruction
 * templates. DO NOT CALL THIS METHOD!
 *
 * @param iTemp Never used.
 * @exception ComponentAlreadyExists Never thrown.
 */
void
UniversalMachine::addInstructionTemplate(InstructionTemplate&)
    throw (ComponentAlreadyExists) {

    const string procName = "UniversalMachine::addInstructionTemplate";
    const string errorMsg =
        "Tried to add an instruction template to UniversalMachine!";
    Application::writeToErrorLog(__FILE__, __LINE__, procName, errorMsg);
    Application::abortProgram();
}


/**
 * Sets the control unit of the machine. Aborts the program if tried to set
 * the control unit after the UniversalMachine is constructed. DO NOT CALL
 * THIS METHOD!
 *
 * @param unit The control unit to be set.
 * @exception ComponentAlreadyExists Never thrown.
 */
void
UniversalMachine::setGlobalControl(ControlUnit& unit)
    throw (ComponentAlreadyExists) {

    if (!isBuilt_) {
        Machine::setGlobalControl(unit);
    } else {
        const string procName = "UniversalMachine::setGlobalControl";
        const string errorMsg =
            "Tried to set control unit to UniversalMachine!";
        Application::writeToErrorLog(__FILE__, __LINE__, procName, errorMsg);
        Application::abortProgram();
    }
}


/**
 * Aborts the program. It is not allowed to remove buses from
 * UniversalMachine. DO NOT CALL THIS METHOD!
 *
 * @param bus Never used.
 * @exception InstanceNotFound Never thrown.
 */
void
UniversalMachine::removeBus(Bus&)
    throw (InstanceNotFound) {

    const string procName = "UniversalMachine::removeBus";
    const string errorMsg = "Tried to remove bus from UniversalMachine!";
    Application::writeToErrorLog(__FILE__, __LINE__, procName, errorMsg);
    Application::abortProgram();
}


/**
 * Aborts the program. It is not allowed to remove sockets from
 * UniversalMachine. DO NOT CALL THIS METHOD!
 *
 * @param socket Never used.
 * @exception InstanceNotFound Never thrown.
 */
void
UniversalMachine::removeSocket(Socket&)
    throw (InstanceNotFound) {

    const string procName = "UniversalMachine::removeSocket";
    const string errorMsg = "Tried to remove socket from UniversalMachine!";
    Application::writeToErrorLog(__FILE__, __LINE__, procName, errorMsg);
    Application::abortProgram();
}


/**
 * Aborts the program. It is not allowed to remove function units from
 * UniversalMachine. DO NOT CALL THIS METHOD!
 *
 * @param unit Never used.
 * @exception InstanceNotFound Never thrown.
 */
void
UniversalMachine::removeFunctionUnit(FunctionUnit&)
    throw (InstanceNotFound) {

    const string procName = "UniversalMachine::removeFunctionUnit";
    const string errorMsg =
        "Tried to remove function unit from UniversalMachine!";
    Application::writeToErrorLog(__FILE__, __LINE__, procName, errorMsg);
    Application::abortProgram();
}


/**
 * Aborts the program. It is not allowed to remove register files from
 * UniversalMachine. DO NOT CALL THIS METHOD!
 *
 * @param unit Never used.
 * @exception InstanceNotFound Never thrown.
 */
void
UniversalMachine::removeRegisterFile(RegisterFile&)
    throw (InstanceNotFound) {

    const string procName = "UniversalMachine::removeRegisterFile";
    const string errorMsg =
        "Tried to remove register file from UniversalMachine!";
    Application::writeToErrorLog(__FILE__, __LINE__, procName, errorMsg);
    Application::abortProgram();
}


/**
 * Aborts the program. It is not allowed to delete address spaces from
 * UniversalMachine. DO NOT CALL THIS METHOD!
 *
 * @param as Never used.
 * @exception InstanceNotFound Never thrown.
 */
void
UniversalMachine::deleteAddressSpace(AddressSpace&)
    throw (InstanceNotFound) {

    const string procName = "UniversalMachine::deleteAddressSpace";
    const string errorMsg =
        "Tried to delete address space from UniversalMachine!";
    Application::writeToErrorLog(__FILE__, __LINE__, procName, errorMsg);
    Application::abortProgram();
}


/**
 * Aborts the program. It is not allowed to unset global control unit from
 * UniversalMachine. DO NOT CALL THIS METHOD!
 */
void
UniversalMachine::unsetGlobalControl() {

    const string procName = "UniversalMachine::unsetglobalControl";
    const string errorMsg =
        "Tried to unset global control unit from UniversalMachine!";
    Application::writeToErrorLog(__FILE__, __LINE__, procName, errorMsg);
    Application::abortProgram();
}


/**
 * Redefinement of this method prevents loading the state of
 * UniversalMachine from an ObjectState tree. Aborts the program if
 * this method is called. DO NOT CALL THIS METHOD.
 *
 * @param state ObjectState instance.
 * @exception ObjectStateLoadingException Never thrown.
 */
void
UniversalMachine::loadState(const ObjectState*)
    throw (ObjectStateLoadingException) {

    const string procName = "UniversalMachine::loadState";
    const string errorMsg =
        "Tried to load UniversalMachine from an ObjectState tree!";
    Application::writeToErrorLog(__FILE__, __LINE__, procName, errorMsg);
    Application::abortProgram();
}
