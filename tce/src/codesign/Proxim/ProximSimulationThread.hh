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
