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
 * @file ControlUnit.cc
 *
 * Implementation of ControlUnit class.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen-no.spam-tut.fi)
 * @author Pekka Jääskeläinen 2006 (pekka.jaaskelainen-no.spam-tut.fi)
 */

#include "ControlUnit.hh"
#include "SpecialRegisterPort.hh"
#include "Guard.hh"
#include "MOMTextGenerator.hh"
#include "Application.hh"
#include "Machine.hh"
#include "ObjectState.hh"

using std::string;

namespace TTAMachine {

// initialization of static data members
const string ControlUnit::OSNAME_CONTROL_UNIT = "control_unit";
const string ControlUnit::OSKEY_DELAY_SLOTS = "d_slots";
const string ControlUnit::OSKEY_GUARD_LATENCY = "g_latency";
const string ControlUnit::OSKEY_RA_PORT = "ra_port";

/**
 * Constructor.
 *
 * @param name Name of the control unit.
 * @param delaySlots Number of delay instruction slots of the transport pipeline.
 * @param globalGuardLatency The global guard latency.
 * @exception OutOfRange If some of the given values is out of valid range.
 * @exception InvalidName If the given name is not a valid component name.
 */
ControlUnit::ControlUnit(
    const string& name,
    int delaySlots,
    int globalGuardLatency)
    throw (OutOfRange, InvalidName):
    FunctionUnit(name), delaySlots_(delaySlots),
    globalGuardLatency_(0), raPort_(NULL) {

    setDelaySlots(delaySlots);
    setGlobalGuardLatency(globalGuardLatency);
}


/**
 * Constructor.
 *
 * Loads the state of the control unit from the given ObjectState instance.
 * Does not load references to other components.
 *
 * @param state The ObjectState instance from which the name is taken.
 * @exception ObjectStateLoadingException If the given ObjectState instance
 *                                        is invalid.
 */
ControlUnit::ControlUnit(const ObjectState* state)
    throw (ObjectStateLoadingException) :
    FunctionUnit(state), delaySlots_(0), globalGuardLatency_(0), 
    raPort_(NULL) {
        
    loadStateWithoutReferences(state);
}


/**
 * Destructor.
 */
ControlUnit::~ControlUnit() {
    unsetMachine();
}


/**
 * Copies the instance.
 *
 * Current FunctionUnit state is copied to a new FunctionUnit object.
 *
 * @return Copy of the instance.
 */
ControlUnit*
ControlUnit::copy() const {

    return new ControlUnit(saveState());
}


/**
 * Attaches the control unit to machine.
 *
 * @param mach The machine.
 * @exception ComponentAlreadyExists If the given machine already has a
 *                                   global control unit.
 */
void
ControlUnit::setMachine(Machine& mach)
    throw (ComponentAlreadyExists) {

    // if global guard latency is zero, there cannot be a register guard
    // that reads a register of local guard latency zero
    if (globalGuardLatency() == 0 && hasLocalGuardLatencyOfZero(mach)) {
        setGlobalGuardLatency(1);
    }   

    internalSetMachine(mach);
    mach.setGlobalControl(*this);
}


/**
 * Detaches the control unit from machine.
 */
void
ControlUnit::unsetMachine() {
    if (!isRegistered()) {
        return;
    } else {
        Machine* mach = machine();
        unsetMachineDerived();
        mach->unsetGlobalControl();
    }
}


/**
 * Sets the number of delay instruction slots of the transport pipeline.
 *
 * The number of delay slots must be a nonnegative integer.
 *
 * @param delaySlots Number of delay slots of the transport pipeline.
 * @exception OutOfRange If the given delay slots is negative
 */
void
ControlUnit::setDelaySlots(int delaySlots)
    throw (OutOfRange) {

    if (delaySlots < 0) {
        throw OutOfRange(__FILE__, __LINE__, __func__);
    }
    delaySlots_ = delaySlots;
}


/**
 * Sets the global guard latency.
 *
 * @param latency The new latency.
 * @exception OutOfRange If the given latency is negative.
 */
void
ControlUnit::setGlobalGuardLatency(int latency)
    throw (OutOfRange) {

    if (latency < 0) {
        throw OutOfRange(__FILE__, __LINE__, __func__);
    }

    // if latency is zero, there must not be a guard term that reads a 
    // registr of a register file with local guard latency of zero
    if (latency == 0 && isRegistered()) {
        if (hasLocalGuardLatencyOfZero(*machine())) {
            MOMTextGenerator textGen;
            boost::format text = textGen.text(
                MOMTextGenerator::TXT_INVALID_GUARD_LATENCY);
            throw OutOfRange(
                __FILE__, __LINE__, __func__, text.str());
        }
    }

    globalGuardLatency_ = latency;
}


/**
 * Returns the number of special register ports in the control unit.
 *
 * @return The number of special register ports.
 */
int
ControlUnit::specialRegisterPortCount() const {
    return portCount() - operationPortCount();
}


/**
 * Tells whether the control unit has a special register port of the given
 * name.
 *
 * @return True if the control unit has the port, otherwise false.
 */
bool
ControlUnit::hasSpecialRegisterPort(const std::string& name) const {
    return hasPort(name) && !hasOperationPort(name);
}


/**
 * Returns a special register port by the given index.
 *
 * @param index The index.
 * @return A special register port.
 * @exception OutOfRange If the given index is less than 0 or greater or
 *                       equal to the number of special register ports.
 */
SpecialRegisterPort*
ControlUnit::specialRegisterPort(int index) const
    throw (OutOfRange) {

    const string procName = "ControlUnit::specialRegisterPort";

    if (index < 0) {
        throw OutOfRange(__FILE__, __LINE__, procName);
    }

    int portCount = this->portCount();
    int srPortCount(-1);

    for (int i = 0; i < portCount; i++) {
        BaseFUPort* port = this->port(i);
        SpecialRegisterPort* srPort =
            dynamic_cast<SpecialRegisterPort*>(port);
        if (srPort != NULL) {
            srPortCount++;
            if (srPortCount == index) {
                return srPort;
            }
        }
    }

    throw OutOfRange(__FILE__, __LINE__, procName);
}


/**
 * Returns a special register port by the given name.
 *
 * @param name Name of the port.
 * @return A special register port.
 * @exception InstanceNotFound If a special register port does not exist by
 *                             the given name.
 */
SpecialRegisterPort*
ControlUnit::specialRegisterPort(const std::string& name) const
    throw (InstanceNotFound) {

    const string procName = "ControlUnit::specialRegisterPort";

    if (!hasPort(name)) {
        throw InstanceNotFound(__FILE__, __LINE__, procName);
    }

    Port* port = this->port(name);
    SpecialRegisterPort* srPort = dynamic_cast<SpecialRegisterPort*>(port);
    if (srPort == NULL) {
        throw InstanceNotFound(__FILE__, __LINE__, procName);
    }
    return srPort;
}


/**
 * Binds the given port to return address port.
 *
 * If there is another port bound to return address port already, unbinds it.
 *
 * @param port The port to be bound.
 * @exception IllegalRegistration If the given port is not a port of this
 *                                control unit.
 */
void
ControlUnit::setReturnAddressPort(const SpecialRegisterPort& port)
    throw (IllegalRegistration) {

    if (port.parentUnit() != this) {
        const string procName = "ControlUnit::setReturnAddressPort";
        throw IllegalRegistration(__FILE__, __LINE__, procName);
    }

    raPort_ = const_cast<SpecialRegisterPort*>(&port);
}


/**
 * Unbinds the return address port if one exists.
 */
void
ControlUnit::unsetReturnAddressPort() {
    raPort_ = NULL;
}


/**
 * Tells whether the control unit has a return address port.
 *
 * @return True if a return address port exists, otherwise false.
 */
bool
ControlUnit::hasReturnAddressPort() const {
    return raPort_ != NULL;
}


/**
 * Returns the return address port.
 *
 * @return The return address port.
 * @exception InstanceNotFound If there is no return address port.
 */
SpecialRegisterPort*
ControlUnit::returnAddressPort() const
    throw (InstanceNotFound) {

    if (raPort_ != NULL) {
        return raPort_;
    } else {
        const string procName = "ControlUnit::returnAddressPort";
        throw InstanceNotFound(__FILE__, __LINE__, procName);
    }
}


/**
 * Saves the contents to an ObjectState tree.
 *
 * @return The newly created ObjectState tree.
 */
ObjectState*
ControlUnit::saveState() const {

    ObjectState* cUnit = FunctionUnit::saveState();
    cUnit->setName(OSNAME_CONTROL_UNIT);

    // set delay slots
    cUnit->setAttribute(OSKEY_DELAY_SLOTS, delaySlots_);

    // set global guard latency
    cUnit->setAttribute(OSKEY_GUARD_LATENCY, globalGuardLatency_);

    // set return address port
    if (hasReturnAddressPort()) {
        cUnit->setAttribute(OSKEY_RA_PORT, returnAddressPort()->name());
    }

    return cUnit;
}


/**
 * Loads its state from the given ObjectState instance.
 *
 * @param state The ObjectState instance.
 * @exception ObjectStateLoadingException If the given ObjectState instance
 *                                        invalid or if connections to other
 *                                        machine parts cannot be made.
 */
void
ControlUnit::loadState(const ObjectState* state)
    throw (ObjectStateLoadingException) {

    FunctionUnit::loadState(state);
    loadStateWithoutReferences(state);
}


/**
 * Removes the given port from the control unit and unsets the binding of
 * return address port if necessary.
 *
 * @param port The port to be removed.
 */
void
ControlUnit::removePort(Port& port) {
    if (&port == raPort_) {
        unsetReturnAddressPort();
    }
    FunctionUnit::removePort(port);
}


/**
 * Loads the state of the control unit without references to other
 * components.
 *
 * @param state The ObjectState instance from which the state is loaded.
 * @exception ObjectStateLoadingException If an error occurs while loading
 *                                        the state.
 */
void
ControlUnit::loadStateWithoutReferences(const ObjectState* state)
    throw (ObjectStateLoadingException) {

    const string procName = "ControlUnit::loadStateWithoutReferences";

    if (state->name() != OSNAME_CONTROL_UNIT) {
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }

    try {
        setDelaySlots(state->intAttribute(OSKEY_DELAY_SLOTS));
        setGlobalGuardLatency(state->intAttribute(OSKEY_GUARD_LATENCY));
        if (state->hasAttribute(OSKEY_RA_PORT)) {
            string portName = state->stringAttribute(OSKEY_RA_PORT);
            setReturnAddressPort(*specialRegisterPort(portName));
        } else {
            unsetReturnAddressPort();
        }

    } catch (Exception& e) {
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName,
                                          e.errorMessage());
    }
}


/**
 * Tells whether the given machine has a guard that reads a register with
 * local guard latency of zero.
 *
 * @param machine The machine.
 * @return True if the machine has the guard, otherwise false.
 */
bool
ControlUnit::hasLocalGuardLatencyOfZero(const Machine& machine) {
    Machine::BusNavigator busNav = machine.busNavigator();
    for (int i = 0; i < busNav.count(); i++) {
        Bus* bus = busNav.item(i);
        for (int i = 0; i < bus->guardCount(); i++) {
            Guard* guard = bus->guard(i);
            RegisterGuard* regGuard = 
                dynamic_cast<RegisterGuard*>(guard);
            if (regGuard != NULL) {
                RegisterFile* rf = regGuard->registerFile();
                if (rf->guardLatency() == 0) {
                    return true;
                }
            }
        }
    }
    return false;
}
}
