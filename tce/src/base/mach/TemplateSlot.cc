/**
 * @file TemplateSlot.cc
 *
 * Implementation of class TemplateSlot.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include "TemplateSlot.hh"
#include "ImmediateUnit.hh"
#include "ContainerTools.hh"
#include "ObjectState.hh"

using std::string;

namespace TTAMachine {

// initialization of static data members
const string TemplateSlot::OSNAME_TEMPLATE_SLOT = "template_slot";
const string TemplateSlot::OSKEY_SLOT = "slot";
const string TemplateSlot::OSKEY_WIDTH = "width";
const string TemplateSlot::OSKEY_DESTINATION = "destination";


/**
 * Constructor.
 *
 * @param slot The bus or which is programmed by the instruction bit field
 *             of this template slot.
 * @param width The number of significant bits that can be encoded in this
 *              instruction field.
 * @param destination The destination ImmediateUnit, that is, the
 *                    ImmediateUnit that contains the registers that can be
 *                    written with the bits in this template slot.
 */
TemplateSlot::TemplateSlot(
    const Bus& slot,
    int width,
    ImmediateUnit& destination) :
    bus_(&slot), immSlot_(NULL), width_(width), destination_(&destination) {
}


/**
 * Constructor.
 *
 * @param slot The immediate slot which is programmed by the instruction bit
 *             field of this template slot.
 * @param width The number of significant bits that can be encoded in this
 *              instruction field.
 * @param destination The destination ImmediateUnit, that is, the 
 *                    ImmediateUnit that contains the registers that can be
 *                    written with the bits in this template slot.
 */
TemplateSlot::TemplateSlot(
    const ImmediateSlot& slot,
    int width,
    ImmediateUnit& destination) :
    bus_(NULL), immSlot_(&slot), width_(width), destination_(&destination) {
}


/**
 * Destructor.
 */
TemplateSlot::~TemplateSlot() {
}


/**
 * Saves the state of the object into an ObjectState instance.
 *
 * @return The newly created ObjectState instance.
 */
ObjectState*
TemplateSlot::saveState() const {
    ObjectState* slotState = new ObjectState(OSNAME_TEMPLATE_SLOT);
    slotState->setAttribute(OSKEY_SLOT, slot());
    slotState->setAttribute(OSKEY_WIDTH, width());
    slotState->setAttribute(OSKEY_DESTINATION, destination()->name());
    return slotState;
}

}
