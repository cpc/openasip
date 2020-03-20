/*
    Copyright (c) 2002-2016 Tampere University.

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
    void removeSubWindow(ProximSimulatorWindow* window);
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
