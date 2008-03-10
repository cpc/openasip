/**
 * @file ProximCDCmd.cc
 *
 * Implementation of ProximCDCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "ProximCDCmd.hh"
#include "WxConversion.hh"
#include "ProximConstants.hh"
#include "Proxim.hh"
#include "FileSystem.hh"
#include "ProximSimulationThread.hh"

/**
 * The Constructor.
 */
ProximCDCmd::ProximCDCmd():
    GUICommand("Change Directory", NULL) {

    }

/**
 * The Destructor.
 */
ProximCDCmd::~ProximCDCmd() {
}


/**
 * Executes the command.
 */
bool
ProximCDCmd::Do() {

    assert(parentWindow() != NULL);
    wxDirDialog dialog(parentWindow(), _T("Choose a directory"), wxGetCwd());

    if (dialog.ShowModal() == wxID_CANCEL) {
	return false;
    }

    std::string command = 
        "cd \"" + WxConversion::toString(dialog.GetPath()) + "\"";

    wxGetApp().simulation()->lineReader().input(command);
    return true;
}


/**
 * Returns full path to the command icon file.
 *
 * @return Full path to the command icon file.
 */
std::string
ProximCDCmd::icon() const {
    return "cd.png";
}


/**
 * Returns ID of this command.
 */
int
ProximCDCmd::id() const {
    return ProximConstants::COMMAND_CD;
}


/**
 * Creates and returns a new isntance of this command.
 *
 * @return Newly created instance of this command.
 */
ProximCDCmd*
ProximCDCmd::create() const {
    return new ProximCDCmd();
}


/**
 * Returns true if the command is enabled, false otherwise.
 *
 * @return Always true.
 */
bool
ProximCDCmd::isEnabled() {
    return true;
}
