/**
 * @file ProximSimulatorSettingsCmd.cc
 *
 * Implementation of ProximSimulatorSettingsCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "ProximSimulatorSettingsCmd.hh"
#include "ProximConstants.hh"
#include "ProximToolbox.hh"
#include "SimulatorSettingsDialog.hh"
#include "ProximLineReader.hh"
#include "TracedSimulatorFrontend.hh"

/**
 * The Constructor.
 */
ProximSimulatorSettingsCmd::ProximSimulatorSettingsCmd():
    GUICommand(ProximConstants::COMMAND_NAME_SIMULATOR_SETTINGS, NULL) {
}


/**
 * The Destructor.
 */
ProximSimulatorSettingsCmd::~ProximSimulatorSettingsCmd() {
}

/**
 * Executes the command.
 *
 * @retutn True, if the command was succesfully executed.
 */
bool
ProximSimulatorSettingsCmd::Do() {
    wxWindow* parent = parentWindow();
    assert(parent != NULL);
    SimulatorFrontend* frontend = ProximToolbox::frontend();
    ProximLineReader& lineReader = ProximToolbox::lineReader();
    SimulatorSettingsDialog dialog(parentWindow(), -1, *frontend, lineReader);
    dialog.ShowModal();
    return true;
}


/**
 * Returns ID of this command.
 */
int
ProximSimulatorSettingsCmd::id() const {
    return ProximConstants::COMMAND_SIMULATOR_SETTINGS;
}


/**
 * Returns path to the icon file for this command.
 */
std::string
ProximSimulatorSettingsCmd::icon() const {
    return "configure.png";
}


/**
 * Creates and returns a new isntance of this command.
 *
 * @return Newly created instance of this command.
 */
ProximSimulatorSettingsCmd*
ProximSimulatorSettingsCmd::create() const {
    return new ProximSimulatorSettingsCmd();
}


/**
 * Returns true if the command is enabled, false otherwise.
 *
 * @return Always true.
 */
bool
ProximSimulatorSettingsCmd::isEnabled() {
    return true;
}
