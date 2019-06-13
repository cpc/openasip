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
    bool inverted, unsigned int encoding, GuardField& parent)
    : GuardEncoding(inverted, encoding) {
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
    const ObjectState* state, GuardField& parent)
    : GuardEncoding(state) {
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
