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
 * @file InstructionField.cc
 *
 * Implementation of InstructionField class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include <list>
#include <boost/format.hpp>

#include "InstructionField.hh"
#include "NullInstructionField.hh"
#include "Application.hh"
#include "ObjectState.hh"

using boost::format;
using std::string;
using std::list;

const std::string InstructionField::OSNAME_INSTRUCTION_FIELD = "instr_field";
const std::string InstructionField::OSKEY_EXTRA_BITS = "extra_bits";
const std::string InstructionField::OSKEY_POSITION = "position";

/**
 * The constructor.
 *
 * @param relativePosition Relative position of the instruction field within
 *                         its parent field.
 * @param parent The parent instruction field.
 */
InstructionField::InstructionField(InstructionField* parent) :
    relativePos_(0), extraBits_(0), parent_(parent) {

    if (parent != NULL) {
        relativePos_ = parent->childFieldCount();
    }
}


/**
 * The constructor.
 *
 * Loads the state of the object from the given ObjectState tree.
 *
 * @param state The ObjectState tree.
 * @param parent The parent instruction field.
 * @exception ObjectStateLoadingException If an error occurs while loading
 *                                        the state.
 */
InstructionField::InstructionField(
    const ObjectState* state,
    InstructionField* parent)
    throw (ObjectStateLoadingException) :
    relativePos_(0), extraBits_(0), parent_(parent) {

    if (parent != NULL) {
        relativePos_ = parent->childFieldCount();
    }

    loadState(state);
}


/**
 * The destructor.
 */
InstructionField::~InstructionField() {
}


/**
 * Returns the parent instruction field.
 *
 * Returns NULL if the field has no parent field (BinaryEncoding).
 *
 * @return The parent instruction field.
 */
InstructionField*
InstructionField::parent() const {
    return parent_;
}


/**
 * Returns the child field which has the given relative position within
 * the instruction field.
 *
 * This base class implementation just checks whether the given position is
 * out of range and throw the exception if necessary.
 *
 * @param position The position.
 * @exception OutOfRange If the position is negative or not smaller than
 *                       the number of child fields.
 */
InstructionField&
InstructionField::childField(int position) const
    throw (OutOfRange) {

    if (position < 0 || position >= childFieldCount()) {
        const string procName = "InstructionField::childField";
        throw OutOfRange(__FILE__, __LINE__, procName);
    }

    return NullInstructionField::instance();
}


/**
 * Returns the bit-accurate position of the field within its parent field.
 *
 * @return The position.
 */
int
InstructionField::bitPosition() const {

    InstructionField* parent = this->parent();

    if (parent == NULL) {
        return 0;
    }

    int position(0);

    for (int i = 0; i < relativePosition(); i++) {
        InstructionField& childField = parent->childField(i);
        position += childField.width();
    }

    return position;
}


/**
 * Returns the relative position of the field compared to sibling fields.
 *
 * If the field is the rightmost field, returns 0. The leftmost field returns
 * the number of sibling fields - 1.
 *
 * @return The relative position.
 */
int
InstructionField::relativePosition() const {
    return relativePos_;
}


/**
 * Sets a new relative position for the field.
 *
 * @param position The new relative position.
 * @exception OutOfRange If the given position is negative.
 */
void
InstructionField::setRelativePosition(int position)
    throw (OutOfRange) {

    if (position < 0 || position >= parent()->childFieldCount()) {
        const string procName = "InstructionField::setRelativePosition";
        throw OutOfRange(__FILE__, __LINE__, procName);
    }

    InstructionField& fieldToMove = parent()->childField(position);
    relativePos_ = position;

    int childFields = parent()->childFieldCount();
    int emptyPosition(-1);
    for (int i = 0; i < childFields; i++) {
        if (&parent()->childField(i) == &NullInstructionField::instance()) {
            emptyPosition = i;
            break;
        }
    }

    if (emptyPosition != -1) {
        if (emptyPosition < fieldToMove.relativePosition()) {
            fieldToMove.
            setRelativePosition(fieldToMove.relativePosition() - 1);
        } else if (emptyPosition > fieldToMove.relativePosition()) {
            fieldToMove.
            setRelativePosition(fieldToMove.relativePosition() + 1);
        } else {
            assert(false);
        }
    }
}


