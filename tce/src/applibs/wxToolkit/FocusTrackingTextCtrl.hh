/**
 * @file FocusTrackingTextCtrl.hh
 *
 * Declaration of FocusTrackingTextCtrl class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_FOCUS_TRACKING_TEXT_CTRL_HH
#define TTA_FOCUS_TRACKING_TEXT_CTRL_HH

#include <wx/wx.h>

/**
 * Textfield widget which emits a wxEVT_COMMAND_TEXT_ENTER when losing
 * keyboard focus.
 *
 * This widget behaves exactly like wxTextCtrl except for the event emited
 * when focus is lost. This widget is useful for textfields which are
 * validated when the user presses enter on the widget or changes the
 * keyboard focus to another window. The created text widget has always
 * the wxTE_PROCESS_ENTER style set.
 */
class FocusTrackingTextCtrl : public wxTextCtrl {
public:
    FocusTrackingTextCtrl(
        wxWindow* parent,
        wxWindowID id,
        const wxString& value = _T(""),
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = _T("FocusTrackingTextCtrl"));
    ~FocusTrackingTextCtrl();
private:
    void onKillFocus(wxFocusEvent& event);

    DECLARE_EVENT_TABLE()
};

#endif
