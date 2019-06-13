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
 * @file NOPEncoding.cc
 *
 * Implementation of NOPEncoding class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include "NOPEncoding.hh"
#include "SourceField.hh"
#include "Application.hh"
#include "ObjectState.hh"

using std::string;

const std::string NOPEncoding::OSNAME_NOP_ENCODING = "nop_encoding";

/**
 * The constructor.
 *
 * Registers the encoding automatically to the parent src/dst field.
 *
 * @param encoding The encoding for inline immediates.
 * @param extraBits The number of extra bits in the encoding.
 * @param parent The parent src/dst field.
 * @exception ObjectAlreadyExists If the given parent field has a NOP
 *                                encoding already or if the encoding is
 *                                ambiguous with some other encoding in the
 *                                parent field.
 */
NOPEncoding::NOPEncoding(
    unsigned int encoding, unsigned int extraBits, SlotField& parent)
    : Encoding(encoding, extraBits, NULL) {
    parent.setNoOperationEncoding(*this);
    setParent(&parent);
}

/**
 * The constructor.
 *
 * Loads the state of the object from the given ObjectState instance.
 *
 * @param state The ObjectState instance.
 * @param parent The parent field.
 * @exception ObjectAlreadyExists If the given parent field has a NOP
 *                                encoding already or if the encoding is
 *                                ambiguous with some other
 *                                encoding in the parent field.
 * @exception ObjectStateLoadingException If the given ObjectState instance
 *                                        is erroneous.
 */
NOPEncoding::NOPEncoding(const ObjectState* state, SlotField& parent)
    : Encoding(state, NULL) {
    const string procName = "NOPEncoding::NOPEncoding";

    if (state->name() != OSNAME_NOP_ENCODING) {
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }

    parent.setNoOperationEncoding(*this);
    setParent(&parent);
}

/**
 * The destructor.
 */
NOPEncoding::~NOPEncoding() {
    SlotField* oldParent = parent();
    assert(oldParent != NULL);
    setParent(NULL);
    oldParent->unsetNoOperationEncoding();
}


/**
 * Returns the parent source field.
 *
 * @return The parent.
 */
SlotField*
NOPEncoding::parent() const {
    InstructionField* parent = Encoding::parent();
    if (parent != NULL) {
        SlotField* sField = dynamic_cast<SlotField*>(parent);
        assert(sField != NULL);
        return sField;
    } else {
        return NULL;
    }
}


/**
 * Returns the position of the encoding within the source field.
 *
 * @return The position of the encoding.
 */
int
NOPEncoding::bitPosition() const {
    if (parent()->componentIDPosition() == BinaryEncoding::LEFT) {
        return parent()->width() - parent()->extraBits() - width();
    } else {
        return 0;
    }
}


/**
 * Saves the state of the object to an ObjectState instance.
 *
 * @return The newly created ObjectState instance.
 */
ObjectState*
NOPEncoding::saveState() const {
    ObjectState* state = Encoding::saveState();
    state->setName(OSNAME_NOP_ENCODING);
    return state;
}
