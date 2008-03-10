/**
 * @file AddFUFromHDBCmd.cc
 *
 * Definition of AddFUFromHDBCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <wx/docview.h>

#include "AddFUFromHDBCmd.hh"
#include "ProDeConstants.hh"
#include "AddFUFromHDBDialog.hh"
#include "ProDe.hh"
#include "Model.hh"
#include "MDFDocument.hh"

using std::string;


/**
 * The Constructor.
 */
AddFUFromHDBCmd::AddFUFromHDBCmd():
    EditorCommand(ProDeConstants::CMD_NAME_ADD_FU_FROM_HDB) {
}


/**
 * The Destructor.
 */
AddFUFromHDBCmd::~AddFUFromHDBCmd() {}


/**
 * Executes the command.
 *
 * @return True, if the command was succesfully executed, false otherwise.
 */
bool
AddFUFromHDBCmd::Do() {

    assert(parentWindow() != NULL);
    assert(view() != NULL);

    Model* model = dynamic_cast<MDFDocument*>(
        view()->GetDocument())->getModel();


    AddFUFromHDBDialog dialog(parentWindow(), model);

    dialog.ShowModal();
    return true;
}


/**
 * Returns id of this command.
 */
int
AddFUFromHDBCmd::id() const {
    return ProDeConstants::COMMAND_ADD_FU_FROM_HDB;
}


/**
 * Creates and returns a new instance of this command.
 */
AddFUFromHDBCmd*
AddFUFromHDBCmd::create() const {
    return new AddFUFromHDBCmd();
}



/**
 * Returns path to the command's icon file.
 */
string
AddFUFromHDBCmd::icon() const {
    return ProDeConstants::CMD_ICON_DEFAULT;
}


/**
 * Returns short version of the command name.
 */
string
AddFUFromHDBCmd::shortName() const {
    return ProDeConstants::CMD_SNAME_ADD_FU_FROM_HDB;
}


/**
 * Returns true when the command is executable, false when not.
 *
 * This command is executable when a document is open.
 *
 * @return True, if a document is open.
 */
bool
AddFUFromHDBCmd::isEnabled() {
    wxDocManager* manager = wxGetApp().docManager();
    if (manager->GetCurrentView() != NULL) {
	return true;
    }
    return false;
}
