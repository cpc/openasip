/**
 * @file FUPortImplementationDialog.hh
 *
 * Declaration of FUPortImplementationDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_FU_PORT_IMPLEMENTATION_DIALOG_HH
#define TTA_FU_PORT_IMPLEMENTATION_DIALOG_HH

#include <wx/wx.h>

namespace HDB {
    class FUPortImplementation;
}

namespace TTAMachine {
    class BaseFUPort;
}

/**
 * Dialog for editing FU port implementations.
 */
class FUPortImplementationDialog : public wxDialog {
public:
    FUPortImplementationDialog(
        wxWindow* parent, wxWindowID id,
        HDB::FUPortImplementation& implementation,
        const TTAMachine::BaseFUPort& architecture);

    virtual ~FUPortImplementationDialog();
    virtual bool TransferDataToWindow();
private:

    void onOK(wxCommandEvent& event);

    /// Enumerated IDs for dialog widgets.
    enum {
        ID_LABEL_NAME = 10000,
        ID_NAME,
        ID_LABEL_ARCHITECTURE_PORT,
        ID_ARCHITECTURE_PORT_NAME,
        ID_LABEL_WIDTH,
        ID_WIDTH,
        ID_LOAD_PORT_LABEL,
        ID_LOAD_PORT,
        ID_GUARD_PORT_LABEL,
        ID_GUARD_PORT,
        ID_OPERAND_LIST,
        ID_LINE
    };

    /// Creates the dialog contents.
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);

    /// FU port implementation to modify.
    HDB::FUPortImplementation& implementation_;
    /// Architecture of the modified port.
    const TTAMachine::BaseFUPort& architecture_;

    /// Name of the port.
    wxString name_;
    /// Name of the load port.
    wxString loadPortName_;
    /// Name of the guard port.
    wxString guardPortName_;
    /// Width formula string.
    wxString widthFormula_;

    /// List widget for the operand bindings.
    wxListCtrl* operandList_;

    DECLARE_EVENT_TABLE()
};

#endif
