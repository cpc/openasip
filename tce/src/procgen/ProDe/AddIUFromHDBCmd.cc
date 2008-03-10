/**
 * @file AddIUFromHDBCmd.cc
 *
 * Definition of AddIUFromHDBCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <wx/docview.h>

#include "AddIUFromHDBCmd.hh"
#include "ProDeConstants.hh"
#include "AddIUFromHDBDialog.hh"
#include "ProDe.hh"
#include "Model.hh"
#include "MDFDocument.hh"

using std::string;


/**
 * The Constructor.
 */
AddIUFromHDBCmd::AddIUFromHDBCmd():
    EditorCommand(ProDeConstants::CMD_NAME_ADD_IU_FROM_HDB) {
}


/**
 * The Destructor.
 */
AddIUFromHDBCmd::~AddIUFromHDBCmd() {}


/**
 * Executes the command.
 *
 * @return True, if the command was succesfully executed, false otherwise.
 */
bool
AddIUFromHDBCmd::Do() {

    assert(parentWindow() != NULL);
    assert(view() != NULL);

    Model* model = dynamic_cast<MDFDocument*>(
        view()->GetDocument())->getModel();


    AddIUFromHDBDialog dialog(parentWindow(), model);

    dialog.ShowModal();
    return true;
}


/**
 * Returns id of this command.
 */
int
AddIUFromHDBCmd::id() const {
    return ProDeConstants::COMMAND_ADD_IU_FROM_HDB;
}


/**
 * Creates and returns a new instance of this command.
 */
AddIUFromHDBCmd*
AddIUFromHDBCmd::create() const {
    return new AddIUFromHDBCmd();
}



/**
 * Returns path to the command's icon file.
 */
string
AddIUFromHDBCmd::icon() const {
    return ProDeConstants::CMD_ICON_DEFAULT;
}


/**
 * Returns short version of the command name.
 */
string
AddIUFromHDBCmd::shortName() const {
    return ProDeConstants::CMD_SNAME_ADD_IU_FROM_HDB;
}


/**
 * Returns true when the command is executable, false when not.
 *
 * This command is executable when a document is open.
 *
 * @return True, if a document is open.
 */
bool
AddIUFromHDBCmd::isEnabled() {
    wxDocManager* manager = wxGetApp().docManager();
    if (manager->GetCurrentView() != NULL) {
	return true;
    }
    return false;
}
