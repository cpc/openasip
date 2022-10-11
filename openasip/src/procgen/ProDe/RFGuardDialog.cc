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
 * @file RFGuardDialog.cc
 *
 * Implementation of RFGuardDialog class.
 *
 * @author Veli-Pekka Jääskeläinen 2004 (vjaaskel-no.spam-cs.tut.fi)
 */

#include <wx/wx.h>
#include <wx/statline.h>
#include <wx/valgen.h>
#include <boost/format.hpp>

#include "RFGuardDialog.hh"
#include "Guard.hh"
#include "WxConversion.hh"
#include "Machine.hh"
#include "RegisterFile.hh"
#include "InformationDialog.hh"
#include "WidgetTools.hh"
#include "GUITextGenerator.hh"
#include "ProDeTextGenerator.hh"

using boost::format;
using std::string;
using namespace TTAMachine;

BEGIN_EVENT_TABLE(RFGuardDialog, wxDialog)
    EVT_CHOICE(ID_RF_NAME, RFGuardDialog::onRFChoice)
    EVT_BUTTON(wxID_OK, RFGuardDialog::onOK)
    EVT_BUTTON(wxID_CANCEL, RFGuardDialog::onCancel)
END_EVENT_TABLE()

/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 */
RFGuardDialog::RFGuardDialog(
    wxWindow* parent,
    Bus* bus,
    RegisterGuard* guard):
    wxDialog(parent, -1, _T(""), wxDefaultPosition),
    inverted_(false),
    newInverted_(false),
    index_(0),
    newIndex_(0),
    rf_(NULL),
    bus_(bus),
    adding_(false) {


    if (guard == NULL) {
        // adding a new guard
        adding_ = true;
        rf_ = bus_->machine()->registerFileNavigator().item(0);
    } else {
        // editing an old guard
        rf_ = guard->registerFile();
        index_ = guard->registerIndex();
        newIndex_ = index_;
        inverted_ = guard->isInverted();

        // The guard is temporarily deleted to simplify legality checks.
        delete guard;
        guard = NULL;
    }
    newInverted_ = inverted_;

    createContents(this, true, true);

    // set pointers to the dialog widgets
    nameChoice_ = dynamic_cast<wxChoice*>(FindWindow(ID_RF_NAME));
    indexChoice_ = dynamic_cast<wxChoice*>(FindWindow(ID_RF_INDEX));
    invertedBox_ = dynamic_cast<wxCheckBox*>(FindWindow(ID_INVERTED));
    
    invertedBox_->SetValidator(wxGenericValidator(&newInverted_));
    indexChoice_->SetValidator(wxGenericValidator(&newIndex_));

    // set widget texts.
    setTexts();
}


/**
 * The Destructor.
 */
RFGuardDialog::~RFGuardDialog() {
}


/**
 * Sets texts for widgets.
 */
void
RFGuardDialog::setTexts() {
    GUITextGenerator* generator = GUITextGenerator::instance();
    ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();

    // Dialog title
    format fmt = prodeTexts->text(
        ProDeTextGenerator::TXT_RF_GUARD_DIALOG_TITLE);
    SetTitle(WxConversion::toWxString(fmt.str()));

    // buttons
    WidgetTools::setLabel(generator, FindWindow(wxID_OK),
                          GUITextGenerator::TXT_BUTTON_OK);

    WidgetTools::setLabel(generator, FindWindow(wxID_CANCEL),
                          GUITextGenerator::TXT_BUTTON_CANCEL);

    WidgetTools::setLabel(generator, FindWindow(ID_HELP),
                          GUITextGenerator::TXT_BUTTON_HELP);

    // widget labels
    WidgetTools::setLabel(prodeTexts, FindWindow(ID_LABEL_NAME),
                          ProDeTextGenerator::TXT_LABEL_RF_NAME);

    WidgetTools::setLabel(prodeTexts, FindWindow(ID_LABEL_INDEX),
                          ProDeTextGenerator::TXT_LABEL_REGISTER_INDEX);

    WidgetTools::setLabel(prodeTexts, FindWindow(ID_INVERTED),
                          ProDeTextGenerator::TXT_LABEL_INVERTED);
}