/**
 * Sets the number of extra (zero) bits to the field.
 *
 * The field can be forced longer than necessary by defining some number of
 * extra bits. In practice, there will be the given amount of zeros always in
 * the MSB end of the field.
 *
 * @param bits The number of extra bits.
 * @exception OutOfRange If the given number is negative.
 */
void
InstructionField::setExtraBits(int bits)
    throw (OutOfRange) {

    if (bits < 0) {
        const string procName = "InstructionField::setExtraBits";
        throw OutOfRange(__FILE__, __LINE__, procName);
    }

    extraBits_ = bits;
}


/**
 * Returns the number of extra bits in the field.
 *
 * @return The number of extra bits.
 */
int
InstructionField::extraBits() const {
    return extraBits_;
}


/**
 * Loads the state of the object from the given ObjectState tree.
 *
 * @param state The ObjectState tree.
 * @exception ObjectStateLoadingException If an error occurs while loading
 *                                        the state.
 */
void
InstructionField::loadState(const ObjectState* state)
    throw (ObjectStateLoadingException) {

    const string procName = "InstructionField::loadState";

    try {
    setExtraBits(state->intAttribute(OSKEY_EXTRA_BITS));
        if (relativePosition() != state->intAttribute(OSKEY_POSITION)) {
            format errorMsg("Invalid relative position %1%. Should be %2%.");
            errorMsg % state->stringAttribute(OSKEY_POSITION) % 
                relativePosition();
            throw ObjectStateLoadingException(
                __FILE__, __LINE__, __func__, errorMsg.str());
        }

    } catch (const Exception& exception) {
        throw ObjectStateLoadingException(
            __FILE__, __LINE__, procName, exception.errorMessage());
    }
}


/**
 * Saves the state of the object to an ObjectState tree.
 *
 * @return The newly created ObjectState tree.
 */
ObjectState*
InstructionField::saveState() const {
    ObjectState* state = new ObjectState(OSNAME_INSTRUCTION_FIELD);
    state->setAttribute(OSKEY_EXTRA_BITS, extraBits());
    state->setAttribute(OSKEY_POSITION, relativePosition());
    return state;
}


/**
 * Sets the parent instruction field.
 *
 * @param parent The parent field.
 */
void
InstructionField::setParent(InstructionField* parent) {
    parent_ = parent;
}


/**
 * Sorts the child ObjectState instances of the given ObjectState instance
 * such that they are returned by child(index) method in the correct order.
 *
 * The correct order means that at first the rightmost subfield is returned
 * and then the next to left and so on.
 *
 * @param state The ObjectState instance.
 */
void
InstructionField::reorderSubfields(ObjectState* state) {

    typedef list<ObjectState*> OSList;
    OSList list;

    for (int i = 0; i < state->childCount();) {
        ObjectState* child = state->child(i);
        if (child->hasAttribute(InstructionField::OSKEY_POSITION)) {
            state->removeChild(child);
            int position = child->intAttribute(
                InstructionField::OSKEY_POSITION);

            // insert the object to the list at correct position
            bool inserted = false;
            for (OSList::iterator iter = list.begin(); iter != list.end();
                 iter++) {
                ObjectState* item = *iter;
                int itemPosition = item->intAttribute(
                    InstructionField::OSKEY_POSITION);
                if (itemPosition > position) {
                    list.insert(iter, child);
                    inserted = true;
                    break;
                }
            }
            if (!inserted) {
                list.push_back(child);
            }
        } else {
            i++;
        }
    }

    // add the sub fields again in the correct order
    for (OSList::const_iterator iter = list.begin(); iter != list.end();
         iter++) {
        ObjectState* child = *iter;
        state->addChild(child);
    }
}

