/**
 * @file RFDialog.hh
 *
 * Declaration of RFDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_RF_DIALOG_HH
#define TTA_RF_DIALOG_HH

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/spinctrl.h>

namespace TTAMachine {
    class RegisterFile;
    class Port;
}

/**
 * Dialog for editing register file parameters.
 */
class RFDialog : public wxDialog {
public:
    RFDialog(wxWindow* parent, TTAMachine::RegisterFile* registerFile);
    virtual ~RFDialog();

private:
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    virtual bool TransferDataToWindow();
    TTAMachine::Port* selectedPort() const;
    void onOK(wxCommandEvent& event);
    void onHelp(wxCommandEvent& event);
    void onName(wxCommandEvent& event);
    void onPortSelection(wxListEvent& event);
    void onPortRightClick(wxListEvent& event);
    void onAddPort(wxCommandEvent& event);
    void onActivatePort(wxListEvent& event);
    void onEditPort(wxCommandEvent& event);
    void onDeletePort(wxCommandEvent& event);
    void updatePortList();
    void setTexts();

    /// Sizer containing the port list and associated buttons.
    wxStaticBoxSizer* portsSizer_;

    /// Register file to modify.
    TTAMachine::RegisterFile* registerFile_;
    /// Name of the register file.
    wxString name_;
    /// Number of registers.
    int size_;
    /// Width.
    int width_;
    /// Max reads.
    int maxReads_;
    /// Maximum writes.
    int maxWrites_;
    /// Guard latency.
    int guardLatency_;

    /// Port list control.
    wxListCtrl* portListCtrl_;
    /// Opcode choice control.
    wxChoice* typeChoice_;

    /// enumerated IDs for the dialog controls
    enum {
        ID_NAME = 10000,
        ID_TYPE,
        ID_SIZE,
        ID_WIDTH,
        ID_MAX_READS,
        ID_MAX_WRITES,
        ID_GUARD_LATENCY,
        ID_PORT_LIST,
        ID_ADD_PORT,
        ID_EDIT_PORT,
        ID_DELETE_PORT,
        ID_HELP,
        ID_LABEL_NAME,
        ID_LABEL_SIZE,
        ID_LABEL_WIDTH,
        ID_LABEL_TYPE,
        ID_LABEL_MAX_READS,
        ID_LABEL_MAX_WRITES,
        ID_LABEL_GUARD_LATENCY
    };

    /// The event table for the controls of the dialog.
    DECLARE_EVENT_TABLE()
};
#endif
