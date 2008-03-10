/**
 * @file FocusTrackingTextCtrl.cc
 *
 * Implementation of FocusTrackingTextCtrl class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#include "FocusTrackingTextCtrl.hh"

BEGIN_EVENT_TABLE(FocusTrackingTextCtrl, wxTextCtrl)
    EVT_KILL_FOCUS(FocusTrackingTextCtrl::onKillFocus)
END_EVENT_TABLE()

/**
 * The Constructor.
 *
 * @param parent Parent window of the control.
 * @param id Numeric id for the control.
 * @param value Initial value of the text field.
 * @param pos Position of the control.
 * @param size Size of the control.
 * @param style Style flags for the text control.
 * @param validator Validator for the control.
 * @param name Name of the control.
 */
FocusTrackingTextCtrl::FocusTrackingTextCtrl(
    wxWindow* parent,
    wxWindowID id,
    const wxString& value,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxValidator& validator,
    const wxString& name) :
    wxTextCtrl(parent, id, value, pos, size,
               (style | wxTE_PROCESS_ENTER) , validator, name) {

}

/**
 * The Destructor.
 */
FocusTrackingTextCtrl::~FocusTrackingTextCtrl() {
}

/**
 * Emits a wxEVT_COMMAND_TEXT_ENTER event when the control loses focus.
 *
 * @param event Focus event of the control losing focus.
 */
void
FocusTrackingTextCtrl::onKillFocus(wxFocusEvent& event) {
    wxCommandEvent textEntered =
        wxCommandEvent(wxEVT_COMMAND_TEXT_ENTER, GetId());
    AddPendingEvent(textEntered);
    event.Skip();
}
