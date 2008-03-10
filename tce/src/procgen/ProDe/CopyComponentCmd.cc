/**
 * @file CopyComponentCmd.cc
 *
 * Implementation of CopyComponentCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#include <wx/docview.h>

#include "Application.hh"
#include "CopyComponentCmd.hh"
#include "ComponentCommand.hh"
#include "MDFView.hh"
#include "MDFDocument.hh"
#include "EditPart.hh"
#include "ProDeConstants.hh"
#include "Request.hh"
#include "ProDe.hh"


using std::string;

/**
 * The Constructor.
 */
CopyComponentCmd::CopyComponentCmd():
    EditorCommand(ProDeConstants::CMD_NAME_COPY) {
}



/**
 * Executes the command.
 *
 * @return True, if the command was succesfully executed, false otherwise.
 */
bool
CopyComponentCmd::Do() {

    assert(view() != NULL);

    // send copy request to the selected EditPart
    Request* copyRequest = new Request(Request::COPY_REQUEST);
    EditPart* selected = dynamic_cast<MDFView*>(view())->selection();
    if (selected == NULL) {
        return false;
    }
    ComponentCommand* command = selected->performRequest(copyRequest);

    // execute the returned command
    if (command == NULL) {
        return false;
    } else {
        dynamic_cast<MDFView*>(view())->clearSelection();
        command->setParentWindow(parentWindow());

	// Copying component to clipboard doesn't modify model.
	// The model is not pushed into the undo stack.
        bool result = command->Do();

        return result;
    }

}


/**
 * Returns command identifier of this command.
 *
 * @return ID for this command to be used in menus and toolbars.
 */
int
CopyComponentCmd::id() const {
    return ProDeConstants::COMMAND_COPY;
}


/**
 * Creates and returns a new instance of this command.
 *
 * @return Newly created instance of this command.
 */
CopyComponentCmd*
CopyComponentCmd::create() const {
    return new CopyComponentCmd();
}



/**
 * Returns path to the command's icon file.
 *
 * @return Full path to the command's icon file.
 */
string
CopyComponentCmd::icon() const {
    return ProDeConstants::CMD_ICON_COPY;
}



/**
 * Returns true when the command is executable, false when not.
 *
 * This command is executable when a component is selected, and the selected
 * component can be copied.
 *
 * @return True, if a component is selected.
 */
bool
CopyComponentCmd::isEnabled() {
    wxDocManager* manager = wxGetApp().docManager();

    MDFView* mdfView = dynamic_cast<MDFView*>(manager->GetCurrentView());
    if (mdfView == NULL) {
	return false;
    }

    EditPart* selected = mdfView->selection();
    Request* copyRequest = new Request(Request::COPY_REQUEST);
    if (selected == NULL || !(selected->canHandle(copyRequest))) {
	delete copyRequest;
	return false;
    }
    delete copyRequest;
    return true;
}
