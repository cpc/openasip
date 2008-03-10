/**
 * @file EditTemplatesCmd.cc
 *
 * Definition of EditTemplatesCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <vector>
#include <wx/docview.h>

#include "EditTemplatesCmd.hh"
#include "ProDeConstants.hh"
#include "TemplateListDialog.hh"
#include "ProDe.hh"
#include "Model.hh"
#include "MDFDocument.hh"
#include "Machine.hh"
#include "ModelConstants.hh"

using std::string;
using std::vector;


/**
 * The Constructor.
 */
EditTemplatesCmd::EditTemplatesCmd():
    EditorCommand(ProDeConstants::CMD_NAME_EDIT_TEMPLATES) {
}


/**
 * The Destructor.
 */
EditTemplatesCmd::~EditTemplatesCmd() {}


/**
 * Executes the command.
 *
 * @return True, if the command was succesfully executed, false otherwise.
 */
bool
EditTemplatesCmd::Do() {

    assert(parentWindow() != NULL);
    assert(view() != NULL);

    Model* model = dynamic_cast<MDFDocument*>(
        view()->GetDocument())->getModel();

    model->pushToStack();

    TemplateListDialog dialog(parentWindow(), model->getMachine());

    if (dialog.ShowModal() == wxID_OK) {
	model->notifyObservers();
        return true;
    } else {
        // Cancel button was pressed, templates are not modified.
	model->popFromStack();
        return false;
    }
}


/**
 * Returns id of this command.
 *
 * @return Id of this command.
 */
int
EditTemplatesCmd::id() const {
    return ProDeConstants::COMMAND_EDIT_TEMPLATES;
}


/**
 * Creates and returns a new instance of this command.
 *
 * @return A new instance of this command.
 */
EditTemplatesCmd*
EditTemplatesCmd::create() const {
    return new EditTemplatesCmd();
}



/**
 * Returns path to the command's icon file.
 *
 * @return path to the command's icon file.
 */
string
EditTemplatesCmd::icon() const {
    return ProDeConstants::CMD_ICON_EDIT_TEMPLATES;
}


/**
 * Returns short version of the command name.
 *
 * @return Short version of the command name.
 */
string
EditTemplatesCmd::shortName() const {
    return ProDeConstants::CMD_SNAME_EDIT_TEMPLATES;
}


/**
 * Returns true when the command is executable, false when not.
 *
 * This command is executable when a document is open.
 *
 * @return True, if a document is open.
 */
bool
EditTemplatesCmd::isEnabled() {
    wxDocManager* manager = wxGetApp().docManager();
    if (manager->GetCurrentView() != NULL) {
	return true;
    }
    return false;
}
