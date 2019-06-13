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
 * @file ImmediateSlot.cc
 *
 * Implementation of ImmediateSlot class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>

#include "ImmediateSlot.hh"
#include "MOMTextGenerator.hh"
#include "Machine.hh"
#include "ObjectState.hh"

using boost::format;
using std::string;

namespace TTAMachine {

const std::string ImmediateSlot::OSNAME_IMMEDIATE_SLOT = "immediate_slot";

/**
 * The constructor.
 *
 * @param name Name of the immediate slot.
 * @param parent The parent machine.
 * @exception InvalidName If the given name is not a valid component name.
 * @exception ComponentAlreadyExists If the machine already contains a
 *                                   bus or immediate slot with the same
 *                                   name.
 */
ImmediateSlot::ImmediateSlot(const std::string& name, Machine& parent)
    : Component(name) {
    setMachine(parent);
}

/**
 * The constructor.
 *
 * Loads the state of the object from the given ObjectState instance.
 *
 * @param state The ObjectState instance.
 * @param parent The parent machine.
 * @exception ObjectStateLoadingException If an error occurs while loading
 *                                        the state.
 * @exception ComponentAlreadyExists If the machine already contains a
 *                                   bus or immediate slot with the same
 *                                   name.
 */
ImmediateSlot::ImmediateSlot(const ObjectState* state, Machine& parent)
    : Component(state) {
    const string procName = "ImmediateSlot::ImmediateSlot";

    if (parent.busNavigator().hasItem(name())) {
        MOMTextGenerator textGenerator;
        format text = textGenerator.text(
            MOMTextGenerator::TXT_BUS_AND_IMM_SLOT_WITH_SAME_NAME);
        text % name();
        throw ComponentAlreadyExists(
            __FILE__, __LINE__, procName, text.str());
    }
    if (parent.immediateSlotNavigator().hasItem(name())) {
        MOMTextGenerator textGenerator;
        format text = textGenerator.text(
            MOMTextGenerator::TXT_IMM_SLOT_EXISTS_BY_SAME_NAME);
        text % name();
        throw ComponentAlreadyExists(
            __FILE__, __LINE__, procName, text.str());
    }

    setMachine(parent);
    loadState(state);
}

/**
 * The destructor.
 */
ImmediateSlot::~ImmediateSlot() {
    unsetMachine();
}


/**
 * Returns the bit width of the immediate slot.
 *
 * The bit width is determined by the instruction templates which use the
 * immediate slot.
 *
 * @return The bit width of the immediate slot.
 */
int
ImmediateSlot::width() const {

    int width = 0;

    Machine::InstructionTemplateNavigator itNav = 
        machine()->instructionTemplateNavigator();
    for (int i = 0; i < itNav.count(); i++) {
        InstructionTemplate* iTemp = itNav.item(i);
        if (iTemp->supportedWidth(name()) > width) {
            width = iTemp->supportedWidth(name());
        }
    }

    return width;
}


/**
 * Registers the immediate slot to a machine.
 *
 * @param mach Machine to which the immediate slot is going to be registered.
 * @exception ComponentAlreadyExists If the given machine already has another
 *                                   immediate slot or bus by the same name.
 */
void
ImmediateSlot::setMachine(Machine& machine) {
    machine.addImmediateSlot(*this);
    internalSetMachine(machine);
}

/**
 * Removes registration of the immediate slot from its current machine.
 */
void
ImmediateSlot::unsetMachine() {
    assert(machine() != NULL);
    Machine* mach = machine();

    // delete the template slots that use this slot
    Machine::InstructionTemplateNavigator itNav = 
        mach->instructionTemplateNavigator();
    for (int i = 0; i < itNav.count(); i++) {
        InstructionTemplate* iTemp = itNav.item(i);
        iTemp->removeSlot(name());
    }

    internalUnsetMachine();
    mach->deleteImmediateSlot(*this);
}


/**
 * Sets the name of the immediate slot.
 *
 * @param name The new name.
 * @exception ComponentAlreadyExists If the machine already contains an 
 *                                   immediate slot or bus with the same
 *                                   name.
 * @exception InvalidName If the given name is not a valid component name.
 */
void
ImmediateSlot::setName(const std::string& name) {
    if (name == this->name()) {
        return;
    }

    Machine* mach = machine();
    if (mach->immediateSlotNavigator().hasItem(name) || 
        mach->busNavigator().hasItem(name)) {
        const string procName = "ImmediateSlot::setName";
        throw ComponentAlreadyExists(__FILE__, __LINE__, procName);
    } else {
        Component::setName(name);
    }
}

/**
 * Saves the state of the object to an ObjectState instance.
 *
 * @return The newly created ObjectState instance.
 */
ObjectState*
ImmediateSlot::saveState() const {
    ObjectState* state = Component::saveState();
    state->setName(OSNAME_IMMEDIATE_SLOT);
    return state;
}


/**
 * Loads the state of the object from the given ObjectState instance.
 *
 * @param state The ObjectState instance.
 * @exception ObjectStateLoadingException If an error occurs when loading
 *                                        the state.
 */
void
ImmediateSlot::loadState(const ObjectState* state) {
    if (state->name() != OSNAME_IMMEDIATE_SLOT) {
        const string procName = "ImmediateSlot::loadState";
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }

    Component::loadState(state);
}
}
