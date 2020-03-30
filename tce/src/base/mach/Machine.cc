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
 * @file Machine.cc
 *
 * Implementation of Machine class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen-no.spam-tut.fi)
 * @note reviewed 16 Jun 2004 by ml, tr, jm, ll
 * @note rating: red
 */

#include <string>
#include <set>
#include <boost/functional/hash.hpp>

#include "Machine.hh"
#include "Bus.hh"
#include "Segment.hh"
#include "Socket.hh"
#include "Bridge.hh"
#include "AddressSpace.hh"
#include "InstructionTemplate.hh"
#include "ImmediateUnit.hh"
#include "FunctionUnit.hh"
#include "RegisterFile.hh"
#include "ControlUnit.hh"
#include "MachineTester.hh"
#include "DummyMachineTester.hh"
#include "ContainerTools.hh"
#include "Guard.hh"
#include "FUPort.hh"
#include "ImmediateSlot.hh"
#include "Application.hh"
#include "ADFSerializer.hh"
#include "ObjectState.hh"

using std::string;
using std::set;

namespace TTAMachine {

// initialization of static data members
const string Machine::OSNAME_MACHINE = "machine";
const string Machine::OSKEY_ALWAYS_WRITE_BACK_RESULTS 
	= "always-write-back";
const string Machine::OSKEY_TRIGGER_INVALIDATES_OLD_RESULTS 
	= "trigger-invalidates";
const string Machine::OSKEY_FUNCTION_UNITS_ORDERED = "fu-ordered";

/**
 * Constructor.
 */
Machine::Machine() : 
    controlUnit_(NULL), doValidityChecks_(true), 
    machineTester_(new MachineTester(*this)), 
    dummyMachineTester_(new DummyMachineTester(*this)),
    EMPTY_ITEMP_NAME_("no_limm"), alwaysWriteResults_(false), 
    triggerInvalidatesResults_(false), fuOrdered_(false),
    littleEndian_(true) {

    new InstructionTemplate(EMPTY_ITEMP_NAME_, *this);
}
    
    
/**
 * Copy constructor.
 *
 * Creates a copy of the given machine.
 *
 * @param old The machine to be copied.
 */
Machine::Machine(const Machine& old) : 
    Serializable(), controlUnit_(NULL), doValidityChecks_(false),
    machineTester_(new MachineTester(*this)), 
    dummyMachineTester_(new DummyMachineTester(*this)),
    littleEndian_(old.littleEndian_) {
    
    ObjectState* state = old.saveState();
    loadState(state);
    delete state;
    doValidityChecks_ = true;
}
    
        
/**
 * Destructor.
 *
 * Deletes all the components of the machine as well.
 */
Machine::~Machine() {

    if (controlUnit_ != NULL) {
        delete controlUnit_;
    }

    delete machineTester_;
    delete dummyMachineTester_;

    // NOTE! other components are deleted in ComponentContainer's destructor
}

bool
Machine::isUniversalMachine() const {
    return false;
}

/**
 * Adds a bus into the machine.
 *
 * @param bus Bus being added.
 * @exception ComponentAlreadyExists If a bus or immediate slot by the same
 *                                   name already exists in the machine.
 */
void
Machine::addBus(Bus& bus) {
    // check that the name is unique among immediate slots
    if (immediateSlotNavigator().hasItem(bus.name())) {
        const string procName = "Machine::addBus";
        throw ComponentAlreadyExists(__FILE__, __LINE__, procName);
    }

    addComponent(busses_, bus);
}

/**
 * Adds a socket into the machine.
 *
 * @param socket Socket being added.
 * @exception ComponentAlreadyExists If a socket by the same name already
 *                                   exists in the machine.
 */
void
Machine::addSocket(Socket& socket) {
    addComponent(sockets_, socket);
}

/**
 * Adds a unit into the machine.
 *
 * Use methods intended to add particular unit types like addFunctionUnit,
 * addRegisterFile and addImmediateUnit if possible. Global control unit
 * must be set using setGlobalControl function.
 *
 * @param unit Unit being added.
 * @exception ComponentAlreadyExists If a unit by the same name and type
 *                                   already exists in the machine.
 * @exception IllegalParameters If ControlUnit is tried to add with this
 *                              method.
 */
void
Machine::addUnit(Unit& unit) {
    FunctionUnit* fu = dynamic_cast<FunctionUnit*>(&unit);
    ImmediateUnit* iu = dynamic_cast<ImmediateUnit*>(&unit);
    RegisterFile* rf = dynamic_cast<RegisterFile*>(&unit);

    if (fu != NULL) {
        addFunctionUnit(*fu);
    } else if (iu != NULL) {
        addImmediateUnit(*iu);
    } else if (rf != NULL) {
        addRegisterFile(*rf);
    } else {
        const string procName = "Machine::addUnit";
        throw IllegalParameters(__FILE__, __LINE__, procName);
    }
}

/**
 * Adds the given function unit to the machine.
 *
 * @param unit The function unit to be added.
 * @exception ComponentAlreadyExists If a function unit by the same
 *                                   name exists already.
 * @exception IllegalParameters If tried to add control unit with this
 *                              method.
 */
void
Machine::addFunctionUnit(FunctionUnit& unit) {
    const string procName = "Machine::addFunctionUnit";

    if (dynamic_cast<ControlUnit*>(&unit) != NULL) {
        throw IllegalParameters(__FILE__, __LINE__, procName);
    }

    if (controlUnit() != NULL && controlUnit()->name() == unit.name()) {
        throw ComponentAlreadyExists(__FILE__, __LINE__, procName);
    }

    addComponent(functionUnits_, unit);
}

/**
 * Adds the given immediate unit to the machine.
 *
 * @param unit The immediate unit to be added.
 * @exception ComponentAlreadyExists If an immediate unit by the same
 *                                   name exists already.
 */
void
Machine::addImmediateUnit(ImmediateUnit& unit) {
    addComponent(immediateUnits_, unit);
}

/**
 * Adds the given register file to the machine.
 *
 * @param unit The register file to be added.
 * @exception ComponentAlreadyExists If a register file by the same name
 *                                   exists already.
 */
void
Machine::addRegisterFile(RegisterFile& unit) {
    addComponent(registerFiles_, unit);
}

/**
 * Adds an address space into the machine.
 *
 * @param as AddressSpace being added.
 * @exception ComponentAlreadyExists If an address space by the same name
 *                                   already exists in the machine.
 */
void
Machine::addAddressSpace(AddressSpace& as) {
    addRegisteredComponent(addressSpaces_, as);
}

/**
 * Adds a bridge into the machine.
 *
 * This method should be called from Bridge constructor only since bridges
 * are added to machine at construction. Do not call this method.
 *
 * @param bridge Bridge being added.
 * @exception ComponentAlreadyExists If a bridge by the same name already
 *                                   exists in the machine.
 */
void
Machine::addBridge(Bridge& bridge) {
    addRegisteredComponent(bridges_, bridge);
}

/**
 * Adds an instruction template for the machine.
 *
 * @param instructionTemplate The instruction template to be added.
 * @exception ComponentAlreadyExists If an instruction template by the same
 *                                   name already exists in the machine.
 */
void
Machine::addInstructionTemplate(InstructionTemplate& instrTempl) {
    addRegisteredComponent(instructionTemplates_, instrTempl);
}

/**
 * Adds an immediate slot to (instruction of) the machine.
 *
 * @param slot The immediate slot to be added.
 * @exception ComponentAlreadyExists If an immediate slot or bus by the same
 *                                   name already exists in the machine.
 */
void
Machine::addImmediateSlot(ImmediateSlot& slot) {
    // check that the name is unique among buses
    if (busNavigator().hasItem(slot.name())) {
        const string procName = "Machine::addImmediateSlot";
        throw ComponentAlreadyExists(__FILE__, __LINE__, procName);
    }

    addRegisteredComponent(immediateSlots_, slot);
}

/**
 * Sets the global control unit for the machine.
 *
 * @param unit The new global control unit.
 * @exception ComponentAlreadyExists If a control unit already exists in the
 *                                   machine.
 */
void
Machine::setGlobalControl(ControlUnit& unit) {
    const string procName = "Machine::setGlobalControl";

    if (controlUnit_ != NULL) {
        throw ComponentAlreadyExists(__FILE__, __LINE__, procName);
    }

    // check that the name is unique among function units
    FunctionUnitNavigator fuNav = functionUnitNavigator();
    if (fuNav.hasItem(unit.name())) {
        throw ComponentAlreadyExists(__FILE__, __LINE__, procName);
    }

    if (unit.machine() == NULL) {
        unit.setMachine(*this);
    } else {
        controlUnit_ = &unit;
    }
}

/**
 * Returns the control unit of the machine.
 *
 * Returns null pointer if there is no control unit in the machine.
 *
 * @return The control unit of the machine.
 */
ControlUnit*
Machine::controlUnit() const {
    return controlUnit_;
}


/**
 * Returns a handle to the busses in the machine.
 *
 * @return Handle to the busses in the machine.
 */
Machine::BusNavigator
Machine::busNavigator() const {
    BusNavigator navigator(busses_);
    return navigator;
}


/**
 * Returns a handle to the sockets in the machine.
 *
 * @return Handle to the sockets in the machine.
 */
Machine::SocketNavigator
Machine::socketNavigator() const {
    SocketNavigator navigator(sockets_);
    return navigator;
}


/**
 * Returns a handle to the function units in the machine.
 *
 * @return Handle to the function units in the machine.
 */
Machine::FunctionUnitNavigator
Machine::functionUnitNavigator() const {
    FunctionUnitNavigator navigator(functionUnits_);
    return navigator;
}


/**
 * Returns a handle to the address spaces in the machine.
 *
 * @return Handle to the address spaces in the machine.
 */
Machine::AddressSpaceNavigator
Machine::addressSpaceNavigator() const {
    AddressSpaceNavigator navigator(addressSpaces_);
    return navigator;
}


/**
 * Returns a handle to the bridges in the machine.
 *
 * @return Handle to the bridges in the machine.
 */
Machine::BridgeNavigator
Machine::bridgeNavigator() const {
    BridgeNavigator navigator(bridges_);
    return navigator;
}


/**
 * Returns a handle to the immediate units in the machine.
 *
 * @return Handle to the immediate units in the machine.
 */
Machine::ImmediateUnitNavigator
Machine::immediateUnitNavigator() const {
    ImmediateUnitNavigator navigator(immediateUnits_);
    return navigator;
}


/**
 * Returns a handle to the instruction templates in the machine.
 *
 * @return Handle to the instruction templates in the machine.
 */
Machine::InstructionTemplateNavigator
Machine::instructionTemplateNavigator() const {
    InstructionTemplateNavigator navigator(instructionTemplates_);
    return navigator;
}


/**
 * Returns a handle to the register files in the machine.
 *
 * @return Handle to the register files in the machine.
 */
Machine::RegisterFileNavigator
Machine::registerFileNavigator() const {
    RegisterFileNavigator navigator(registerFiles_);
    return navigator;
}


/**
 * Returns a handle to the immediate slots in the machine.
 *
 * @return Handle to the immediate slots in the machine.
 */
Machine::ImmediateSlotNavigator
Machine::immediateSlotNavigator() const {
    ImmediateSlotNavigator navigator(immediateSlots_);
    return navigator;
}


/**
 * Removes bus from the machine.
 *
 * The machine will stay consistent after removal.
 *
 * @param bus Bus to be removed.
 * @exception InstanceNotFound If the machine does not have the given bus.
 */
void
Machine::removeBus(Bus& bus) {
    removeComponent(busses_, bus);
}

/**
 * Removes socket from the machine.
 *
 * The machine will stay consistent after removal.
 *
 * @param socket Socket to be removed.
 * @exception InstanceNotFound If the machine does not have the given socket.
 */
void
Machine::removeSocket(Socket& socket) {
    removeComponent(sockets_, socket);
}

/**
 * Removes unit from the machine.
 *
 * The machine will stay consistent after removal. Global control unit must
 * be removed using unsetGlobalControl method.
 *
 * @param unit Unit to be removed.
 * @exception InstanceNotFound If the machine does not have the given unit.
 * @exception IllegalParameters If ControlUnit is tried to remove.
 */
void
Machine::removeUnit(Unit& unit) {
    FunctionUnit* fu = dynamic_cast<FunctionUnit*>(&unit);
    ImmediateUnit* iu = dynamic_cast<ImmediateUnit*>(&unit);
    RegisterFile* rf = dynamic_cast<RegisterFile*>(&unit);

    if (fu != NULL) {
        removeFunctionUnit(*fu);
    } else if (iu != NULL) {
        removeImmediateUnit(*iu);
    } else if (rf != NULL) {
        removeRegisterFile(*rf);
    } else {
        const string procName = "Machine::removeUnit";
        throw IllegalParameters(__FILE__, __LINE__, procName);
    }
}

/**
 * Removes the given function unit from the machine.
 *
 * @param unit The function unit to be removed.
 * @exception InstanceNotFound If the given function unit does not belong to
 *                             the machine.
 */
void
Machine::removeFunctionUnit(FunctionUnit& unit) {
    removeComponent(functionUnits_, unit);
}

/**
 * Removes the given immediate unit from the machine.
 *
 * @param unit The immediate unit to be removed.
 * @exception InstanceNotFound If the given immediate unit does not belong to
 *                             the machine.
 */
void
Machine::removeImmediateUnit(ImmediateUnit& unit) {
    removeComponent(immediateUnits_, unit);
}

/**
 * Removes the given register file from the machine.
 *
 * @param unit The register file to be removed.
 * @exception InstanceNotFound If the given register file does not belong to
 *                             the machine.
 */
void
Machine::removeRegisterFile(RegisterFile& unit) {
    removeComponent(registerFiles_, unit);
}

/**
 * Removes the global control unit from the machine.
 *
 * WARNING: The unit is not deleted. You should have a reference to it to be
 * able to delete it later.
 */
void
Machine::unsetGlobalControl() {
    if (controlUnit_ == NULL) {
        return;
    } else {
        if (controlUnit_->machine() == NULL) {
            controlUnit_ = NULL;
        } else {
            controlUnit_->unsetMachine();
        }
    }
}


/**
 * Deletes the given bridge.
 *
 * The machine will stay consistent after deletion.
 *
 * @param bridge Bridge to be deleted.
 * @exception InstanceNotFound If the machine does not have the given bridge.
 */
void
Machine::deleteBridge(Bridge& bridge) {
    deleteComponent(bridges_, bridge);
}

/**
 * Deletes the given instruction template.
 *
 * @param instrTempl The instruction template to be deleted.
 * @exception InstanceNotFound If the machine does not have the given
 *                             instruction template.
 */
void
Machine::deleteInstructionTemplate(InstructionTemplate& instrTempl) {
    deleteComponent(instructionTemplates_, instrTempl);
}

/**
 * Deletes the given address space.
 *
 * @param as The address space to be deleted.
 * @exception InstanceNotFound If the machine does not have the given address
 *                             space.
 */
void
Machine::deleteAddressSpace(AddressSpace& as) {
    deleteComponent(addressSpaces_, as);
}

/**
 * Deletes the given immediate slot.
 *
 * @param slot The immediate slot to be deleted.
 * @exception InstanceNotFound If the machine does not have the given
 *                             immediate slot.
 */
void
Machine::deleteImmediateSlot(ImmediateSlot& slot) {
    deleteComponent(immediateSlots_, slot);
}

/**
 * Sets new position for the given bus.
 *
 * @param bus The bus being moved.
 * @param newPosition The index of the new position 
 *                    (the first position is 0).
 * @exception IllagalRegistration If the given bus is not registered to the 
 *                                machine.
 * @exception OutOfRange If the given position is less than 0 or not smaller
 *                       than the number of buses in the machine.
 */
void
Machine::setBusPosition(const Bus& bus, int newPosition) {
    const string procName = "Machine::setBusPosition";

    if (bus.machine() != this) {
        throw IllegalRegistration(__FILE__, __LINE__, procName);
    }

    if (newPosition < 0 || newPosition >= busNavigator().count()) {
        throw OutOfRange(__FILE__, __LINE__, procName);
    }

    busses_.moveToPosition(&bus, newPosition);
}

/**
 * Returns the machine tester for the machine.
 *
 * @return The machine tester for the machine.
 */
MachineTester&
Machine::machineTester() const {
    if (doValidityChecks_) {
        return *machineTester_;
    } else {
        return *dummyMachineTester_;
    }
}


/**
 * Saves the machine to ObjectState tree.
 *
 * @return Root of the created ObjectState tree.
 */
ObjectState*
Machine::saveState() const {

    ObjectState* rootState = new ObjectState(OSNAME_MACHINE);

    saveComponentStates(busses_, rootState);
    saveComponentStates(sockets_, rootState);
    saveComponentStates(bridges_, rootState);
    saveComponentStates(functionUnits_, rootState);
    saveComponentStates(registerFiles_, rootState);
    saveComponentStates(immediateUnits_, rootState);
    saveComponentStates(addressSpaces_, rootState);
    saveComponentStates(instructionTemplates_, rootState);
    saveComponentStates(immediateSlots_, rootState);

    // save global control unit
    if (controlUnit_ != NULL) {
        rootState->addChild(controlUnit_->saveState());
    }

    rootState->setAttribute(
        OSKEY_ALWAYS_WRITE_BACK_RESULTS, alwaysWriteResults_);
    rootState->setAttribute(
        OSKEY_TRIGGER_INVALIDATES_OLD_RESULTS, triggerInvalidatesResults_);
    rootState->setAttribute(
        OSKEY_FUNCTION_UNITS_ORDERED, fuOrdered_);
    rootState->setAttribute("little-endian", littleEndian_);
    
    return rootState;
}


/**
 * Loads the state of the machine from the given ObjectState tree.
 *
 * @param state Root node of the ObjectState tree.
 * @exception ObjectStateLoadingException If the given ObjectState tree is
 *                                        invalid.
 */
void
Machine::loadState(const ObjectState* state) {
    string procName = "Machine::loadState";

    if (state->name() != OSNAME_MACHINE) {
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }

    // delete all the old components
    busses_.deleteAll();
    sockets_.deleteAll();
    instructionTemplates_.deleteAll();
    registerFiles_.deleteAll();
    immediateUnits_.deleteAll();
    functionUnits_.deleteAll();
    addressSpaces_.deleteAll();
    bridges_.deleteAll();
    immediateSlots_.deleteAll();

    if (controlUnit_ != NULL) {
        delete controlUnit_;
        controlUnit_ = NULL;
    }

    Component* toAdd = NULL;

    setAlwaysWriteResults(
        state->hasAttribute(OSKEY_ALWAYS_WRITE_BACK_RESULTS) &&
        state->boolAttribute(OSKEY_ALWAYS_WRITE_BACK_RESULTS));        
    setTriggerInvalidatesResults(
        state->hasAttribute(OSKEY_TRIGGER_INVALIDATES_OLD_RESULTS) &&
        state->boolAttribute(OSKEY_TRIGGER_INVALIDATES_OLD_RESULTS));
    setFUOrdered(
        state->hasAttribute(OSKEY_FUNCTION_UNITS_ORDERED) &&
        state->boolAttribute(OSKEY_FUNCTION_UNITS_ORDERED));
    setLittleEndian(
        state->hasAttribute("little-endian") &&
        state->boolAttribute("little-endian"));

    try {
        // create skeletons
        for (int i = 0; i < state->childCount(); i++) {
            toAdd = NULL;
            ObjectState* child = state->child(i);
            if (child->name() == Bus::OSNAME_BUS) {
                Bus* bus = new Bus(child);
                toAdd = bus;
                addBus(*bus);
            } else if (child->name() == Socket::OSNAME_SOCKET) {
                Socket* socket = new Socket(child);
                toAdd = socket;
                addSocket(*socket);
            } else if (child->name() == Bridge::OSNAME_BRIDGE) {
                // bridge is registered automatically
                new Bridge(child, *this);
            } else if (child->name() == AddressSpace::OSNAME_ADDRESS_SPACE) {
                // address space is registered automatically and its state is
                // loaded completely --> no need to call loadState later
                new AddressSpace(child, *this);
            } else if (child->name() == RegisterFile::OSNAME_REGISTER_FILE) {
                RegisterFile* regFile = new RegisterFile(child);
                toAdd = regFile;
                addRegisterFile(*regFile);
            } else if (child->name() == FunctionUnit::OSNAME_FU) {
                FunctionUnit* fu = new FunctionUnit(child);
                toAdd = fu;
                addFunctionUnit(*fu);
            } else if (child->name() == 
                    ImmediateUnit::OSNAME_IMMEDIATE_UNIT) {
                ImmediateUnit* immUnit = new ImmediateUnit(child);
                toAdd = immUnit;
                addImmediateUnit(*immUnit);
            } else if (child->name() == ControlUnit::OSNAME_CONTROL_UNIT) {
                ControlUnit* cUnit = new ControlUnit(child);
                toAdd = cUnit;
                setGlobalControl(*cUnit);
            } else if (child->name() == ImmediateSlot::OSNAME_IMMEDIATE_SLOT) {
                new ImmediateSlot(child, *this);
            } else if (child->name() != 
                       InstructionTemplate::OSNAME_INSTRUCTION_TEMPLATE) {
                throw ObjectStateLoadingException(
                    __FILE__, __LINE__, procName);
            }
        }

    } catch (const Exception& e) {
        if (toAdd != NULL) {
            delete toAdd;
        }
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName,
                                          e.errorMessage());
    }

