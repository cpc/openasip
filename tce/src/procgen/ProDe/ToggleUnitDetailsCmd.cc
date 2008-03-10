/**
 * @file ToggleUnitDetailsCmd.cc
 *
 * Definition of ToggleUnitDetails class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <wx/docview.h>

#include "ToggleUnitDetailsCmd.hh"
#include "MDFView.hh"
#include "MachineCanvas.hh"
#include "ProDeConstants.hh"
#include "ProDe.hh"
#include "MachineCanvasOptions.hh"

using std::string;

/**
 * The Constructor.
 */
ToggleUnitDetailsCmd::ToggleUnitDetailsCmd() :
    EditorCommand(ProDeConstants::CMD_NAME_TOGGLE_UNIT_DETAILS) {

}


/**
 * The Destructor.
 */
ToggleUnitDetailsCmd::~ToggleUnitDetailsCmd() {
}

/**
 * Executes the command.
 *
 * @return true, if the command was succesfully executed, false otherwise.
 */
bool
ToggleUnitDetailsCmd::Do() {
    MDFView* mView = dynamic_cast<MDFView*>(view());
    MachineCanvas* canvas = mView->canvas();
    OptionValue& option = canvas->options().optionValue(
        MachineCanvasOptions::SHOW_UNIT_INFO_STRING);
    option.setBoolValue(!option.isFlagOn());
    canvas->updateMachine();
    return true;
}


/**
 * Returns state of the check-item related to this command.
 *
 * @return true, if the command check item is checked.
 */
bool
ToggleUnitDetailsCmd::isChecked() const {
    MDFView* mView = dynamic_cast<MDFView*>(view());
    if (mView == NULL) {
        return false;
    }
    MachineCanvas* canvas = mView->canvas();
    if (canvas == NULL) {
        return false;
    }
    OptionValue& option = canvas->options().optionValue(
        MachineCanvasOptions::SHOW_UNIT_INFO_STRING);
    return option.isFlagOn();
}



/**
 * Returns id of this command.
 *
 * @return ID for this command to be used in menus and toolbars.
 */
int
ToggleUnitDetailsCmd::id() const {
    return ProDeConstants::COMMAND_TOGGLE_UNIT_DETAILS;
}


/**
 * Creates and returns a new instance of this command.
 *
 * @return Newly created instance of this command.
 */
ToggleUnitDetailsCmd*
ToggleUnitDetailsCmd::create() const {
    return new ToggleUnitDetailsCmd();
}


/**
 * Returns short version of the command name.
 *
 * @return Short name of the command to be used in the toolbar.
 */
string
ToggleUnitDetailsCmd::shortName() const {
    return ProDeConstants::CMD_SNAME_TOGGLE_UNIT_DETAILS;
}


/**
 * Returns true when the command is executable, false when not.
 *
 * This command is executable when a document is open.
 *
 * @return True, if a document is open.
 */
bool
ToggleUnitDetailsCmd::isEnabled() {
    wxDocManager* manager = wxGetApp().docManager();
    if (manager->GetCurrentView() != NULL) {
	return true;
    }
    return false;
}
