/**
 * @file RFPortImplementationDialog.hh
 *
 * Declaration of RFPortImplementationDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_RF_PORT_IMPLEMENTATION_DIALOG_HH
#define TTA_RF_PORT_IMPLEMENTATION_DIALOG_HH

#include <wx/wx.h>

namespace HDB {
    class RFPortImplementation;
}

/**
 * Dialog for editing register file port implementations.
 */
class RFPortImplementationDialog : public wxDialog {
public:
    RFPortImplementationDialog(
        wxWindow* parent, wxWindowID id,
        HDB::RFPortImplementation& implementation);

    virtual ~RFPortImplementationDialog();
private:

    void onOK(wxCommandEvent& event);

    /// Enumerated IDs for dialog widgets.
    enum {
        ID_LABEL_NAME = 10000,
        ID_NAME,
        ID_LABEL_LOAD_PORT,
        ID_LOAD_PORT,
        ID_LABEL_OPCODE_PORT,
        ID_OPCODE_PORT,
        ID_LABEL_OPCODE_PORT_WIDTH,
        ID_OPCODE_PORT_WIDTH,
        ID_DIRECTION,
        ID_LINE
    };

    /// Creates the dialog contents.
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);

    /// FU port implementation to modify.
    HDB::RFPortImplementation& port_;

    wxString name_;
    wxString loadPort_;
    wxString opcodePort_;
    wxString opcodePortWidth_;
    int direction_;

    DECLARE_EVENT_TABLE()
};

#endif
