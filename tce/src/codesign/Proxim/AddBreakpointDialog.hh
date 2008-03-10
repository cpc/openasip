/**
 * @file AddBreakpointDialog.hh
 *
 * Declaration of AddBreakpointDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_ADD_BREAKPOINT_DIALOG_HH
#define TTA_ADD_BREAKPOINT_DIALOG_HH

#include <wx/wx.h>

/**
 * Dialog for adding new breakpoints.
 */
class AddBreakpointDialog : public wxDialog {
public:
    AddBreakpointDialog(wxWindow* parent, wxWindowID id);
    virtual ~AddBreakpointDialog();
private:
    void onOK(wxCommandEvent& event);
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);

    /// Widget IDs
    enum {
        ID_LABEL_NEW_BP,
        ID_ADDRESS,
        ID_LINE
    };

    DECLARE_EVENT_TABLE()
};
#endif
