/**
 * @file AddIUCmd.cc
 *
 * Definition of AddIUCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#include <wx/wx.h>
#include <wx/docview.h>

#include "AddIUCmd.hh"
#include "IUDialog.hh"
#include "Model.hh"
#include "MDFDocument.hh"
#include "ModelConstants.hh"
#include "ProDeConstants.hh"
#include "ErrorDialog.hh"
#include "ProDe.hh"
#include "ImmediateUnit.hh"

using std::string;
using namespace TTAMachine;

/**
 * The Constructor.
 */
AddIUCmd::AddIUCmd() :
    EditorCommand(ProDeConstants::CMD_NAME_ADD_IU) {
}



/**
 * Executes the command.
 *
 * @return true, if the command was succesfully executed, false otherwise.
 */
bool
AddIUCmd::Do() {

    assert(parentWindow() != NULL);
    assert(view() != NULL);


    Model* model =
	dynamic_cast<MDFDocument*>(view()->GetDocument())->getModel();

    model->pushToStack();

    // Generate name for the new IU.
    Machine::ImmediateUnitNavigator navigator =
        model->getMachine()->immediateUnitNavigator();
    string newName = ProDeConstants::COMP_NEW_NAME_PREFIX_IU;
    int i = 1;
    while (navigator.hasItem(newName)) {
        newName = ProDeConstants::COMP_NEW_NAME_PREFIX_IU +
            Conversion::toString(i);
        i++;
    }

    ImmediateUnit* iu =
        new ImmediateUnit(newName, ModelConstants::DEFAULT_IU_SIZE,
                          ModelConstants::DEFAULT_WIDTH, 
                          ModelConstants::DEFAULT_IU_MAX_READS, 
                          ModelConstants::DEFAULT_IU_GUARD_LATENCY,
                          Machine::ZERO);

    iu->setMachine(*(model->getMachine()));

    IUDialog dialog(parentWindow(), iu);

    if (dialog.ShowModal() == wxID_OK) {
	model->notifyObservers();
	return true;
    } else {
	// immediate unit creation was cancelled
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
AddIUCmd::id() const {
    return ProDeConstants::COMMAND_ADD_IU;
}


/**
 * Creates and returns a new instance of this command.
 *
 * @return Newly created instance of this command.
 */
AddIUCmd*
AddIUCmd::create() const {
    return new AddIUCmd();
}


/**
 * Returns short version of the command name.
 *
 * @return Short name of the command to be used in the toolbar.
 */
string
AddIUCmd::shortName() const {
    return ProDeConstants::CMD_SNAME_ADD_IU;
}


/**
 * Returns true when the command is executable, false when not.
 *
 * This command is executable when a document is open.
 *
 * @return True, if a document is open.
 */
bool
AddIUCmd::isEnabled() {
    wxDocManager* manager = wxGetApp().docManager();
    if (manager->GetCurrentView() != NULL) {
	return true;
    }
    return false;
}
