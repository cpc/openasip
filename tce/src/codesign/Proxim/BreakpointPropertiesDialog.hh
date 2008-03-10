/**
 * @file BreakpointPropertiesDialog.hh
 *
 * Declaration of BreakpointPropertiesDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_BREAKPOINT_PROPERTIES_DIALOG_HH
#define TTA_BREAKPOINT_PROPERTIES_DIALOG_HH

#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <wx/statline.h>

class StopPointManager;

/**
 * Dialog for editing breakpoint properties in Proxim.
 */
class BreakpointPropertiesDialog : public wxDialog {
public:
    BreakpointPropertiesDialog(
        wxWindow* parent, StopPointManager& manager, unsigned int handle);

    virtual ~BreakpointPropertiesDialog();
private:
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    virtual bool TransferDataToWindow();
    void onOK(wxCommandEvent& event);
    /// Widget IDs.
    enum {
        ID_TEXT_BREAKPOINT = 10000,
        ID_TEXT_BP_ID,
        ID_LABEL_CONDITION,
        ID_CONDITION,
        ID_LABEL_IGNORE_COUNT,
        ID_IGNORE_CTRL,
        ID_LINE,
        ID_HELP,
        ID_CLOSE
    };
    /// Stoppoint manager containing the breakpoint,
    StopPointManager& manager_;
    /// Handle of the breakpoint to modify.
    unsigned int handle_;

    DECLARE_EVENT_TABLE()
};
#endif
