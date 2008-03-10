/**
 * @file AddRFFromHDBCmd.cc
 *
 * Definition of AddRFFromHDBCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <wx/docview.h>

#include "AddRFFromHDBCmd.hh"
#include "ProDeConstants.hh"
#include "AddRFFromHDBDialog.hh"
#include "ProDe.hh"
#include "Model.hh"
#include "MDFDocument.hh"

using std::string;


/**
 * The Constructor.
 */
AddRFFromHDBCmd::AddRFFromHDBCmd():
    EditorCommand(ProDeConstants::CMD_NAME_ADD_RF_FROM_HDB) {
}


/**
 * The Destructor.
 */
AddRFFromHDBCmd::~AddRFFromHDBCmd() {}


/**
 * Executes the command.
 *
 * @return True, if the command was succesfully executed, false otherwise.
 */
bool
AddRFFromHDBCmd::Do() {

    assert(parentWindow() != NULL);
    assert(view() != NULL);

    Model* model = dynamic_cast<MDFDocument*>(
        view()->GetDocument())->getModel();


    AddRFFromHDBDialog dialog(parentWindow(), model);

    dialog.ShowModal();
    return true;
}


/**
 * Returns id of this command.
 */
int
AddRFFromHDBCmd::id() const {
    return ProDeConstants::COMMAND_ADD_RF_FROM_HDB;
}


/**
 * Creates and returns a new instance of this command.
 */
AddRFFromHDBCmd*
AddRFFromHDBCmd::create() const {
    return new AddRFFromHDBCmd();
}



/**
 * Returns path to the command's icon file.
 */
string
AddRFFromHDBCmd::icon() const {
    return ProDeConstants::CMD_ICON_DEFAULT;
}


/**
 * Returns short version of the command name.
 */
string
AddRFFromHDBCmd::shortName() const {
    return ProDeConstants::CMD_SNAME_ADD_RF_FROM_HDB;
}


/**
 * Returns true when the command is executable, false when not.
 *
 * This command is executable when a document is open.
 *
 * @return True, if a document is open.
 */
bool
AddRFFromHDBCmd::isEnabled() {
    wxDocManager* manager = wxGetApp().docManager();
    if (manager->GetCurrentView() != NULL) {
	return true;
    }
    return false;
}
