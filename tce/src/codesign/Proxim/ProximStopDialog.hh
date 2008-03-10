/**
 * @file ProximStopDialog.hh
 *
 * Declaration of ProximStopDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROXIM_STOP_DIALOG_HH
#define TTA_PROXIM_STOP_DIALOG_HH

#include <wx/wx.h>
#include "SimulatorConstants.hh"

class SimulatorFrontend;

/**
 * Dialog which is diaplayed when the simulation is running.
 *
 * The dialog contains Stop-button for stopping the simulation, and some
 * information of the simulation progress.
 */
class ProximStopDialog : public wxDialog {
public:
    ProximStopDialog(
        wxWindow* parent,
        wxWindowID id,
        SimulatorFrontend& frontend);

    virtual ~ProximStopDialog();

private:
    void onStop(wxCommandEvent& event);
    void onClose(wxCloseEvent& event);
    void onUpdate(wxTimerEvent& event);

    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);

    /// Widget IDs.
    enum {
        ID_STOP = 10000,
        ID_LABEL_CYCLES,
        ID_CYCLES,
        ID_LABEL_TIME,
        ID_TIME,
        ID_LABEL_CPS,
        ID_CPS,
        ID_TEXT,
        ID_LINE
    };

    /// Simulator frontend to stop and track.
    SimulatorFrontend& frontend_;

    /// Timer which updates the dialog periodically.
    wxTimer* timer_;
    /// Stop watch measuring the simulation runtime.
    wxStopWatch* watch_;

    /// Text displaying the elapsed simulation time.
    wxStaticText* timeCtrl_;
    /// Text displaying the simulated cycle count.
    wxStaticText* cyclesCtrl_;
    /// Text displaying the average cps value.
    wxStaticText* cpsCtrl_;

    /// Time elapsed on the cps update.
    long time_;
    /// Cycles simulated on the last cps update.
    ClockCycleCount cycles_;

    /// Dialog update itnerval in milliseconds.
    static const int UPDATE_INTERVAL;

    DECLARE_EVENT_TABLE()
};
#endif
