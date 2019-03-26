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
 * @file ImmediateUnit.cc
 *
 * Implementation of ImmediateUnit class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen-no.spam-tut.fi)
 * @author Esa Määttä 2007 (esa.maatta-no.spam-tut.fi)
 */

#include "ImmediateUnit.hh"
#include "Port.hh"
#include "ObjectState.hh"

using std::string;

// constant latency value for immediate unit
const int LATENCY = 1;

// constant max writes value for immediate unit
const int MAX_WRITES = 1;

// initialization of static data members
const string TTAMachine::ImmediateUnit::OSNAME_IMMEDIATE_UNIT = 
    "immediate-unit";
const string TTAMachine::ImmediateUnit::OSKEY_EXTENSION = "extension";
const string TTAMachine::ImmediateUnit::OSVALUE_SIGN = "sign";
const string TTAMachine::ImmediateUnit::OSVALUE_ZERO = "zero";
const string TTAMachine::ImmediateUnit::OSKEY_LATENCY = "latency";

namespace TTAMachine {

/**
 * Constructor.
 *
 * @param name Name of the immediate unit.
 * @param size Number of immediate registers in the immediate unit.
 * @param width Bit width of the long immediate registers in the immediate
 *              unit.
 * @param extension Extension mode applied to the long immediate when it is
 *                  narrower than the immediate register,
 *                  see Machine::Extension.
 * @param latency Number of cycles needed between the encoding of a long
 *                immediate and its earliest transport on a data bus.
 *                The value "0" indicates that the long immediate can be
 *                read onto a bus in the same cycle in which the instruction
 *                that encodes it is executed.
 * @exception OutOfRange If the given size or width is invalid.
 * @exception InvalidName If the given name is not a valid name for a
                          component.
 */
ImmediateUnit::ImmediateUnit(
    const string& name,
    unsigned int size,
    unsigned int width,
    unsigned int maxReads,
    unsigned int guardLatency,
    Machine::Extension extension)
    throw (OutOfRange, InvalidName) :
    RegisterFile(name, size, width, maxReads, MAX_WRITES, guardLatency,
    RegisterFile::NORMAL), extension_(extension), latency_(LATENCY) {

    RegisterFile::setNumberOfRegisters(size);
    setWidth(width);
}


/**
 * Constructor.
 *
 * Loads the state of the immediate unit from the given ObjectState instance.
 * Does not load references to other components.
 *
 * @param state The ObjectState instance.
 * @exception ObjectStateLoadingException If the given ObjectState instance
 *                                        is invalid or if connections to
 *                                        other machine parts cannot be made.
 */
ImmediateUnit::ImmediateUnit(const ObjectState* state)
    throw (ObjectStateLoadingException) :
    RegisterFile(state) {

    loadStateWithoutReferences(state);
}


/**
 * Destructor.
 */
ImmediateUnit::~ImmediateUnit() {
    unsetMachine();
}


/**
 * Sets the name of the immediate unit.
 *
 * @param name Name of the immediate unit.
 * @exception ComponentAlreadyExists If an immediate unit with the given
 *                                   name is already in the same machine.
 * @exception InvalidName If the given name is not a valid name for a
 *                        component.
 */
void
ImmediateUnit::setName(const string& name)
    throw (ComponentAlreadyExists, InvalidName) {

    if (name == this->name()) {
        return;
    }

    if (machine() != NULL) {
        if (machine()->immediateUnitNavigator().hasItem(name)) {
            string procName = "ImmediateUnit::setName";
            throw ComponentAlreadyExists(__FILE__, __LINE__, procName);
        } else {
            Component::setName(name);
        }
    } else {
        Component::setName(name);
    }
}


/**
 * Returns the extension mode of the immediate unit.
 *
 * @return The extension mode of the immediate unit.
 */
Machine::Extension
ImmediateUnit::extensionMode() const {
    return extension_;
}


/**
 * Returns the minimum number of cycles needed between the encoding of a long
 * immediate and its earliest transport on a data bus.
 *
 * @return Latency of the immediate unit.
 */
int
ImmediateUnit::latency() const {
    return latency_;
}


/**
 * Sets the maximum number of ports that can write a register all in the same
 * cycle.
 *
 * For immediate unit the value is fixed to 1.
 *
 * @param writes Maximum number of ports.
 * @exception OutOfRange If the given number of maximum writes is out of
 *                       range.
 */
void
ImmediateUnit::setMaxWrites(int maxWrites)
    throw (OutOfRange) {

    if (maxWrites != MAX_WRITES) {
        std::string procName = "ImmediateUnit::setMaxWrites";
        throw OutOfRange(__FILE__, __LINE__, procName);
    }
    RegisterFile::setMaxWrites(maxWrites);
}


/**
 * Sets the extension mode for the immediate unit.
 *
 * @param mode The new extension mode.
 */
void
ImmediateUnit::setExtensionMode(Machine::Extension mode) {
    extension_ = mode;
}


/**
 * Sets the latency of the immediate unit.
 *
 * @param latency The new latency.
 * @exception OutOfRange If the given latency is less than zero.
 */
void
ImmediateUnit::setLatency(int latency)
    throw (OutOfRange) {

    if (latency != LATENCY) {
        string procName = "ImmediateUnit::setLatency";
        throw OutOfRange(__FILE__, __LINE__, procName);
    }
    latency_ = latency;
}


/**
 * Removes the immediate unit from machine.
 */
void
ImmediateUnit::unsetMachine() {
    Machine* mach = machine();
    if (mach == NULL) {
        return;
    }

    Machine::InstructionTemplateNavigator itNav =
        mach->instructionTemplateNavigator();
    Machine::BusNavigator busNav = mach->busNavigator();
    for (int itIndex = 0; itIndex < itNav.count(); itIndex++) {
        InstructionTemplate* it = itNav.item(itIndex);
        it->removeSlots(*this);
    }
    BaseRegisterFile::unsetMachine();
    mach->removeImmediateUnit(*this);
}


/**
 * Saves the contents to an ObjectState tree.
 *
 * @return The newly created ObjectState tree.
 */
ObjectState*
ImmediateUnit::saveState() const {

    ObjectState* iUnit = RegisterFile::saveState();
    iUnit->setName(OSNAME_IMMEDIATE_UNIT);
    
    // set extension mode
    if (extension_ == Machine::SIGN) {
        iUnit->setAttribute(OSKEY_EXTENSION, OSVALUE_SIGN);
    } else {
        iUnit->setAttribute(OSKEY_EXTENSION, OSVALUE_ZERO);
    }

    return iUnit;
}


/**
 * Loads its state from the given ObjectState instance.
 *
 * @param state The ObjectState instance.
 * @exception ObjectStateLoadingException If the given ObjectState instance
 *                                        is invalid or if the connections
 *                                        to other machine parts cannot be
 *                                        made.
 */
void
ImmediateUnit::loadState(const ObjectState* state)
    throw (ObjectStateLoadingException) {

    loadStateWithoutReferences(state);
    RegisterFile::loadState(state);
}


/**
 * Loads the state of the immediate unit without references to other
 * components.
 *
 * @param state The ObjectState instance from which the state is loaded.
 * @exception ObjectStateLoadingException If an error occurs while loading
 *                                        the state.
 */
void
ImmediateUnit::loadStateWithoutReferences(const ObjectState* state)
    throw (ObjectStateLoadingException) {

    const string procName = "ImmediateUnit::loadStateWithoutReferences";

    if (state->name() != OSNAME_IMMEDIATE_UNIT) {
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }

    try {
        string extension = state->stringAttribute(OSKEY_EXTENSION);
        if (extension == OSVALUE_SIGN) {
            setExtensionMode(Machine::SIGN);
        } else if (extension == OSVALUE_ZERO) {
            setExtensionMode(Machine::ZERO);
        } else {
            throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
        }

        setLatency(LATENCY);
    } catch (const Exception& e) {
        throw ObjectStateLoadingException(
            __FILE__, __LINE__, procName, e.errorMessage());
    }
}

}
