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
 * @file UnconditionalGuardEncoding.cc
 *
 * Implementation of UnconditionalGuardEncoding class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>

#include "UnconditionalGuardEncoding.hh"
#include "GuardField.hh"
#include "ObjectState.hh"

using std::string;

const std::string UnconditionalGuardEncoding::
OSNAME_UNCONDITIONAL_GUARD_ENCODING = "uc_guard_encoding";

/**
 * The constructor.
 *
 * Registers the encoding to the given guard field automatically.
 *
 * @param inverted Indicates whether the encoding is for always-true or
 *                 always-false guard expression.
 * @param encoding The encoding for unconditional guard.
 * @param parent The parent guard field.
 * @exception ObjectAlreadyExists If the guard field has an encoding for the
 *                                same unconditional guard expression 
 *                                already, or if the given encoding is
 *                                already assigned.
 */
UnconditionalGuardEncoding::UnconditionalGuardEncoding(
    bool inverted,
    unsigned int encoding,
    GuardField& parent)
    throw (ObjectAlreadyExists) : 
    GuardEncoding(inverted, encoding) {

    parent.addGuardEncoding(*this);
    setParent(&parent);
}


/**
 * The constructor.
 *
 * Loads the state of the object from the given ObjectState instance.
 *
 * @param state The ObjectState instance.
 * @param parent The parent guard field.
 * @exception ObjectStateLoadingException If an error occurs while loading
 *                                        the state.
 * @exception ObjectAlreadyExists If the guard field has an encoding for the
 *                                same unconditional guard expression
 *                                already, of if the encoding in the
 *                                ObjectState instance is already assigned.
 */
UnconditionalGuardEncoding::UnconditionalGuardEncoding(
    const ObjectState* state,
    GuardField& parent)
    throw (ObjectStateLoadingException, ObjectAlreadyExists) :
    GuardEncoding(state) {

    if (state->name() != OSNAME_UNCONDITIONAL_GUARD_ENCODING) {
        const string procName = 
            "UnconditionalGuardEncoding::UnconditionalGuardEncoding";
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }

    parent.addGuardEncoding(*this);
    setParent(&parent);
}


/**
 * The destructor.
 */
UnconditionalGuardEncoding::~UnconditionalGuardEncoding() {
    GuardField* oldParent = parent();
    setParent(NULL);
    oldParent->removeUnconditionalGuardEncoding(*this);
}


/**
 * Saves the state of the object to an ObjectState instance.
 *
 * @return The newly created ObjectState instance.
 */
ObjectState*
UnconditionalGuardEncoding::saveState() const {
    ObjectState* state = GuardEncoding::saveState();
    state->setName(OSNAME_UNCONDITIONAL_GUARD_ENCODING);
    return state;
}
