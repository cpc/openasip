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
 * @file LImmDstRegisterField.cc
 *
 * Implementation of LImmDstRegisterField class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>

#include "LImmDstRegisterField.hh"
#include "BinaryEncoding.hh"
#include "MapTools.hh"
#include "Application.hh"
#include "ObjectState.hh"

using std::string;

const std::string LImmDstRegisterField::OSNAME_LIMM_DST_REGISTER_FIELD = 
    "dst_reg_field";
const std::string LImmDstRegisterField::OSKEY_WIDTH = "width";
const std::string LImmDstRegisterField::OSNAME_IU_DESTINATION = "iu_dst";
const std::string LImmDstRegisterField::OSKEY_ITEMP = "itemp";
const std::string LImmDstRegisterField::OSKEY_DST_IU = "iu";

/**
 * The constructor.
 *
 * Registers the field automatically to the given parent.
 *
 * @param width Width of the field.
 * @param parent The parent BinaryEncoding instance.
 * @exception OutOfRange If the given width is 0 or smaller.
 */
LImmDstRegisterField::LImmDstRegisterField(
    int width,
    BinaryEncoding& parent)
    throw (OutOfRange) : InstructionField(&parent), width_(width) {

    if (width < 1) {
        throw OutOfRange(__FILE__, __LINE__, __func__);
    }

    setParent(NULL);
    parent.addLongImmDstRegisterField(*this);
    setParent(&parent);
}


/**
 * The constructor.
 *
 * Loads the state of the object from the given ObjectState instance.
 * Registers the field automatically to the given parent.
 *
 * @param state The ObjectState instance.
 * @param parent The parent BinaryEncoding instance.
 * @exception ObjectStateLoadingException If the given ObjectState instance
 *                                        is erroneous.
 */
LImmDstRegisterField::LImmDstRegisterField(
    const ObjectState* state,
    BinaryEncoding& parent)
    throw (ObjectStateLoadingException) :
    InstructionField(state, &parent), width_(0) {

    loadState(state);
    setParent(NULL);
    parent.addLongImmDstRegisterField(*this);
    setParent(&parent);
}


/**
 * The destructor.
 */
LImmDstRegisterField::~LImmDstRegisterField() {
    BinaryEncoding* parent = this->parent();
    setParent(NULL);
    parent->removeLongImmDstRegisterField(*this);
}


/**
 * Returns the parent BinaryEncoding instance.
 *
 * @return The parent BinaryEncoding instance.
 */
BinaryEncoding*
LImmDstRegisterField::parent() const {
    InstructionField* parent = InstructionField::parent();
    if (parent != NULL) {
        BinaryEncoding* bem = dynamic_cast<BinaryEncoding*>(parent);
        assert(bem != NULL);
        return bem;
    } else {
        return NULL;
    }
}


/**
 * Sets that the register index of the given immediate unit in the given
 * instruction template is given in this field.
 *
 * @param instructionTemplate The instruction template.
 * @param immediateUnit The immediate unit.
 * @exception NotAvailable If index of some other immediate unit is assigned
 *                         to this field in the same instruction template.
 *
 */
void
LImmDstRegisterField::addDestination(
    const std::string& instructionTemplate,
    const std::string& immediateUnit)
    throw (NotAvailable) {

    if (MapTools::containsKey(destinationMap_, instructionTemplate) &&
        MapTools::valueForKey<string>(destinationMap_, instructionTemplate)
        != immediateUnit) {
        throw NotAvailable(__FILE__, __LINE__, __func__);
    } else {
        destinationMap_.insert(
            std::pair<string, string>(instructionTemplate, immediateUnit));
    }
}


/**
 * Returns the number of instruction templates that use this destination 
 * register field.
 *
 * @return The number of instruction templates.
 */
int
LImmDstRegisterField::instructionTemplateCount() const {
    return destinationMap_.size();
}


/**
 * By the given index, returns an instruction template that uses this
 * destination register field.
 *
 * @param index The index.
 * @exception OutOfRange If the index is negative or not smaller than the
 *                       number of instruction templates.
 */
std::string
LImmDstRegisterField::instructionTemplate(int index) const
    throw (OutOfRange) {

    if (index < 0 || index >= instructionTemplateCount()) {
        throw OutOfRange(__FILE__, __LINE__, __func__);
    }

    StringMap::const_iterator iter = destinationMap_.begin();
    for (int i = 0; i < index; i++) {
        iter++;
    }

    return iter->first;
}


/**
 * Tells whether the field is used by the given instruction template.
 *
 * @param instructionTemplate The instruction template.
 */
bool
LImmDstRegisterField::usedByInstructionTemplate(
    const std::string& instructionTemplate) const {

    return MapTools::containsKey(destinationMap_, instructionTemplate);
}


/**
 * Returns the name of the immediate unit for which the field is assigned
 * in the given instruction template.
 *
 * @param instructionTemplate Name of the instruction template.
 * @exception NotAvailable If the field is not used in the given instruction
 *                         template.
 */
std::string
LImmDstRegisterField::immediateUnit(
    const std::string& instructionTemplate) const
    throw (NotAvailable) {

    try {
        return MapTools::valueForKey<string>(
            destinationMap_, instructionTemplate);
    } catch (const Exception&) {
        throw NotAvailable(__FILE__, __LINE__, __func__);
    }
}


/**
 * Returns the width of the field.
 *
 * @return The bit width of the field.
 */
int
LImmDstRegisterField::width() const {
    return width_;
}


/**
 * Returns the number of child fields, that is always 0.
 *
 * @return 0.
 */
int
LImmDstRegisterField::childFieldCount() const {
    return 0;
}


/**
 * Saves the state of the object to an ObjectState instance.
 *
 * @return The newly created ObjectState instance.
 */
ObjectState*
LImmDstRegisterField::saveState() const {

    ObjectState* state = InstructionField::saveState();
    state->setName(OSNAME_LIMM_DST_REGISTER_FIELD);
    state->setAttribute(OSKEY_WIDTH, width());

    // add destinations
    for (StringMap::const_iterator iter = destinationMap_.begin();
         iter != destinationMap_.end(); iter++) {
        ObjectState* dstState = new ObjectState(OSNAME_IU_DESTINATION);
        state->addChild(dstState);
        dstState->setAttribute(OSKEY_ITEMP, iter->first);
        dstState->setAttribute(OSKEY_DST_IU, iter->second);
    }

    return state;
}


/**
 * Loads the state of the object from the given ObjectState instance.
 *
 * @param state The ObjectState instance.
 */
void
LImmDstRegisterField::loadState(const ObjectState* state)
    throw (ObjectStateLoadingException) {

    if (state->name() != OSNAME_LIMM_DST_REGISTER_FIELD) {
        throw ObjectStateLoadingException(__FILE__, __LINE__, __func__);
    }

    InstructionField::loadState(state);

    try {
        width_ = state->intAttribute(OSKEY_WIDTH);
        for (int i = 0; i < state->childCount(); i++) {
            ObjectState* child = state->child(i);
            if (child->name() != OSNAME_IU_DESTINATION) {
                throw ObjectStateLoadingException(
                    __FILE__, __LINE__, __func__);
            }
            string iTemp = child->stringAttribute(OSKEY_ITEMP);
            string iu = child->stringAttribute(OSKEY_DST_IU);
            addDestination(iTemp, iu);
        }
    } catch (const Exception& e) {
        throw ObjectStateLoadingException(
            __FILE__, __LINE__, __func__, e.errorMessage());
    }
}
            
