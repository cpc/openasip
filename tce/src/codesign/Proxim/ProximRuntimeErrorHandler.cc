/**
 * @file ProximRuntimeErrorHandler.cc
 *
 * Implementation of ProximRuntimeErrorHandler class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <string>
#include "ProximRuntimeErrorHandler.hh"
#include "SimulationEventHandler.hh"
#include "SimulatorToolbox.hh"
#include "SimulatorEvent.hh"

using std::string;

/**
 * The Constructor.
 *
 * The created error handler is registered automatically as simulator
 * event handler listener.
 *
 * @param frontend Simulator frontend to stop when a fatal error occurs.
 * @param gui GUI to send the error events to.
 */
ProximRuntimeErrorHandler::ProximRuntimeErrorHandler(
    SimulatorFrontend& frontend,
    wxEvtHandler& gui):
    Listener(),
    frontend_(frontend),
    gui_(gui) {

    frontend.eventHandler().registerListener(
        SimulationEventHandler::SE_RUNTIME_ERROR, this);
}

/**
 * The Destructor.
 */
ProximRuntimeErrorHandler::~ProximRuntimeErrorHandler() {
    frontend_.eventHandler().unregisterListener(
	SimulationEventHandler::SE_RUNTIME_ERROR, this);
}


/**
 * Handles simulator runtime errors.
 *
 * This event handler sends a SimulatorEvent to the GUI when a runtime error
 * occurs. An error message explaining the error is sent as the event
 * string payload. If a fatal error occured, the simulator is stopped.
 */
void
ProximRuntimeErrorHandler::handleEvent() {

    size_t minorErrors = SimulatorToolbox::programErrorReportCount(
	SimulatorToolbox::RES_MINOR);
    size_t fatalErrors = SimulatorToolbox::programErrorReportCount(
	SimulatorToolbox::RES_FATAL);


    // Send runtime warning event if minor errors occured.
    if (minorErrors > 0) {
	string message;
	for (size_t i = 0; i < minorErrors; ++i) {
	    message += "warning: runtime error: \n";
	    message += SimulatorToolbox::programErrorReport(
		SimulatorToolbox::RES_MINOR, i) + "\n\n";
	}    
	SimulatorEvent simEvent(
	    SimulatorEvent::EVT_SIMULATOR_RUNTIME_WARNING, message);
	wxPostEvent(&gui_, simEvent);
    }

    // Send runtime error event if fatal errors occured.
    if (fatalErrors > 0) {
	string message;
	for (size_t i = 0; i < fatalErrors; ++i) {
	    message += "error: runtime error: \n";
	    message += SimulatorToolbox::programErrorReport(
		SimulatorToolbox::RES_FATAL, i) + "\n";

	}

	SimulatorEvent simEvent(
	    SimulatorEvent::EVT_SIMULATOR_RUNTIME_ERROR, message);
	wxPostEvent(&gui_, simEvent);

	// Stop simulation.
	frontend_.prepareToStop(SRE_RUNTIME_ERROR);
    }

    SimulatorToolbox::clearProgramErrorReports();
}

