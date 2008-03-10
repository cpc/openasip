/**
 * @file ProximBreakpointWindow.cc
 *
 * Implementation of ProximBreakpointWindow class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <wx/statline.h>
#include <wx/spinctrl.h>
#include <wx/notebook.h>
#include <wx/listctrl.h>

#include "ProximBreakpointWindow.hh"
#include "Application.hh"
#include "StopPointManager.hh"
#include "StopPoint.hh"
#include "Breakpoint.hh"
#include "Watch.hh"
#include "Proxim.hh"
#include "ProximSimulationThread.hh"
#include "TracedSimulatorFrontend.hh"
#include "WxConversion.hh"
#include "SimulatorEvent.hh"
#include "ProximToolbox.hh"
#include "ProximDisassemblyWindow.hh"
#include "BreakpointPropertiesDialog.hh"
#include "AddBreakpointDialog.hh"
#include "AddWatchDialog.hh"
#include "ErrorDialog.hh"
#include "WatchPropertiesDialog.hh"

BEGIN_EVENT_TABLE(ProximBreakpointWindow, ProximSimulatorWindow)
    EVT_BUTTON(ID_CLOSE, ProximBreakpointWindow::onClose)
    EVT_BUTTON(ID_DELETE_BREAKPOINT, ProximBreakpointWindow::onDeleteBreakpoint)
    EVT_BUTTON(ID_DELETE_WATCH, ProximBreakpointWindow::onDeleteWatch)
    EVT_BUTTON(ID_WATCH_PROPERTIES, ProximBreakpointWindow::onWatchProperties)
    EVT_BUTTON(ID_LOOKUP_BREAKPOINT, ProximBreakpointWindow::onBreakpointLookup)
    EVT_BUTTON(ID_BREAKPOINT_PROPERTIES, ProximBreakpointWindow::onBreakpointProperties)
    EVT_BUTTON(ID_ADD_BREAKPOINT, ProximBreakpointWindow::onAddBreakpoint)
    EVT_BUTTON(ID_ADD_WATCH, ProximBreakpointWindow::onAddWatch)
    EVT_SIMULATOR_COMMAND_DONE(0, ProximBreakpointWindow::onBreakpointsModified)
    EVT_LIST_ITEM_SELECTED(ID_WATCH_LIST, ProximBreakpointWindow::onBreakpointSelection)
    EVT_LIST_ITEM_DESELECTED(ID_WATCH_LIST, ProximBreakpointWindow::onBreakpointSelection)
    EVT_LIST_ITEM_SELECTED(ID_BREAKPOINT_LIST, ProximBreakpointWindow::onBreakpointSelection)
    EVT_LIST_ITEM_DESELECTED(ID_BREAKPOINT_LIST, ProximBreakpointWindow::onBreakpointSelection)
END_EVENT_TABLE()


/**
 * The Constructor.
 *
 * @param parent Parent window of the frame.
 * @param id Window ID.
 */
ProximBreakpointWindow::ProximBreakpointWindow(
    ProximMainFrame* parent, wxWindowID id):
    ProximSimulatorWindow(parent, id),
    breakpointList_(NULL),
    watchList_(NULL) {

    createContents(this, true, true);

    breakpointList_->InsertColumn(0, _T("Handle"), wxLIST_FORMAT_LEFT, 100);
    breakpointList_->InsertColumn(1, _T("Enabled"), wxLIST_FORMAT_CENTER, 80);
    breakpointList_->InsertColumn(2, _T("Address"), wxLIST_FORMAT_LEFT, 300);

    watchList_->InsertColumn(0, _T("Handle"), wxLIST_FORMAT_LEFT, 100);
    watchList_->InsertColumn(1, _T("Enabled"), wxLIST_FORMAT_CENTER, 80);
    watchList_->InsertColumn(2, _T("Expression"), wxLIST_FORMAT_LEFT, 300);

   SimulatorFrontend* simulation = ProximToolbox::frontend();

    if (simulation->isSimulationInitialized() ||
        simulation->isSimulationRunning() ||
        simulation->isSimulationStopped() ||
        simulation->hasSimulationEnded()) {

        refreshStopPoints();
    }

    // Disable conditional buttons initially.
    FindWindow(ID_DELETE_BREAKPOINT)->Disable();
    FindWindow(ID_LOOKUP_BREAKPOINT)->Disable();
    FindWindow(ID_BREAKPOINT_PROPERTIES)->Disable();
    FindWindow(ID_DELETE_WATCH)->Disable();        
    FindWindow(ID_BREAKPOINT_ENABLED)->Disable();
    FindWindow(ID_WATCH_ENABLED)->Disable();
    FindWindow(ID_WATCH_PROPERTIES)->Disable();
}

