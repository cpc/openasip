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
 * @file Guard.cc
 *
 * Implementation of Guard class and its derived classes.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen-no.spam-tut.fi)
 */

#include <string>

#include "Guard.hh"
#include "FUPort.hh"
#include "RegisterFile.hh"
#include "FunctionUnit.hh"
#include "ControlUnit.hh"
#include "Bus.hh"
#include "MOMTextGenerator.hh"
#include "Conversion.hh"

using std::string;
using boost::format;

namespace TTAMachine {

/////////////////////////////////////////////////////////////////////////////
// Guard
/////////////////////////////////////////////////////////////////////////////

// initialization of static data members
const string Guard::OSNAME_GUARD = "guard";
const string Guard::OSKEY_INVERTED = "inverted";


/**
 * Constructor.
 *
 * @param inverted Indicates whether the condition term is inverted.
 * @param parentBus Parent bus component of the guard.
 */
Guard::Guard(bool inverted, Bus& parentBus) :
    inverted_(inverted), parent_(&parentBus) {

    // parentBus.addGuard() cannot be called here because isEqual method
    // does not work until the whole
    // (RegisterGuard/PortGuard/UnconditionalGuard) is instantiated
}


/**
 * Constructor.
 *
 * Loads its state from the given ObjectState instance.
 *
 * @param state The ObjectState instance from which the state is loaded.
 * @param parentBus Parent bus of the guard.
 * @exception ObjectStateLoadingException If the given ObjectState instance
 *                                        is invalid.
 */
Guard::Guard(const ObjectState* state, Bus& parentBus)
    throw (ObjectStateLoadingException) :
    parent_(&parentBus) {

    loadState(state);
}


/**
 * Destructor.
 */
Guard::~Guard() {
    Bus* parent = parent_;
    parent_ = NULL;
    parent->removeGuard(*this);
}


/**
 * Checks whether this guard is more restrictive than the given one.
 *
 * This method is meant for complex (two-term) guards but they are not
 * supported yet, so returns always false.
 *
 * @param guard The guard to compare.
 * @return False.
 */
bool
Guard::isMoreRestrictive(const Guard& guard) const {
    // dummy if-clause to avoid compilation warning of unused parameter
    if (&guard == 0) {
    }
    return false;
}


/**
 * Checks whether this guard is less restrictive than the given one.
 *
 * This method is meant for complex (two-term) guards but they are not
 * supported yet, so returns always false.
 *
 * @param guard The guard to compare.
 * @return False.
 */
bool
Guard::isLessRestrictive(const Guard& guard) const {
    // dummy if-clause to avoid compilation warning of unused parameter
    if (&guard == 0) {
    }
    return false;
}


/**
 * Checks whether this guard and given one are disjoint.
 *
 * The guards are disjoint if neither is the exact subset of the other.
 *
 * @param guard The guard to compare.
 * @return True if the guards are disjoint, otherwise false.
 */
bool
Guard::isDisjoint(const Guard& guard) const {
    return !isEqual(guard);
}


/**
 * Saves the state of the object to an ObjectState object.
 *
 * @return The newly created ObjectState object.
 */
ObjectState*
Guard::saveState() const {
    ObjectState* state = new ObjectState(OSNAME_GUARD);
    state->setAttribute(OSKEY_INVERTED, inverted_);
    return state;
}


/**
 * Loads its state from the given ObjectState instance.
 *
 * @param state The ObjectState instance.
 * @exception ObjectStateLoadingException If the given ObjectState is
 *                                        invalid.
 */
void
Guard::loadState(const ObjectState* state)
    throw (ObjectStateLoadingException) {

    try {
        inverted_ = state->intAttribute(OSKEY_INVERTED);
    } catch (Exception& e) {
        string procName = "Guard::loadState";
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName,
                                          e.errorMessage());
    }
}


/////////////////////////////////////////////////////////////////////////////
// PortGuard
/////////////////////////////////////////////////////////////////////////////

// initialization of static strings used to identify the ObjectState instance
const string PortGuard::OSNAME_PORT_GUARD = "portguard";
const string PortGuard::OSKEY_FU = "fu";
const string PortGuard::OSKEY_PORT = "port";


/**
 * Constructor.
 *
 * @param inverted Incates whether the condition term is inverted.
 * @param port Port from which the condition term is taken.
 * @param parentBus Parent bus component of the guard.
 * @exception IllegalRegistration If the given port is not registered to the
 *                                same machine as the given parent bus.
 * @exception ComponentAlreadyExists If the parent bus already has an equal
 *                                   guard.
 */
PortGuard::PortGuard(bool inverted, FUPort& port, Bus& parentBus)
    throw (IllegalRegistration, ComponentAlreadyExists) :
    Guard(inverted, parentBus), port_(&port) {

    FunctionUnit* unit = port.parentUnit();
    parentBus.ensureRegistration(*unit);
    parentBus.addGuard(*this);
}


