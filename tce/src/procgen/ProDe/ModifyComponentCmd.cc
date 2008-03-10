/**
 * @file ModifyComponentCmd.cc
 *
 * Implementation of ModifyComponentCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <wx/docview.h>

#include "ModifyComponentCmd.hh"
#include "ComponentCommand.hh"
#include "WarningDialog.hh"
#include "Request.hh"
#include "MDFView.hh"
#include "MDFDocument.hh"
#include "EditPart.hh"
#include "ProDeConstants.hh"

using std::string;

/**
 * The Constructor.
 */
ModifyComponentCmd::ModifyComponentCmd():
    EditorCommand(ProDeConstants::CMD_NAME_MODIFY_COMP) {
}



/**
 * Executes the command.
 *
 * @return True, if the command was succesfully executed, false otherwise.
 */
bool
ModifyComponentCmd::Do() {

    assert(parentWindow() != NULL);
    assert(view() != NULL);

    // send modify request to the selected EditPart
    Request* modifyRequest = new Request(Request::MODIFY_REQUEST);
    EditPart* selected = dynamic_cast<MDFView*>(view())->selection();
    if (selected == NULL) {
	return false;
    }

    ComponentCommand* command = selected->performRequest(modifyRequest);

    // execute the returned command
    if (command == NULL) {
	return false;
    } else {

	Model* model = dynamic_cast<MDFDocument*>(
	    wxGetApp().docManager()->GetCurrentDocument())->getModel();

	model->pushToStack();

	dynamic_cast<MDFView*>(view())->clearSelection();
	command->setParentWindow(parentWindow());
	bool result = command->Do();
	if (result) {
	    // Model was modified.
	    model->notifyObservers();
	} else {
	    // Modification was cancelled.
	    model->popFromStack();
	}
	return result;
    }
}


/**
 * Returns command identifier of this command.
 *
 * @return ID for this command to be used in menus and toolbars.
 */
int
ModifyComponentCmd::id() const {
    return ProDeConstants::COMMAND_MODIFY_COMP;
}


/**
 * Creates and returns a new instance of this command.
 *
 * @return Newly created instance of this command.
 */
ModifyComponentCmd*
ModifyComponentCmd::create() const {
    return new ModifyComponentCmd();
}


/**
 * Returns short version of the command name.
 *
 * @return Short name of the command to be used in the toolbar.
 */
string
ModifyComponentCmd::shortName() const {
    return ProDeConstants::CMD_SNAME_MODIFY_COMP;
}

/**
 * Returns path to the command's icon file.
 *
 * @return Full path to the command's icon file.
 */
string
ModifyComponentCmd::icon() const {
    return ProDeConstants::CMD_ICON_MODIFY_COMP;
}



/**
 * Returns true when the command is executable, false when not.
 *
 * This command is executable when a block is selected, and the
 * selected block can handle modify requests.
 *
 * @return True, if a modifyable block is selected.
 */
bool
ModifyComponentCmd::isEnabled() {
    wxDocManager* manager = wxGetApp().docManager();

    MDFView* mdfView = dynamic_cast<MDFView*>(manager->GetCurrentView());
    if (mdfView == NULL) {
	return false;
    }
    Request* modifyRequest = new Request(Request::MODIFY_REQUEST);
    if (mdfView->selection() == NULL ||
        !mdfView->selection()->canHandle(modifyRequest)) {

	return false;
    }
    return true;
}
