/**
 * @file ProximResumeCmd.cc
 *
 * Implementation of ProximResumeCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "ProximResumeCmd.hh"
#include "ProximConstants.hh"
#include "Proxim.hh"
#include "ProximSimulationThread.hh"
#include "TracedSimulatorFrontend.hh"

/**
 * The Constructor.
 */
ProximResumeCmd::ProximResumeCmd():
    GUICommand(ProximConstants::COMMAND_NAME_RESUME, NULL) {

    simulator_ = wxGetApp().simulation()->frontend();
}

/**
 * The Destructor.
 */
ProximResumeCmd::~ProximResumeCmd() {
}


/**
 * Executes the command.
 */
bool
ProximResumeCmd::Do() {
    wxGetApp().simulation()->lineReader().input("resume");
    return true;
}


/**
 * Returns full path to the command icon file.
 *
 * @return Full path to the command icon file.
 */
std::string
ProximResumeCmd::icon() const {
    return "resume.png";
}


/**
 * Returns ID of this command.
 */
int
ProximResumeCmd::id() const {
    return ProximConstants::COMMAND_RESUME;
}


/**
 * Creates and returns a new isntance of this command.
 *
 * @return Newly created instance of this command.
 */
ProximResumeCmd*
ProximResumeCmd::create() const {
    return new ProximResumeCmd();
}


/**
 * Returns true if the command is enabled, false otherwise.
 *
 * @return True if the simulation is stopped but not finished.
 */
bool
ProximResumeCmd::isEnabled() {

    if (simulator_ != NULL && simulator_->isSimulationStopped()) {
	return true;
    } else {
	return false;
    }
}
