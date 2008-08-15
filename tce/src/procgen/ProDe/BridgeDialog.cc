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
 * @file BridgeDialog.cc
 *
 * Definition of BridgeDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <string>
#include <boost/format.hpp>

#include "BridgeDialog.hh"
#include "ProDeConstants.hh"
#include "WxConversion.hh"
#include "ModelConstants.hh"
#include "Conversion.hh"
#include "WarningDialog.hh"
#include "ErrorDialog.hh"
#include "InformationDialog.hh"
#include "Machine.hh"
#include "Bridge.hh"
#include "Bus.hh"
#include "UserManualCmd.hh"
#include "MachineTester.hh"
#include "ProDeTextGenerator.hh"
#include "MDFView.hh"
#include "ProDe.hh"
#include "MachineCanvasTool.hh"
#include "WidgetTools.hh"
#include "GUITextGenerator.hh"
#include "MachineCanvas.hh"

using std::string;
using boost::format;
using namespace TTAMachine;

BEGIN_EVENT_TABLE(BridgeDialog, wxDialog)
    EVT_TEXT(ID_NAME, BridgeDialog::onName)
    EVT_TEXT(ID_OPPOSITE_BRIDGE, BridgeDialog::onName)
    EVT_BUTTON(wxID_OK, BridgeDialog::onOK)
    EVT_BUTTON(wxID_CANCEL, BridgeDialog::onCancel)
    EVT_CHOICE(ID_INPUT_BUS, BridgeDialog::onInputBus)
    EVT_CHOICE(ID_OUTPUT_BUS, BridgeDialog::onOutputBus)
    EVT_CHECKBOX(ID_BIDIRECTIONAL, BridgeDialog::onBidirectional)

END_EVENT_TABLE()

/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param bridge Bridge to be modified with the dialog.
 * @param opposite Bridge to be modified having the opposite direction
 *                 compared to bridge.
 */
BridgeDialog::BridgeDialog(
    wxWindow* parent,
    Bridge* bridge,
    Bridge* opposite):
    wxDialog(parent, -1, _T(""), wxDefaultPosition),
    name_(_T("")),
    oppositeName_(_T("")),
    oppositeNameCtrl_(NULL),
    inputBusChoice_(NULL),
    outputBusChoice_(NULL),
    bidirectionalBox_(NULL) {

    assert(bridge != NULL);

    // The canvas tool has to disabled.
    MDFView* view =
        dynamic_cast<MDFView*>(wxGetApp().docManager()->GetCurrentView());
    view->canvas()->tool()->deactivate();


    // initialize the dialog
    createContents(this, true, true);
    FindWindow(wxID_OK)->Disable();

    wxTextCtrl* nameCtrl = dynamic_cast<wxTextCtrl*>(FindWindow(ID_NAME));
    assert(nameCtrl != 0);
    nameCtrl->SetValidator(wxTextValidator(wxFILTER_ASCII, &name_));

    oppositeNameCtrl_ =
        dynamic_cast<wxTextCtrl*>(FindWindow(ID_OPPOSITE_BRIDGE));
    assert(oppositeNameCtrl_ != 0);
    oppositeNameCtrl_->SetValidator(wxTextValidator(wxFILTER_ASCII,
                                                    &oppositeName_));

    inputBusChoice_ = dynamic_cast<wxChoice*>(FindWindow(ID_INPUT_BUS));
    assert(inputBusChoice_ != NULL);
    outputBusChoice_ = dynamic_cast<wxChoice*>(FindWindow(ID_OUTPUT_BUS));
    assert(outputBusChoice_ != NULL);

    bidirectionalBox_ =
        dynamic_cast<wxCheckBox*>(FindWindow(ID_BIDIRECTIONAL));
    assert(bidirectionalBox_ != NULL);


    // Machine tester is used to check legality of the bridge modifications.
    // The modified bridge has to be deleted. Otherwise it will conflict
    // with the modification legality checks.

    // save bridge information and delete the bridges
    machine_ = bridge->machine();
    inputBus_ = WxConversion::toWxString(bridge->sourceBus()->name());
    outputBus_ = WxConversion::toWxString(bridge->destinationBus()->name());
    name_ = WxConversion::toWxString(bridge->name());
    if (opposite != NULL) {
        bidirectional_ = true;
        oppositeName_ = WxConversion::toWxString(opposite->name());
        assert(opposite->sourceBus() == bridge->destinationBus());
        assert(opposite->destinationBus() == bridge->sourceBus());
        delete opposite;
    } else {
        bidirectional_ = false;
    }
    delete bridge;

    // set widget texts
    setTexts();

    TransferDataToWindow();
}


