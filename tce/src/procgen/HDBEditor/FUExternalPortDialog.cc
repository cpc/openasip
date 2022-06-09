/*
    Copyright (c) 2002-2014 Tampere University.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file FUExternalPortDialog.cc
 *
 * Implementation of FUExternalPortDialog class.
 *
 * @author Veli-Pekka Jääskeläinen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <wx/wx.h>
#include <wx/checklst.h>
#include <wx/statline.h>
#include <wx/valgen.h>
#include "FUExternalPortDialog.hh"
#include "FUExternalPort.hh"
#include "WxConversion.hh"
#include "ErrorDialog.hh"
#include "FUImplementation.hh"


using namespace HDB;
using std::string;

BEGIN_EVENT_TABLE(FUExternalPortDialog, wxDialog)
    EVT_BUTTON(wxID_OK, FUExternalPortDialog::onOK)
END_EVENT_TABLE()

/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param id Window identifier for the dialog window.
 * @param port FU external port to modify.
 */
FUExternalPortDialog::FUExternalPortDialog(
    wxWindow* parent, wxWindowID id, FUExternalPort& port,
    const FUImplementation& fu) :
    wxDialog(parent, id, _T("Function Unit Port Port")),
    port_(port), fu_(fu), depList_(NULL) {

    initialize();
}

/**
 * The Destructor.
 */
FUExternalPortDialog::~FUExternalPortDialog() {
}

/**
 * Creates and initializes the dialog widgets.
 */
void
FUExternalPortDialog::initialize() {
    
    createContents(this, true, true);
    
    depList_ = dynamic_cast<wxCheckListBox*>(
        FindWindow(ID_PARAMETER_DEPS));
    
    name_ = WxConversion::toWxString(port_.name());
    widthFormula_ = WxConversion::toWxString(port_.widthFormula());
    description_ = WxConversion::toWxString(port_.description());
    direction_ = port_.direction();
    
    FindWindow(ID_NAME)->SetValidator(wxTextValidator(wxFILTER_ASCII, &name_));
    FindWindow(ID_WIDTH)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &widthFormula_));
    FindWindow(ID_DIRECTION)->SetValidator(wxGenericValidator(&direction_));
    FindWindow(ID_DESCRIPTION)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &description_));
    
    depList_->Clear();
    for (int i = 0; i < fu_.parameterCount(); i++) {
        string parameter = fu_.parameter(i).name;
        depList_->Append( WxConversion::toWxString(parameter));

        for (int dep = 0; dep < port_.parameterDependencyCount(); dep++) {
            if (port_.parameterDependency(dep) == parameter) {
                depList_->Check(i);
            }
        }
    }
}


/**
 * Event handler for the dialog OK-button.
 */
void
FUExternalPortDialog::onOK(wxCommandEvent&) {

    TransferDataFromWindow();

    name_ = name_.Trim(true).Trim(false);
    widthFormula_ = widthFormula_.Trim(true).Trim(false);
    description_ = description_.Trim(true).Trim(false);
    
    if (name_.IsEmpty()) {
        wxString message = _T("Name field must not be empty.");
        ErrorDialog dialog(this, message);
        dialog.ShowModal();
        return;
    }

    if (widthFormula_.IsEmpty()) {
        wxString message =
            _T("Width formula field must not be empty.");
        ErrorDialog dialog(this, message);
        dialog.ShowModal();
        return;
    }

    port_.setName(WxConversion::toString(name_));
    port_.setWidthFormula(WxConversion::toString(widthFormula_));
    port_.setDescription(WxConversion::toString(description_));

    if (direction_ == 0) {
        port_.setDirection(IN);
    } else if (direction_ == 1) {
        port_.setDirection(OUT);
    } else if (direction_ == 2) {
        port_.setDirection(BIDIR);
    } else {
        assert(false);
    }

    for (int i = 0; i < fu_.parameterCount(); i++) {
        string parameter = WxConversion::toString(depList_->GetString(i));
        if (depList_->IsChecked(i)) {          
            port_.setParameterDependency(parameter);
        } else {
            port_.unsetParameterDependency(parameter);
        }
    }

    EndModal(wxID_OK);

}

/**
 * Creates the dialog contents.
 */
wxSizer*
FUExternalPortDialog::createContents(
    wxWindow *parent, bool call_fit, bool set_sizer) {
    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *item1 = new wxBoxSizer( wxVERTICAL );

    wxFlexGridSizer *item2 = new wxFlexGridSizer( 2, 0, 0 );

    wxStaticText *item3 = new wxStaticText( parent, ID_LABEL_NAME, wxT("Name:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item3, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxTextCtrl *item4 = new wxTextCtrl( parent, ID_NAME, wxT(""), wxDefaultPosition, wxSize(300,-1), 0 );
    item2->Add( item4, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item5 = new wxStaticText( parent, ID_LABEL_WIDTH, wxT("Width formula:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item5, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxTextCtrl *item6 = new wxTextCtrl( parent, ID_WIDTH, wxT(""), wxDefaultPosition, wxSize(200,-1), 0 );
    item2->Add( item6, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticText *item7 = new wxStaticText( parent, ID_LABEL_DESCRIPTION, wxT("Description:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item7, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxTextCtrl *item8 = new wxTextCtrl( parent, ID_DESCRIPTION, wxT(""), wxDefaultPosition, wxSize(300,-1), 0 );
    item2->Add( item8, 0, wxALIGN_CENTER|wxALL, 5 );

    item1->Add( item2, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxGridSizer *item9 = new wxGridSizer( 2, 0, 0 );

    wxString strs10[] = 
    {
        wxT("In"), 
        wxT("Out"), 
        wxT("Bidirectional")
    };
    wxRadioBox *item10 = new wxRadioBox( parent, ID_DIRECTION, wxT("Direction:"), wxDefaultPosition, wxDefaultSize, 3, strs10, 1, wxRA_SPECIFY_COLS );
    item9->Add( item10, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticBox *item12 = new wxStaticBox( parent, -1, wxT("Parameter dependency:") );
    wxStaticBoxSizer *item11 = new wxStaticBoxSizer( item12, wxVERTICAL );
    
    wxWindow *item13 = new wxCheckListBox(parent, ID_PARAMETER_DEPS, wxDefaultPosition, wxSize(200, 150));
    wxASSERT( item13 );
    item11->Add( item13, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item9->Add( item11, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item1->Add( item9, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item0->Add( item1, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticLine *item14 = new wxStaticLine( parent, ID_LINE, wxDefaultPosition, wxSize(20,-1), wxLI_HORIZONTAL );
    item0->Add( item14, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item15 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item16 = new wxButton( parent, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item15->Add( item16, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item17 = new wxButton( parent, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item15->Add( item17, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item15, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;
}
