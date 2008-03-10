/**
 * @file AddFUCmd.cc
 *
 * Definition of AddFUCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#include <wx/wx.h>
#include <wx/docview.h>

#include "AddFUCmd.hh"
#include "FUDialog.hh"
#include "FunctionUnit.hh"
#include "Model.hh"
#include "ModelConstants.hh"
#include "MDFDocument.hh"
#include "ProDeConstants.hh"
#include "ProDe.hh"
#include "Machine.hh"

using std::string;
using namespace TTAMachine;

/**
 * The Constructor.
 */
AddFUCmd::AddFUCmd() : EditorCommand(ProDeConstants::CMD_NAME_ADD_FU) {
}



/**
 * Executes the command.
 *
 * @return true, if the command was succesfully executed, false otherwise.
 */
bool
AddFUCmd::Do() {

    assert(parentWindow() != NULL);
    assert(view() != NULL);

    Model* model = dynamic_cast<MDFDocument*>(
        view()->GetDocument())->getModel();

    model->pushToStack();

    // Generate name for the new FU.
    Machine::FunctionUnitNavigator navigator =
        model->getMachine()->functionUnitNavigator();
    string newName = ProDeConstants::COMP_NEW_NAME_PREFIX_FU;
    int i = 1;
    while (navigator.hasItem(newName)) {
        newName = ProDeConstants::COMP_NEW_NAME_PREFIX_FU +
            Conversion::toString(i);
        i++;
    }

    FunctionUnit* functionUnit = new FunctionUnit(newName);
    functionUnit->setMachine(*(model->getMachine()));

    FUDialog dialog(parentWindow(), functionUnit);

    if (dialog.ShowModal() == wxID_OK) {
	model->notifyObservers();
	return true;
    } else {
	// FU creation was cancelled
	model->popFromStack();
	return false;
    }
}


/**
 * Returns id of this command.
 *
 * @return ID for this command to be used in menus and toolbars.
 */
int
AddFUCmd::id() const {
    return ProDeConstants::COMMAND_ADD_FU;
}


/**
 * Creates and returns a new instance of this command.
 *
 * @return Newly created instance of this command.
 */
AddFUCmd*
AddFUCmd::create() const {
    return new AddFUCmd();
}


/**
 * Returns short version of the command name.
 *
 * @return Short name of the command to be used in the toolbar.
 */
string
AddFUCmd::shortName() const {
    return ProDeConstants::CMD_SNAME_ADD_FU;
}


/**
 * Returns true when the command is executable, false when not.
 *
 * This command is executable when a document is open.
 *
 * @return True, if a document is open.
 */
bool
AddFUCmd::isEnabled() {
    wxDocManager* manager = wxGetApp().docManager();
    if (manager->GetCurrentView() != NULL) {
	return true;
    }
    return false;
}