    // load states of each component
    try {
        for (int i = 0; i < state->childCount(); i++) {
            ObjectState* child = state->child(i);
            string name = child->stringAttribute(Component::OSKEY_NAME);
            if (child->name() == Bus::OSNAME_BUS) {
                BusNavigator busNav = busNavigator();
                Bus* bus = busNav.item(name);
                bus->loadState(child);
            } else if (child->name() == Socket::OSNAME_SOCKET) {
                SocketNavigator socketNav = socketNavigator();
                Socket* socket = socketNav.item(name);
                socket->loadState(child);
            } else if (child->name() == Bridge::OSNAME_BRIDGE) {
                BridgeNavigator bridgeNav = bridgeNavigator();
                Bridge* bridge = bridgeNav.item(name);
                bridge->loadState(child);
            } else if (child->name() == RegisterFile::OSNAME_REGISTER_FILE) {
                RegisterFileNavigator rfNav = registerFileNavigator();
                RegisterFile* rf = rfNav.item(name);
                rf->loadState(child);
            } else if (child->name() == FunctionUnit::OSNAME_FU) {
                FunctionUnitNavigator fuNav = functionUnitNavigator();
                FunctionUnit* fu = fuNav.item(name);
                fu->loadState(child);
            } else if (child->name() ==
                       ImmediateUnit::OSNAME_IMMEDIATE_UNIT) {
                ImmediateUnitNavigator iuNav = immediateUnitNavigator();
                ImmediateUnit* iu = iuNav.item(name);
                iu->loadState(child);
            } else if (child->name() ==
                       ControlUnit::OSNAME_CONTROL_UNIT) {
                ControlUnit* cu = controlUnit();
                cu->loadState(child);
            } else if (child->name() ==
                       InstructionTemplate::OSNAME_INSTRUCTION_TEMPLATE) {
                // instruction template loads its state completely
                new InstructionTemplate(child, *this);
            }
        }
    } catch (const Exception& e) {
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName,
                                          e.errorMessage());
    }

    // create an empty instruction template if there is no instruction
    // templates
    InstructionTemplateNavigator itNav = instructionTemplateNavigator();
    if (itNav.count() == 0) {
        new InstructionTemplate(EMPTY_ITEMP_NAME_, *this);
    }
}

