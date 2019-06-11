/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file ProximStopDialog.hh
 *
 * Declaration of ProximStopDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
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
