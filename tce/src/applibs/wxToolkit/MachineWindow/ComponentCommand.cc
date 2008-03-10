/**
 * @file ComponentCommand.cc
 * 
 * Implementation of ComponentCommand class.
 * 
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "ComponentCommand.hh"

/**
 * The Constructor.
 */
ComponentCommand::ComponentCommand(): parentWindow_(NULL) {
}


/**
 * The Destructor.
 */
ComponentCommand::~ComponentCommand() {
}


/**
 * Sets a parent window for the dialogs created by this command.
 * 
 * @param window A parent window for the dialogs.
 */
void
ComponentCommand::setParentWindow(wxWindow* window) {
    parentWindow_ = window;
}


/**
 * Returns the parent window for the dialogs created by this command.
 *
 * @return The parent window for the dialogs created by this command.
 */
wxWindow*
ComponentCommand::parentWindow() {
    return parentWindow_;
}