/**
 * Copies state from one machine to this machine.
 * (used for object copying).
 * 
 * @param machine machine to copy from
 * 
 */
void
Machine::copyFromMachine(Machine& machine) {
    ObjectState* state = machine.saveState();
    loadState(state);
    delete state;
}


/**
 * Loads a Machine from the given ADF file.
 *
 * @param adfFileName The name of the file to load the ADF from.
 * @return A machine instance.
 * @exception Exception In case some error occured.
 */
TTAMachine::Machine*
Machine::loadFromADF(const std::string& adfFileName) {
    ADFSerializer serializer;
    serializer.setSourceFile(adfFileName);

    return serializer.readMachine();
}

void
Machine::writeToADF(const std::string& adfFileName) const {
    ADFSerializer serializer;
    serializer.setDestinationFile(adfFileName);
    serializer.writeMachine(*this);
}

/**
 * Returns a hash string of the machine to determine quickly
 * in case two machines are the same.
 *
 * The hash consists of a 32bit hash of the .adf xml data concat with
 * the .adf data length as a hex string. 
 *
 * @note The hash string is dependent on the XML format and does 
 * not account any architecture file changes that still retain
 * the identical architecture.
 */
TCEString
Machine::hash() const {
    ADFSerializer serializer;
    std::string buffer;
    serializer.setDestinationString(buffer);
    serializer.writeMachine(*this);

    boost::hash<std::string> string_hasher;
    size_t h = string_hasher(buffer);

    TCEString hash =
        (Conversion::toHexString(buffer.length())).substr(2);

    hash += "_";
    hash += (Conversion::toHexString(h)).substr(2);
    return hash;
}

