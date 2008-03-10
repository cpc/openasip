/**
 * @file ProximQuitCmd.cc
 *
 * Implementation of ProximQuitCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "ProximQuitCmd.hh"
#include "ProximConstants.hh"
#include "Proxim.hh"
#include "ProximSimulationThread.hh"
#include "TracedSimulatorFrontend.hh"
#include "ConfirmDialog.hh"
#include "GUIOptions.hh"
#include "GUIOptionsSerializer.hh"
#include "ErrorDialog.hh"
#include "WxConversion.hh"
#include "Exception.hh"
#include "ProximToolbox.hh"

/**
 * The Constructor.
 */
ProximQuitCmd::ProximQuitCmd():
    GUICommand(ProximConstants::COMMAND_NAME_QUIT, NULL) {

    simulator_ = ProximToolbox::frontend();
}

/**
 * The Destructor.
 */
ProximQuitCmd::~ProximQuitCmd() {
}


/**
 * Executes the command.
 *
 * @return True, if the command was succesfully executed.
 */
bool
ProximQuitCmd::Do() {

    assert(parentWindow() != NULL);

    if (simulator_->isSimulationRunning() || simulator_->isSimulationStopped()) {
	wxString message = _T("Simulation is still running.\n");
	message.Append(_T("Are you sure you want to stop the simulation "));
	message.Append(_T("and quit?"));
        ConfirmDialog dialog(parentWindow(), message);

	if (dialog.ShowModal() != wxID_YES) {
	    return false;
	}

	wxGetApp().simulation()->finishSimulation();
    }

    GUIOptions& options = wxGetApp().options();
    if (options.isModified()) {
        wxString message = _T("Options are modified.\n");
        message.Append(_T("Do you want to save changes?"));
        ConfirmDialog dialog(parentWindow(), message);

        int returnValue = dialog.ShowModal();
        if (returnValue == wxID_YES) {

            GUIOptionsSerializer writer(ProximConstants::CONFIGURATION_NAME);
            try {
                writer.setDestinationFile(options.fileName());
                writer.writeOptions(options);
                options.clearModified();
            } catch (Exception& e) {
                wxString message = _T("Error saving options:\n\n");
                message.Append(WxConversion::toWxString(e.errorMessage()));
                ErrorDialog errorDialog(parentWindow(), message);
                errorDialog.ShowModal();
                return false;
            }
        } else if (returnValue == wxID_CANCEL) {
            return false;
        }
    }

    ProximToolbox::lineReader().input(ProximConstants::SCL_QUIT);

    return true;
}


/**
 * Returns full path to the command icon file.
 *
 * @return Full path to the command icon file.
 */
std::string
ProximQuitCmd::icon() const {
    return "quit.png";
}


/**
 * Returns ID of this command.
 */
int
ProximQuitCmd::id() const {
    return ProximConstants::COMMAND_QUIT;
}


/**
 * Creates and returns a new isntance of this command.
 *
 * @return Newly created instance of this command.
 */
ProximQuitCmd*
ProximQuitCmd::create() const {
    return new ProximQuitCmd();
}


/**
 * Returns true if the command is enabled, false otherwise.
 *
 * @return Always true.
 */
bool
ProximQuitCmd::isEnabled() {
    return true;
}
