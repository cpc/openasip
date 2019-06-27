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
 * @file FUPort.cc
 *
 * Implementation of class FUPort.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen-no.spam-tut.fi)
 * @note reviewed 14 Jun 2004 by am, tr, ao, ll
 * @note rating: red
 */

#include <string>
#include <set>

#include "Machine.hh"
#include "Guard.hh"
#include "FUPort.hh"
#include "FunctionUnit.hh"
#include "HWOperation.hh"
#include "MOMTextGenerator.hh"
#include "ObjectState.hh"

using std::string;
using std::set;
using boost::format;

namespace TTAMachine {

// initialization of static data members
const string FUPort::OSNAME_FUPORT = "fu_port";
const string FUPort::OSKEY_TRIGGERING = "triggering";
const string FUPort::OSKEY_OPCODE_SETTING = "oc_setting";
const string FUPort::OSKEY_NO_REGISTER = "no_register";
/**
 * Constructor.
 *
 * @param name Name of the port.
 * @param width Bit width of the port.
 * @param parent The function unit to which the port belongs.
 * @param triggers If true, writing (or reading) this port starts the
 *                 execution of a new operation.
 * @param setsOpcode If true, writing (or reading) this port selects the
 *                   operation to be executed. Opcode-setting ports must
 *                   be triggering.
 * @param noRegister If true, the port do not have internal register.
 * @exception ComponentAlreadyExists If the function unit already has another
 *                                   port by the same name or another port
 *                                   that sets operation code.
 * @exception OutOfRange If the given bit width is less or equal to zero.
 * @exception IllegalParameters If setsOpcode argument is true and
 *                              isTriggering false.
 * @exception InvalidName If the given name is not a valid component name.
 */
FUPort::FUPort(
    const std::string& name, int width, FunctionUnit& parent, bool triggers,
    bool setsOpcode, bool noRegister)
    : BaseFUPort(name, width, parent),
      triggers_(triggers),
      setsOpcode_(setsOpcode),
      noRegister_(noRegister) {
    const std::string procName = "FUPort::FUPort";

    if (setsOpcode && !triggers) {
        throw IllegalParameters(__FILE__, __LINE__, procName);
    }

    // check that parent unit will not have two operation code setting
    // ports
    if (setsOpcode) {
        for (int i = 0; i < parent.portCount(); i++) {
            BaseFUPort* port = parent.port(i);
            if (port->isOpcodeSetting() && port != this) {
                throw ComponentAlreadyExists(
                    __FILE__, __LINE__, procName);
            }
        }
    }
}

/**
 * Constructor.
 *
 * This constructor does not check that the parent unit will not have
 * two operation code setting ports. This constructor is used by
 * UniversalFUPort class.
 *
 * @param name Name of the port.
 * @param width Bit width of the port.
 * @param parent The function unit to which the port belongs.
 * @param triggers If true, writing (or reading) this port starts the
 *                 execution of a new operation.
 * @param setsOpcode If true, writing (or reading) this port selects the
 *                   operation to be executed. Opcode-setting ports must
 *                   be triggering.
 * @param noRegister If true, the port do not have internal register.
 * @param dummy This parameter is not used and exists only to make some
 *              difference to the other constructor.
 * @exception ComponentAlreadyExists If the function unit already has another
 *                                   port by the same name.
 * @exception OutOfRange If the given bit width is less or equal to zero.
 * @exception IllegalParameters If setsOpcode argument is true and
 *                              isTriggering false.
 * @exception InvalidName If the given name is not a valid component name.
 */
FUPort::FUPort(
    const std::string& name, int width, FunctionUnit& parent, bool triggers,
    bool setsOpcode, bool noRegister, bool /*dummy*/)
    : BaseFUPort(name, width, parent),
      triggers_(triggers),
      setsOpcode_(setsOpcode),
      noRegister_(noRegister) {
    if (setsOpcode != triggers) {
        const std::string procName = "FUPort::FUPort";
        const std::string error = "Port must trigger iff sets opcode.";
        throw IllegalParameters(__FILE__, __LINE__, procName, error);
    }
}

/**
 * Constructor.
 *
 * Loads its state from the given ObjectState instance but does not create
 * connections to sockets. This constructor is used when the state of a
 * function unit is loaded. Do not use this constructor.
 *
 * @param state The ObjectState instance.
 * @param parent The parent function unit which contains the port.
 * @exception ObjectStateLoadingException If the given ObjectState instance
 *                                        is invalid.
 */
FUPort::FUPort(const ObjectState* state, Unit& parent)
    : BaseFUPort(state, parent), triggers_(false), setsOpcode_(false) {
    loadStateWithoutReferences(state);

    if (setsOpcode_ != triggers_) {
        const std::string procName = "FUPort::FUPort";
        const std::string error = "Port must trigger iff sets opcode.";
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName, 
                error);
    }
}