/*
 * Returns true if result value always needs to be written to GPR.
 *
 */
bool 
Machine::alwaysWriteResults() const {
    return alwaysWriteResults_;
}
    
/*
 * Returns true if triggering make content of the register where result will
 * be stored invalid.
 *
 */    
bool 
Machine::triggerInvalidatesResults() const {
    return triggerInvalidatesResults_;
}

/*
 * Returns true if sequential order of FUs in ADF file is significant.
 *
 * In certain architectures (Cell SPU) the "relative order" of the function 
 * units matters when it comes to accessing same register by multiple operations
 * in the same cycle. 
 *
 */    
bool 
Machine::isFUOrdered() const {
    return fuOrdered_;
}

/* 
 * Sets whether or not result must always be written to register.
 */
void 
Machine::setAlwaysWriteResults(bool result){
    alwaysWriteResults_ = result;
}
    
/* 
 * Sets whether triggering invalidates register where result will be stored.
 */
    
void 
Machine::setTriggerInvalidatesResults(bool trigger) {
    triggerInvalidatesResults_ = trigger;
}
/* 
 * Sets whether or not order of FUs in ADF file is significant.
 *
 * In certain architectures (Cell SPU) the "relative order" of the function 
 * units matters when it comes to accessing same register by multiple operations
 * in the same cycle. 
 */
