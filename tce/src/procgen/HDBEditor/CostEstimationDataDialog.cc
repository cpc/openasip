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
 * @file CostEstimationDataDialog.cc
 *
 * Implementation of CostEstimationDataDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <wx/statline.h>
#include <wx/valgen.h>
#include "CostEstimationDataDialog.hh"
#include "WxConversion.hh"
#include "HDBManager.hh"
#include "ErrorDialog.hh"
#include "Conversion.hh"

using namespace HDB;

BEGIN_EVENT_TABLE(CostEstimationDataDialog, wxDialog)
    EVT_BUTTON(wxID_OK, CostEstimationDataDialog::onOK)
    EVT_CHOICE(ID_ENTRY_TYPE, CostEstimationDataDialog::onEntryTypeSelection)
END_EVENT_TABLE()

const wxString CostEstimationDataDialog::ENTRY_TYPE_NONE = _T("None");
const wxString CostEstimationDataDialog::ENTRY_TYPE_FU = _T("FU");
const wxString CostEstimationDataDialog::ENTRY_TYPE_RF = _T("RF");
const wxString CostEstimationDataDialog::ENTRY_TYPE_BUS = _T("Bus");
const wxString CostEstimationDataDialog::ENTRY_TYPE_SOCKET = _T("Socket");

/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param id Window identifier for the dialog.
 * @param hdb HDBManager where the data is added/modified.
 * @param dataID ID of the cost data to modify or -1 if a new data is being
 *                  added.
 */
CostEstimationDataDialog::CostEstimationDataDialog(
    wxWindow* parent, wxWindowID id, HDBManager& hdb, RowID pluginID,
    RowID dataID):
    wxDialog(parent, id, _T("Cost Estimation Data")),
    hdb_(hdb), pluginID_(pluginID), dataID_(dataID) {

    createContents(this, true, true);

    typeChoice_ = dynamic_cast<wxChoice*>(FindWindow(ID_ENTRY_TYPE));
    idChoice_ = dynamic_cast<wxChoice*>(FindWindow(ID_ENTRY_ID));
    
    typeChoice_->Append(ENTRY_TYPE_NONE);
    typeChoice_->Append(ENTRY_TYPE_FU);
    typeChoice_->Append(ENTRY_TYPE_RF);
    typeChoice_->Append(ENTRY_TYPE_BUS);
    typeChoice_->Append(ENTRY_TYPE_SOCKET);

    FindWindow(ID_NAME)->SetValidator(wxGenericValidator(&name_));
    FindWindow(ID_VALUE)->SetValidator(wxGenericValidator(&value_));

    if (dataID_ >= 0) {
        typeChoice_->Disable();
        idChoice_->Disable();
    }

}

/**
 * The Destructor.
 */
CostEstimationDataDialog::~CostEstimationDataDialog() {
}

/**
 * Transfers data to the dialog widgets.
 *
 * @return True, if the data was succesfully transferred.
 */
bool
CostEstimationDataDialog::TransferDataToWindow() {

    if (dataID_ >= 0) {
        const CostEstimationData data = hdb_.costEstimationData(dataID_);
        name_ = WxConversion::toWxString(data.name());
        value_ = WxConversion::toWxString(data.value().stringValue());

        if (data.hasFUReference()) {
            typeChoice_->SetStringSelection(ENTRY_TYPE_FU);
            idChoice_->Append(WxConversion::toWxString(data.fuReference()));
        } else if (data.hasRFReference()) {
            typeChoice_->SetStringSelection(ENTRY_TYPE_RF);            
            idChoice_->Append(WxConversion::toWxString(data.rfReference()));
        } else if (data.hasBusReference()) {
            typeChoice_->SetStringSelection(ENTRY_TYPE_BUS);
            idChoice_->Append(WxConversion::toWxString(data.busReference()));
        } else if (data.hasSocketReference()) {
            typeChoice_->SetStringSelection(ENTRY_TYPE_SOCKET);
            idChoice_->Append(
                WxConversion::toWxString(data.socketReference()));
        }
        idChoice_->SetSelection(0);
    } else {
        typeChoice_->SetSelection(0);
        wxCommandEvent dummy;
        onEntryTypeSelection(dummy);
    }

    return wxDialog::TransferDataToWindow();
}

/**
 * Event handler for the entry type choicer selections.
 */