/**
 * Constructor.
 *
 * Loads its state from the given ObjectState instance.
 *
 * @param state The ObjectState instance.
 * @param parentBus Parent bus of the guard.
 * @exception ObjectStateLoadingException If the reference to the function
 *                                        unit port cannot be resolved or if
 *                                        the given ObjectState instance is
 *                                        invalid.
 */
PortGuard::PortGuard(const ObjectState* state, Bus& parentBus)
    throw (ObjectStateLoadingException) :
    Guard(state, parentBus) {

    loadState(state);
    try {
        parentBus.addGuard(*this);
    } catch (const ComponentAlreadyExists&) {
        const string procName = "PortGuard::PortGuard";
        MOMTextGenerator textGen;
        format errorMsg = textGen.text(
            MOMTextGenerator::TXT_EQUAL_PORT_GUARDS);
        errorMsg % port()->name() % port()->parentUnit()->name() %
            parentBus.name();
        throw ObjectStateLoadingException(
            __FILE__, __LINE__, procName, errorMsg.str());
    }
}


/**
 * Destructor.
 */
PortGuard::~PortGuard() {
}


/**
 * Returns true if the guard is equal with the given guard.
 *
 * @param guard The other guard.
 * @return True if the guard is equal with the given guard.
 */
bool
PortGuard::isEqual(const Guard& guard) const {
    const PortGuard* portGuard = dynamic_cast<const PortGuard*>(&guard);
    if (portGuard == NULL) {
        return false;
    } else {
        if (port() == portGuard->port() &&
            isInverted() == portGuard->isInverted()) {
            return true;
        } else {
            return false;
        }
    }
}


/**
 * Saves the contents to an ObjectState object.
 *
 * @return The created tree ObjectState object.
 */
ObjectState*
PortGuard::saveState() const {

    ObjectState* guardState = Guard::saveState();
    guardState->setName(OSNAME_PORT_GUARD);

    FunctionUnit* unit = port_->parentUnit();
    guardState->setAttribute(OSKEY_FU, unit->name());
    guardState->setAttribute(OSKEY_PORT, port_->name());

    return guardState;
}


/**
 * Loads its state from the given ObjectState instance.
 *
 * @param state The ObjectState instance.
 * @exception ObjectStateLoadingException If the given ObjectState instance
 *                                        is invalid or if the reference to
 *                                        the function unit port cannot be
 *                                        resolved.
 */
void
PortGuard::loadState(const ObjectState* state)
    throw (ObjectStateLoadingException) {

    string procName = "PortGuard::loadState";
    MOMTextGenerator textGenerator;

    if (state->name() != OSNAME_PORT_GUARD) {
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }

    Guard::loadState(state);

    try {
        string fuName = state->stringAttribute(OSKEY_FU);
        string portName = state->stringAttribute(OSKEY_PORT);

        Machine* mach = parentBus()->machine();
        if (mach == NULL) {
            format text = textGenerator.text(MOMTextGenerator::
                                             TXT_GUARD_REF_LOAD_ERR);
            text % parentBus()->name();
            throw ObjectStateLoadingException(__FILE__, __LINE__, procName,
                                              text.str());
        }

        Machine::FunctionUnitNavigator fuNav = mach->functionUnitNavigator();
        FunctionUnit* fu = NULL;
        FUPort* port = NULL;

        try {
            fu = fuNav.item(fuName);
        } catch (InstanceNotFound& e) {
            format text = textGenerator.text(MOMTextGenerator::
                                             TXT_GUARD_REF_LOAD_ERR_FU);
            text % parentBus()->name() % fuName;
            throw ObjectStateLoadingException(__FILE__, __LINE__, procName,
                                              text.str());
        }

        try {
            port = fu->operationPort(portName);
        } catch (InstanceNotFound& e) {
            format text = textGenerator.text(MOMTextGenerator::
                                             TXT_GUARD_REF_LOAD_ERR_PORT);
            text % parentBus()->name() % portName % fuName;
            throw ObjectStateLoadingException(__FILE__, __LINE__, procName,
                                              text.str());
        }

        port_ = port;

    } catch (Exception& e) {
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName,
                                          e.errorMessage());
    }
}


/////////////////////////////////////////////////////////////////////////////
// RegisterGuard
/////////////////////////////////////////////////////////////////////////////

// initialization of static data members
const string RegisterGuard::OSNAME_REGISTER_GUARD = "registerguard";
const string RegisterGuard::OSKEY_REGFILE = "regfile";
const string RegisterGuard::OSKEY_INDEX = "index";

