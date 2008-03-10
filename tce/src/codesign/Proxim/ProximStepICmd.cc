/**
 * @file ProximStepICmd.cc
 *
 * Implementation of ProximStepICmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "ProximStepICmd.hh"
#include "ProximConstants.hh"
#include "Proxim.hh"
#include "ProximSimulationThread.hh"
#include "TracedSimulatorFrontend.hh"

/**
 * The Constructor.
 */
ProximStepICmd::ProximStepICmd():
    GUICommand(ProximConstants::COMMAND_NAME_STEPI, NULL) {

    simulator_ = wxGetApp().simulation()->frontend();
}

/**
 * The Destructor.
 */
ProximStepICmd::~ProximStepICmd() {
}


/**
 * Executes the command.
 */
bool
ProximStepICmd::Do() {
    wxGetApp().simulation()->lineReader().input(
        ProximConstants::SCL_STEP_INSTRUCTION);
    return true;
}


/**
 * Returns full path to the command icon file.
 *
 * @return Full path to the command icon file.
 */
std::string
ProximStepICmd::icon() const {
    return "stepi.png";
}


/**
 * Returns ID of this command.
 */
int
ProximStepICmd::id() const {
    return ProximConstants::COMMAND_STEPI;
}


/**
 * Creates and returns a new isntance of this command.
 *
 * @return Newly created instance of this command.
 */
ProximStepICmd*
ProximStepICmd::create() const {
    return new ProximStepICmd();
}


/**
 * Returns true if the command is enabled, false otherwise.
 *
 * @return True if the simulation is initialized or stopped.
 */
bool
ProximStepICmd::isEnabled() {

    if (simulator_ != NULL &&
        (simulator_->isSimulationInitialized() ||
         simulator_->isSimulationStopped())) {

	return true;
    } else {
	return false;
    }
}
