/**
 * @file EditImmediateSlotsCmd.cc
 *
 * Definition of EditImmediateSlotsCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <wx/docview.h>

#include "EditImmediateSlotsCmd.hh"
#include "ProDeConstants.hh"
#include "ImmediateSlotDialog.hh"
#include "ProDe.hh"
#include "Model.hh"
#include "MDFDocument.hh"

using std::string;


/**
 * The Constructor.
 */
EditImmediateSlotsCmd::EditImmediateSlotsCmd():
    EditorCommand(ProDeConstants::CMD_NAME_EDIT_IMMEDIATE_SLOTS) {
}


/**
 * The Destructor.
 */
EditImmediateSlotsCmd::~EditImmediateSlotsCmd() {}


/**
 * Executes the command.
 *
 * @return True, if the command was succesfully executed, false otherwise.
 */
bool
EditImmediateSlotsCmd::Do() {

    assert(parentWindow() != NULL);
    assert(view() != NULL);

    Model* model = dynamic_cast<MDFDocument*>(
        view()->GetDocument())->getModel();

    model->pushToStack();

    ImmediateSlotDialog dialog(parentWindow(), model->getMachine());

    if (dialog.ShowModal() == wxID_OK) {
	model->notifyObservers();
        return true;
    } else {
        // Cancel button was pressed, immediate slot modifications are
	// cancelled.
	model->popFromStack();
        return false;
    }
}


/**
 * Returns id of this command.
 */
int
EditImmediateSlotsCmd::id() const {
    return ProDeConstants::COMMAND_EDIT_IMMEDIATE_SLOTS;
}


/**
 * Creates and returns a new instance of this command.
 */
EditImmediateSlotsCmd*
EditImmediateSlotsCmd::create() const {
    return new EditImmediateSlotsCmd();
}



/**
 * Returns path to the command's icon file.
 */
string
EditImmediateSlotsCmd::icon() const {
    return ProDeConstants::CMD_ICON_DEFAULT;
}


/**
 * Returns short version of the command name.
 */
string
EditImmediateSlotsCmd::shortName() const {
    return ProDeConstants::CMD_SNAME_EDIT_IMMEDIATE_SLOTS;
}


/**
 * Returns true when the command is executable, false when not.
 *
 * This command is executable when a document is open.
 *
 * @return True, if a document is open.
 */
bool
EditImmediateSlotsCmd::isEnabled() {
    wxDocManager* manager = wxGetApp().docManager();
    if (manager->GetCurrentView() != NULL) {
	return true;
    }
    return false;
}
