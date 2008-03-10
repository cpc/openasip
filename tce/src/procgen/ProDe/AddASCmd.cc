/**
 * @file AddASCmd.cc
 *
 * Definition of AddASCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#include <vector>
#include <wx/wx.h>
#include <wx/docview.h>

#include "AddASCmd.hh"
#include "AddressSpaceDialog.hh"
#include "Model.hh"
#include "MDFDocument.hh"
#include "ProDeConstants.hh"
#include "ProDe.hh"
#include "Machine.hh"
#include "ModelConstants.hh"

using std::string;
using namespace TTAMachine;

/**
 * The Constructor.
 */
AddASCmd::AddASCmd() :
    EditorCommand(ProDeConstants::CMD_NAME_ADD_AS) {
}



/**
 * Executes the command.
 *
 * @return true, if the command was succesfully executed, false otherwise.
 */
bool
AddASCmd::Do() {

    assert(parentWindow() != NULL);
    assert(view() != NULL);

    Model* model = dynamic_cast<MDFDocument*>(
        view()->GetDocument())->getModel();

    model->pushToStack();

    // Generate name for the new AS.
    Machine::AddressSpaceNavigator navigator =
        model->getMachine()->addressSpaceNavigator();
    int i = 1;
    string newName = ProDeConstants::COMP_NEW_NAME_PREFIX_AS +
        Conversion::toString(i);
    while (navigator.hasItem(newName)) {
        newName = ProDeConstants::COMP_NEW_NAME_PREFIX_AS +
            Conversion::toString(i);
        i++;
    }

    AddressSpace* addressSpace =
	new AddressSpace(newName,
			 ModelConstants::DEFAULT_WIDTH,
			 ModelConstants::DEFAULT_AS_MIN_ADDRESS,
			 ModelConstants::DEFAULT_AS_MAX_ADDRESS,
			 *model->getMachine());


    AddressSpaceDialog dialog(parentWindow(), addressSpace);

    if (dialog.ShowModal() == wxID_OK) {
	model->notifyObservers();
	return true;
    } else {
	// as creation was cancelled
	model->popFromStack();
	return false;
    }

    return false;
}


/**
 * Returns id of this command.
 *
 * @return ID for this command to be used in menus and toolbars.
 */
int
AddASCmd::id() const {
    return ProDeConstants::COMMAND_ADD_AS;
}


/**
 * Creates and returns a new instance of this command.
 *
 * @return Newly created instance of this command.
 */
AddASCmd*
AddASCmd::create() const {
    return new AddASCmd();
}


/**
 * Returns short version of the command name.
 *
 * @return Short name of the command to be used in the toolbar.
 */
string
AddASCmd::shortName() const {
    return ProDeConstants::CMD_SNAME_ADD_AS;
}


/**
 * Returns true when the command is executable, false when not.
 *
 * This command is executable when a document is open.
 *
 * @return True, if a document is open.
 */
bool
AddASCmd::isEnabled() {
    wxDocManager* manager = wxGetApp().docManager();
    if (manager->GetCurrentView() != NULL) {
	return true;
    }
    return false;
}
