/**
 * @file src/codesign/osal/OSEd/OSEdOptionsDialog.hh
 *
 * Declaration of OSEdOptionsDialog class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_OSED_OPTIONS_DIALOG_HH
#define TTA_OSED_OPTIONS_DIALOG_HH

#include <wx/wx.h>

/**
 * Dialog for modifying OSEd options.
 */
class OSEdOptionsDialog : public wxDialog {
public:
    OSEdOptionsDialog(wxWindow* parent);
    virtual ~OSEdOptionsDialog();

private:
    /// Copying not allowed.
    OSEdOptionsDialog(const OSEdOptionsDialog&);
    /// Assignment not allowed.
    OSEdOptionsDialog& operator=(const OSEdOptionsDialog&);

    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    void setTexts();
    bool TransferDataToWindow();
    void onSave(wxCommandEvent& event);
    void onBrowse(wxCommandEvent& event);

    /**
     * Widget ids.
     */
    enum {
        ID_TEXT = 1000000,
        ID_EDITOR,
        ID_BUTTON_SAVE,
        ID_BUTTON_BROWSE
    };

    /// The name of the editor.
    wxString editor_;

    DECLARE_EVENT_TABLE()
};

#endif
