/**
 * @file ProximKillCmd.cc
 *
 * Implementation of ProximKillCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "ProximKillCmd.hh"
#include "ProximConstants.hh"
#include "Proxim.hh"
#include "ProximSimulationThread.hh"
#include "TracedSimulatorFrontend.hh"
#include "ConfirmDialog.hh"

/**
 * The Constructor.
 */
ProximKillCmd::ProximKillCmd():
    GUICommand(ProximConstants::COMMAND_NAME_KILL, NULL) {

    simulator_ = wxGetApp().simulation()->frontend();
}

/**
 * The Destructor.
 */
ProximKillCmd::~ProximKillCmd() {
}


/**
 * Executes the command.
 */
bool
ProximKillCmd::Do() {
    assert(parentWindow() != NULL);
    wxString message = _T("Are you sure you want to restart simulation?");
    wxString title = _T("Confirm kill");
    wxMessageDialog dialog(
	parentWindow(), message, title, wxYES_NO | wxCENTRE | wxICON_QUESTION);

    if (dialog.ShowModal() == wxID_YES) {
	wxGetApp().simulation()->killSimulation();
    }

    return true;
}


/**
 * Returns full path to the command icon file.
 *
 * @return Full path to the command icon file.
 */
std::string
ProximKillCmd::icon() const {
    return "kill.png";
}


/**
 * Returns ID of this command.
 */
int
ProximKillCmd::id() const {
    return ProximConstants::COMMAND_KILL;
}


/**
 * Creates and returns a new isntance of this command.
 *
 * @return Newly created instance of this command.
 */
ProximKillCmd*
ProximKillCmd::create() const {
    return new ProximKillCmd();
}


/**
 * Returns true if the command is enabled, false otherwise.
 *
 * @return True if the simulation is running.
 */
bool
ProximKillCmd::isEnabled() {
    if (simulator_ != NULL &&
        (simulator_->isSimulationRunning() ||
         simulator_->isSimulationStopped() ||
         simulator_->hasSimulationEnded())) {

	return true;
    } else {
	return false;
    }
}
