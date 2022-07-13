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
 * @file TemplateSlot.cc
 *
 * Implementation of class TemplateSlot.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include "TemplateSlot.hh"
#include "MapTools.hh"
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
const string TemplateSlot::OSKEY_RF_READ = "rf_read";
const string TemplateSlot::OSKEY_RF_WRITE = "rf_write";
const string TemplateSlot::OSKEY_FU_READ = "fu_read";
const string TemplateSlot::OSKEY_FU_WRITE = "fu_write";


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
 * Creates an implicit slot in a template that does not target
 * an ImmediateUnit.
 * 
 * @param slot The bus or which is programmed by the instruction bit field
 *             of this template slot.
 */

TemplateSlot::TemplateSlot(const Bus& slot) : 
    bus_(&slot), width_(0), destination_(NULL) {
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