/**
 * The Destructor.
 */
ProximBreakpointWindow::~ProximBreakpointWindow() {
}

/**
 * Event handler for the close button.
 *
 * Closes the window.
 */
void
ProximBreakpointWindow::onClose(wxCommandEvent&) {
    GetParent()->Close();
}


/**
 * Opens dialog displaying properties of the selected breakpoint when the
 * Properties-button is pressed.
 */
void
ProximBreakpointWindow::onBreakpointProperties(wxCommandEvent&) {

    long item = breakpointList_->GetNextItem(
        -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    
    if (item == -1) {
        return;
    }

    StopPointManager& manager =
        wxGetApp().simulation()->frontend()->stopPointManager();

    int handle = bpListItemHandle_[item];

    BreakpointPropertiesDialog dialog(this, manager, handle);
    dialog.ShowModal();

    refreshStopPoints();
}

/**
 * Called when the simulator resets program, machine or memory model.
 */
void
ProximBreakpointWindow::reset() {
    refreshStopPoints();
}

/**
 * Opens dialog displaying properties of the selected watch when the
 * Properties-button is pressed in the watches tab.
 */
void
ProximBreakpointWindow::onWatchProperties(wxCommandEvent&) {

    long item = watchList_->GetNextItem(
        -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    
    if (item == -1) {
        return;
    }

    StopPointManager& manager =
        wxGetApp().simulation()->frontend()->stopPointManager();

    int handle = watchListItemHandle_[item];

    WatchPropertiesDialog dialog(this, -1, manager, handle);
    dialog.ShowModal();

    refreshStopPoints();
}

/**
 * Updates the lists of breakpoints and watches.
 */
void
ProximBreakpointWindow::refreshStopPoints() {

    breakpointList_->DeleteAllItems();
    watchList_->DeleteAllItems();

    // Clear "list item to handle" mapping.
    bpListItemHandle_.clear();
    watchListItemHandle_.clear();

    SimulatorFrontend* frontend = ProximToolbox::frontend();

    if (frontend == NULL ||
        !(frontend->isSimulationInitialized() ||
          frontend->isSimulationRunning() ||
          frontend->isSimulationStopped() ||
          frontend->hasSimulationEnded())) {

        // Simulation not initiailized.
        return;
    }

    StopPointManager& manager = frontend->stopPointManager();

    int bpRow = 0;
    int watchRow = 0;


    // Add all stoppoints to appropriate lists.
    for (unsigned i = 0; i < manager.stopPointCount(); i++) {

        unsigned handle = manager.stopPointHandle(i);
        const StopPoint* stoppoint = 
            &manager.stopPointWithHandleConst(handle);


        // Check if the stoppoint is breakpoint.
        const Breakpoint* breakpoint =
            dynamic_cast<const Breakpoint*>(stoppoint);

        if (breakpoint != NULL) {
            // Stoppoint is breakpoint.
            bpListItemHandle_.insert(
                std::pair<unsigned, unsigned>(bpRow, handle));
            breakpointList_->InsertItem(
                bpRow, WxConversion::toWxString(handle));

            if (breakpoint->isEnabled()) {
                breakpointList_->SetItem(bpRow, 1, _T("X"));

            }
            InstructionAddress address = breakpoint->address();
            breakpointList_->SetItem(
                bpRow, 2, WxConversion::toWxString(address));
            bpRow++;
            continue;
        }


        // Check if the stoppoint is watch.
        const Watch* watch = dynamic_cast<const Watch*>(stoppoint);

        if (watch != NULL) {
            // Stoppoint is watch.
            watchListItemHandle_.insert(
                std::pair<unsigned, unsigned>(watchRow, handle));
            watchList_->InsertItem(0, WxConversion::toWxString(handle));
            if (watch->isEnabled()) {
                watchList_->SetItem(bpRow, 1, _T("X"));
            }
            std::string expression = watch->expression().script()[0];
            watchList_->SetItem(0, 2, WxConversion::toWxString(expression));
            watchRow++;
            continue;
        }
        
    }

    // Update enabled/disabled state of the dialog buttons.
    wxListEvent dummy;
    onBreakpointSelection(dummy);
}


/**
 * Deletes the breakpoint selected in the breakpoint list.
 */
void
ProximBreakpointWindow::onDeleteBreakpoint(wxCommandEvent&) {

    long item = breakpointList_->GetNextItem(
        -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    
    if (item == -1) {
        return;
    }

    StopPointManager& manager =
        wxGetApp().simulation()->frontend()->stopPointManager();

    int handle = bpListItemHandle_[item];
    manager.deleteStopPoint(handle);
    refreshStopPoints();
}


/**
 * Enables and disables delete buttons for stop points according to
 * stop point list selections.
 */
void
ProximBreakpointWindow::onBreakpointSelection(wxListEvent&) {

    if (breakpointList_->GetSelectedItemCount() > 0) {
        FindWindow(ID_DELETE_BREAKPOINT)->Enable();
        FindWindow(ID_LOOKUP_BREAKPOINT)->Enable();
        FindWindow(ID_BREAKPOINT_PROPERTIES)->Enable();
    } else {
        FindWindow(ID_DELETE_BREAKPOINT)->Disable();
        FindWindow(ID_LOOKUP_BREAKPOINT)->Disable();
        FindWindow(ID_BREAKPOINT_PROPERTIES)->Disable();
    }

    if (watchList_->GetSelectedItemCount() > 0) {
        FindWindow(ID_DELETE_WATCH)->Enable();
        FindWindow(ID_WATCH_PROPERTIES)->Enable();
    } else {
        FindWindow(ID_DELETE_WATCH)->Disable();
        FindWindow(ID_WATCH_PROPERTIES)->Disable();
    }
}


/**
 * Refreshes the breakpoint list when breakpoints are modified.
 *
 * @param event Event to handle.
 */
void
ProximBreakpointWindow::onBreakpointsModified(SimulatorEvent& event) {
    refreshStopPoints();
    // Skip event so it's passed to the parent class.
    event.Skip();
}


/**
 * Deletes the watch selected in the watch list.
 */
void
ProximBreakpointWindow::onDeleteWatch(wxCommandEvent&) {

    long item = watchList_->GetNextItem(
        -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    
    if (item == -1) {
        return;
    }

    StopPointManager& manager =
        wxGetApp().simulation()->frontend()->stopPointManager();

    int handle = watchListItemHandle_[item];
    manager.deleteStopPoint(handle);
    refreshStopPoints();
}

/**
 * Shows the selected breakpoint location in the program disassembly window.
 */
void
ProximBreakpointWindow::onBreakpointLookup(wxCommandEvent&) {

    long item = breakpointList_->GetNextItem(
        -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    
    if (item == -1) {
        return;
    }

    StopPointManager& manager =
        wxGetApp().simulation()->frontend()->stopPointManager();

    int handle = bpListItemHandle_[item];    
    const Breakpoint& breakpoint = dynamic_cast<const Breakpoint&>(
        manager.stopPointWithHandleConst(handle));
    unsigned address = breakpoint.address();
    ProximToolbox::disassemblyWindow()->showAddress(address);
}

/**
 * Opens a dialog for adding a new breakpoint.
 */
void
ProximBreakpointWindow::onAddBreakpoint(wxCommandEvent&) {
    
    SimulatorFrontend* simulation = ProximToolbox::frontend();

    if (simulation->isSimulationInitialized() ||
        simulation->isSimulationRunning() ||
        simulation->isSimulationStopped()) {

        AddBreakpointDialog dialog(this, -1);
        dialog.ShowModal();
    } else {
        wxString message = _T("Simulation not initialized.");
        ErrorDialog error(this, message);
        error.ShowModal();
    }
}

/**
 * Opens a dialog for adding a new watch.
 */
void
ProximBreakpointWindow::onAddWatch(wxCommandEvent&) {

    SimulatorFrontend* simulation = ProximToolbox::frontend();
    
    if (simulation->isSimulationInitialized() ||
        simulation->isSimulationRunning() ||
        simulation->isSimulationStopped()) {

        AddWatchDialog dialog(this, -1);
        if (dialog.ShowModal() == wxID_OK) {
            refreshStopPoints();
        }

    } else {
        wxString message = _T("Simulation not initialized.");
        ErrorDialog error(this, message);
        error.ShowModal();
    }
}


/**
 * Creates the window contents.
 */
wxSizer*
ProximBreakpointWindow::createContents(
    wxWindow *parent, bool call_fit, bool set_sizer) {

    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxNotebook *item2 = new wxNotebook( parent, ID_NOTEBOOK, wxDefaultPosition, wxSize(200,160), 0 );
#if !wxCHECK_VERSION(2,5,2)
    wxNotebookSizer *item1 = new wxNotebookSizer( item2 );
#else
    wxWindow *item1 = item2;
#endif

    wxPanel *item3 = new wxPanel( item2, -1 );
    createBreakpointTab( item3, FALSE );
    item2->AddPage( item3, wxT("Breakpoints") );

    wxPanel *item4 = new wxPanel( item2, -1 );
    createWatchTab( item4, FALSE );
    item2->AddPage( item4, wxT("Watches") );

    item0->Add( item1, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticLine *item5 = new wxStaticLine( parent, ID_LINE, wxDefaultPosition, wxSize(20,-1), wxLI_HORIZONTAL );
    item0->Add( item5, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxGridSizer *item6 = new wxGridSizer( 2, 0, 0 );

    wxButton *item7 = new wxButton( parent, ID_HELP, wxT("&Help"), wxDefaultPosition, wxDefaultSize, 0 );
    item6->Add( item7, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxButton *item8 = new wxButton( parent, ID_CLOSE, wxT("&Close"), wxDefaultPosition, wxDefaultSize, 0 );
    item6->Add( item8, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item0->Add( item6, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;
}

/**
 * Creates the breakpoint tab.
 */
wxSizer*
ProximBreakpointWindow::createBreakpointTab(
    wxWindow *parent, bool call_fit, bool set_sizer) {

    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *item1 = new wxBoxSizer( wxHORIZONTAL );

    wxBoxSizer *item2 = new wxBoxSizer( wxVERTICAL );

    wxButton *item3 = new wxButton( parent, ID_BREAKPOINT_PROPERTIES, wxT("Properties..."), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item3, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item4 = new wxButton( parent, ID_LOOKUP_BREAKPOINT, wxT("Lookup"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item4, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item5 = new wxButton( parent, ID_DELETE_BREAKPOINT, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item5, 0, wxALIGN_CENTER|wxALL, 5 );

    wxCheckBox *item6 = new wxCheckBox( parent, ID_BREAKPOINT_ENABLED, wxT("Enabled"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item6, 0, wxALIGN_CENTER|wxALL, 5 );

    item1->Add( item2, 0, wxALIGN_BOTTOM|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    breakpointList_ = new wxListCtrl( parent, ID_BREAKPOINT_LIST, wxDefaultPosition, wxSize(400,300), wxLC_REPORT|wxSUNKEN_BORDER );
    item1->Add( breakpointList_, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item1, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item8 = new wxButton( parent, ID_ADD_BREAKPOINT, wxT("Add breakpoint..."), wxDefaultPosition, wxDefaultSize, 0 );
    item0->Add( item8, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;
}

/**
 * Creates the watches tab.
 */
wxSizer*
ProximBreakpointWindow::createWatchTab(
    wxWindow *parent, bool call_fit, bool set_sizer) {

    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *item1 = new wxBoxSizer( wxHORIZONTAL );

    wxBoxSizer *item2 = new wxBoxSizer( wxVERTICAL );

    wxButton *item3 = new wxButton( parent, ID_WATCH_PROPERTIES, wxT("Properties..."), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item3, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item4 = new wxButton( parent, ID_DELETE_WATCH, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item4, 0, wxALIGN_CENTER|wxALL, 5 );

    wxCheckBox *item5 = new wxCheckBox( parent, ID_WATCH_ENABLED, wxT("Enabled"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item5, 0, wxALIGN_CENTER|wxALL, 5 );

    item1->Add( item2, 0, wxALIGN_BOTTOM|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    watchList_ = new wxListCtrl( parent, ID_WATCH_LIST, wxDefaultPosition, wxSize(400,300), wxLC_REPORT|wxSUNKEN_BORDER );
    item1->Add( watchList_, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item1, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item7 = new wxButton( parent, ID_ADD_WATCH, wxT("Add watch..."), wxDefaultPosition, wxDefaultSize, 0 );
    item0->Add( item7, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;
}

