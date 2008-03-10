/**
 * @file FUExternalPortDialog.hh
 *
 * Declaration of FUExternalPortDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_FU_EXTERNAL_PORT_DIALOG_HH
#define TTA_FU_EXTERNAL_PORT_DIALOG_HH

#include <wx/wx.h>

class wxCheckListBox;

namespace HDB {
    class FUExternalPort;
    class FUImplementation;
}

/**
 * Dialog for editing FU external ports.
 */
class FUExternalPortDialog : public wxDialog {
public:
    FUExternalPortDialog(
        wxWindow* parent, wxWindowID id,
        HDB::FUExternalPort& implementation,
        const HDB::FUImplementation& fu);

    virtual ~FUExternalPortDialog();

private:
    void initialize();
    void onOK(wxCommandEvent& event);

    /// Enumerated IDs for dialog widgets.
    enum {
        ID_LABEL_NAME = 10000,
        ID_NAME,
        ID_LABEL_WIDTH,
        ID_WIDTH,
        ID_LABEL_DESCRIPTION,
        ID_DESCRIPTION,
        ID_DIRECTION,
        ID_PARAMETER_DEPS,
        ID_LINE
    };

    /// Creates the dialog contents.
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);

    /// FU port implementation to modify.
    HDB::FUExternalPort& port_;
    /// Parent FU implementation of the port.
    const HDB::FUImplementation& fu_;

    wxString name_;
    wxString widthFormula_;
    wxString description_;
    int direction_;

    wxCheckListBox* depList_;

    DECLARE_EVENT_TABLE()
};

#endif
