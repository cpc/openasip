/**
 * @file ProximSimulatorWindow.hh
 *
 * Declaration of ProximSimulatorWindow class.
 *
 * @note rating: red
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_PROXIM_SIMULATOR_WINDOW_HH
#define TTA_PROXIM_SIMULATOR_WINDOW_HH

#include <wx/panel.h>

class ProximMainFrame;
class SimulatorEvent;

/**
 * Base class for Proxim windows which listen to the simulator events.
 *
 * This baseclass handles the adding and removing of the window from the
 * ProximMainFrame's list of subwindows automatically.
 */
class ProximSimulatorWindow : public wxPanel {
public:
    virtual void reset();
protected:
    ProximSimulatorWindow(
        ProximMainFrame* mainFrame,
        wxWindowID id = -1,
        wxPoint pos = wxDefaultPosition,
        wxSize size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL);
    virtual ~ProximSimulatorWindow();
private:
    virtual void onSimulatorBusy(SimulatorEvent& event);
    virtual void onSimulatorDone(SimulatorEvent& event);
    ProximSimulatorWindow(const ProximSimulatorWindow&);
    ProximSimulatorWindow& operator=(const ProximSimulatorWindow&);

    DECLARE_EVENT_TABLE()
};

#endif
