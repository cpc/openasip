/**
 * @file AddRFCmd.cc
 *
 * Definition of AddRFCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <wx/wx.h>
#include <wx/docview.h>

#include "AddRFCmd.hh"
#include "RFDialog.hh"
#include "Model.hh"
#include "MDFDocument.hh"
#include "ProDeConstants.hh"
#include "ModelConstants.hh"
#include "ErrorDialog.hh"
#include "ProDe.hh"
#include "RegisterFile.hh"
#include "Machine.hh"

using std::string;
using namespace TTAMachine;

/**
 * The Constructor.
 */
AddRFCmd::AddRFCmd() :
    EditorCommand(ProDeConstants::CMD_NAME_ADD_RF) {
}



/**
 * Executes the command.
 *
 * @return true, if the command was succesfully executed, false otherwise.
 */
bool
AddRFCmd::Do() {

    assert(parentWindow() != NULL);
    assert(view() != NULL);

    Model* model = dynamic_cast<MDFDocument*>(
        view()->GetDocument())->getModel();

    model->pushToStack();

    // Generate name for the new RF.
    Machine::RegisterFileNavigator navigator =
        model->getMachine()->registerFileNavigator();
    string newName = ProDeConstants::COMP_NEW_NAME_PREFIX_RF;
    int i = 1;
    while (navigator.hasItem(newName)) {
        newName = ProDeConstants::COMP_NEW_NAME_PREFIX_RF +
            Conversion::toString(i);
        i++;
    }

    RegisterFile* rf = new RegisterFile(
        newName, ModelConstants::DEFAULT_RF_SIZE,
        ModelConstants::DEFAULT_WIDTH,
        ModelConstants::DEFAULT_RF_MAX_READS,
        ModelConstants::DEFAULT_RF_MAX_WRITES,
        ModelConstants::DEFAULT_RF_GUARD_LATENCY, RegisterFile::NORMAL);
    
    rf->setMachine(*(model->getMachine()));

    RFDialog dialog(parentWindow(), rf);

    if (dialog.ShowModal() == wxID_OK) {
	model->notifyObservers();
	return true;
    } else {
	// register file creation was cancelled
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
AddRFCmd::id() const {
    return ProDeConstants::COMMAND_ADD_RF;
}


/**
 * Creates and returns a new instance of this command.
 *
 * @return Newly created instance of this command.
 */
AddRFCmd*
AddRFCmd::create() const {
    return new AddRFCmd();
}

/**
 * Returns short version of the command name.
 *
 * @return Short name of the command to be used in the toolbar.
 */
string
AddRFCmd::shortName() const {
    return ProDeConstants::CMD_SNAME_ADD_RF;
}


/**
 * Returns true when the command is executable, false when not.
 *
 * This command is executable when a document is open.
 *
 * @return True, if a document is open.
 */
bool
AddRFCmd::isEnabled() {
    wxDocManager* manager = wxGetApp().docManager();
    if (manager->GetCurrentView() != NULL) {
	return true;
    }
    return false;
}
