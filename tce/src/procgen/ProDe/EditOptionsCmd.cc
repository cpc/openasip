/**
 * @file EditOptionsCmd.cc
 *
 * Implementation of EditOptionsCmd class.
 *
 * @author Veli-Pekka Jääskeläinen (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "EditOptionsCmd.hh"
#include "ProDeOptionsDialog.hh"
#include "ProDeConstants.hh"
#include "ProDeOptions.hh"
#include "ProDe.hh"
#include "MainFrame.hh"

using std::string;

/**
 * The Constructor.
 */
EditOptionsCmd::EditOptionsCmd():
    EditorCommand(ProDeConstants::CMD_NAME_EDIT_OPTIONS) {

}


/**
 * The Destructor.
 */
EditOptionsCmd::~EditOptionsCmd() {
}


/**
 * Executes the command.
 *
 * @return True, if the command was succesfully executed, false otherwise.
 */
bool
EditOptionsCmd::Do() {

    ProDeOptionsDialog dialog(
        parentWindow(), *wxGetApp().options(), *wxGetApp().commandRegistry());

    if (dialog.ShowModal() == wxID_OK) {
	if (wxGetApp().options()->toolbarVisibility()) {
	    wxGetApp().mainFrame()->createToolbar();
	    wxGetApp().mainFrame()->createMenubar();
	}
    }
    return true;
}


/**
 * Returns id of this command.
 *
 * @return ID for this command to be used in menus and toolbars.
 */
int
EditOptionsCmd::id() const {
    return ProDeConstants::COMMAND_EDIT_OPTIONS;
}


/**
 * Creates and returns a new instance of this command.
 *
 * @return Newly created instance of this command.
 */
EditOptionsCmd*
EditOptionsCmd::create() const {
    return new EditOptionsCmd();
}


/**
 * Returns short version of the command name.
 *
 * @return Short name of the command to be used in the toolbar.
 */
string
EditOptionsCmd::shortName() const {
    return ProDeConstants::CMD_SNAME_EDIT_OPTIONS;
}


/**
 * Returns path to the command's icon file.
 *
 * @return Full path to the command's icon file.
 */
string
EditOptionsCmd::icon() const {
    return ProDeConstants::CMD_ICON_EDIT_OPTIONS;
}


/**
 * This command is always executable.
 *
 * @return Always true.
 */
bool
EditOptionsCmd::isEnabled() {
    return true;
}
