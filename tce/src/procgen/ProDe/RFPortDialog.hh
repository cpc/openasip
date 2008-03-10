/**
 * @file RFPortDialog.hh
 *
 * Declaration of RFPortDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_RF_PORT_DIALOG_HH
#define TTA_RF_PORT_DIALOG_HH

namespace TTAMachine {
    class Port;
    class Socket;
}

/**
 * Dialog for querying register file port parameters from the user.
 */
class RFPortDialog : public wxDialog {
public:
    RFPortDialog(wxWindow* parent, TTAMachine::Port* port);
    virtual ~RFPortDialog();

private:
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    virtual bool TransferDataToWindow();
    void onOK(wxCommandEvent& event);
    void onCancel(wxCommandEvent& event);
    void onHelp(wxCommandEvent& event);
    void onName(wxCommandEvent& event);
    void onSocketChoice(wxCommandEvent& event);
    void updateSockets();
    void setTexts();

    /// Port to modify.
    TTAMachine::Port* port_;
    /// Name of the port.
    wxString name_;
    /// Input socket choice control.
    wxChoice* inputSocketChoice_;
    /// Output socket choice control.
    wxChoice* outputSocketChoice_;
    /// Original input socket of the port.
    TTAMachine::Socket* oldInput_;
    /// Original output socket of the port.
    TTAMachine::Socket* oldOutput_;

    /// enumerated IDs for the port controls
    enum {
	ID_NAME = 10000,
	ID_INPUT_SOCKET,
	ID_OUTPUT_SOCKET,
	ID_HELP
    };

    /// The event table for the controls of the dialog.
    DECLARE_EVENT_TABLE()
};
#endif
