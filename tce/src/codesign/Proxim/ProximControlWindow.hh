/**
 * @file ProximControlWindow.cc
 *
 * Declaration of ProximControlWindow class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */


#ifndef TTA_PROXIM_CONTROL_WINDOW_HH
#define TTA_PROXIM_CONTROL_WINDOW_HH


#include "ProximSimulatorWindow.hh"
#include "SimulatorEvent.hh"

class ProximLineReader;
class SimulatorFrontend;
class wxSizer;

/**
 * Proxim subwindow with buttons to control the simulation process.
 *
 * ProximControlWindow doesn't handle button events. The events are
 * passed to the parent window. Button enabled status is updated
 * using wxUpdateUI events.
 */
class ProximControlWindow : public ProximSimulatorWindow {
public:
    ProximControlWindow(ProximMainFrame* parent, int id);
    virtual ~ProximControlWindow();
    virtual void reset();

private:
    wxSizer* createContents(wxWindow *parent, bool call_fit, bool set_sizer);
};

#endif
