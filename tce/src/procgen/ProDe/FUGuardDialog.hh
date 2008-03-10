/**
 * @file FUGuardDialog.hh
 *
 * Declaration of FUGuardDualog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_FU_GUARD_DIALOG_HH
#define TTA_FU_GUARD_DIALOG_HH

#include <wx/wx.h>

namespace TTAMachine {
    class Bus;
    class PortGuard;
    class FunctionUnit;
    class FUPort;
}

/**
 * Dialog for editing function unit port guard properties.
 */
class FUGuardDialog : public wxDialog {
public:
    FUGuardDialog(
        wxWindow* parent,
        TTAMachine::Bus* bus,
        TTAMachine::PortGuard* guard = NULL);
    virtual ~FUGuardDialog();
    virtual bool TransferDataToWindow();

private:
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    void onFUChoice(wxCommandEvent& event);
    void onOK(wxCommandEvent& event);
    void onCancel(wxCommandEvent& event);
    TTAMachine::FunctionUnit* selectedFU() const;
    TTAMachine::FUPort* selectedPort() const;
    void setTexts();

    /// Port guard to edit.
    TTAMachine::PortGuard* guard_;
    /// Choice widget for the function unit selection.
    wxChoice* nameChoice_;
    /// Choice widget for the port selection.
    wxChoice* portChoice_;
    /// Checkbox widget for the inverted flag.
    wxCheckBox* invertedBox_;

    /// Inverted flag for the guard.
    bool inverted_;
    /// Modified inverted flag.
    bool newInverted_;
    /// Port of the guard.
    TTAMachine::FUPort* port_;
    /// Parent bus of the guard.
    TTAMachine::Bus* bus_;
    /// True if a new guard is being added, false otherwise.
    bool adding_;

    // IDs for the dialog widgets.
    enum {
        ID_LABEL_NAME = 10000,
        ID_LABEL_PORT,
        ID_FU_NAME,
        ID_FU_PORT,
        ID_INVERTED,
        ID_HELP,
        ID_LINE
    };

    DECLARE_EVENT_TABLE()
};
#endif
