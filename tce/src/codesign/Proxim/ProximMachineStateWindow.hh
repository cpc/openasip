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
 * @file ProximMachineStateWindow.cc
 *
 * Declaration of ProximMachineStateWindow class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
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

    static const double ZOOM_STEP;
    static const double MIN_ZOOM_FACTOR;
    static const double MAX_ZOOM_FACTOR;

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
