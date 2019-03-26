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
#include "ObjectState.hh"

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