void 
Machine::setFUOrdered(bool order){
    fuOrdered_ = order;
}

/**
 * Saves the state of each component in the given container and add the
 * created ObjectStates as children of the given parent ObjectState.
 *
 * @param container The container.
 * @param parent The parent ObjectState.
 */
template <typename ContainerType>
void
Machine::saveComponentStates(ContainerType& container, ObjectState* parent) {
    for (int i = 0; i < container.count(); i++) {
        Serializable* component = container.item(i);
        parent->addChild(component->saveState());
    }
}

/**
 * Gets the maximum latency of any operation supported by this machine.
 */
int
Machine::maximumLatency() const {
    int maxLatency = 0;
    FunctionUnitNavigator fuNav = functionUnitNavigator();
    for (int i = 0; i < fuNav.count(); i++) {
        FunctionUnit* fu = fuNav.item(i);
        int fuLatency = fu->maxLatency();
        if (fuLatency > maxLatency) {
            maxLatency = fuLatency;
        }
    }
    return maxLatency;
}

/**
 * Tells whether any FU in the machine has given operation or not
 *
 * @param opName name of the operation.
 */
bool
Machine::hasOperation(const TCEString& opName) const {
    FunctionUnitNavigator fuNav = functionUnitNavigator();
    for (int i = 0; i < fuNav.count(); i++) {
        FunctionUnit* fu = fuNav.item(i);
        if (fu->hasOperation(opName)) {
            return true;
        }
    }
    if (controlUnit()->hasOperation(opName)) {
        return true;
    }
    return false;
}
}
