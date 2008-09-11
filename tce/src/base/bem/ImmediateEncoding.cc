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
 * @file ImmediateEncoding.cc
 *
 * Implementation of ImmediateEncoding class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>

#include "ImmediateEncoding.hh"
#include "SourceField.hh"
#include "Application.hh"

using std::string;

const std::string ImmediateEncoding::OSNAME_IMM_ENCODING = "imm_encoding";
const std::string ImmediateEncoding::OSKEY_IMM_WIDTH = "imm_width";

/**
 * The constructor.
 *
 * Registers the encoding automatically to the parent source field.
 *
 * @param encoding The encoding for inline immediates.
 * @param extraBits The number of extra bits in the encoding.
 * @param immediateWidth Width of the inline immediate.
 * @param parent The parent source field.
 * @exception ObjectAlreadyExists If the given source field has an immediate
 *                                encoding already or if the encoding is
 *                                ambiguous with some socket or bridge
 *                                encoding in the source field.
 * @exception OutOfRange If the given immediate width is negative.
 */
ImmediateEncoding::ImmediateEncoding(
    unsigned int encoding,
    unsigned int extraBits,
    int immediateWidth,
    SourceField& parent)
    throw (ObjectAlreadyExists, OutOfRange) :
    Encoding(encoding, extraBits, NULL), immediateWidth_(immediateWidth) {

    if (immediateWidth < 0) {
        throw OutOfRange(__FILE__, __LINE__, __func__);
    }

    parent.setImmediateEncoding(*this);
    setParent(&parent);
}


/**
 * The constructor.
 *
 * Loads the state of the object from the given ObjectState instance.
 *
 * @param state The ObjectState instance.
 * @param parent The parent source field.
 * @exception ObjectAlreadyExists If the given source field has an immediate
 *                                encoding already or if the encoding is
 *                                ambiguous with some socket or bridge
 *                                encoding in the source field.
 * @exception ObjectStateLoadingException If the given ObjectState instance
 *                                        is erroneous.
 */
ImmediateEncoding::ImmediateEncoding(
    const ObjectState* state,
    SourceField& parent)
    throw (ObjectAlreadyExists, ObjectStateLoadingException) :
    Encoding(state, NULL) {

    const string procName = "ImmediateEncoding::ImmediateEncoding";

    if (state->name() != OSNAME_IMM_ENCODING) {
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }

    try {
        immediateWidth_ = state->intAttribute(OSKEY_IMM_WIDTH);
        if (immediateWidth_ < 0) {
            throw OutOfRange(__FILE__, __LINE__, __func__);
        }
    } catch (const Exception& e) {
        throw ObjectStateLoadingException(
            __FILE__, __LINE__, __func__, e.errorMessage());
    }

    parent.setImmediateEncoding(*this);    
    setParent(&parent);
}


/**
 * The destructor.
 */
ImmediateEncoding::~ImmediateEncoding() {
    SourceField* oldParent = parent();
    assert(oldParent != NULL);
    setParent(NULL);
    oldParent->unsetImmediateEncoding();
}


/**
 * Returns the parent source field.
 *
 * @return The parent.
 */
SourceField*
ImmediateEncoding::parent() const {
    InstructionField* parent = Encoding::parent();
    if (parent != NULL) {
        SourceField* sField = dynamic_cast<SourceField*>(parent);
        assert(sField != NULL);
        return sField;
    } else {
        return NULL;
    }
}


/**
 * Returns the immediate width.
 *
 * @return The immediate width.
 */
int
ImmediateEncoding::immediateWidth() const {
    return immediateWidth_;
}


/**
 * Returns the encoding width.
 *
 * @return The encoding width.
 */
int
ImmediateEncoding::encodingWidth() const {
    return Encoding::width();
}


/**
 * Returns the position of the immediate encoding.
 *
 * @return The position of the immediate encoding.
 */
int
ImmediateEncoding::encodingPosition() const {
    if (parent()->componentIDPosition() == BinaryEncoding::LEFT) {
        return parent()->width() - parent()->extraBits() - encodingWidth();
    } else {
        return 0;
    }
}


/**
 * Returns the position of the short immediate within the source field.
 *
 * @return The position of the short immediate.
 */
int
ImmediateEncoding::immediatePosition() const {
    if (parent()->componentIDPosition() == BinaryEncoding::LEFT) {
        return 0;
    } else {
        return parent()->width() - parent()->extraBits() - immediateWidth();
    }
}


/**
 * Returns always 0.
 *
 * @return The position of the encoding.
 */
int
ImmediateEncoding::bitPosition() const {
    return 0;
}


/**
 * Returns the width of the encoding + immediate width.
 *
 * @return The width.
 */
int
ImmediateEncoding::width() const {
    return immediateWidth() + encodingWidth();
}


/**
 * Saves the state of the object to an ObjectState instance.
 *
 * @return The newly created ObjectState instance.
 */
ObjectState*
ImmediateEncoding::saveState() const {
    ObjectState* state = Encoding::saveState();
    state->setName(OSNAME_IMM_ENCODING);
    state->setAttribute(OSKEY_IMM_WIDTH, immediateWidth());
    return state;
}
