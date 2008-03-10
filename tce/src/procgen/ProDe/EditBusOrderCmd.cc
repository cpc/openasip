/**
 * @file EditBusOrderCmd.cc
 *
 * Definition of EditBusOrderCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */


#include <wx/docview.h>

#include "EditBusOrderCmd.hh"
#include "ProDeConstants.hh"
#include "ProDeBusOrderDialog.hh"
#include "ProDe.hh"
#include "Model.hh"
#include "MDFDocument.hh"
#include "Machine.hh"

using std::string;


/**
 * The Constructor.
 */
EditBusOrderCmd::EditBusOrderCmd():
    EditorCommand(ProDeConstants::CMD_NAME_EDIT_BUS_ORDER) {
}


/**
 * The Destructor.
 */
EditBusOrderCmd::~EditBusOrderCmd() {}


/**
 * Executes the command.
 *
 * @return True, if the command was succesfully executed, false otherwise.
 */
bool
EditBusOrderCmd::Do() {

    assert(parentWindow() != NULL);
    assert(view() != NULL);

    Model* model = dynamic_cast<MDFDocument*>(
        view()->GetDocument())->getModel();

    model->pushToStack();

    ProDeBusOrderDialog dialog(parentWindow(), *model->getMachine());

    if (dialog.ShowModal() == wxID_OK) {
	model->notifyObservers();
        return true;
    } else {
        // Cancel button was pressed, address spaces are not modified.
	model->popFromStack();
        return false;
    }
}


/**
 * Returns id of this command.
 */
int
EditBusOrderCmd::id() const {
    return ProDeConstants::COMMAND_EDIT_BUS_ORDER;
}


/**
 * Creates and returns a new instance of this command.
 */
EditBusOrderCmd*
EditBusOrderCmd::create() const {
    return new EditBusOrderCmd();
}


/**
 * Returns short version of the command name.
 */
string
EditBusOrderCmd::shortName() const {
    return ProDeConstants::CMD_SNAME_EDIT_ADDRESS_SPACES;
}


/**
 * Returns true when the command is executable, false when not.
 *
 * This command is executable when a document is open.
 *
 * @return True, if a document is open.
 */
bool
EditBusOrderCmd::isEnabled() {
    wxDocManager* manager = wxGetApp().docManager();
    if (manager->GetCurrentView() != NULL) {
	return true;
    }
    return false;
}
