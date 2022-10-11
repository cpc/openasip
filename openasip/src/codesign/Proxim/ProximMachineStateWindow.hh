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
 * @file ProximMachineStateWindow.cc
 *
 * Declaration of ProximMachineStateWindow class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */


#ifndef TTA_PROXIM_MACHINE_STATE_WINDOW_HH
#define TTA_PROXIM_MACHINE_STATE_WINDOW_HH

#include "ProximSimulatorWindow.hh"
#include "SimulatorEvent.hh"

namespace TTAMachine {
    class Machine;
    class MachinePart;
}

class MachineCanvas;
class SimulatorFrontend;
class wxUpdateUIEvent;
class UtilizationWindow;

/**
 * Proxim subwindow which displays the simulated machine state.
 *
 * This window listens to SimulatorEvents and updates the window
 * contents automatically.
 */
class ProximMachineStateWindow : public ProximSimulatorWindow {
public:
    ProximMachineStateWindow(ProximMainFrame* parent, int id);
    virtual ~ProximMachineStateWindow();
    virtual void reset();
    void setMachine(TTAMachine::Machine* machine);
    void setStatusText(std::string status);
    void appendUtilizationData(std::string text);
    void appendDetails(std::string text);
    void clearDetails();

    // Command IDs.
    enum {
	COMMAND_ZOOM_IN = 20000,
	COMMAND_ZOOM_OUT,
        COMMAND_TOGGLE_UNIT_INFO,
        COMMAND_TOGGLE_MOVES,
        COMMAND_TOGGLE_UTILIZATIONS,
        COMMAND_EXPORT
    };

private:
    void onMachineLoaded(const SimulatorEvent& event);
    void onProgramLoaded(const SimulatorEvent& event);
    void onSimulationStop(const SimulatorEvent& event);
    void createContents();
    void onZoom(wxCommandEvent& event);
    void setUtilizationHighlights();
    void addMoves();
    void onToggleMoves(wxCommandEvent& event);
    void onToggleUtilizations(wxCommandEvent& event);
    void onToggleUnitInfo(wxCommandEvent& event);
    void onUpdateUIEvent(wxUpdateUIEvent& event);
    void onExport(wxCommandEvent& event);

    /// Machine visualization canvas.
    MachineCanvas* canvas_;
    /// Simulator instance which contains the registers to display.
    SimulatorFrontend* simulator_;
    /// Toplevel sizer for the window widgets.
    wxBoxSizer* sizer_;
    /// Statusbar of the window.
    wxStatusBar* statusbar_;

    // Widget IDs.
    enum {
	ID_MACHINE_CANVAS = 10000,
        ID_UTILIZATION_WINDOW,
        ID_TITLE,
        ID_DETAILS,
        ID_LINE,
        ID_LABEL_UTILIZATION,
        ID_UTILIZATION,
        ID_SPLITTER
    };

    /// Tells if the moves are displayed or not.
    bool showMoves_;
    /// Tells if the machine part utilizations are displayed or not.
    bool showUtilizations_;
    /// Utilziation window where to display utilization data of the components.
    //UtilizationWindow* utilizationWindow_;

    wxTextCtrl* detailsCtrl_;
    wxTextCtrl* utilizationCtrl_;

    /// Minimum width for the splitter window panes.
    static const int MINIMUM_PANE_WIDTH;
    /// Initial width for the component detail pane.
    static const int INITIAL_DETAILS_PANE_WIDTH;

    DECLARE_EVENT_TABLE()
};

#endif
