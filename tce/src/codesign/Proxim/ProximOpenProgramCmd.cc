/**
 * @file ProximOpenProgramCmd.cc
 *
 * Implementation of ProximOpenProgramCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "ProximOpenProgramCmd.hh"
#include "WxConversion.hh"
#include "ProximConstants.hh"
#include "Proxim.hh"
#include "ProximSimulationThread.hh"

/**
 * The Constructor.
 */
ProximOpenProgramCmd::ProximOpenProgramCmd():
    GUICommand(ProximConstants::COMMAND_NAME_OPEN_PROGRAM, NULL) {
}


/**
 * The Destructor.
 */
ProximOpenProgramCmd::~ProximOpenProgramCmd() {
}


/**
 * Executes the command.
 */
bool
ProximOpenProgramCmd::Do() {
    assert(parentWindow() != NULL);

    wxString wildcard = _T("TPEF Program Files (*.tpf, *.tpef)|*.tpf;*.tpef");
    wildcard.Append(_T("|All files|*"));
    wxFileDialog dialog(
	parentWindow(), _T("Choose a file."), _T(""), _T(""),
	wildcard, wxOPEN);

    if (dialog.ShowModal() == wxID_CANCEL) {
	return false;
    }

    std::string command;
    std::string file = WxConversion::toString(dialog.GetPath());

    command = ProximConstants::SCL_LOAD_PROGRAM + " \"" + file + "\"";
    wxGetApp().simulation()->lineReader().input(command);

    return true;
}


/**
 * Returns full path to the command icon file.
 *
 * @return Full path to the command icon file.
 */
std::string
ProximOpenProgramCmd::icon() const {
    return "open_program.png";
}


/**
 * Returns ID of this command.
 */
int
ProximOpenProgramCmd::id() const {
    return ProximConstants::COMMAND_OPEN_PROGRAM;
}


/**
 * Creates and returns a new instance of this command.
 *
 * @return Newly created instance of this command.
 */
ProximOpenProgramCmd*
ProximOpenProgramCmd::create() const {
    return new ProximOpenProgramCmd();
}


/**
 * Returns true if the command is enabled, false otherwise.
 *
 * @return Always true.
 */
bool
ProximOpenProgramCmd::isEnabled() {
    return true;
}


/**
 * Returns shortened name of the command for toolbar button text.
 *
 * @return Short version of the command name.
 */
std::string
ProximOpenProgramCmd::shortName() const {
    return "Program";
}
