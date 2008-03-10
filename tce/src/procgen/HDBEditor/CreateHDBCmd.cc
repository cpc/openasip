/**
 * @file CreateHDBCmd.cc
 *
 * Implementation of CreateHDBCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "CreateHDBCmd.hh"
#include "WxConversion.hh"
#include "HDBEditorConstants.hh"
#include "HDBEditor.hh"
#include "HDBEditorMainFrame.hh"

/**
 * The Constructor.
 */
CreateHDBCmd::CreateHDBCmd():
    GUICommand(HDBEditorConstants::COMMAND_NAME_CREATE_HDB, NULL) {
}


/**
 * The Destructor.
 */
CreateHDBCmd::~CreateHDBCmd() {
}


/**
 * Executes the command.
 */
bool
CreateHDBCmd::Do() {
    assert(parentWindow() != NULL);

    wxString wildcard = _T("Hardware Database files (*.hdb)|*.hdb");
    wildcard.Append(_T("|All files|*"));
    wxFileDialog dialog(
	parentWindow(), _T("Enter path for the new HDB"), _T(""), _T(""),
	wildcard, wxSAVE);

    if (dialog.ShowModal() == wxID_CANCEL) {
	return false;
    }

    std::string command;
    std::string file = WxConversion::toString(dialog.GetPath());

    return wxGetApp().mainFrame().createHDB(file);
}


/**
 * Returns full path to the command icon file.
 *
 * @return Full path to the command icon file.
 */
std::string
CreateHDBCmd::icon() const {
    return "filenew.png";
}


/**
 * Returns ID of this command.
 */
int
CreateHDBCmd::id() const {
    return HDBEditorConstants::COMMAND_CREATE_HDB;
}


/**
 * Creates and returns a new isntance of this command.
 *
 * @return Newly created instance of this command.
 */
CreateHDBCmd*
CreateHDBCmd::create() const {
    return new CreateHDBCmd();
}


/**
 * Returns true if the command is enabled, false otherwise.
 *
 * @return Always true.
 */
bool
CreateHDBCmd::isEnabled() {
    return true;
}
