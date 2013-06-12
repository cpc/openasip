/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file AutoSelectImplementationsDialog.cc
 *
 * Definition of AutoSelectImplementationsDialog class.
 *
 * @author Mikko Järvelä 2013 (jarvela7-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <wx/valgen.h>
#include <wx/statline.h>
#include "AutoSelectImplementationsDialog.hh"
#include "HDBRegistry.hh"
#include "WxConversion.hh"
#include "ComponentImplementationSelector.hh"
#include "RegisterFile.hh"
#include "ErrorDialog.hh"
#include "ConfirmDialog.hh"
#include "WarningDialog.hh"

using namespace IDF;
using namespace TTAMachine;
using namespace HDB;


BEGIN_EVENT_TABLE(AutoSelectImplementationsDialog, wxDialog)
    EVT_BUTTON(ID_BROWSE, AutoSelectImplementationsDialog::onBrowse)
    EVT_BUTTON(ID_FIND, AutoSelectImplementationsDialog::onFind)
    EVT_BUTTON(ID_CLOSE, AutoSelectImplementationsDialog::onClose)
END_EVENT_TABLE()


/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param machine Current machine.
 * @param impl Implementation of the machine.
 */
AutoSelectImplementationsDialog::AutoSelectImplementationsDialog(
    wxWindow* parent,
    Machine& machine,
    MachineImplementation& impl) :
    wxDialog(parent, -1, _T("Auto Select Implementations"), wxDefaultPosition),
    machine_(machine),
    impl_(impl) {
    
    createContents(this, true, true);
    
    hdbChoice_ = dynamic_cast<wxChoice*>(FindWindow(ID_HDB_CHOICE));
    cboxRF_ = dynamic_cast<wxCheckBox*>(FindWindow(ID_RF));
    cboxIU_ = dynamic_cast<wxCheckBox*>(FindWindow(ID_IU));
    cboxFU_ = dynamic_cast<wxCheckBox*>(FindWindow(ID_FU));
    
    // set all checkboxes as selected
    cboxRF_->SetValue(true);
    cboxIU_->SetValue(true);
    cboxFU_->SetValue(true);

    // add available HDBs to the choice list
    HDBRegistry& registry = HDBRegistry::instance();
    registry.loadFromSearchPaths();
    for (int i = 0; i < registry.hdbCount(); i++) {
        hdbs_.insert(registry.hdbPath(i));
    }
    if (!hdbs_.empty()) {
        std::set<TCEString>::iterator iter = hdbs_.begin();
        for (; iter != hdbs_.end(); iter++) {
            hdbChoice_->Append(WxConversion::toWxString(*iter));
        }
        hdbChoice_->SetSelection(0);
    }
}

/**
 * The Destructor.
 */
AutoSelectImplementationsDialog::~AutoSelectImplementationsDialog() {
}

/**
 * File dialog for choosing a custom HDB file.
 */
void
AutoSelectImplementationsDialog::onBrowse(wxCommandEvent&) {

    wxFileDialog dialog(
        this, _T("Choose a HDB file containing the implementation"),
        _T(""), _T(""), _T("HDBs|*.hdb|All files|*.*"),
        (wxOPEN | wxFILE_MUST_EXIST));

    if (dialog.ShowModal() == wxID_OK) {
        wxString hdb = dialog.GetPath();
        int item = hdbChoice_->FindString(hdb);
        if (item == wxNOT_FOUND) {
            item = hdbChoice_->Append(hdb);
        }
        hdbChoice_->Select(item);
    }
}

/**
 * Exit the dialog.
 */
void
AutoSelectImplementationsDialog::onClose(wxCommandEvent&) {
    
    Close();
}

/**
 * Searches implementations for empty RF/IU/FU units.
 *
 * After search is over and if implementations were found, user is asked
 * if he/she wants to integrate the found implementations to the current
 * machine implementation.
 */
void
AutoSelectImplementationsDialog::onFind(wxCommandEvent&) {

    foundRF_.clear();
    foundIU_.clear();
    foundFU_.clear();

    // if none of the check boxes are selected, no point in searching
    if (!cboxRF_->GetValue() && !cboxIU_->GetValue() && !cboxFU_->GetValue()) {
        return;
    }

    // extract path of the HDB file that user has chosen
    TCEString path = WxConversion::toString(hdbChoice_->GetStringSelection());
    // make sure the file exists
    if (!FileSystem::fileExists(path)) {
        wxString message = _T("Error: path ");
        message.append(WxConversion::toWxString(path));
        message.append(_T(" could not be found!\n"));
        ErrorDialog dialog(this, message);
        dialog.ShowModal();
        return;
    }

    // pass the hdb file to search functions for implementation searching
    try {
        HDBManager& hdb = HDBRegistry::instance().hdb(path);
        
        if (cboxRF_->GetValue()) {
            findRFImplementations(hdb);
        }
        if (cboxIU_->GetValue()) {
            findIUImplementations(hdb);
        }
        if (cboxFU_->GetValue()) {
            findFUImplementations(hdb);
        }
    } catch (Exception& e) {
        wxString message = _T("");
        message.append(WxConversion::toWxString(e.errorMessage()));
        ErrorDialog dialog(this, message);
        dialog.ShowModal();
        return;
    }
    
    unsigned int rfCount = static_cast<unsigned int>(foundRF_.size());
    unsigned int iuCount = static_cast<unsigned int>(foundIU_.size());
    unsigned int fuCount = static_cast<unsigned int>(foundFU_.size());

    // form a message describing how many implementations were found
    wxString message = _T("");
    message.append(WxConversion::toWxString(rfCount+iuCount+fuCount));
    message.append(_T(" implementations were found for ")); 
    if (cboxRF_->GetValue()) {
        message.append(_T("RF ("));
        message.append(WxConversion::toWxString(rfCount));
        message.append(_T(")"));
    }
    if (cboxIU_->GetValue()) {
        if (cboxRF_->GetValue()) {
            message.append(_T(" / "));
        }
        message.append(_T("IU ("));
        message.append(WxConversion::toWxString(iuCount));
        message.append(_T(")"));
    }
    if (cboxFU_->GetValue()) {
        if (cboxIU_->GetValue() || cboxRF_->GetValue()) {
            message.append(_T(" / "));
        }
        message.append(_T("FU ("));
        message.append(WxConversion::toWxString(fuCount));
        message.append(_T(")"));
    }
    message.append(_T(" units that have no implementations yet.\n\n"));
    
    // if no implementations were found, prompt user and exit
    if ((rfCount+iuCount+fuCount) == 0) {
        WarningDialog dialog(this, message);
        dialog.ShowModal();
        return;
    }
    
    message.append(_T("Press Yes to integrate found implementations into"));
    message.append(_T(" the empty units.\n"));

    // ask if user wants to save the found implementations
    ConfirmDialog dialog(this, message);
    if (dialog.ShowModal() != wxID_YES) {
        return;
    }
    
    // add all the newly found implementations to the machine implementation
    
    // steps: set the new HDB file and ID pair for the unit implementation,
    // and then add the unit implementation to the machine implementation 
    std::map<const RFImplementationLocation*, HdbIdPair>::iterator itRF;
    for (itRF = foundRF_.begin(); itRF != foundRF_.end(); ++itRF) {
        const RFImplementationLocation* unit = itRF->first;
        HdbIdPair hdbID = itRF->second;
        RFImplementationLocation* rf = new RFImplementationLocation(
            hdbID.hdbFile, hdbID.id, unit->unitName());
        try {
            impl_.addRFImplementation(rf);
        } catch (...) {
            // do nothing if the implementation could not be added
        }
    }

    std::map<const RFImplementationLocation*, HdbIdPair>::iterator itIU;
    for (itIU = foundIU_.begin(); itIU != foundIU_.end(); ++itIU) {
        const RFImplementationLocation* unit = itIU->first;
        HdbIdPair hdbID = itIU->second;
        RFImplementationLocation* iu = new RFImplementationLocation(
            hdbID.hdbFile, hdbID.id, unit->unitName());
        try {
            impl_.addIUImplementation(iu);
        } catch (...) {
            // do nothing if the implementation could not be added
        }
    }

    std::map<const FUImplementationLocation*, HdbIdPair>::iterator itFU;
    for (itFU = foundFU_.begin(); itFU != foundFU_.end(); ++itFU) {
        const FUImplementationLocation* unit = itFU->first;
        HdbIdPair hdbID = itFU->second;
        FUImplementationLocation* fu = new FUImplementationLocation(
            hdbID.hdbFile, hdbID.id, unit->unitName());
        try {
            impl_.addFUImplementation(fu);
        } catch (...) {
            // do nothing if the implementation could not be added
        }
    }

    // exit to update the RF/IU/FU implementation list view for user
    wxCommandEvent dummy;
    onClose(dummy);
}

/**
 * Function that searches RF implementations from given HDB file.
 *
 * @param hdb HDB file from which implementations are searched.
 */
void
AutoSelectImplementationsDialog::findRFImplementations(HDBManager& hdb) {

    // this component searches proper implementations for register files
    ComponentImplementationSelector* selector = 
        new ComponentImplementationSelector();
    // add the HDB file to selector for searching
    selector->addHDB(hdb);

    std::map<const RFImplementationLocation*, CostEstimates*> rfImpls;
    std::map<const RFImplementationLocation*, CostEstimates*>::iterator it;

    // loop through machine's register files
    int rfCount = machine_.registerFileNavigator().count();
    for (int i = 0; i < rfCount; ++i) {
        Component* comp = machine_.registerFileNavigator().item(i);
        RegisterFile* rf = dynamic_cast<RegisterFile*>(comp);
        
        // if register file doesn't have an implementation...
        if (rf != NULL && !impl_.hasRFImplementation(rf->name())) {
            // ...try to find one using the selector component
            rfImpls = selector->rfImplementations(*rf, rf->isUsedAsGuard());
            it = rfImpls.begin();

            // if an implementation was found, save it into the std::map
            if (rfImpls.size() > 0 && it->first != NULL) {
                const RFImplementationLocation* location = it->first;
                HdbIdPair locationInfo;
                locationInfo.hdbFile = hdb.fileName();
                locationInfo.id = location->id();

                foundRF_.insert(
                    std::pair<const RFImplementationLocation*, HdbIdPair>(
                        location, locationInfo));
            }
        }
    }

    delete selector;
}

/**
 * Function that searches IU implementations from given HDB file.
 *
 * @param hdb HDB file from which implementations are searched.
 */
void
AutoSelectImplementationsDialog::findIUImplementations(HDBManager& hdb) {

    // this component searches proper implementations for immediate units
    ComponentImplementationSelector* selector = 
        new ComponentImplementationSelector();
    // add the HDB file to selector for searching
    selector->addHDB(hdb);

    std::map<const RFImplementationLocation*, CostEstimates*> iuImpls;
    std::map<const RFImplementationLocation*, CostEstimates*>::iterator it;

    // loop through machine's immediate units
    int iuCount = machine_.immediateUnitNavigator().count();
    for (int i = 0; i < iuCount; ++i) {
        Component* comp = machine_.immediateUnitNavigator().item(i);
        ImmediateUnit* iu = dynamic_cast<ImmediateUnit*>(comp);

        // if immediate unit doesn't have an implementation...
        if (iu != NULL  && !impl_.hasIUImplementation(iu->name())) {
            // ...try to find one using the selector component
            iuImpls = selector->iuImplementations(*iu);
            it = iuImpls.begin();

            // if an implementation was found, save it into the std::map
            if (iuImpls.size() > 0 && it->first != NULL) {
                const RFImplementationLocation* location = it->first;
                HdbIdPair locationInfo;
                locationInfo.hdbFile = hdb.fileName();
                locationInfo.id = location->id();

                foundIU_.insert(
                    std::pair<const RFImplementationLocation*, HdbIdPair>(
                        location, locationInfo));
            }
        }
    }

    delete selector;
}

/**
 * Function that searches FU implementations from given HDB file.
 *
 * @param hdb HDB file from which implementations are searched.
 */
void
AutoSelectImplementationsDialog::findFUImplementations(HDBManager& hdb) {

    // this component searches proper implementations for function units
    ComponentImplementationSelector* selector = 
        new ComponentImplementationSelector();
    // add the HDB file to selector for searching
    selector->addHDB(hdb);

    std::map<const FUImplementationLocation*, CostEstimates*> fuImpls;
    std::map<const FUImplementationLocation*, CostEstimates*>::iterator it;

    // loop through machine's function units
    int fuCount = machine_.functionUnitNavigator().count();
    for (int i = 0; i < fuCount; ++i) {
        Component* comp = machine_.functionUnitNavigator().item(i);
        FunctionUnit* fu = dynamic_cast<FunctionUnit*>(comp);

        // if function unit doesn't have an implementation...
        if (fu != NULL && !impl_.hasFUImplementation(fu->name())) {
            // ...try to find one using the selector component
            fuImpls = selector->fuImplementations(*fu);
            it = fuImpls.begin();

            // if an implementation was found, save it into the std::map
            if (fuImpls.size() > 0 && it->first != NULL) {
                const FUImplementationLocation* location = it->first;
                HdbIdPair locationInfo;
                locationInfo.hdbFile = hdb.fileName();
                locationInfo.id = location->id();

                foundFU_.insert(
                    std::pair<const FUImplementationLocation*, HdbIdPair>(
                        location, locationInfo));
            }
        }
    }

    delete selector;
}

/**
 * Creates the dialog widgets.
 */
wxSizer*
AutoSelectImplementationsDialog::createContents(
    wxWindow *parent, 
    bool call_fit, 
    bool set_sizer) {

    wxFlexGridSizer *item0 = new wxFlexGridSizer( 1, 0, 0 );
    item0->AddGrowableCol( 0 );
    item0->AddGrowableRow( 1 );

    wxBoxSizer *item1 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticText *itemText = new wxStaticText( parent, ID_TEXT, wxT("HDB file:"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( itemText, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxString *strs2 = (wxString*) NULL;
    wxChoice *item2 = new wxChoice( parent, ID_HDB_CHOICE, wxDefaultPosition, wxSize(250,-1), 0, strs2, 0 );
    item1->Add( item2, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item3 = new wxButton( parent, ID_BROWSE, wxT("Browse..."), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item3, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *buttonFind = new wxButton( parent, ID_FIND, wxT("Find"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( buttonFind, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item1, 0, wxALIGN_CENTER|wxALL, 5 );

    wxBoxSizer *cboxSizer = new wxBoxSizer( wxHORIZONTAL );

    wxStaticText *cboxText = new wxStaticText( parent, ID_TEXT, wxT("Do selections for:"), wxDefaultPosition, wxDefaultSize, 0 );
    cboxSizer->Add( cboxText, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxCheckBox *cboxRF = new wxCheckBox( parent, ID_RF, wxT("Register Files"), wxDefaultPosition, wxDefaultSize, 0 );
    cboxSizer->Add( cboxRF, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxCheckBox *cboxIU = new wxCheckBox( parent, ID_IU, wxT("Immediate Units"), wxDefaultPosition, wxDefaultSize, 0 );
    cboxSizer->Add( cboxIU, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxCheckBox *cboxFU = new wxCheckBox( parent, ID_FU, wxT("Function Units"), wxDefaultPosition, wxDefaultSize, 0 );
    cboxSizer->Add( cboxFU, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
    
    item0->Add( cboxSizer, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticLine *item5 = new wxStaticLine( parent, ID_LINE, wxDefaultPosition, wxSize(20,-1), wxLI_HORIZONTAL );
    item0->Add( item5, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
    
    wxBoxSizer *item6 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *closeButton = new wxButton( parent, ID_CLOSE, wxT("Close"), wxDefaultPosition, wxDefaultSize, 0 );
    item6->Add( closeButton, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item6, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );


    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }

    return item0;
}
