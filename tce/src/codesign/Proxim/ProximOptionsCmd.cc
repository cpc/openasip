/**
 * @file ProximOptionsCmd.cc
 * 
 * Implementation of ProximOptionsCmd class.
 * 
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "ProximOptionsCmd.hh"
#include "OptionsDialog.hh"
#include "ProximConstants.hh"
#include "GUIOptions.hh"
#include "Proxim.hh"
#include "ProximMainFrame.hh"
#include "ProximToolbox.hh"

using std::string;

/**
 * The Constructor.
 */
ProximOptionsCmd::ProximOptionsCmd():
    GUICommand(ProximConstants::COMMAND_NAME_EDIT_OPTIONS, NULL) {

}


/**
 * The Destructor.
 */
ProximOptionsCmd::~ProximOptionsCmd() {
}


/**
 * Executes the command.
 * 
 * @return True, if the command was succesfully executed, false otherwise.
 */
bool
ProximOptionsCmd::Do() {

    OptionsDialog dialog(
        parentWindow(), wxGetApp().options(), wxGetApp().commandRegistry());

    if (dialog.ShowModal() == wxID_OK) {
	if (wxGetApp().options().toolbarVisibility()) {
            ProximToolbox::mainFrame()->createToolbar();
	}
        ProximToolbox::mainFrame()->createMenubar();
    }
    return true;
}


/**
 * Returns id of this command.
 * 
 * @return ID for this command to be used in menus and toolbars.
 */
int
ProximOptionsCmd::id() const {
    return ProximConstants::COMMAND_EDIT_OPTIONS;
}
		  
		  
/**
 * Creates and returns a new instance of this command.
 * 
 * @return Newly created instance of this command.
 */
ProximOptionsCmd*
ProximOptionsCmd::create() const {
    return new ProximOptionsCmd();
}


/**
 * Returns path to the command's icon file.
 * 
 * @return Full path to the command's icon file.
 */
string
ProximOptionsCmd::icon() const {
    return "options.png";
}


/**
 * This command is always executable.
 * 
 * @return Always true.
 */
bool
ProximOptionsCmd::isEnabled() {
    return true;
}
