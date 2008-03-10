/**
 * @file ValidateMachineDialog.hh
 *
 * Declaration of ValidateMachineDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_VALIDATE_MACHINE_DIALOG_HH
#define TTA_VALIDATE_MACHINE_DIALOG_HH

#include <wx/wx.h>

class wxHtmlWindow;

namespace TTAMachine {
    class Machine;
}

/**
 * Dialog for listing and editing address spaces of the machine.
 */
class ValidateMachineDialog : public wxDialog {
public:
    ValidateMachineDialog(
        wxWindow* parent, const TTAMachine::Machine& machine);
    ~ValidateMachineDialog();

private:
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    void onValidate(wxCommandEvent& event);
    void onClose(wxCommandEvent& event);
    void onCheck(wxCommandEvent& event);

    /// Machine to be validated.
    const TTAMachine::Machine& machine_;
    /// HTML-widget for validation result texts.
    wxHtmlWindow* resultsWindow_;

    wxCheckBox* checkAnsiC_;
    wxCheckBox* checkGlobalConnReg_;

    // enumerated ids for dialog controls
    enum {
        ID_VALIDATE = 10000,
        ID_CLOSE,
        ID_RESULTS,
        ID_LINE,
        ID_CHECK_ANSI_C,
        ID_CHECK_GLOBAL_CONN_REGISTER
    };

    DECLARE_EVENT_TABLE()
};
#endif
