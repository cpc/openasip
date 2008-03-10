/**
 * @file GUICommand.cc
 *
 * Definition of GUICommand class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <string>

#include "GUICommand.hh"

using std::string;

/**
 * The Constructor.
 */
GUICommand::GUICommand(std::string name, wxWindow* parent = NULL) :
    parent_(parent), name_(name) {

}



/**
 * The Destructor.
 */
GUICommand::~GUICommand() {
}


/**
 * Sets the parent window of the command.
 *
 * Window will be used as parent for the dialogs created by this command.
 *
 * @param parent Parent window for the command dialogs.
 */
void
GUICommand::setParentWindow(wxWindow* parent) {
    parent_ = parent;
}


/**
 * Returns parent window of the command.
 *
 * @return Parent window of the command.
 */
wxWindow*
GUICommand::parentWindow() const {
    return parent_;
}


/**
 * Returns short version of the command name.
 *
 * Base class implementation returns the command normal name.
 *
 * @return Short version of the command name.
 */
std::string
GUICommand::shortName() const {
    return name_;
}


/**
 * Returns name of the command.
 *
 * @return Long name of the command.
 */
std::string
GUICommand::name() const {
    return name_;
}


/**
 * Returns true if a checkbox related to the command is checked.
 *
 * This is meaningful only for toggle-type commands.
 *
 * @return True, if the command feature is toggled on.
 */
bool
GUICommand::isChecked() const {
    return false;
}
