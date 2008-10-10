/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
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
