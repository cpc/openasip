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
 * @file AddBreakpointDialog.cc
 *
 * Implementation of AddBreakpointDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <string>
#include <wx/statline.h>
#include "AddBreakpointDialog.hh"
#include "ProximToolbox.hh"
#include "Exception.hh"
#include "ProximLineReader.hh"
#include "Conversion.hh"
#include "WxConversion.hh"
#include "ErrorDialog.hh"
#include "ProximConstants.hh"
#include "Proxim.hh"

BEGIN_EVENT_TABLE(AddBreakpointDialog, wxDialog)
    EVT_BUTTON(wxID_OK, AddBreakpointDialog::onOK)
END_EVENT_TABLE()

/**
 * The Constructor.
 *
 * @paran parent Parent window of the dialog.
 * @param id Identifier of the window.
 */
AddBreakpointDialog::AddBreakpointDialog(wxWindow* parent, wxWindowID id) :
  wxDialog(parent, id, _T("Add Breakpoint"), wxDefaultPosition) {

    createContents(this, true, true);
}

/**
 * The Destructor.
 */
AddBreakpointDialog::~AddBreakpointDialog() {
}


/**
 * Event handler for the OK-button.
 *
 * Adds a breakpoint at the address defined in the address field.
 */
void
AddBreakpointDialog::onOK(wxCommandEvent&) {

    wxString addressString =
        dynamic_cast<wxTextCtrl*>(FindWindow(ID_ADDRESS))->GetValue();

    unsigned address = 0;
    try {
        address =
            Conversion::toUnsignedInt(WxConversion::toString(addressString));
    } catch (NumberFormatException& e) {
        wxString message = _T("Invalid address");
        ErrorDialog errorDialog(this, message);
        errorDialog.ShowModal();
        return;
    }
    std::string command = ProximConstants::SCL_SET_BREAKPOINT + " " +
        Conversion::toString(address);
    ProximToolbox::lineReader().input(command);
    EndModal(wxID_OK);
}

/**
 * Creates the dialog widgets.
 */
wxSizer*
AddBreakpointDialog::createContents(
    wxWindow *parent, bool call_fit, bool set_sizer) {

    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxStaticText *item1 = new wxStaticText( parent, ID_LABEL_NEW_BP, wxT("Set Breakpoint at"), wxDefaultPosition, wxDefaultSize, 0 );
    item0->Add( item1, 0, wxALIGN_CENTER|wxALL, 5 );

    wxTextCtrl *item2 = new wxTextCtrl( parent, ID_ADDRESS, wxT(""), wxDefaultPosition, wxSize(200,-1), 0 );
    item0->Add( item2, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticLine *item3 = new wxStaticLine( parent, ID_LINE, wxDefaultPosition, wxSize(20,-1), wxLI_HORIZONTAL );
    item0->Add( item3, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxGridSizer *item4 = new wxGridSizer( 2, 0, 0 );

    wxButton *item5 = new wxButton( parent, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item4->Add( item5, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item6 = new wxButton( parent, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item4->Add( item6, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item4, 0, wxALIGN_CENTER|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;
}
