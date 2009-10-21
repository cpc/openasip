/*
    Copyright (c) 2002-2009 Tampere University of Technology.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file ProximRuntimeErrorHandler.cc
 *
 * Implementation of ProximRuntimeErrorHandler class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2009 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <string>
#include "ProximRuntimeErrorHandler.hh"
#include "SimulationEventHandler.hh"
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

    size_t minorErrors = frontend_.programErrorReportCount(
        SimulatorFrontend::RES_MINOR);
    size_t fatalErrors = frontend_.programErrorReportCount(
        SimulatorFrontend::RES_FATAL);

    // Send runtime warning event if minor errors occured.
    if (minorErrors > 0) {
        string message;
        for (size_t i = 0; i < minorErrors; ++i) {
            message += "warning: runtime error: \n";
            message += frontend_.programErrorReport(
                SimulatorFrontend::RES_MINOR, i) + "\n\n";
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
            message += 
                frontend_.programErrorReport(
                    SimulatorFrontend::RES_FATAL, i) + "\n";
            
        }

        SimulatorEvent simEvent(
            SimulatorEvent::EVT_SIMULATOR_RUNTIME_ERROR, message);
        wxPostEvent(&gui_, simEvent);
        
        // Stop simulation.
        frontend_.prepareToStop(SRE_RUNTIME_ERROR);
    }

    frontend_.clearProgramErrorReports();
}