/**
 * Constructor.
 *
 * @param inverted Indicates whether the condition term is inverted.
 * @param regFile RegisterFile from which the condition term is taken.
 * @param registerIndex Index of the register from which the condition
 *                      term is taken.
 * @param parentBus Parent bus component of the guard.
 * @exception IllegalRegistration If the given register file is not
 *                                registered to the same machine as the
 *                                parent bus of the guard.
 * @exception ComponentAlreadyExists If the parent bus already has an equal
 *                                   guard.
 * @exception OutOfRange If the given register file does not have a register
 *                       by the given register index.
 * @exception InvalidData If local + global guard latency would be zero.
 */
RegisterGuard::RegisterGuard(
    bool inverted,
    RegisterFile& regFile,
    int registerIndex,
    Bus& parentBus)
    throw (IllegalRegistration, ComponentAlreadyExists, OutOfRange,
           InvalidData):
    Guard(inverted, parentBus), regFile_(&regFile),
    registerIndex_(registerIndex) {

    parentBus.ensureRegistration(regFile);
    if (registerIndex < 0 || regFile.numberOfRegisters() <= registerIndex) {
        string procName = "RegisterGuard::RegisterGuard";
        throw OutOfRange(__FILE__, __LINE__, procName);
    }

    // make sure global + local guard latency > 0
    Machine* mach = parentBus.machine();
    ControlUnit* gcu = mach->controlUnit();
    if (gcu != NULL && gcu->globalGuardLatency() == 0 &&
        regFile.guardLatency() == 0) {
        MOMTextGenerator textGen;
        boost::format text = textGen.text(
            MOMTextGenerator::TXT_INVALID_GUARD_LATENCY);
        throw InvalidData(__FILE__, __LINE__, __func__, text.str());
    }

    parentBus.addGuard(*this);
}


/**
 * Constructor.
 *
 * Creates a skeleton object without references to other machine parts.
 * Loads its state from the given ObjectState instance. This constructor
 * should be used by Bus::loadStateWithoutReferences only. Do not use this
 * constructor.
 *
 * @param state The ObjectState instance.
 * @param parentBus Parent bus of the guard.
 * @exception ObjectStateLoadingException If the given ObjectState instance
 *                                        is invalid or if the reference to
 *                                        the register cannot be resolved.
 */
RegisterGuard::RegisterGuard(const ObjectState* state, Bus& parentBus)
    throw (ObjectStateLoadingException) :
    Guard(state, parentBus) {

    loadState(state);
    try {
        parentBus.addGuard(*this);
    } catch (const ComponentAlreadyExists&) {
        const string procName = "RegisterGuard::RegisterGuard";
        MOMTextGenerator textGen;
        format errorMsg = textGen.text(
            MOMTextGenerator::TXT_EQUAL_REGISTER_GUARDS);
        errorMsg % registerIndex() % registerFile()->name() %
            parentBus.name();
        throw ObjectStateLoadingException(
            __FILE__, __LINE__, procName, errorMsg.str());
    }
}


/**
 * Destructor.
 */
RegisterGuard::~RegisterGuard() {
}


/**
 * Returns true if the guard is equal with the given guard.
 *
 * @param guard The other guard.
 * @return True if the guard is equal with the given guard.
 */
bool
RegisterGuard::isEqual(const Guard& guard) const {
    const RegisterGuard* regGuard =
        dynamic_cast<const RegisterGuard*>(&guard);
    if (regGuard == NULL) {
        return false;
    } else {
        if (registerFile() == regGuard->registerFile() &&
            registerIndex() == regGuard->registerIndex() &&
            isInverted() == regGuard->isInverted()) {
            return true;
        } else {
            return false;
        }
    }
}


/**
 * Saves the contents to an ObjectState object.
 *
 * @return The created ObjectState object..
 */
ObjectState*
RegisterGuard::saveState() const {

    ObjectState* guardState = Guard::saveState();
    guardState->setName(OSNAME_REGISTER_GUARD);
    guardState->setAttribute(OSKEY_REGFILE, regFile_->name());
    guardState->setAttribute(OSKEY_INDEX, registerIndex_);

    return guardState;
}


/**
 * Loads its state from the given ObjectState instance.
 *
 * @param state The ObjectState instance.
 * @exception ObjectStateLoadingException If the given ObjectState instance
 *                                        is invalid or if the reference to
 *                                        the register file cannot be
 *                                        resolved.
 */
