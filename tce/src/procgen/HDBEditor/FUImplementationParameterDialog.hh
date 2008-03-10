/**
 * @file FUImplementationParameterDialog.hh
 *
 * Declaration of FUImplementationParameterDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_FU_IMPLEMENTATION_PARAMETER_DIALOG_HH
#define TTA_FU_IMPLEMENTATION_PARAMETER_DIALOG_HH

#include <wx/wx.h>

#include "FUImplementation.hh"


/**
 * Dialog for editing FU port implementations.
 */
class FUImplementationParameterDialog : public wxDialog {
public:
    FUImplementationParameterDialog(
        wxWindow* parent, wxWindowID id,
        HDB::FUImplementation::Parameter& implementation);

    virtual ~FUImplementationParameterDialog();
private:

    void onOK(wxCommandEvent& event);

    /// Enumerated IDs for dialog widgets.
    enum {
        ID_LABEL_NAME = 10000,
        ID_NAME,
        ID_LABEL_TYPE,
        ID_TYPE,
        ID_LABEL_VALUE,
        ID_VALUE,
        ID_LINE
    };

    /// Creates the dialog contents.
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);

    /// FU port implementation to modify.
    HDB::FUImplementation::Parameter& parameter_;
 
    wxString name_;
    wxString type_;
    wxString value_;

    DECLARE_EVENT_TABLE()
};

#endif
