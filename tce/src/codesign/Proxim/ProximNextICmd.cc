/**
 * @file ProximNextICmd.cc
 *
 * Implementation of ProximNextICmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "ProximNextICmd.hh"
#include "ProximConstants.hh"
#include "Proxim.hh"
#include "ProximSimulationThread.hh"
#include "TracedSimulatorFrontend.hh"

/**
 * The Constructor.
 */
ProximNextICmd::ProximNextICmd():
    GUICommand(ProximConstants::COMMAND_NAME_NEXTI, NULL) {

    simulator_ = wxGetApp().simulation()->frontend();
}

/**
 * The Destructor.
 */
ProximNextICmd::~ProximNextICmd() {
}


/**
 * Executes the command.
 */
bool
ProximNextICmd::Do() {
    wxGetApp().simulation()->lineReader().input(
        ProximConstants::SCL_NEXT_INSTRUCTION);
    return true;
}


/**
 * Returns full path to the command icon file.
 *
 * @return Full path to the command icon file.
 */
std::string
ProximNextICmd::icon() const {
    return "nexti.png";
}


/**
 * Returns ID of this command.
 */
int
ProximNextICmd::id() const {
    return ProximConstants::COMMAND_NEXTI;
}


/**
 * Creates and returns a new isntance of this command.
 *
 * @return Newly created instance of this command.
 */
ProximNextICmd*
ProximNextICmd::create() const {
    return new ProximNextICmd();
}


/**
 * Returns true if the command is enabled, false otherwise.
 *
 * @return True if the simulation is initialized or stopped.
 */
bool
ProximNextICmd::isEnabled() {

    if (simulator_ != NULL &&
        (simulator_->isSimulationInitialized() ||
         simulator_->isSimulationStopped())) {

	return true;
    } else {
	return false;
    }
}
