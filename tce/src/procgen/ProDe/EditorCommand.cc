/**
 * @file EditorCommand.cc
 *
 * Definition of EditorCommand class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#include <string>

#include "EditorCommand.hh"
#include "ProDeConstants.hh"

using std::string;

/**
 * The Constructor.
 */
EditorCommand::EditorCommand(std::string name, wxWindow* parent) :
    GUICommand(name, parent),
    view_(NULL) {

}



/**
 * The Destructor.
 */
EditorCommand::~EditorCommand() {
}




/**
 * Sets the MDFView of the command.
 *
 * @param view MDFView to be assigned for the command.
 */
void
EditorCommand::setView(wxView* view) {
    view_ = view;
}


/**
 * Returns MDFView of the command.
 *
 * @return MDFView of the command.
 */
wxView*
EditorCommand::view() const {
    return view_;
}


/**
 * Returns path to the icon of the command to be used on the toolbar.
 *
 * @return Base class implementation returns default icon's path.
 */
string
EditorCommand::icon() const {
    return ProDeConstants::CMD_ICON_DEFAULT;
}


/**
 * Returns true if command is currently executable, otherwise false.
 *
 * Base class implementation returns always false.
 *
 * @return True if command is currently executable, otherwise false.
 */
bool
EditorCommand::isEnabled() {
    return false;
}
