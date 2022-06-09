/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file RFPortImplementationDialog.cc
 *
 * Implementation of RFPortImplementationDialog class.
 *
 * @author Veli-Pekka Jääskeläinen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <wx/statline.h>
#include <wx/valgen.h>
#include "RFPortImplementationDialog.hh"
#include "RFPortImplementation.hh"
#include "WxConversion.hh"
#include "ErrorDialog.hh"

using namespace HDB;

BEGIN_EVENT_TABLE(RFPortImplementationDialog, wxDialog)
    EVT_BUTTON(wxID_OK, RFPortImplementationDialog::onOK)
END_EVENT_TABLE()

/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param id Window identifier for the dialog window.
 * @param port RF port implementation to modify.
 */
RFPortImplementationDialog::RFPortImplementationDialog(
    wxWindow* parent, wxWindowID id, RFPortImplementation& port) :
    wxDialog(parent, id, _T("Register File Port Implementation")),
    port_(port) {

    createContents(this, true, true);

    name_ = WxConversion::toWxString(port_.name());
    loadPort_ = WxConversion::toWxString(port_.loadPort());
    opcodePort_ = WxConversion::toWxString(port_.opcodePort());
    opcodePortWidth_ =
        WxConversion::toWxString(port_.opcodePortWidthFormula());

    direction_ = port_.direction();

    FindWindow(ID_NAME)->SetValidator(wxTextValidator(wxFILTER_ASCII, &name_));
    FindWindow(ID_LOAD_PORT)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &loadPort_));
    FindWindow(ID_OPCODE_PORT)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &opcodePort_));
    FindWindow(ID_OPCODE_PORT_WIDTH)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &opcodePortWidth_));
    FindWindow(ID_DIRECTION)->SetValidator(wxGenericValidator(&direction_));

    TransferDataToWindow();
}

/**
 * The Destructor.
 */
RFPortImplementationDialog::~RFPortImplementationDialog() {
}

/**
 * Event handler for the dialog OK-button.
 */
void
RFPortImplementationDialog::onOK(wxCommandEvent&) {

    TransferDataFromWindow();

    name_ = name_.Trim(true).Trim(false);
    loadPort_ = loadPort_.Trim(true).Trim(false);
    opcodePort_ = opcodePort_.Trim(true).Trim(false);
    opcodePortWidth_ = opcodePortWidth_.Trim(true).Trim(false);

    if (name_.IsEmpty()) {
        wxString message = _T("Name field must not be empty.");
        ErrorDialog dialog(this, message);
        dialog.ShowModal();
        return;
    }

    port_.setName(WxConversion::toString(name_));
    port_.setLoadPort(WxConversion::toString(loadPort_));
    port_.setOpcodePort(WxConversion::toString(opcodePort_));
    port_.setOpcodePortWidthFormula(WxConversion::toString(opcodePortWidth_));

    if (direction_ == 0) {
        port_.setDirection(IN);
    } else if (direction_ == 1) {
        port_.setDirection(OUT);
    } else if (direction_ == 2) {
        port_.setDirection(BIDIR);
    } else {
        assert(false);
    }
    EndModal(wxID_OK);

}

/**
 * Creates the dialog contents.
 */
wxSizer*
RFPortImplementationDialog::createContents(
    wxWindow *parent, bool call_fit, bool set_sizer) {

    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *item1 = new wxBoxSizer( wxHORIZONTAL );

    wxFlexGridSizer *item2 = new wxFlexGridSizer( 2, 0, 0 );

    wxStaticText *item3 = new wxStaticText( parent, ID_LABEL_NAME, wxT("Name:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item3, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxTextCtrl *item4 = new wxTextCtrl( parent, ID_NAME, wxT(""), wxDefaultPosition, wxSize(200,-1), 0 );
    item2->Add( item4, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item5 = new wxStaticText( parent, ID_LABEL_LOAD_PORT, wxT("Load port:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item5, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxTextCtrl *item6 = new wxTextCtrl( parent, ID_LOAD_PORT, wxT(""), wxDefaultPosition, wxSize(200,-1), 0 );
    item2->Add( item6, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticText *item7 = new wxStaticText( parent, ID_LABEL_OPCODE_PORT, wxT("Opcode port:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item7, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxTextCtrl *item8 = new wxTextCtrl( parent, ID_OPCODE_PORT, wxT(""), wxDefaultPosition, wxSize(200,-1), 0 );
    item2->Add( item8, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item9 = new wxStaticText( parent, ID_LABEL_OPCODE_PORT_WIDTH, wxT("Opcode port width formula:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item9, 0, wxALIGN_CENTER|wxALL, 5 );

    wxTextCtrl *item10 = new wxTextCtrl( parent, ID_OPCODE_PORT_WIDTH, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    item2->Add( item10, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item1->Add( item2, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxString strs11[] = 
    {
        wxT("In"), 
        wxT("Out"), 
        wxT("Bidirectional")
    };
    wxRadioBox *item11 = new wxRadioBox( parent, ID_DIRECTION, wxT("Direction:"), wxDefaultPosition, wxDefaultSize, 3, strs11, 1, wxRA_SPECIFY_COLS );
    item1->Add( item11, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    item0->Add( item1, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticLine *item12 = new wxStaticLine( parent, ID_LINE, wxDefaultPosition, wxSize(20,-1), wxLI_HORIZONTAL );
    item0->Add( item12, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item13 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item14 = new wxButton( parent, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item13->Add( item14, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item15 = new wxButton( parent, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item13->Add( item15, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item13, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;
}
