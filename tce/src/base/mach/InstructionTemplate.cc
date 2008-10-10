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
 * @file InstructionTemplate.cc
 *
 * Implementation of class InstructionTemplate.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen-no.spam-tut.fi)
 */

#include <set>

#include "InstructionTemplate.hh"
#include "TemplateSlot.hh"
#include "Machine.hh"
#include "Bus.hh"
#include "ImmediateUnit.hh"
#include "MOMTextGenerator.hh"
#include "ContainerTools.hh"
#include "SequenceTools.hh"

using std::string;
using std::set;
using boost::format;

namespace TTAMachine {

const string InstructionTemplate::OSNAME_INSTRUCTION_TEMPLATE = "i_template";

/**
 * Constructor.
 *
 * Creates an empty instruction template. Add slots to make the instruction
 * template non-empty. An empty instruction template represents the encoding
 * format of instructions that don't specify any bits of a long immediate or
 * any immediate register-write action.
 *
 * @param name Name of the instruction template.
 * @param owner The machine to which the instruction template is going to be
 *              registered.
 * @exception ComponentAlreadyExists If the given machine already contains
 *                                   another instruction template by the same
 *                                   name.
 * @exception InvalidName If the given name is not a valid component name.
 */
InstructionTemplate::InstructionTemplate(
    const string& name,
    Machine& owner)
    throw (ComponentAlreadyExists, InvalidName) :
    Component(name) {

    setMachine(owner);
}


/**
 * Constructor.
 *
 * Loads the state of the instruction template completely from the given
 * ObjectState tree.
 *
 * @param state The ObjectState instance.
 * @param owner The machine to which the instruction template is going to be
 *              registered.
 * @exception ObjectStateLoadingException If the given ObjectState instance
 *                                        is invalid or if there exists an
 *                                        instruction template by the same
 *                                        as the upcoming name in the
 *                                        machine or if references to other
 *                                        components cannot be resolved.
 */
InstructionTemplate::InstructionTemplate(
    const ObjectState* state,
    Machine& owner)
    throw (ObjectStateLoadingException) :
    Component(state) {

    const string procName = "InstructionTemplate::InstructionTemplate";

    try {
        setMachine(owner);
    } catch (const ComponentAlreadyExists&) {
        MOMTextGenerator textGenerator;
        format errorMsg = textGenerator.text(
            MOMTextGenerator::TXT_IT_EXISTS_BY_NAME);
        errorMsg % name();
        throw ObjectStateLoadingException(
            __FILE__, __LINE__, procName, errorMsg.str());
    }

    try {
        loadState(state);
    } catch (const ObjectStateLoadingException&) {
        unsetMachine();
        throw;
    }
}


/**
 * Destructor.
 *
 * Deletes all the slots of this instruction template too.
 */
InstructionTemplate::~InstructionTemplate() {
    unsetMachine();
}


/**
 * Sets the name of the instruction template.
 *
 * @param name Name of the instruction template.
 * @exception ComponentAlreadyExists If an instruction template with the
 *                                   given name is already in the same
 *                                   machine.
 * @exception InvalidName If the given name is not a valid component name.
 */
void
InstructionTemplate::setName(const string& name)
    throw (ComponentAlreadyExists, InvalidName) {

    if (name == this->name()) {
        return;
    }

    if (machine() != NULL) {
        if (machine()->instructionTemplateNavigator().hasItem(name)) {
            string procName = "InstructionTemplate::setName";
            throw ComponentAlreadyExists(__FILE__, __LINE__, procName);
        } else {
            Component::setName(name);
        }
    } else {
        Component::setName(name);
    }
}


/**
 * Adds a template slot.
 *
 * The given bit width must be greater than 0.
 *
 * @param slotName Transport bus in which (part of) part of the long 
 *                 immediate is stored.
 * @param width The number of long immediate bits that are encoded on the 
 *              slot.
 * @param dstUnit Destination immediate unit.
 * @exception InstanceNotFound If the machine does not have a bus or
 *                             immediate slot with the given name.
 * @exception IllegalRegistration If the given destination unit is not 
 *                                registered to the same machine.
 * @exception ComponentAlreadyExists If the given slot is already in use.
 * @exception OutOfRange If the given bit width is out of range.
 */
void
InstructionTemplate::addSlot(
    const std::string& slotName,
    int width,
    ImmediateUnit& dstUnit)
    throw (InstanceNotFound, IllegalRegistration, ComponentAlreadyExists, 
           OutOfRange) {

    const string procName = "InstructionTemplate::addSlot";

    ensureRegistration(dstUnit);

    if (width <= 0) {
        throw OutOfRange(__FILE__, __LINE__, procName);
    }

    if (templateSlot(slotName) != NULL) {
        throw ComponentAlreadyExists(__FILE__, __LINE__, procName);
    }
        
    if (machine()->busNavigator().hasItem(slotName)) {
        Bus* bus = machine()->busNavigator().item(slotName);
        slots_.push_back(new TemplateSlot(*bus, width, dstUnit));
    } else if (machine()->immediateSlotNavigator().hasItem(slotName)) {
        ImmediateSlot* slot = machine()->immediateSlotNavigator().item(
            slotName);
        slots_.push_back(new TemplateSlot(*slot, width, dstUnit));
    } else {
        throw InstanceNotFound(__FILE__, __LINE__, procName);
    }
}


/**
 * Removes the template slot which uses the given slot.
 *
 * @param slotName Name of the slot.
 */
void
InstructionTemplate::removeSlot(const std::string& slotName) {
    TemplateSlot* slot = templateSlot(slotName);
    if (slot != NULL) {
        bool removed = ContainerTools::deleteValueIfExists(slots_, slot);
        assert(removed);
    }
}


/**
 * Removes the template slots that has the given immediate unit as
 * destination unit.
 *
 * @param dstUnit The destination immediate unit.
 */
void
InstructionTemplate::removeSlots(const ImmediateUnit& dstUnit) {
    for (SlotTable::iterator iter = slots_.begin(); iter != slots_.end();) {
        TemplateSlot* slot = *iter;
        if (slot->destination() == &dstUnit) {
            delete slot;
            SlotTable::iterator next = slots_.erase(iter);
            iter = next;
        } else {
            iter++;
        }
    }
}

/**
 * Return the total number of slots in the template.
 *
 * @return The total number of slots in the template.
 */
int
InstructionTemplate::slotCount() const {
    return slots_.size();
}

/**
 * Return the slot in the given index.
 *
 * @param index Index of slot.
 * @return The slot in the given index.
 * @exception OutOfRange If index is equal to or greater than the number
 * of slots in the template.
 */
TemplateSlot*
InstructionTemplate::slot(int index) const {
    if (index < 0 || index >= static_cast<int>(slots_.size())) {
        string msg = "Slot index out of bounds.";
        throw OutOfRange(__FILE__, __LINE__, __func__, msg);
    } else {
        return slots_[index];
    }
}

/**
 * Returns true if the instruction template uses the given slot.
 *
 * @param slotName Name of the slot.
 * @return True if the instruction template uses the given slot.
 */
bool
InstructionTemplate::usesSlot(const std::string& slotName) const {
    TemplateSlot* slot = templateSlot(slotName);
    return (slot != NULL);
}


/**
 * Returns true if the instruction template reserves the template slot given
 * by the slotName input argument for long immediate bits that are written
 * into a register of the given immediate unit.
 *
 * @param slotName Name of the slot.
 * @param dstUnit The destination immediate unit.
 */
bool
InstructionTemplate::destinationUsesSlot(
    const std::string& slotName,
    const ImmediateUnit& dstUnit) const {

    TemplateSlot* slot = templateSlot(slotName);
    if (slot == NULL) {
        return false;
    }

    return (slot->destination() == &dstUnit);
}


/**
 * Returns how many immediate units are written in parallel by this
 * instruction template.
 *
 * @return The number of immediate units.
 */
int
InstructionTemplate::numberOfDestinations() const {

    std::set<ImmediateUnit*> destinations;

    for (SlotTable::const_iterator iter = slots_.begin();
         iter != slots_.end(); iter++) {
        TemplateSlot* slot = *iter;
        destinations.insert(slot->destination());
    }

    return destinations.size();
}


/**
 * Checks whether the given immediate unit is one of the immediate
 * destinations in this instruction template.
 *
 * @param dstUnit The immediate unit to check.
 * @return True if the given immediate unit is a destination, otherwise
 *         false.
 */
bool
InstructionTemplate::isOneOfDestinations(
    const ImmediateUnit& dstUnit) const {

    for (SlotTable::const_iterator iter = slots_.begin();
         iter != slots_.end(); iter++) {
        TemplateSlot* slot = *iter;
        if (slot->destination() == &dstUnit) {
            return true;
        }
    }
    return false;
}


/**
 * Returns the destination unit of the given slot.
 *
 * @param slotName Name of the slot.
 * @return The destination unit.
 * @exception InstanceNotFound If the instruction template does not use
 *                             the given slot to encode long immediate.
 */
ImmediateUnit*
InstructionTemplate::destinationOfSlot(const std::string& slotName) const
    throw (InstanceNotFound) {

    for (SlotTable::const_iterator iter = slots_.begin(); 
         iter != slots_.end(); iter++) {
        TemplateSlot* slot = *iter;
        if (slot->slot() == slotName) {
            return slot->destination();
        }
    }

    const string procName = "InstructionTemplate::destinationOfSlot";
    throw InstanceNotFound(__FILE__, __LINE__, procName);
}


/**
 * Tells the number of slots that are used to encode the long immediate
 * which is written to the given destination unit.
 *
 * @param dstUnit The destination unit.
 * @return The number of slots.
 */
int
InstructionTemplate::numberOfSlots(const ImmediateUnit& dstUnit) const {
    int slots(0);
    for (SlotTable::const_iterator iter = slots_.begin();
         iter != slots_.end(); iter++) {
        TemplateSlot* slot = *iter;
        if (slot->destination() == &dstUnit) {
            slots++;
        }
    }
    return slots;
}


/**
 * By the given index, returns the slot used to encode a part of the long
 * immediate that is to be written to the given destination unit.
 *
 * The slot are returned in the same order the parts of the long immediates
 * are concatenated when they are written to the destination registers. The
 * most significant end of the immediate is returned by index 0.
 *
 * @param dstUnit The destination unit.
 * @param index The index.
 * @exception OutOfRange If the given index is negative or not smaller than
 *                       the number of slots that are written to the given
 *                       destination unit.
 */
std::string
InstructionTemplate::slotOfDestination(
    const ImmediateUnit& dstUnit,
    int index) const
    throw (OutOfRange) {
    
    if (index < 0 || index >= numberOfSlots(dstUnit)) {
        const string procName = "InstructionTemplate::slotOfDestination";
        throw OutOfRange(__FILE__, __LINE__, procName);
    }

    int count(0);
    
    for (SlotTable::const_iterator iter = slots_.begin(); 
         iter != slots_.end(); iter++) {
        TemplateSlot* slot = *iter;
        if (slot->destination() == &dstUnit) {
            if (count == index) {
                return slot->slot();
            } else {
                count++;
            }
        }
    }

    assert(false);
    return NULL;
}


/**
 * Returns the bit width of the widest long immediate that can be encoded by
 * this instruction template.
 *
 * @return The bit width.
 */
int
InstructionTemplate::supportedWidth() const {

    int maxWidth(0);

    Machine::ImmediateUnitNavigator iuNav =
        machine()->immediateUnitNavigator();
    for (int i = 0; i < iuNav.count(); i++) {
        ImmediateUnit* iu = iuNav.item(i);
        if (supportedWidth(*iu) > maxWidth) {
            maxWidth = supportedWidth(*iu);
        }
    }

    return maxWidth;
}


/**
 * Returns the bit width of the widest long immediate that can be encoded by
 * this instruction template and that writes a register of the given
 * immediate unit.
 *
 * @param dstUnit The immediate unit.
 * @return The bit width.
 */
int
InstructionTemplate::supportedWidth(const ImmediateUnit& dstUnit) const {

    int totalWidth(0);

    for (SlotTable::const_iterator iter = slots_.begin();
         iter != slots_.end(); iter++) {
        TemplateSlot* slot = *iter;
        if (slot->destination() == &dstUnit) {
            totalWidth += slot->width();
        }
    }

    return totalWidth;
}


/**
 * Returns the bit width of the long immediate that can be encoded in the
 * given slot by this instruction template.
 *
 * @param slotName Name of the slot.
 * @return The bit width.
 */
int
InstructionTemplate::supportedWidth(const std::string& slotName) const {
    TemplateSlot* slot = templateSlot(slotName);
    if (slot == NULL) {
        return 0;
    } else {
        return slot->width();
    }
}


/**
 * Checks whether the instruction template is an empty instruction template
 * which cannot have any slots.
 *
 * @return True if the instruction template is empty, otherwise false.
 */
bool
InstructionTemplate::isEmpty() const {
    return (slots_.size() == 0);
}


/**
 * Adds the instruction template to the given machine.
 *
 * @param machine Machine to which the instruction template is added.
 * @exception ComponentAlreadyExists If there already is another instruction
 *                                   template by the same name or another
 *                                   empty instruction template in the given
 *                                   machine.
 */
void
InstructionTemplate::setMachine(Machine& machine)
    throw (ComponentAlreadyExists) {

    machine.addInstructionTemplate(*this);
    internalSetMachine(machine);
}


/**
 * Removes the instruction template from its machine.
 *
 * The instruction template is also deleted because it cannot be alone.
 * It must be registered to a machine.
 */
void
InstructionTemplate::unsetMachine() {
    deleteAllSlots();
    Machine* mach = machine();
    assert(mach != NULL);
    internalUnsetMachine();
    mach->deleteInstructionTemplate(*this);
}


/**
 * Saves its state to an ObjectState tree.
 *
 * @return The newly created ObjectState tree.
 */
ObjectState*
InstructionTemplate::saveState() const {
    ObjectState* state = Component::saveState();
    state->setName(OSNAME_INSTRUCTION_TEMPLATE);
    SlotTable::const_iterator iter = slots_.begin();
    while (iter != slots_.end()) {
        TemplateSlot* templateSlot = *iter;
        ObjectState* slotState = templateSlot->saveState();
        state->addChild(slotState);
        iter++;
    }
    return state;
}


/**
 * Loads its state from the given ObjectState tree.
 *
 * @param state The ObjectState tree.
 * @exception ObjectStateLoadingException If the given ObjectState tree is
 *                                        invalid.
 */
void
InstructionTemplate::loadState(const ObjectState* state)
    throw (ObjectStateLoadingException) {

    const string procName = "InstructionTemplate::loadState";

    if (state->name() != OSNAME_INSTRUCTION_TEMPLATE) {
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }

    deleteAllSlots();
    Component::loadState(state);

    // add template slots
    try {
        for (int i = 0; i < state->childCount(); i++) {
            MOMTextGenerator textGenerator;
            ObjectState* child = state->child(i);
            string slotName = child->stringAttribute(
                TemplateSlot::OSKEY_SLOT);
            if (!machine()->busNavigator().hasItem(slotName) &&
                !machine()->immediateSlotNavigator().hasItem(slotName)) {
                format errorMsg = textGenerator.text(
                    MOMTextGenerator::TXT_IT_REF_LOAD_ERR_SLOT);
                errorMsg % slotName % name();
                throw ObjectStateLoadingException(
                    __FILE__, __LINE__, procName, errorMsg.str());
            }

            int width = child->intAttribute(TemplateSlot::OSKEY_WIDTH);
            string destination = child->stringAttribute(
                TemplateSlot::OSKEY_DESTINATION);
            Machine::ImmediateUnitNavigator iuNav =
                machine()->immediateUnitNavigator();
            ImmediateUnit* iu = NULL;

            try {
                iu = iuNav.item(destination);
            } catch (const InstanceNotFound&) {
                format errorMsg = textGenerator.text(
                    MOMTextGenerator::TXT_IT_REF_LOAD_ERR_IU);
                errorMsg % destination % name();
                throw ObjectStateLoadingException(
                    __FILE__, __LINE__, procName, errorMsg.str());
            }

            addSlot(slotName, width, *iu);
        }

    } catch (const Exception& exception) {
        throw ObjectStateLoadingException(
            __FILE__, __LINE__, procName, exception.errorMessage());
    }
}


/**
 * Deletes all the template slots.
 */
void
InstructionTemplate::deleteAllSlots() {
    SequenceTools::deleteAllItems(slots_);
}


/**
 * Returns the template slot that uses slot with the given name.
 *
 * Returns NULL if no template slot uses the given slot.
 *
 * @param slotName Name of the slot.
 * @return The template slot or NULL.
 */
TemplateSlot*
InstructionTemplate::templateSlot(const std::string& slotName) const {
    for (SlotTable::const_iterator iter = slots_.begin();
         iter != slots_.end(); iter++) {
        TemplateSlot* slot = *iter;
        if (slot->slot() == slotName) {
            return slot;
        }
    }
    return NULL;
}

}
