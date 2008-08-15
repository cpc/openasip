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
 * @file ToolbarButton.cc
 *
 * Implementation of ToolbarButton class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include "ToolbarButton.hh"
#include "ObjectState.hh"

using std::string;

// initialization of static data members
const string ToolbarButton::OSNAME_TOOLBAR_BUTTON = "tbbutton";
const string ToolbarButton::OSKEY_SLOT = "slot";
const string ToolbarButton::OSKEY_ACTION = "action";


/**
 * Constructor.
 *
 * @param slot Number of the slot in which the button is placed.
 * @param action Name of the action performed by the button.
 */
ToolbarButton::ToolbarButton(
    int slot, 
    const std::string& action) :
    slot_(slot), action_(action) {
}


/**
 * Constructor.
 *
 * Loads the state from the given ObjectsState object.
 */
ToolbarButton::ToolbarButton(const ObjectState* state) {
    loadState(state);
}


/**
 * Copy constructor.
 */
ToolbarButton::ToolbarButton(const ToolbarButton& old):
    Serializable() {
    slot_ = old.slot_;
    action_ = old.action_;
}


/**
 * Destructor.
 */
ToolbarButton::~ToolbarButton() {
}


/**
 * Returns the name of the action performed by the button.
 *
 * @return Name of the action performed by the button.
 */
std::string 
ToolbarButton::action() const {
    return action_;
}


/**
 * Returns the position of the slot which contains the button.
 *
 * @return Position of the slot which contains the button.
 */
int 
ToolbarButton::slot() const {
    return slot_;
}


/**
 * Loads the state of the object from the given ObjectState object.
 *
 * @param state ObjectState from which the state is loaded.
 * @exception ObjectStateLoadingException If the given ObjectState instance
 *                                        is invalid.
 */
void
ToolbarButton::loadState(const ObjectState* state)
    throw (ObjectStateLoadingException) {

    string procName = "ToolbarButton::loadState";
        
    if (state->name() != OSNAME_TOOLBAR_BUTTON) {
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }

    try {
        slot_ = state->intAttribute(OSKEY_SLOT);
        action_ = state->stringAttribute(OSKEY_ACTION);
    } catch (...) {
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }
}


/**
 * Creates an ObjectState object and saves the state of the object into it.
 *
 * @return The created ObjectState object.
 */
ObjectState*
ToolbarButton::saveState() const {
    ObjectState* state = new ObjectState(OSNAME_TOOLBAR_BUTTON);
    state->setAttribute(OSKEY_SLOT, slot_);
    state->setAttribute(OSKEY_ACTION, action_);
    return state;
}
