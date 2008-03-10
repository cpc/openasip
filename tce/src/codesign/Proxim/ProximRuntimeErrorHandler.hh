/**
 * @file ProximRuntimeErrorHandler.hh
 *
 * Declaration of ProximRuntimeErrorHandler class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROXIM_RUNTIME_ERROR_HANDLER_HH
#define TTA_PROXIM_RUNTIME_ERROR_HANDLER_HH

#include <wx/wx.h>
#include "Listener.hh"
#include "SimulatorFrontend.hh"


/**
 * A class which handles simulation runtime errors in Proxim.
 *
 * The errors are reported to the GUI using SimulatorEvents, and the
 * simultion is stopped id necessary.
 */
class ProximRuntimeErrorHandler : public Listener {
public:
    ProximRuntimeErrorHandler(
	SimulatorFrontend& frontend,
	wxEvtHandler& gui);
    ~ProximRuntimeErrorHandler();

    virtual void handleEvent();

private:
    /// Simulator frontend to stop when a fatal error occurs.
    SimulatorFrontend& frontend_;
    /// GUI to send the simulator events to.
    wxEvtHandler& gui_;
};

#endif
