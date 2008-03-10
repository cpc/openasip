/**
 * @file IUPortDialog.hh
 *
 * Declaration of IUPortDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_IU_PORT_DIALOG_HH
#define TTA_IU_PORT_DIALOG_HH

#include <wx/wx.h>
#include <wx/statline.h>

namespace TTAMachine {
    class Port;
}

/**
 * Dialog for editing immediate unit ports.
 */
class IUPortDialog : public wxDialog {
public:
    IUPortDialog(wxWindow* parent, TTAMachine::Port* port);
    virtual ~IUPortDialog();

private:
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    virtual bool TransferDataToWindow();
    void onOK(wxCommandEvent& event);
    void onHelp(wxCommandEvent& event);
    void onName(wxCommandEvent& event);
    void updateSocket();
    void setTexts();

    /// Immediate unit port to edit with the dialog.
    TTAMachine::Port* port_;
    /// Name of the port.
    wxString name_;
    /// Output socket choice control.
    wxChoice* outputSocketChoice_;

    /// Enumerated IDs for dialog controls.
    enum {
	ID_NAME= 10000,
	ID_OUTPUT_SOCKET,
	ID_HELP,
        ID_LABEL_NAME,
        ID_LABEL_OUTPUT_SOCKET,
        ID_LINE
    };

    /// The event table for the controls of the dialog.
   DECLARE_EVENT_TABLE()
};
#endif