/**
 * Destructor.
 */
FUPort::~FUPort() {
    cleanupGuards();
    cleanupOperandBindings();
}


/**
 * Returns true if reading (or writing) this port starts the execution of a
 * new operation, otherwise false.
 *
 * @return True if reading (or writing) this port starts the execution of a
 *         new operation, otherwise false.
 */
bool
FUPort::isTriggering() const {
    return triggers_;
}


/**
 * Returns true if reading (or writing) this port selects the operation to be
 * executed, otherwise false.
 *
 * @return True if reading (or writing) this port selects the operation to be
 *         executed, otherwise false.
 */
bool
FUPort::isOpcodeSetting() const {
    return setsOpcode_;
}


/**
 * Sets/unsets the port to a triggering and opcode setting.
 *
 * Triggering port is always the opcode setting port in TCE v1.0 and
 * only one port can trigger in TCE v1.0.
 * If ports parent unit already has a triggering port this ports triggering
 * status is removed.
 *
 * @param triggers When true, the port is set to a triggering and opcode
 * setting port.
 */
void
FUPort::setTriggering(bool triggers) {
    if (triggers) {
        setsOpcode_ = true;

        FunctionUnit* parent = this->parentUnit();
        if (parent != NULL) {
            for (int i = 0; i < parent->portCount(); i++) {
                BaseFUPort* port = parent->port(i);
                if (port->isTriggering() && port != this) {
                    dynamic_cast<FUPort*>(port)->setTriggering(false);
                    break; // only one other port can be triggering
                }
            }
        }
    } else {
        setsOpcode_ = false;
    }
    triggers_ = triggers;
}


/**
 * Saves the state of the object to an ObjectState instance.
 *
 * @return The newly created ObjectState instance.
 */
ObjectState*
FUPort::saveState() const {
    ObjectState* state = BaseFUPort::saveState();
    state->setName(OSNAME_FUPORT);
    state->setAttribute(OSKEY_TRIGGERING, triggers_);
    state->setAttribute(OSKEY_OPCODE_SETTING, setsOpcode_);
    state->setAttribute(OSKEY_NO_REGISTER, noRegister_);    
    return state;
}


/**
 * Loads the state of the object from the given ObjectState instance.
 *
 * @param state The ObjectState instance.
 * @exception ObjectStateLoadingException If an error occurs while loading
 *                                        the state.
 */
void
FUPort::loadState(const ObjectState* state) {
    const string procName = "FUPort::loadState";

    if (state->name() != OSNAME_FUPORT) {
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }

    loadStateWithoutReferences(state);
    BaseFUPort::loadState(state);
    updateBindingString();
}

/**
 * Returns a description of operand bindings of the port.
 *
 * This returned string can be used to compare the bindings of to FUPorts using a string
 * comparison because Operations are listed in alphapetical order.
 *
 * @todo this should be moved to BaseFUPort.
 *
 * @return String describing the operand bindings of the port.
 */
std::string
FUPort::bindingString() const {
    return bindingString_;
}

/**
 * Updates the string of bindings of the port.
 *
 * Separated to another method so it's not recomputed every time it's asked.
 *
 * @todo this should be moved to BaseFUPort.
 *
 * @return String of bindings. Operations are listed in alphapetical order.
 */
void
FUPort::updateBindingString() const {

    if (parentUnit() == NULL) {
        bindingString_ = "";
        return;
    }

    set<string> bindings;
    for (int i = 0; i < parentUnit()->operationCount(); i++) {
        if (parentUnit()->operation(i)->isBound(*this)) {
            string binding = parentUnit()->operation(i)->name();
            binding += ".";
            binding += Conversion::toString(
                parentUnit()->operation(i)->io(*this));
            bindings.insert(binding);
        }
    }
    set<string>::const_iterator iter = bindings.begin();
    string result;
    while (iter != bindings.end()) {
        result += (*iter);
        iter++;
        if (iter != bindings.end()) {
            result += ",";
        }
    }
    bindingString_ = result;
}


/**
 * Checks if the two ports have same architecture.
 *
 * Compares also operation bindings. Names are allowed to differ.
 *
 * @return True if the two ports have equal architecture.
 */
