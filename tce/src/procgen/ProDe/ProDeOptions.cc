/**
 * @file ProDeOptions.cc
 *
 * Implementation of class ProDeOptions.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
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
