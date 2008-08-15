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
 * @file ProximBreakpointWindow.hh
 *
 * Declaration of PorixmBreakpointWindow class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROXIM_BREAKPOINT_WINDOW
#define TTA_PROXIM_BREAKPOINT_WINDOW

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <map>
#include "ProximSimulatorWindow.hh"

class SimulatorEvent;
class StopPointManager;

/**
 * Proxim window for managing simulation breakpoints and watches.
 */
class ProximBreakpointWindow : public ProximSimulatorWindow {
public:
    ProximBreakpointWindow(ProximMainFrame* parent, wxWindowID id);
    virtual ~ProximBreakpointWindow();
    virtual void reset();

private:
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    wxSizer* createBreakpointTab(
        wxWindow* parent, bool call_fit = true, bool set_sizer = true);
    wxSizer* createWatchTab(
        wxWindow* parent, bool call_fit = true, bool set_sizer = true);
    void refreshStopPoints();
    void onClose(wxCommandEvent& event);

    void onBreakpointsModified(SimulatorEvent& event);
    void onDeleteBreakpoint(wxCommandEvent& event);
    void onDeleteWatch(wxCommandEvent& event);
    void onBreakpointLookup(wxCommandEvent& event);
    void onBreakpointProperties(wxCommandEvent& event);
    void onBreakpointSelection(wxListEvent& event);
    void onAddBreakpoint(wxCommandEvent& event);
    void onAddWatch(wxCommandEvent& event);
    void onWatchProperties(wxCommandEvent& event);

    /// List widget for breakpoints.
    wxListCtrl* breakpointList_;
    /// List widget for watches.
    wxListCtrl* watchList_;

    /// Map for translating breakpoint list item numbers to stop point handles.
    std::map<unsigned, unsigned> bpListItemHandle_;
    /// Map for translating watch list item numbers to stop point handles.
    std::map<unsigned, unsigned> watchListItemHandle_;

    /// Widget IDs.
    enum {
        ID_NOTEBOOK = 10000,
        ID_HELP,
        ID_CLOSE,
        ID_LINE,
        ID_LOOKUP_BREAKPOINT,
        ID_BREAKPOINT_PROPERTIES,
        ID_DELETE_BREAKPOINT,
        ID_ADD_BREAKPOINT,
        ID_BREAKPOINT_ENABLED,
        ID_WATCH_PROPERTIES,
        ID_DELETE_WATCH,
        ID_ADD_WATCH,
        ID_WATCH_ENABLED,
        ID_BREAKPOINT_LIST,
        ID_WATCH_LIST
    };

    DECLARE_EVENT_TABLE()
    
};
#endif
