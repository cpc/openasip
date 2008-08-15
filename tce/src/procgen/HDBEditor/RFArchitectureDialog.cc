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
 * @file RFArchParamatersDialog.cc
 *
 * Implementation of RFArchitectureDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <wx/statline.h>
#include <wx/valgen.h>
#include <wx/spinctrl.h>
#include "RFArchitectureDialog.hh"
#include "RFArchitecture.hh"
#include "WxConversion.hh"

BEGIN_EVENT_TABLE(RFArchitectureDialog, wxDialog)
    EVT_BUTTON(wxID_OK, RFArchitectureDialog::onOK)
END_EVENT_TABLE()

/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param id Window identifier for the dialog.
 * @param arch Register file architecture to modify.
 */
RFArchitectureDialog::RFArchitectureDialog(
    wxWindow* parent, wxWindowID id, HDB::RFArchitecture& arch) :
    wxDialog(parent, id, _T("Register file architecture.")),
    arch_(arch), size_(0), width_(0) {

   if (!arch_.hasParameterizedSize()) {
       size_ = arch_.size();
   }

   if (!arch_.hasParameterizedWidth()) {
       width_ = arch_.width();
   }

   readPorts_ = arch_.readPortCount();
   writePorts_ = arch_.writePortCount();
   bidirPorts_ = arch_.bidirPortCount();
   maxReads_ = arch_.maxReads();
   maxWrites_ = arch_.maxWrites();
   latency_ = arch_.latency();

   paramWidth_ = arch_.hasParameterizedWidth();
   paramSize_ = arch_.hasParameterizedSize();
   guardSupport_ = arch_.hasGuardSupport();

   createContents(this, true, true);

   FindWindow(ID_SIZE)->SetValidator(wxGenericValidator(&size_));
   FindWindow(ID_WIDTH)->SetValidator(wxGenericValidator(&width_));
   FindWindow(ID_MAX_READS)->SetValidator(wxGenericValidator(&maxReads_));
   FindWindow(ID_MAX_WRITES)->SetValidator(wxGenericValidator(&maxWrites_));
   FindWindow(ID_READ_PORTS)->SetValidator(wxGenericValidator(&readPorts_));
   FindWindow(ID_WRITE_PORTS)->SetValidator(wxGenericValidator(&writePorts_));
   FindWindow(ID_BIDIR_PORTS)->SetValidator(wxGenericValidator(&bidirPorts_));

   FindWindow(ID_PARAM_SIZE)->SetValidator(wxGenericValidator(&paramSize_));
   FindWindow(ID_PARAM_WIDTH)->SetValidator(wxGenericValidator(&paramWidth_));
   FindWindow(ID_GUARD_SUPPORT)->SetValidator(
       wxGenericValidator(&guardSupport_));

}

/**
 * The Destructor.
 */
RFArchitectureDialog::~RFArchitectureDialog() {
}


/**
 * Event handler for the dialog OK-button.
 */
void
RFArchitectureDialog::onOK(wxCommandEvent&) {

    TransferDataFromWindow();
    
    if (!paramSize_) {
        arch_.setSize(size_);
    }
    if (!paramWidth_) {
        arch_.setWidth(width_);
    }

    arch_.setReadPortCount(readPorts_);
    arch_.setWritePortCount(writePorts_);
    arch_.setBidirPortCount(bidirPorts_);
    arch_.setMaxReads(maxReads_);
    arch_.setMaxWrites(maxWrites_);
    arch_.setLatency(latency_);
    arch_.setGuardSupport(guardSupport_);

    EndModal(wxID_OK);
}

/**
 * Creates the dialog contents.
 */