/**
 * Transfers data from the Guard object to the dialog widgets.
 *
 * @return true, if the transfer was succesful, false otherwise.
 */
bool
RFGuardDialog::TransferDataToWindow() {
    nameChoice_->Clear();
    indexChoice_->Clear();

    Machine::RegisterFileNavigator navigator =
        bus_->machine()->registerFileNavigator();

    // set register file name
    for (int i = 0; i < navigator.count(); i++) {
        wxString name = WxConversion::toWxString(navigator.item(i)->name());
        nameChoice_->Append(name);
    }
    nameChoice_->SetStringSelection(WxConversion::toWxString(rf_->name()));

    // set register index
    for (int i = 0; i < selectedRF()->numberOfRegisters(); i++) {
        wxString index = WxConversion::toWxString(i);
        indexChoice_->Append(index);
    }
    
    // add choice to create guard for every index
    if (adding_) {
        ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();
        format fmtAll = prodeTexts->text(ProDeTextGenerator::TXT_ALL);
        indexChoice_->Append(WxConversion::toWxString(fmtAll.str()));
    }
    
    indexChoice_->SetSelection(newIndex_);

    // set inverted flag
    invertedBox_->SetValue(newInverted_);

    return wxDialog::TransferDataToWindow();
}


/**
 * Updates the index choice when the register file selection is changed.
 */
void
RFGuardDialog::onRFChoice(wxCommandEvent&) {

    int selection = indexChoice_->GetSelection();

    indexChoice_->Clear();

    const RegisterFile* rf = selectedRF();
    for (int i = 0; i < rf->numberOfRegisters(); i++) {
        wxString index = WxConversion::toWxString(i);
        indexChoice_->Append(index);
    }
    
    // add choice to create guard for every index
    if (adding_) {
        ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();
        format fmtAll = prodeTexts->text(ProDeTextGenerator::TXT_ALL);
        indexChoice_->Append(WxConversion::toWxString(fmtAll.str()));
    }
    
    if (static_cast<int>(indexChoice_->GetCount()) >= selection) {
        indexChoice_->SetSelection(selection);
    } else {
        indexChoice_->SetSelection(0);
    }
}


/**
 * Returns a pointer to the selected register file.
 *
 * @return Pointer to the selected refister file.
 */
const RegisterFile*
RFGuardDialog::selectedRF() const {
    string name = WxConversion::toString(nameChoice_->GetStringSelection());
    Machine::RegisterFileNavigator navigator =
        bus_->machine()->registerFileNavigator();
    RegisterFile* rf = navigator.item(name);
    return rf;
}


/**
 * Updates the guard object when the OK-button is pressed.
 *
 * Closes the dialog.
 */
