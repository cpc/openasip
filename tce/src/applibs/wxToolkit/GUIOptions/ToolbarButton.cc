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
