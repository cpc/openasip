/**
 * @file WatchPropertiesDialog.hh
 *
 * Declaration of WatchPropertiesDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_WATCH_PROPERTIES_DIALOG_HH
#define TTA_WATCH_PROPERTIES_DIALOG_HH

#include <wx/wx.h>

class StopPointManager;

/**
 * Dialog for modifying watchpoint properties.
 */
class WatchPropertiesDialog : public wxDialog {
public:
    WatchPropertiesDialog(
        wxWindow* parent, wxWindowID id,
        StopPointManager& manager, int handle);
    ~WatchPropertiesDialog();
private:
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool call_sizer);
    virtual bool TransferDataToWindow();
    void onOK(wxCommandEvent& event);

    /// Widget IDs.
    enum {
        ID_LABEL_WATCH = 10000,
        ID_LABEL_EXPRESSION,
        ID_LABEL_CONDITION,
        ID_LABEL_IGNORE_COUNT,

        ID_WATCH_HANDLE,
        ID_EXPRESSION,
        ID_CONDITION,
        ID_IGNORE_COUNT,
        ID_LINE
    };

    /// Stop point manager of the simulator.
    StopPointManager& manager_;
    /// Handle of the watch to modify.
    int handle_;

    /// Text widget for the watch expression script.
    wxTextCtrl* expressionCtrl_;
    /// Text widget fot the watch condition script.
    wxTextCtrl* conditionCtrl_;
    /// Spin button widget for the watch ignore count.
    wxSpinCtrl* ignoreCtrl_;

    DECLARE_EVENT_TABLE()
};
#endif
