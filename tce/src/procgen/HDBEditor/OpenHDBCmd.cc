/**
 * @file OpenHDBCmd.cc
 *
 * Implementation of OpenHDBCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "OpenHDBCmd.hh"
#include "WxConversion.hh"
#include "HDBEditorConstants.hh"
#include "HDBEditor.hh"
#include "HDBEditorMainFrame.hh"

/**
 * The Constructor.
 */
OpenHDBCmd::OpenHDBCmd():
    GUICommand(HDBEditorConstants::COMMAND_NAME_OPEN_HDB, NULL) {
}


/**
 * The Destructor.
 */
OpenHDBCmd::~OpenHDBCmd() {
}


/**
 * Executes the command.
 */
bool
OpenHDBCmd::Do() {
    assert(parentWindow() != NULL);

    wxString wildcard = _T("Hardware Database files (*.hdb)|*.hdb");
    wildcard.Append(_T("|All files|*"));
    wxFileDialog dialog(
	parentWindow(), _T("Choose a file."), _T(""), _T(""),
	wildcard, wxOPEN);

    if (dialog.ShowModal() == wxID_CANCEL) {
	return false;
    }

    std::string command;
    std::string file = WxConversion::toString(dialog.GetPath());

    wxGetApp().mainFrame().setHDB(file);

    return true;
}


/**
 * Returns full path to the command icon file.
 *
 * @return Full path to the command icon file.
 */
std::string
OpenHDBCmd::icon() const {
    return "fileopen.png";
}


/**
 * Returns ID of this command.
 */
int
OpenHDBCmd::id() const {
    return HDBEditorConstants::COMMAND_OPEN_HDB;
}


/**
 * Creates and returns a new isntance of this command.
 *
 * @return Newly created instance of this command.
 */
OpenHDBCmd*
OpenHDBCmd::create() const {
    return new OpenHDBCmd();
}


/**
 * Returns true if the command is enabled, false otherwise.
 *
 * @return Always true.
 */
bool
OpenHDBCmd::isEnabled() {
    return true;
}