void
CostEstimationDataDialog::onEntryTypeSelection(wxCommandEvent&) {

    idChoice_->Clear();

    if (typeChoice_->GetStringSelection() == ENTRY_TYPE_FU) {
        // Append FU entry IDs to the entry id choicer
        const std::set<RowID> fuIDs = hdb_.fuEntryIDs();
        std::set<RowID>::const_iterator iter = fuIDs.begin();
        for (; iter != fuIDs.end(); iter++) {
            idChoice_->Append(WxConversion::toWxString(*iter));
        }
    } else if (typeChoice_->GetStringSelection() == ENTRY_TYPE_RF) {
        // Append RF entry IDs to the entry id choicer
        const std::set<RowID> rfIDs = hdb_.rfEntryIDs();
        std::set<RowID>::const_iterator iter = rfIDs.begin();
        for (; iter != rfIDs.end(); iter++) {
            idChoice_->Append(WxConversion::toWxString(*iter));
        }
    } else if (typeChoice_->GetStringSelection() == ENTRY_TYPE_BUS) {
        // Append FU entry IDs to the entry id choicer
        const std::set<RowID> busIDs = hdb_.busEntryIDs();
        std::set<RowID>::const_iterator iter = busIDs.begin();
        for (; iter != busIDs.end(); iter++) {
            idChoice_->Append(WxConversion::toWxString(*iter));
        }

    } else if (typeChoice_->GetStringSelection() == ENTRY_TYPE_SOCKET) {
        // Append FU entry IDs to the entry id choicer
        const std::set<RowID> socketIDs = hdb_.socketEntryIDs();
        std::set<RowID>::const_iterator iter = socketIDs.begin();
        for (; iter != socketIDs.end(); iter++) {
            idChoice_->Append(WxConversion::toWxString(*iter));
        }
    } else {
        idChoice_->Disable();
        return;
    }

    idChoice_->Enable();
    idChoice_->SetSelection(0);
}

/**
 * Event handler for the ok-button.
 */
void
CostEstimationDataDialog::onOK(wxCommandEvent&) {

     TransferDataFromWindow();

     if (name_.IsEmpty()) {
             wxString message(_T("Data name not set."));
             ErrorDialog dialog(this, message);
             dialog.ShowModal();
             return;
     }

     CostEstimationData data;
     data.setPluginID(pluginID_);
     data.setName(WxConversion::toString(name_));
     DataObject value(WxConversion::toString(value_));
     data.setValue(value);


     std::string idStr =
         WxConversion::toString(idChoice_->GetStringSelection());

     if (typeChoice_->GetStringSelection() != ENTRY_TYPE_NONE) {
         
         if (idStr == "") {
             wxString message(_T("Invalid entry reference."));
             ErrorDialog dialog(this, message);
             dialog.ShowModal();
             return;
         }

         RowID id = Conversion::toInt(idStr);

         if (typeChoice_->GetStringSelection() == ENTRY_TYPE_FU) {
             data.setFUReference(id);
         } else if (typeChoice_->GetStringSelection() == ENTRY_TYPE_RF) {
             data.setRFReference(id);
         } else if (typeChoice_->GetStringSelection() == ENTRY_TYPE_BUS) {
             data.setBusReference(id);
         } else if (typeChoice_->GetStringSelection() == ENTRY_TYPE_SOCKET) {
             data.setSocketReference(id);
         }
     }

     if (dataID_ >= 0) {
         hdb_.modifyCostEstimationData(dataID_, data);
     } else {
         hdb_.addCostEstimationData(data);
     }

     EndModal(wxID_OK);
}

/**
 * Creates the dialog widgets.
 */
wxSizer*
CostEstimationDataDialog::createContents(
    wxWindow* parent, bool call_fit, bool set_sizer) {

    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxFlexGridSizer *item1 = new wxFlexGridSizer( 2, 0, 0 );

    wxStaticText *item2 = new wxStaticText( parent, ID_TEXT, wxT("Entry type:"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item2, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxString *strs3 = (wxString*) NULL;
    wxChoice *item3 = new wxChoice( parent, ID_ENTRY_TYPE, wxDefaultPosition, wxSize(100,-1), 0, strs3, 0 );
    item1->Add( item3, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticText *item4 = new wxStaticText( parent, ID_TEXT, wxT("Entry ID:"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item4, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxString *strs5 = (wxString*) NULL;
    wxChoice *item5 = new wxChoice( parent, ID_ENTRY_ID, wxDefaultPosition, wxSize(100,-1), 0, strs5, 0 );
    item1->Add( item5, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticText *item6 = new wxStaticText( parent, ID_TEXT, wxT("Name:"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item6, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxTextCtrl *item7 = new wxTextCtrl( parent, ID_NAME, wxT(""), wxDefaultPosition, wxSize(200,-1), 0 );
    item1->Add( item7, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item8 = new wxStaticText( parent, ID_TEXT, wxT("Value:"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item8, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxTextCtrl *item9 = new wxTextCtrl( parent, ID_VALUE, wxT(""), wxDefaultPosition, wxSize(200,-1), 0 );
    item1->Add( item9, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item1, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticLine *item10 = new wxStaticLine( parent, ID_LINE, wxDefaultPosition, wxSize(20,-1), wxLI_HORIZONTAL );
    item0->Add( item10, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item11 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item12 = new wxButton( parent, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item11->Add( item12, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item13 = new wxButton( parent, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item11->Add( item13, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item11, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;
}
