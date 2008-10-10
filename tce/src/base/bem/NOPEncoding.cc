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
    unsigned int encoding,
    unsigned int extraBits,
    SlotField& parent)
    throw (ObjectAlreadyExists) :
    Encoding(encoding, extraBits, NULL) {

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
NOPEncoding::NOPEncoding(
    const ObjectState* state,
    SlotField& parent)
    throw (ObjectAlreadyExists, ObjectStateLoadingException) :
    Encoding(state, NULL) {

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
