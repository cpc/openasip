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
