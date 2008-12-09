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
