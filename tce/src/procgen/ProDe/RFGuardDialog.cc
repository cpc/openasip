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
 * @file RFGuardDialog.cc
 *
 * Implementation of RFGuardDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel-no.spam-cs.tut.fi)
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

    RegisterFile* rf = selectedRF();
    for (int i = 0; i < rf->numberOfRegisters(); i++) {
        wxString index = WxConversion::toWxString(i);
        indexChoice_->Append(index);
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
RegisterFile*
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
        new RegisterGuard(newInverted_, *selectedRF(), newIndex_, *bus_);
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
        new RegisterGuard(inverted_, *rf_, index_, *bus_);
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
    item1->Add( item2, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxString *strs3 = (wxString*) NULL;
    wxChoice *item3 = new wxChoice( parent, ID_RF_NAME, wxDefaultPosition, wxSize(100,-1), 0, strs3, 0 );
    item1->Add( item3, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticText *item4 = new wxStaticText( parent, ID_LABEL_INDEX, wxT("Register Index:"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item4, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxString *strs5 = (wxString*) NULL;
    wxChoice *item5 = new wxChoice( parent, ID_RF_INDEX, wxDefaultPosition, wxSize(100,-1), 0, strs5, 0 );
    item1->Add( item5, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item0->Add( item1, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxCheckBox *item6 = new wxCheckBox( parent, ID_INVERTED, wxT("Inverted"), wxDefaultPosition, wxDefaultSize, 0 );
    item0->Add( item6, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticLine *item7 = new wxStaticLine( parent, ID_LINE, wxDefaultPosition, wxSize(20,-1), wxLI_HORIZONTAL );
    item0->Add( item7, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

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
