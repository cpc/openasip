/**
 * @file SaveDocumentAsCmd.cc
 *
 * Definition of SaveDocumentAsCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#include <wx/docview.h>

#include "SaveDocumentAsCmd.hh"
#include "ProDeConstants.hh"
#include "ProDe.hh"

using std::string;

/**
 * The Constructor.
 */
SaveDocumentAsCmd::SaveDocumentAsCmd():
    EditorCommand(ProDeConstants::CMD_NAME_SAVEAS_DOC) {

}


/**
 * The Destructor.
 */
SaveDocumentAsCmd::~SaveDocumentAsCmd() {}


/**
 * Executes the command.
 *
 * @return Always false. Returning true here would cause problems with the
 *         wxCommandProcessor, and the command is never undoable so it's
 *         safe to return false even if the command was succesfully executed.
 */
bool
SaveDocumentAsCmd::Do() {
    wxCommandEvent dummy(wxID_SAVEAS, 0);
    wxGetApp().docManager()->OnFileSaveAs(dummy);
    return false;
}


/**
 * Returns id of this command.
 *
 * @return ID for this command to be used in menus and toolbars.
 */
int
SaveDocumentAsCmd::id() const {
    return ProDeConstants::COMMAND_SAVEAS_DOC;
}


/**
 * Creates and returns a new instance of this command.
 *
 * @return Newly created instance of this command.
 */
SaveDocumentAsCmd*
SaveDocumentAsCmd::create() const {
    return new SaveDocumentAsCmd();
}


/**
 * Returns short version of the command name.
 *
 * @return Short name of the command to be used in the toolbar.
 */
string
SaveDocumentAsCmd::shortName() const {
    return ProDeConstants::CMD_SNAME_SAVEAS_DOC;
}

/**
 * Returns path to the command's icon file.
 *
 * @return Full path to the command's icon file.
 */
string
SaveDocumentAsCmd::icon() const {
    return ProDeConstants::CMD_ICON_SAVEAS_DOC;
}


/**
 * Returns true when the command is executable, false when not.
 *
 * This command is executable when a document is open.
 *
 * @return True, if a document is open.
 */
bool
SaveDocumentAsCmd::isEnabled() {
    wxDocManager* manager = wxGetApp().docManager();
    if (manager->GetCurrentView() != NULL) {
	return true;
    }
    return false;
}
