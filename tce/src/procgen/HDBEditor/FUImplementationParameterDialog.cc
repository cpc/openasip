/**
 * @file FUImplementationParameterDialog.cc
 *
 * Implementation of FUImplementationParameterDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <wx/statline.h>
#include <wx/valgen.h>
#include "FUImplementationParameterDialog.hh"
#include "WxConversion.hh"
#include "ErrorDialog.hh"

using namespace HDB;

BEGIN_EVENT_TABLE(FUImplementationParameterDialog, wxDialog)
    EVT_BUTTON(wxID_OK, FUImplementationParameterDialog::onOK)
END_EVENT_TABLE()

/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param id Window identifier for the dialog window.
 * @param parameter FU implementation parameter to modify.
 */
FUImplementationParameterDialog::FUImplementationParameterDialog(
    wxWindow* parent, wxWindowID id, FUImplementation::Parameter& parameter) :
    wxDialog(parent, id, _T("Parameter")),
    parameter_(parameter) {

    createContents(this, true, true);

    name_ = WxConversion::toWxString(parameter_.name);
    type_ = WxConversion::toWxString(parameter_.type);
    value_ = WxConversion::toWxString(parameter_.value);

    FindWindow(ID_NAME)->SetValidator(wxTextValidator(wxFILTER_ASCII, &name_));
    FindWindow(ID_TYPE)->SetValidator(wxTextValidator(wxFILTER_ASCII, &type_));
    FindWindow(ID_VALUE)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &value_));

    TransferDataToWindow();
}

/**
 * The Destructor.
 */
FUImplementationParameterDialog::~FUImplementationParameterDialog() {
}


/**
 * Event handler for the dialog OK-button.
 */
void
FUImplementationParameterDialog::onOK(wxCommandEvent&) {

    TransferDataFromWindow();

    name_ = name_.Trim(true).Trim(false);
    type_ = type_.Trim(true).Trim(false);
    value_ = value_.Trim(true).Trim(false);


    if (name_.IsEmpty()) {
        wxString message = _T("Name field must not be empty.");
        ErrorDialog dialog(this, message);
        dialog.ShowModal();
        return;
    }

    if (type_.IsEmpty()) {
        wxString message = _T("Type field must not be empty.");
        ErrorDialog dialog(this, message);
        dialog.ShowModal();
        return;
    }

    parameter_.name = WxConversion::toString(name_);
    parameter_.type = WxConversion::toString(type_);
    parameter_.value = WxConversion::toString(value_);

    EndModal(wxID_OK);
}

/**
 * Creates the dialog contents.
 */
wxSizer*
FUImplementationParameterDialog::createContents(
    wxWindow *parent, bool call_fit, bool set_sizer) {

    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxFlexGridSizer *item1 = new wxFlexGridSizer( 2, 0, 0 );

    wxStaticText *item2 = new wxStaticText( parent, ID_LABEL_NAME, wxT("Name:"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item2, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxTextCtrl *item3 = new wxTextCtrl( parent, ID_NAME, wxT(""), wxDefaultPosition, wxSize(200,-1), 0 );
    item1->Add( item3, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item4 = new wxStaticText( parent, ID_LABEL_TYPE, wxT("Type:"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item4, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxTextCtrl *item5 = new wxTextCtrl( parent, ID_TYPE, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    item1->Add( item5, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticText *item6 = new wxStaticText( parent, ID_LABEL_VALUE, wxT("Value:"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item6, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxTextCtrl *item7 = new wxTextCtrl( parent, ID_VALUE, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    item1->Add( item7, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item0->Add( item1, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticLine *item8 = new wxStaticLine( parent, ID_LINE, wxDefaultPosition, wxSize(20,-1), wxLI_HORIZONTAL );
    item0->Add( item8, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item9 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item10 = new wxButton( parent, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item9->Add( item10, 0, wxALIGN_CENTER, 5 );

    wxButton *item11 = new wxButton( parent, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item9->Add( item11, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item9, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;
}
