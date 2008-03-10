/**
 * @file CloseDocumentCmd.cc
 *
 * Definition of CloseDocumentCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <wx/docview.h>

#include "CloseDocumentCmd.hh"
#include "ProDeConstants.hh"
#include "ProDe.hh"

using std::string;

/**
 * The Constructor.
 */
CloseDocumentCmd::CloseDocumentCmd():
    EditorCommand(ProDeConstants::CMD_NAME_CLOSE_DOC) {

}


/**
 * The Destructor.
 */
CloseDocumentCmd::~CloseDocumentCmd() {}


/**
 * Executes the command.
 *
 * @return Always false. Returning true here would cause problems with the
 *         wxCommandProcessor, and the command is never undoable so it's
 *         safe to return false even if the command was succesfully executed.
 */
bool
CloseDocumentCmd::Do() {
    // close the active document
    wxDocManager* docManager = wxGetApp().docManager();
    wxDocument *doc = docManager->GetCurrentDocument();
    if (!doc)
	return false;
    if (doc->Close()) {
	doc->DeleteAllViews();
    }
    return false;
}


/**
 * Returns id of this command.
 *
 * @return ID for this command to be used in menus and toolbars.
 */
int
CloseDocumentCmd::id() const {
    return ProDeConstants::COMMAND_CLOSE_DOC;
}


/**
 * Creates and returns a new instance of this command.
 *
 * @return Newly created instance of this command.
 */
CloseDocumentCmd*
CloseDocumentCmd::create() const {
    return new CloseDocumentCmd();
}


/**
 * Returns short version of the command name.
 *
 * @return Short name of the command to be used in the toolbar.
 */
string
CloseDocumentCmd::shortName() const {
    return ProDeConstants::CMD_SNAME_CLOSE_DOC;
}

/**
 * Returns path to the command's icon file.
 *
 * @return Full path to the command's icon file.
 */
string
CloseDocumentCmd::icon() const {
    return ProDeConstants::CMD_ICON_CLOSE_DOC;
}


/**
 * Returns true when the command is executable, false when not.
 *
 * This command is executable when a document is open.
 *
 * @return True, if a document is open.
 */
bool
CloseDocumentCmd::isEnabled() {
    wxDocManager* manager = wxGetApp().docManager();
    if (manager->GetCurrentView() != NULL) {
	return true;
    }
    return false;
}