void
RegisterGuard::loadState(const ObjectState* state)
    throw (ObjectStateLoadingException) {

    string procName = "RegisterGuard::loadState";
    MOMTextGenerator textGenerator;

    if (state->name() != OSNAME_REGISTER_GUARD) {
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }

    if (!parentBus()->isRegistered()) {
        format text = textGenerator.text(MOMTextGenerator::
                                         TXT_GUARD_REF_LOAD_ERR);
        text % parentBus()->name();
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName,
                                          text.str());
    }

    Guard::loadState(state);

    try {
        string regFileName = state->stringAttribute(OSKEY_REGFILE);
        int regIndex = state->intAttribute(OSKEY_INDEX);

        Machine* mach = parentBus()->machine();
        Machine::RegisterFileNavigator regNav =
            mach->registerFileNavigator();
        if (regNav.hasItem(regFileName)) {
            RegisterFile* regFile = regNav.item(regFileName);

            // check local + global guard latency > 0
            ControlUnit* gcu = mach->controlUnit();
            if (regFile->guardLatency() == 0 && gcu != NULL &&
                gcu->globalGuardLatency() == 0) {
                format text = textGenerator.text(
                    MOMTextGenerator::TXT_INVALID_GUARD_LATENCY);
                throw ObjectStateLoadingException(
                    __FILE__, __LINE__, __func__, text.str());
            }
            
            if (regFile->numberOfRegisters() > regIndex) {
                regFile_ = regFile;
                registerIndex_ = regIndex;
            } else {
                format text = textGenerator.text(
                    MOMTextGenerator::TXT_GUARD_REF_LOAD_ERR_REGISTER);
                text % parentBus()->name() % Conversion::toString(regIndex)
                    % regFileName %
                    Conversion::toString(regFile->numberOfRegisters());
                throw ObjectStateLoadingException(__FILE__, __LINE__,
                                                  procName, text.str());
            }
        } else {
            format text = textGenerator.text(MOMTextGenerator::
                                             TXT_GUARD_REF_LOAD_ERR_RF);
            text % parentBus()->name() % regFileName;
            throw ObjectStateLoadingException(__FILE__, __LINE__, procName,
                                              text.str());
        }
    } catch (Exception& e) {
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName,
                                          e.errorMessage());
    }
}


/////////////////////////////////////////////////////////////////////////////
// UnconditionalGuard
/////////////////////////////////////////////////////////////////////////////

// initialization of static data members
const string
UnconditionalGuard::OSNAME_UNCONDITIONAL_GUARD = "unconditional";

/**
 * Constructor.
 *
 * @param inverted If true, the guard condition is always false and
 *                 vice versa.
 * @param parentBus Parent bus component of the guard.
 * @exception ComponentAlreadyExists If the parent bus already has an equal
 *                                   guard.
 */
UnconditionalGuard::UnconditionalGuard(bool inverted, Bus& parentBus)
    throw (ComponentAlreadyExists) :
    Guard(inverted, parentBus) {

    parentBus.addGuard(*this);
}


/**
 * Constructor.
 *
 * Loads its state from the given ObjectState instance.
 *
 * @param state The ObjectState instance.
 * @param parentBus The parent bus of the guard.
 * @exception ObjectStateLoadingException If the given ObjectState instance
 *                                        is invalid.
 */
UnconditionalGuard::UnconditionalGuard(
    const ObjectState* state,
    Bus& parentBus)
    throw (ObjectStateLoadingException) :
    Guard(state, parentBus) {

    loadState(state);
    try {
        parentBus.addGuard(*this);
    } catch (const ComponentAlreadyExists&) {
        const string procName = "UnconditionalGuard::UnconditionalGuard";
        MOMTextGenerator textGen;
        format errorMsg = textGen.text(
            MOMTextGenerator::TXT_EQUAL_UNCONDITIONAL_GUARDS);
        errorMsg % parentBus.name();
        throw ObjectStateLoadingException(
            __FILE__,  __LINE__, procName, errorMsg.str());
    }
}


/**
 * Destructor.
 */
UnconditionalGuard::~UnconditionalGuard() {
}


/**
 * Returns true if the guard is equal with the given guard.
 *
 * @param guard The other guard.
 * @return True if the guard is equal with the given guard.
 */
bool
UnconditionalGuard::isEqual(const Guard& guard) const {
    const UnconditionalGuard* ucGuard =
        dynamic_cast<const UnconditionalGuard*>(&guard);
    if (ucGuard == NULL) {
        return false;
    } else {
        if (isInverted() == ucGuard->isInverted()) {
            return true;
        } else {
            return false;
        }
    }
}


/**
 * Saves the contents to an ObjectState object.
 *
 * @return The created ObjectState object.
 */
ObjectState*
UnconditionalGuard::saveState() const {
    ObjectState* guardState = Guard::saveState();
    guardState->setName(OSNAME_UNCONDITIONAL_GUARD);
    return guardState;
}


/**
 * Loads its state from the given ObjectState instance.
 *
 * @param state The ObjectState instance.
 * @exception ObjectStateLoadingException If the given ObjectState instance
 *                                         is invalid.
 */
void
UnconditionalGuard::loadState(const ObjectState* state)
    throw (ObjectStateLoadingException) {

    string procName = "UnconditionalGuard::loadState";

    if (state->name() != OSNAME_UNCONDITIONAL_GUARD) {
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }

    Guard::loadState(state);
}

}