void
RFGuardDialog::onOK(wxCommandEvent&) {
    TransferDataFromWindow();
    try {
        // get currently selected index and its string representation
        int indexSelection = indexChoice_->GetSelection();
        assert(indexSelection != wxNOT_FOUND);
        wxString choiceText = indexChoice_->GetString(indexSelection);
        
        // get the string that resembles all indices
        ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();
        format fmtAll = prodeTexts->text(ProDeTextGenerator::TXT_ALL);
        bool singleGuard = true;

        // check if user has selected all indices
        if (choiceText.IsSameAs(WxConversion::toWxString(fmtAll.str()))) {
            singleGuard = false;
        }

        if (!adding_ || (adding_ && singleGuard)) {
            // add / edit one guard
            new RegisterGuard(newInverted_, *selectedRF(), newIndex_, 
                              bus_);
        } else {
            // add new guard for every index

            string rfNameSel = WxConversion::toString(
                            nameChoice_->GetStringSelection());
            
            // loop all bus guards and mark old guards (if there is any) 
            // of the selected register file name to be deleted
            std::vector<RegisterGuard*> toBeDeleted;
            for (int i = 0; i < bus_->guardCount(); ++i) {
                Guard* g = bus_->guard(i);
                RegisterGuard* rfGuard = 
                    dynamic_cast<RegisterGuard*>(g);
                
                if (rfGuard != NULL) {
                    string rfName = rfGuard->registerFile()->name();

                    // if register file names match
                    if (rfName.compare(rfNameSel) == 0) {
                        // if inverted setting is same
                        if (rfGuard->isInverted() == newInverted_) {
                            toBeDeleted.push_back(rfGuard);
                        }
                    }
                }
            }

            // delete old guards
            for (unsigned int i = 0; i < toBeDeleted.size(); ++i) {
                if (toBeDeleted.at(i) != NULL) {
                    delete toBeDeleted.at(i);
                    toBeDeleted.at(i) = NULL;
                }
            }

            // create new guard for every file register's index
            for (int index = 0; index < selectedRF()->numberOfRegisters(); 
                 ++index) {
                new RegisterGuard(newInverted_, *selectedRF(), index, 
                              bus_);
            }
        }
    } catch (ComponentAlreadyExists& e) {
        ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();
        format message =
            prodeTexts->text(ProDeTextGenerator::MSG_ERROR_GUARD_EXISTS);
        InformationDialog dialog(
            this, WxConversion::toWxString(message.str()));
        dialog.ShowModal();
        return;
    }
    
    EndModal(wxID_OK);
}


/**
 * Cancels the dialog effects by creating the original register guard.
 *
 * Closes the dialog.
 */
void
RFGuardDialog::onCancel(wxCommandEvent&) {
    if (adding_ == false) {
        new RegisterGuard(inverted_, *rf_, index_, bus_);
    }
    EndModal(wxID_CANCEL);
}


/**
 * Creates contents of the dialog window.
 *
 * Code generated with wxDesigner.
 *
 * @param parent Parent dialog of the contents.
 * @param call_fit If true, fits sizer in dialog window.
 * @param set_sizer If true, sets sizer as dialog's sizer.
 * @return Top level sizer of the contents.
 */
wxSizer*
RFGuardDialog::createContents(
    wxWindow *parent, bool call_fit, bool set_sizer) {

    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxGridSizer *item1 = new wxGridSizer( 2, 0, 0 );

    wxStaticText *item2 = new wxStaticText( parent, ID_LABEL_NAME, wxT("Register File Name:"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item2, 0, wxALL, 5 );

    wxString *strs3 = (wxString*) NULL;
    wxChoice *item3 = new wxChoice( parent, ID_RF_NAME, wxDefaultPosition, wxSize(100,-1), 0, strs3, 0 );
    item1->Add( item3, 0, wxGROW|wxALL, 5 );

    wxStaticText *item4 = new wxStaticText( parent, ID_LABEL_INDEX, wxT("Register Index:"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item4, 0, wxALL, 5 );

    wxString *strs5 = (wxString*) NULL;
    wxChoice *item5 = new wxChoice( parent, ID_RF_INDEX, wxDefaultPosition, wxSize(100,-1), 0, strs5, 0 );
    item1->Add( item5, 0, wxGROW|wxALL, 5 );

    item0->Add( item1, 0, wxGROW|wxALL, 5 );

    wxCheckBox *item6 = new wxCheckBox( parent, ID_INVERTED, wxT("Inverted"), wxDefaultPosition, wxDefaultSize, 0 );
    item0->Add( item6, 0, wxALL, 5 );

    wxStaticLine *item7 = new wxStaticLine( parent, ID_LINE, wxDefaultPosition, wxSize(20,-1), wxLI_HORIZONTAL );
    item0->Add( item7, 0, wxGROW|wxALL, 5 );

    wxBoxSizer *item8 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item9 = new wxButton( parent, ID_HELP, wxT("&Help"), wxDefaultPosition, wxDefaultSize, 0 );
    item8->Add( item9, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item10 = new wxButton( parent, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item8->Add( item10, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item11 = new wxButton( parent, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item8->Add( item11, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item8, 0, wxALIGN_CENTER|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }

    return item0;
}
