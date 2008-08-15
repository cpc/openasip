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