/**
 * The Destructor.
 */
BridgeDialog::~BridgeDialog() {
}


/**
 * Sets texts for widgets.
 */
void
BridgeDialog::setTexts() {
    GUITextGenerator* generator = GUITextGenerator::instance();
    ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();

    // Dialog title
    format fmt = prodeTexts->text(ProDeTextGenerator::TXT_BRIDGE_DIALOG_TITLE);
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
                          ProDeTextGenerator::TXT_LABEL_NAME);

    WidgetTools::setLabel(prodeTexts, FindWindow(ID_LABEL_OPPOSITE_NAME),
                          ProDeTextGenerator::TXT_LABEL_OPPOSITE_BRIDGE);

    WidgetTools::setLabel(prodeTexts, FindWindow(ID_LABEL_INPUT_BUS),
                          ProDeTextGenerator::TXT_LABEL_INPUT_BUS);

    WidgetTools::setLabel(prodeTexts, FindWindow(ID_OUTPUT_BUS),
                          ProDeTextGenerator::TXT_LABEL_OUTPUT_BUS);

    WidgetTools::setLabel(prodeTexts, FindWindow(ID_BIDIRECTIONAL),
                          ProDeTextGenerator::TXT_LABEL_BIDIRECTIONAL);
}


/**
 * Transfers data from the bridge object(s) to the dialog widgets.
 *
 * @return False, if an error occured in the transfer, true otherwise.
 */
bool
BridgeDialog::TransferDataToWindow() {

   bidirectionalBox_->SetValue(bidirectional_);
    if (bidirectional_) {
        oppositeNameCtrl_->Enable();
    } else {
        oppositeNameCtrl_->Disable();
    }

    updateBusChoices();

    bool transferResult = wxWindow::TransferDataToWindow();

    // Force update of the OK-button enabled/disabled state.
    wxCommandEvent dummy;
    onName(dummy);

    return transferResult;
}


/**
 * Updates the bus choicers.
 */
void
BridgeDialog::updateBusChoices() {

    Machine::BusNavigator navigator = machine_->busNavigator();
    MachineTester tester(*machine_);
    Bus* source = navigator.item(WxConversion::toString(inputBus_));

    // Add input buses which can be bridged.
    inputBusChoice_->Clear();
    for (int i = 0;i < navigator.count(); i++) {
        wxString busName = WxConversion::toWxString(navigator.item(i)->name());
        for (int j = 0;j < navigator.count(); j++) {
            if (tester.canBridge(*navigator.item(i), *navigator.item(j))) {
                inputBusChoice_->Append(busName);
                break;
            }
        }
    }
    inputBusChoice_->SetStringSelection(inputBus_);

    // Add outputbuses which are legal for the current input bus.
    outputBusChoice_->Clear();
    for (int i = 0;i < navigator.count(); i++) {
        wxString busName = WxConversion::toWxString(navigator.item(i)->name());
        if (tester.canBridge(*source, *navigator.item(i))) {
            outputBusChoice_->Append(busName);
        }
    }
    outputBusChoice_->SetStringSelection(outputBus_);
}


/**
 * Validates input in the controls, and updates the Bridge object.
 */
