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
 * @file ProximMainFrame.hh
 *
 * Declaration of ProximMainFrame class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef PROXIM_MAIN_FRAME_HH
#define PROXIM_MAIN_FRAME_HH

#include <vector>
#include <wx/wx.h>
#include <wx/splitter.h>
#include "SimulatorEvent.hh"
#include "Exception.hh"


class ConsoleWindow;
class ProximSimulatorWindow;
class wxProgressDialog;

/**
 * Main window of the Proxim application.
 *
 * ProximMainFrame is the top level window of the Proxim window hierarchy.
 * This class is responsible for managing the simulation windows. All
 * simulator backend events are passed to all subwindows. This class is
 * also responsible for the menubar and executing command registry commands
 * corresponding to the menu bar item ids. All subwindows are handled as
 * wxPanels, and the simulator events are passed as custom wxEvents
 * (see SimulatorEvent class).
 */
class ProximMainFrame : public wxFrame {
public:
    ProximMainFrame(
	const wxString& title, const wxPoint& pos, const wxSize& size);

    virtual ~ProximMainFrame();

    void addSubWindow(ProximSimulatorWindow* window);
    void removeSubWindow(ProximSimulatorWindow* window)
        throw (InstanceNotFound);
    void createToolbar();
    void createMenubar();
    void reset();
private:
    void initialize();
    void dockWindow(wxWindow* window);

    void onCommandEvent(wxCommandEvent& event);
    void onToggleWindow(wxCommandEvent& event);
    void onSimulatorTerminated(SimulatorEvent& event);
    void onSimulatorEvent(SimulatorEvent& event);
    void onKeyEvent(wxKeyEvent& event);
    void onClose(wxCloseEvent& event);
    void onStatusTimer(wxTimerEvent& event);
    void onReset(const SimulatorEvent& event);
    void updateCommand(wxUpdateUIEvent& event);
    void updateToggleItem(wxUpdateUIEvent& event);
    void updateSimulationStatus();
    void updateMemoryWindowMenuItem();
    wxString menuAccelerator(int id);

    /// Toplevel sizer of the window managing the basic layout.
    wxBoxSizer* sizer_;
    /// Top level splitter window of the splitted subwindow hierarchy.
    wxWindow* topSplitter_;
    /// List of subwindows where to send the simulator events.
    std::vector<ProximSimulatorWindow*> subwindows;

    /// Mainframe toolbar.
    wxToolBar* toolbar_;
    /// True, if an instance of the stop dialog is currently visible.
    bool stopDialogInstantiated_;

    /// Mutex for the resetCondition_.
    wxMutex* resetMutex_;
    /// Condition which is signalled when the subwindow reset is complete.
    wxCondition* resetCondition_;

    /// True if the subwindow reset is complete.
    bool isReset_;

    DECLARE_EVENT_TABLE()
};
#endif
