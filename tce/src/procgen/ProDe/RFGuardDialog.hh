/**
 * @file RFGuardDialog.hh
 *
 * Declaration of RFGuardDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_RF_GUARD_DIALOG_HH
#define TTA_RF_GUARD_DIALOG_HH

#include <wx/wx.h>

namespace TTAMachine {
    class Bus;
    class RegisterGuard;
    class RegisterFile;
}

/**
 * Dialog for editing register file guard properties.
 */
class RFGuardDialog : public wxDialog {
public:
    RFGuardDialog(
        wxWindow* parent,
        TTAMachine::Bus* bus,
        TTAMachine::RegisterGuard* guard = NULL);
    virtual ~RFGuardDialog();
    virtual bool TransferDataToWindow();

private:
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    void onRFChoice(wxCommandEvent& event);
    void onOK(wxCommandEvent& event);
    void onCancel(wxCommandEvent& event);
    TTAMachine::RegisterFile* selectedRF() const;
    void setTexts();

    /// RegisterGuard to edit.
    TTAMachine::RegisterGuard* guard_;
    /// Choice widget for register file name selection.
    wxChoice* nameChoice_;
    /// Choice widget for register index selection.
    wxChoice* indexChoice_;
    /// Checkbox widget for the inverted flag.
    wxCheckBox* invertedBox_;

    /// Inverted flag of the guard.
    bool inverted_;
    /// Modified inverted flag of the guard.
    bool newInverted_;
    /// Register index of the guard.
    int index_;
    /// Modified register index of the guard.
    int newIndex_;
    /// Guard register.
    TTAMachine::RegisterFile* rf_;
    /// Parent bus of the register guard.
    TTAMachine::Bus* bus_;
    /// True if a new guard is being added, false otherwise.
    bool adding_;

    // IDs for dialog widgets
    enum {
        ID_LABEL_NAME = 10000,
        ID_RF_NAME,
        ID_LABEL_INDEX,
        ID_RF_INDEX,
        ID_INVERTED,
        ID_HELP,
        ID_LINE
    };

    DECLARE_EVENT_TABLE()
};
#endif