void
BridgeDialog::onOK(wxCommandEvent&) {

    if (!Validate()) {
        return;
    }

    if (!TransferDataFromWindow()) {
        return;
    }

    string trimmedName = WxConversion::toString(name_.Trim(false).Trim(true));
    string trimmedOppName =
        WxConversion::toString(oppositeName_.Trim(false).Trim(true));

    // Check the name validity.
    if (!MachineTester::isValidComponentName(trimmedName) ||
        (bidirectional_ &&
         !MachineTester::isValidComponentName(trimmedOppName))) {

        ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();
        format message =
            prodeTexts->text(ProDeTextGenerator::MSG_ERROR_ILLEGAL_NAME);
        InformationDialog warning(
            this, WxConversion::toWxString(message.str()));
        warning.ShowModal();
        return;
    }

    // Check that the opposing bridge names aren't same.
    if (bidirectional_ && trimmedName == trimmedOppName) {
        ProDeTextGenerator* generator = ProDeTextGenerator::instance();
        format message =
            generator->text(ProDeTextGenerator::MSG_ERROR_BRIDGE_NAMES);
        WarningDialog warning(this, WxConversion::toWxString(message.str()));
        warning.ShowModal();
        return;
    }

    Machine::BridgeNavigator navigator = machine_->bridgeNavigator();

    // Check that the opposite bridge name is not reserved.
    if (bidirectional_ && navigator.hasItem(trimmedOppName)) {
        ProDeTextGenerator* generator =
            ProDeTextGenerator::instance();
        format message =
            generator->text(ProDeTextGenerator::MSG_ERROR_SAME_NAME);
        string component =
            generator->text(ProDeTextGenerator::COMP_A_BRIDGE).str();
        message % trimmedOppName % component;
        WarningDialog warning(this, WxConversion::toWxString(message.str()));
        warning.ShowModal();
        return;
    }
    // Check that the bridge name is not reserved.
    if (navigator.hasItem(trimmedName)) {
        ProDeTextGenerator* generator =
            ProDeTextGenerator::instance();
        format message =
            generator->text(ProDeTextGenerator::MSG_ERROR_SAME_NAME);
        string acomponent =
            generator->text(ProDeTextGenerator::COMP_A_BRIDGE).str();
        string machine =
            generator->text(ProDeTextGenerator::COMP_MACHINE).str();
        string component =
            generator->text(ProDeTextGenerator::COMP_BRIDGE).str();

        message % trimmedName % acomponent % machine % component;
        WarningDialog warning(this, WxConversion::toWxString(message.str()));
        warning.ShowModal();
        return;
    }

    Machine::BusNavigator busNavigator = machine_->busNavigator();
    MachineTester tester(*machine_);
    Bus* source = busNavigator.item(WxConversion::toString(inputBus_));
    Bus* destination = busNavigator.item(WxConversion::toString(outputBus_));

    // Add the new/modified bridges to the machine.
    new Bridge(trimmedName, *source, *destination);

    if (bidirectional_ && tester.canBridge(*destination, *source)) {
        new Bridge(trimmedOppName, *destination, *source);
    }
    MDFView* view =
        dynamic_cast<MDFView*>(wxGetApp().docManager()->GetCurrentView());
    view->canvas()->tool()->activate();
    EndModal(wxID_OK);
}


void
BridgeDialog::onCancel(wxCommandEvent&) {
    MDFView* view =
        dynamic_cast<MDFView*>(wxGetApp().docManager()->GetCurrentView());
    view->canvas()->tool()->activate();
    EndModal(wxID_CANCEL);
}

/**
 * Checks whether name field is empty and disables OK button of the
 * dialog if it is.
 */
void
BridgeDialog::onName(wxCommandEvent&) {

    wxTextCtrl* nameCtrl = dynamic_cast<wxTextCtrl*>(FindWindow(ID_NAME));
    wxTextCtrl* oppNameCtrl =
        dynamic_cast<wxTextCtrl*>(FindWindow(ID_OPPOSITE_BRIDGE));

    wxString name = nameCtrl->GetValue();
    wxString trimmedName = name.Trim(false).Trim(true);
    wxString oppositeName = oppNameCtrl->GetValue();
    wxString trimmedOppositeName = oppositeName.Trim(false).Trim(true);

    if (trimmedName == _T("") ||
        (bidirectional_ && trimmedOppositeName == _T(""))) {

        FindWindow(wxID_OK)->Disable();
    } else {
        FindWindow(wxID_OK)->Enable();
    }
}


/**
 * Handles changes in input bus control.
 *
 * Checks that input bus is not the same as output bus. If it is the
 * output bus is changed to the previous value of the input bus.
 */
void
BridgeDialog::onOutputBus(wxCommandEvent&) {
    wxString outputBus = outputBusChoice_->GetStringSelection();
    outputBus_ = outputBus;
    updateBusChoices();
}


/**
 * Handles changes in the input bus control.
 *
 * Output bus choicer is updated to contain only valid output busses
 * for the selected input bus.
 */
