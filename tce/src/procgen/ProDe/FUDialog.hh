/**
 * @file FUDialog.hh
 *
 * Declaration of FUDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_FU_DIALOG_HH
#define TTA_FU_DIALOG_HH

#include <string>
#include <wx/wx.h>
#include <wx/listctrl.h>

namespace TTAMachine {
    class FunctionUnit;
    class FUPort;
    class HWOperation;
}

/**
 * Dialog for querying FunctionUnit parameters from the user.
 */
class FUDialog : public wxDialog {
public:
    FUDialog(wxWindow* parent, TTAMachine::FunctionUnit* functionUnit);
    virtual ~FUDialog();

protected:
    virtual bool TransferDataToWindow();
private:
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);

    TTAMachine::FUPort* selectedPort();
    TTAMachine::HWOperation* selectedOperation();
    void onOK(wxCommandEvent& event);
    void onHelp(wxCommandEvent& event);
    void onName(wxCommandEvent& event);
    void onPortSelection(wxListEvent& event);
    void onPortRightClick(wxListEvent& event);
    void onAddPort(wxCommandEvent& event);
    void onDeletePort(wxCommandEvent& event);
    void onEditPort(wxCommandEvent& event);
    void onActivatePort(wxListEvent& event);
    void onOperationSelection(wxListEvent& event);
    void onAddOperation(wxCommandEvent& event);
    void onAddOperationFromOpset(wxCommandEvent& event);
    void onDeleteOperation(wxCommandEvent& event);
    void onEditOperation(wxCommandEvent& event);
    void onSetAddressSpace(wxCommandEvent& event);
    void onActivateOperation(wxListEvent& event);
    void onOperationRightClick(wxListEvent& event);
    void updateAddressSpaceChoice();
    void updatePortList();
    void updateOperationList();
    void setTexts();

    /// FunctionUnit to modify.
    TTAMachine::FunctionUnit* functionUnit_;
    /// Name of the function unit.
    wxString name_;

    /// Port list control.
    wxListCtrl* portListCtrl_;
    /// Operation list control.
    wxListCtrl* operationListCtrl_;
    /// Address Space choice control.
    wxChoice* addressSpaceChoice_;

    /// Sizer for the port list controls.
    wxStaticBoxSizer* portsSizer_;
    /// Sizer for the operation list controls.
    wxStaticBoxSizer* operationsSizer_;

    enum {
	ID_NAME=10000,
	ID_ADDRESS_SPACE,
	ID_OPERATION_LIST,
	ID_ADD_OPERATION,
	ID_EDIT_OPERATION,
        ID_ADD_OPERATION_FROM_OPSET,
	ID_DELETE_OPERATION,
        ID_PORT_LIST,
	ID_ADD_PORT,
	ID_EDIT_PORT,
	ID_DELETE_PORT,
	ID_HELP,
        ID_LABEL_NAME,
        ID_LABEL_AS
    };

    /// The event table for the controls of the dialog.
    DECLARE_EVENT_TABLE()
};
#endif