wxSizer*
RFArchitectureDialog::createContents(
    wxWindow *parent, bool call_fit, bool set_sizer) {

    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *item1 = new wxBoxSizer( wxHORIZONTAL );

    wxFlexGridSizer *item2 = new wxFlexGridSizer( 2, 0, 0 );

    wxStaticText *item3 = new wxStaticText( parent, ID_LABEL_SIZE, wxT("Size:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item3, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxSpinCtrl *item4 = new wxSpinCtrl( parent, ID_SIZE, wxT("0"), wxDefaultPosition, wxSize(100,-1), 0, 0, 10000, 0 );
    item2->Add( item4, 0, wxALIGN_CENTER|wxALL, 5 );
    
    wxStaticText *item5 = new wxStaticText( parent, ID_LABEL_WIDTH, wxT("Width:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item5, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxSpinCtrl *item6 = new wxSpinCtrl( parent, ID_WIDTH, wxT("0"), wxDefaultPosition, wxSize(100,-1), 0, 0, 10000, 0 );
    item2->Add( item6, 0, wxALIGN_CENTER|wxALL, 5 );

    // Modifying latency disabled for now!
    //wxStaticText *item7 = new wxStaticText( parent, ID_LABEL_LATENCY, wxT("Latency:"), wxDefaultPosition, wxDefaultSize, 0 );
    //item2->Add( item7, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    //wxSpinCtrl *item8 = new wxSpinCtrl( parent, ID_LATENCY, wxT("0"), wxDefaultPosition, wxSize(100,-1), 0, 0, 100, 0 );
    //item2->Add( item8, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item9 = new wxStaticText( parent, ID_LABEL_MAX_READS, wxT("Max reads:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item9, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxSpinCtrl *item10 = new wxSpinCtrl( parent, ID_MAX_READS, wxT("0"), wxDefaultPosition, wxSize(100,-1), 0, 0, 10000, 0 );
    item2->Add( item10, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item11 = new wxStaticText( parent, ID_LABEL_MAX_WRITES, wxT("Max writes:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item11, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxSpinCtrl *item12 = new wxSpinCtrl( parent, ID_MAX_WRITES, wxT("0"), wxDefaultPosition, wxSize(100,-1), 0, 0, 100, 0 );
    item2->Add( item12, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item13 = new wxStaticText( parent, ID_LABEL_READ_PORTS, wxT("Read ports:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item13, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxSpinCtrl *item14 = new wxSpinCtrl( parent, ID_READ_PORTS, wxT("0"), wxDefaultPosition, wxSize(100,-1), 0, 0, 100, 0 );
    item2->Add( item14, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item15 = new wxStaticText( parent, ID_LABEL_WRITE_PORTS, wxT("Write ports:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item15, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxSpinCtrl *item16 = new wxSpinCtrl( parent, ID_WRITE_PORTS, wxT("0"), wxDefaultPosition, wxSize(100,-1), 0, 0, 100, 0 );
    item2->Add( item16, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item17 = new wxStaticText( parent, ID_LABEL_BIDIR_PORTS, wxT("Bidirectional ports:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item17, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxSpinCtrl *item18 = new wxSpinCtrl( parent, ID_BIDIR_PORTS, wxT("0"), wxDefaultPosition, wxSize(100,-1), 0, 0, 100, 0 );
    item2->Add( item18, 0, wxALIGN_CENTER|wxALL, 5 );

    item1->Add( item2, 0, wxALIGN_CENTER|wxALL, 5 );

    wxBoxSizer *item19 = new wxBoxSizer( wxVERTICAL );

    wxCheckBox *item20 = new wxCheckBox( parent, ID_PARAM_WIDTH, wxT("Parametrized width"), wxDefaultPosition, wxDefaultSize, 0 );
    item19->Add( item20, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxCheckBox *item21 = new wxCheckBox( parent, ID_PARAM_SIZE, wxT("Parametrized size"), wxDefaultPosition, wxDefaultSize, 0 );
    item19->Add( item21, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxCheckBox *item22 = new wxCheckBox( parent, ID_GUARD_SUPPORT, wxT("Guard support"), wxDefaultPosition, wxDefaultSize, 0 );
    item19->Add( item22, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item1->Add( item19, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    item0->Add( item1, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticLine *item23 = new wxStaticLine( parent, ID_LINE, wxDefaultPosition, wxSize(20,-1), wxLI_HORIZONTAL );
    item0->Add( item23, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item24 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item25 = new wxButton( parent, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item24->Add( item25, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item26 = new wxButton( parent, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item24->Add( item26, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item24, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;
}
