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
 * @file ProDeOptions.cc
 *
 * Implementation of class ProDeOptions.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <cassert>

#include "ProDeOptions.hh"
#include "Conversion.hh"
#include "SequenceTools.hh"

using std::string;

// initialization of static data members
const string ProDeOptions::CONFIGURATION_NAME = "prode-configuration";
const string ProDeOptions::OSKEY_UNDO_STACK_SIZE = "undosize";


/**
 * Constructor.
 */
ProDeOptions::ProDeOptions() :
    GUIOptions(CONFIGURATION_NAME),
    undoStackSize_(10) {
}


/**
 * Constructor.
 *
 * Loads the state from the given ObjectState tree.
 *
 * @param state The ObjectState tree.
 * @exception ObjectStateLoadingException If the given ObjectState tree is
 *            invalid.
 */
ProDeOptions::ProDeOptions(const ObjectState* state)
    throw (ObjectStateLoadingException) :
    GUIOptions(CONFIGURATION_NAME) {

    loadState(state);
}


/**
 * Copy constructor.
 *
 * This constructor creates identical options with the given options class.
 */
ProDeOptions::ProDeOptions(const ProDeOptions& old) :
    GUIOptions(old) {

    undoStackSize_ = old.undoStackSize_;
}


/**
 * Destructor.
 */
ProDeOptions::~ProDeOptions() {
}


/**
 * Returns the size of the undo stack.
 *
 * @return The size of the undo stack.
 */
int
ProDeOptions::undoStackSize() const {
    return undoStackSize_;
}


/**
 * Sets the size of the undo stack.
 *
 * @param size The new size.
 */
void
ProDeOptions::setUndoStackSize(int size) {
    undoStackSize_ = size;
}


/**
 * Loads the state of the object from the given ObjectState object.
 *
 * @param state ObjectState from which the state is loaded.
 * @exception ObjectStateLoadingException If the given ObjectState instance
 *                                        is invalid.
 */
void
ProDeOptions::loadState(const ObjectState* state)
    throw (ObjectStateLoadingException) {

    deleteAllKeyboardShortcuts();
    deleteAllToolbarButtons();

    const string procName = "ProDeOptions::loadState";

    GUIOptions::loadState(state);

    try {
        undoStackSize_ = state->intAttribute(OSKEY_UNDO_STACK_SIZE);
    } catch (...) {
        deleteAllKeyboardShortcuts();
        deleteAllToolbarButtons();
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }
}


/**
 * Creates an ObjectState object and saves the state of the object into it.
 *
 * @return The created ObjectState object.
 */
ObjectState*
ProDeOptions::saveState() const {
    ObjectState* state = GUIOptions::saveState();
    state->setAttribute(OSKEY_UNDO_STACK_SIZE, undoStackSize_);
    return state;
}