bool
FUPort::isArchitectureEqual(FUPort* port) {

    if (triggers_ != port->isTriggering()) {
        return false;
    }
    if (setsOpcode_ != port->isOpcodeSetting()) {
        return false;
    }
    if (width() != port->width()) {
        return false;
    }
    if (bindingString() != port->bindingString()) {
        return false;
    }
    return true;
}


/**
 * Cleans up the guards that refer to this port.
 */
void
FUPort::cleanupGuards() const {

    // delete guards referencing to this port
    Unit* parent = Port::parentUnit();
    Machine* machine = parent->machine();

    if (machine != NULL) {
        Machine::BusNavigator navi = machine->busNavigator();

        for (int busIndex = 0; busIndex < navi.count(); busIndex++) {
            Bus* bus = navi.item(busIndex);
            int guardIndex = 0;

            while (guardIndex < bus->guardCount()) {
                Guard* guard = bus->guard(guardIndex);
                PortGuard* portGuard = dynamic_cast<PortGuard*>(guard);

                if (portGuard != NULL && portGuard->port() == this) {
                    // guard is removed automatically from bus
                    delete portGuard;
                } else {
                    guardIndex++;
                }
            }
        }
    }
}



/**
 * Removes all the operand bindings of the parent unit that use this port.
 */
void
FUPort::cleanupOperandBindings() const {

    // NOTE! Cannot call directly FUPort::parentUnit because this method is
    // called from FUPort's destructor and it is possible that FunctionUnit
    // instance does not exist any more. This is the case if FunctionUnit is
    // deleted because its destructor is called before Unit's destructor.
    Unit* parentUnit = Port::parentUnit();
    FunctionUnit* parentFU = dynamic_cast<FunctionUnit*>(parentUnit);

    if (parentFU != NULL) {
        for (int i = 0; i < parentFU->operationCount(); i++) {
            HWOperation* operation = parentFU->operation(i);
            operation->unbindPort(*this);
        }
    }
    bindingString_ = "";
}


/**
 * Loads its state from the given ObjectState instance but does not create
 * connections to sockets.
 *
 * @param state The ObjectState instance.
 * @exception ObjectStateLoadingException If the given ObjectState instance
 *                                        is invalid.
 */
void
FUPort::loadStateWithoutReferences(const ObjectState* state) {
    const string procName = "FUPort::loadStateWithoutReferences";

    try {
        setTriggering(state->intAttribute(OSKEY_TRIGGERING));
        setNoRegister(state->intAttribute(OSKEY_NO_REGISTER));            
        if (state->boolAttribute(OSKEY_OPCODE_SETTING)) {

            // Check that parent unit does not contain another operation
            // code setting port. Cannot call directly setOpcodeSetting
            // method because FunctionUnit part of parent unit may not have
            // been instantiated when this method is called (for example if
            // FunctionUnit is created by FunctionUnit(ObjectState*).

            Unit* parentUnit = Port::parentUnit();
            MOMTextGenerator textGenerator;

            for (int i = 0; i < parentUnit->portCount(); i++) {
                Port* port = parentUnit->port(i);
                BaseFUPort* fuPort = dynamic_cast<BaseFUPort*>(port);
                assert(fuPort != NULL);
                if (fuPort->isOpcodeSetting() && fuPort != this) {
                    format text = textGenerator.text(
                        MOMTextGenerator::TXT_OPCODE_SETTING_PORT_EXISTS);
                    text % parentUnit->name();
                    throw ObjectStateLoadingException(
                        __FILE__, __LINE__, procName, text.str());
                }
            }

            setsOpcode_ = true;
            if (!triggers_) {
                format text = textGenerator.text(
                    MOMTextGenerator::TXT_OPCODE_SETTING_MUST_BE_TRIGGERING);
                text % name() % parentUnit->name();
                throw ObjectStateLoadingException(
                    __FILE__, __LINE__, procName, text.str());
            }
        }

    } catch (Exception& e) {
        throw ObjectStateLoadingException(
            __FILE__, __LINE__, procName, e.errorMessage());
    }
    updateBindingString();
}

/*
 * Returns true if FU port do not have register.
 *
 *
 */
bool
FUPort::noRegister() const {
    return noRegister_;
}
    
/*
 * Defines whether or not FUPort has internal register.
 *
 *
 */
void
FUPort::setNoRegister(bool noRegister) {
    noRegister_ = noRegister;
}
    
}
