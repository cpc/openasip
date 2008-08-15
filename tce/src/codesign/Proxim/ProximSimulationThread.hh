/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file ProximSimulationThread.hh
 *
 * Declaration of ProximSimulationThread class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROXIM_SIMULATION_THREAD_HH
#define TTA_PROXIM_SIMULATION_THREAD_HH


#include <wx/wx.h>
#include "ProximMainFrame.hh"
#include "ProximLineReader.hh"
#include "Listener.hh"

class SimulatorInterpreter;
class SimulatorInterpreterContext;
class TracedSimulatorFrontend;
class ProximRuntimeErrorHandler;

/**
 * ProximSimulationThread class wraps the simulation backend in a separate
 * worker thread.
 *
 * The worker thread utilizes ProximLinereader to read commands from the user.
 * Simulator events and input requests are passed to the GUI thread as custom
 * wxEvents (see SimulatorEvent class).
 */
class ProximSimulationThread : public wxThread, public Listener {
public:
    ProximSimulationThread();
    virtual ~ProximSimulationThread();
    virtual ExitCode Entry();
    virtual void handleEvent(int event);
    ProximLineReader& lineReader();
    TracedSimulatorFrontend* frontend();
    void initialize(ProximMainFrame* gui);
    SimulatorInterpreter* interpreter();

    void requestStop();
    void killSimulation();
    void finishSimulation();
    bool isBusy();

private:
    /// SimulatorInterpreterContext. ?
    SimulatorInterpreterContext* interpreterContext_;
    /// SimulatorInterpreter where the user input is passed to.
    SimulatorInterpreter* interpreter_;
    /// Simulator backend.
    TracedSimulatorFrontend* simulation_;
    /// Linereader used for reading the user input.
    ProximLineReader* lineReader_;
    /// Proxim main frame where the simulator events are passed to.
    ProximMainFrame* gui_;
    /// Runtime error handler.
    ProximRuntimeErrorHandler* runtimeErrorHandler_;

};
#endif
