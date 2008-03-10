/**
 * @file ProximRunCmd.cc
 *
 * Implementation of ProximRunCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "ProximRunCmd.hh"
#include "ProximConstants.hh"
#include "Proxim.hh"
#include "ProximSimulationThread.hh"
#include "TracedSimulatorFrontend.hh"

/**
 * The Constructor.
 */
ProximRunCmd::ProximRunCmd():
    GUICommand(ProximConstants::COMMAND_NAME_RUN, NULL) {

    simulator_ = wxGetApp().simulation()->frontend();
}

/**
 * The Destructor.
 */
ProximRunCmd::~ProximRunCmd() {
}


/**
 * Executes the command.
 */
bool
ProximRunCmd::Do() {
    wxGetApp().simulation()->lineReader().input("run");
    return true;
}


/**
 * Returns full path to the command icon file.
 *
 * @return Full path to the command icon file.
 */
std::string
ProximRunCmd::icon() const {
    return "run.png";
}


/**
 * Returns ID of this command.
 */
int
ProximRunCmd::id() const {
    return ProximConstants::COMMAND_RUN;
}


/**
 * Creates and returns a new isntance of this command.
 *
 * @return Newly created instance of this command.
 */
ProximRunCmd*
ProximRunCmd::create() const {
    return new ProximRunCmd();
}


/**
 * Returns true if the command is enabled, false otherwise.
 *
 * @return True if the simulation is initialized and ready to run,
 *         false otherwise.
 */
bool
ProximRunCmd::isEnabled() {

    if (simulator_ != NULL && simulator_->isSimulationInitialized()) {
	return true;
    } else {
	return false;
    }
}
