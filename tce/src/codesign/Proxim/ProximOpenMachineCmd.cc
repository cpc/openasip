/**
 * @file ProximOpenMachineCmd.cc
 *
 * Implementation of ProximOpenMachineCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <wx/filedlg.h>
#include "ProximOpenMachineCmd.hh"
#include "WxConversion.hh"
#include "ProximConstants.hh"
#include "Proxim.hh"
#include "FileSystem.hh"
#include "ProximSimulationThread.hh"

/**
 * The Constructor.
 */
ProximOpenMachineCmd::ProximOpenMachineCmd():
    GUICommand(ProximConstants::COMMAND_NAME_OPEN_MACHINE, NULL) {

    }

/**
 * The Destructor.
 */
ProximOpenMachineCmd::~ProximOpenMachineCmd() {
}


/**
 * Executes the command.
 */
bool
ProximOpenMachineCmd::Do() {
    assert(parentWindow() != NULL);

    wxString wildcard = _T("Architecture Definition Files (*.adf)|*.adf");
    wildcard.Append(_T("|Processor Configuration Files (*.pcf)|*.pcf"));
    wildcard.Append(_T("|All files|*.*"));
    wxFileDialog dialog(
	parentWindow(), _T("Choose a file."), _T(""), _T(""),
	wildcard, wxOPEN);

    if (dialog.ShowModal() == wxID_CANCEL) {
	return false;
    }

    std::string command;
    std::string file = WxConversion::toString(dialog.GetPath());
    std::string extension = FileSystem::fileExtension(file);

    // Simulator interpreter command is chose according to file extension.
    // Processor configuration files (.pcf), have to be opened using
    // 'conf' command.
    if (extension == ".pcf") {
	command = ProximConstants::SCL_LOAD_CONF + " \"" + file + "\"";
    } else {
	command = ProximConstants::SCL_LOAD_MACHINE + " \"" + file + "\"";
    }

    wxGetApp().simulation()->lineReader().input(command);
    return true;
}


/**
 * Returns full path to the command icon file.
 *
 * @return Full path to the command icon file.
 */
std::string
ProximOpenMachineCmd::icon() const {
    return "open_machine.png";
}


/**
 * Returns ID of this command.
 */
int
ProximOpenMachineCmd::id() const {
    return ProximConstants::COMMAND_OPEN_MACHINE;
}


/**
 * Creates and returns a new isntance of this command.
 *
 * @return Newly created instance of this command.
 */
ProximOpenMachineCmd*
ProximOpenMachineCmd::create() const {
    return new ProximOpenMachineCmd();
}


/**
 * Returns true if the command is enabled, false otherwise.
 *
 * @return Always true.
 */
bool
ProximOpenMachineCmd::isEnabled() {
    return true;
}

/**
 * Returns shortened name of the command for toolbar button text.
 *
 * @return Short version of the command name.
 */
std::string
ProximOpenMachineCmd::shortName() const {
    return "Machine";
}