void
BridgeDialog::onInputBus(wxCommandEvent&) {

    string outputBus =
        WxConversion::toString(outputBusChoice_->GetStringSelection());
    string inputBus =
        WxConversion::toString(inputBusChoice_->GetStringSelection());

    MachineTester tester(*machine_);
    Machine::BusNavigator navigator = machine_->busNavigator();
    Bus* source = navigator.item(inputBus);
    Bus* destination = navigator.item(outputBus);

    if (!tester.canBridge(*source, *destination)) {
        outputBus = "";
        for (int i=0; i < navigator.count(); i++) {
            if (tester.canBridge(*source, *navigator.item(i))) {
                outputBus = navigator.item(i)->name();
                break;
            }
        }
        assert(outputBus != "");
        outputBus_ = WxConversion::toWxString(outputBus);
    }

    inputBus_ = WxConversion::toWxString(inputBus);
    updateBusChoices();
}


/**
 * Handles changes in bidirectional control.
 *
 * When bdirectional is on, the opposite bridge name control is
 * enabled, otherwise it is disabled.
 */
void
BridgeDialog::onBidirectional(wxCommandEvent&) {
    if (bidirectionalBox_->IsChecked()) {
        oppositeNameCtrl_->Enable();
        bidirectional_ = true;
    } else {
        oppositeNameCtrl_->Disable();
        bidirectional_ = false;
    }
    wxCommandEvent dummy;
    onName(dummy);
}


/**
 * Creates the dialog window contents.
 *
 * This method was initially generated with wxDesigner, thus the ugly code
 * and too long lines.
 *
 * @return Main sizer of the created contents.
 * @param parent The dialog window.
 * @param call_fit If true, fits the contents inside the dialog.
 * @param set_sizer If true, sets the main sizer as dialog contents.
 */
wxSizer*
BridgeDialog::createContents(wxWindow *parent, bool call_fit, bool set_sizer) {

    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *item1 = new wxBoxSizer( wxHORIZONTAL );

    wxFlexGridSizer *item2 = new wxFlexGridSizer( 2, 0, 0 );

    wxStaticText *item3 = new wxStaticText( parent, ID_LABEL_NAME, wxT("Name:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item3, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxTextCtrl *item4 = new wxTextCtrl( parent, ID_NAME, wxT(""), wxDefaultPosition, wxSize(120,-1), 0 );
    item2->Add( item4, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item5 = new wxStaticText( parent, ID_LABEL_INPUT_BUS, wxT("Input Bus:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item5, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxString *strs6 = (wxString*) NULL;
    wxChoice *item6 = new wxChoice( parent, ID_INPUT_BUS, wxDefaultPosition, wxSize(100,-1), 0, strs6, 0 );
    item2->Add( item6, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticText *item7 = new wxStaticText( parent, ID_LABEL_OUTPUT_BUS, wxT("Output Bus:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item7, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxString *strs8 = (wxString*) NULL;
    wxChoice *item8 = new wxChoice( parent, ID_OUTPUT_BUS, wxDefaultPosition, wxSize(100,-1), 0, strs8, 0 );
    item2->Add( item8, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item1->Add( item2, 0, wxALIGN_CENTER|wxALL, 5 );

    wxBoxSizer *item9 = new wxBoxSizer( wxVERTICAL );

    wxCheckBox *item10 = new wxCheckBox( parent, ID_BIDIRECTIONAL, wxT("Bidirectional"), wxDefaultPosition, wxDefaultSize, 0 );
    item9->Add( item10, 0, wxALIGN_CENTER|wxALL, 5 );

    item9->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item11 = new wxStaticText( parent, ID_LABEL_OPPOSITE_NAME, wxT("Opposite Bridge Name:"), wxDefaultPosition, wxDefaultSize, 0 );
    item9->Add( item11, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 15 );

    wxTextCtrl *item12 = new wxTextCtrl( parent, ID_OPPOSITE_BRIDGE, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    item9->Add( item12, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item1->Add( item9, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    item0->Add( item1, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticLine *item13 = new wxStaticLine( parent, ID_LINE, wxDefaultPosition, wxSize(20,-1), wxLI_HORIZONTAL );
    item0->Add( item13, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxGridSizer *item14 = new wxGridSizer( 2, 0, 0 );

    wxButton *item15 = new wxButton( parent, ID_HELP, wxT("&Help"), wxDefaultPosition, wxDefaultSize, 0 );
    item14->Add( item15, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item16 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item17 = new wxButton( parent, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item16->Add( item17, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item18 = new wxButton( parent, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item16->Add( item18, 0, wxALIGN_CENTER|wxALL, 5 );

    item14->Add( item16, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item0->Add( item14, 0, wxALIGN_CENTER, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }

    return item0;
}
