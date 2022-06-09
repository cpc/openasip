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
 * @file AddWatchDialog.cc
 *
 * Implementation of AddWatchDialog class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <string>
#include <vector>
#include <wx/statline.h>
#include <wx/spinctrl.h>
#include "AddWatchDialog.hh"
#include "ProximToolbox.hh"
#include "Machine.hh"
#include "RegisterFile.hh"
#include "FunctionUnit.hh"
#include "Port.hh"
#include "AddressSpace.hh"
#include "WxConversion.hh"
#include "ErrorDialog.hh"
#include "ProximLineReader.hh"
#include "SimulatorFrontend.hh"
#include "SimulatorInterpreter.hh"
#include "ProximConstants.hh"
#include "ExpressionScript.hh"
#include "TclConditionScript.hh"
#include "Conversion.hh"

using namespace TTAMachine;
using std::string;
using std::vector;

BEGIN_EVENT_TABLE(AddWatchDialog, wxDialog)
    EVT_CHOICE(ID_FU_CHOICE, AddWatchDialog::onFUChoice)
    EVT_CHOICE(ID_RF_CHOICE, AddWatchDialog::onRFChoice)
    EVT_CHOICE(ID_ADDRESS_SPACE, AddWatchDialog::onASChoice)

    EVT_RADIOBUTTON(ID_RB_MEMORY, AddWatchDialog::onWatchTypeChange)
    EVT_RADIOBUTTON(ID_RB_REGISTER, AddWatchDialog::onWatchTypeChange)
    EVT_RADIOBUTTON(ID_RB_PORT, AddWatchDialog::onWatchTypeChange)
    EVT_RADIOBUTTON(ID_RB_BUS, AddWatchDialog::onWatchTypeChange)
    EVT_RADIOBUTTON(ID_RB_EXPRESSION, AddWatchDialog::onWatchTypeChange)

    EVT_BUTTON(wxID_OK, AddWatchDialog::onOK)
END_EVENT_TABLE()

/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param id Dialog identifier.
 * @param watch Watch point to edit, or NULL to create a new watch point.
 */
AddWatchDialog::AddWatchDialog(
    wxWindow* parent, wxWindowID id, Watch* /*watch*/) :
  wxDialog(parent, id, _T("Watch point"), wxDefaultPosition) {

    createContents(this, true, true);

    // Initialize widget pointers.
    asChoice_ = dynamic_cast<wxChoice*>(FindWindow(ID_ADDRESS_SPACE));
    rfChoice_ = dynamic_cast<wxChoice*>(FindWindow(ID_RF_CHOICE));
    indexChoice_ = dynamic_cast<wxChoice*>(FindWindow(ID_INDEX_CHOICE));
    fuChoice_ = dynamic_cast<wxChoice*>(FindWindow(ID_FU_CHOICE));
    portChoice_ = dynamic_cast<wxChoice*>(FindWindow(ID_PORT_CHOICE));
    busChoice_ = dynamic_cast<wxChoice*>(FindWindow(ID_BUS_CHOICE));

    endAddressCtrl_ = dynamic_cast<wxSpinCtrl*>(FindWindow(ID_END_ADDRESS));
    startAddressCtrl_ =
        dynamic_cast<wxSpinCtrl*>(FindWindow(ID_START_ADDRESS));

    initialize();
}


/**
 * The Destructor.
 */
AddWatchDialog::~AddWatchDialog() {
}

/**
 * Initializes the dialog choicer items.
 */
void
AddWatchDialog::initialize() {

    Machine machine = ProximToolbox::machine();

    // Address space choicer.
    const Machine::AddressSpaceNavigator asNavigator =
        machine.addressSpaceNavigator();
    for (int i = 0; i < asNavigator.count(); i++) {
        string asName = asNavigator.item(i)->name();
        asChoice_->Append(WxConversion::toWxString(asName));
    }

    // Register file choicer.
    const Machine::RegisterFileNavigator rfNavigator =
        machine.registerFileNavigator();
    for (int i = 0; i < rfNavigator.count(); i++) {
        string rfName = rfNavigator.item(i)->name();
        rfChoice_->Append(WxConversion::toWxString(rfName));
    }

    // Function unit choicer.
    const Machine::FunctionUnitNavigator fuNavigator =
        machine.functionUnitNavigator();
    for (int i = 0; i < fuNavigator.count(); i++) {
        string fuName = fuNavigator.item(i)->name();
        fuChoice_->Append(WxConversion::toWxString(fuName));
    }

    // Bus choicer.
    const Machine::BusNavigator busNavigator = machine.busNavigator();
    for (int i = 0; i < busNavigator.count(); i++) {
        string busName = busNavigator.item(i)->name();
        busChoice_->Append(WxConversion::toWxString(busName));
    }

    wxCommandEvent dummy;
    dummy.SetId(ID_RB_PORT);
    onWatchTypeChange(dummy);
}

/**
 * Disables and enables dialog widgets when the watch type selection changes.
 *
 * @param event Radio-button event of the watch type changing.
 */
void
AddWatchDialog::onWatchTypeChange(wxCommandEvent& event) {

    // Disable all watch type specific widgets.
    FindWindow(ID_LABEL_AS)->Disable();
    FindWindow(ID_LABEL_UNIT)->Disable();
    FindWindow(ID_LABEL_PORT)->Disable();
    FindWindow(ID_LABEL_RF)->Disable();
    FindWindow(ID_LABEL_BUS)->Disable();
    FindWindow(ID_LABEL_START_ADDRESS)->Disable();
    FindWindow(ID_LABEL_END_ADDRESS)->Disable();
    FindWindow(ID_LABEL_INDEX)->Disable();
    FindWindow(ID_START_ADDRESS)->Disable();
    FindWindow(ID_END_ADDRESS)->Disable();
    FindWindow(ID_BUS_CHOICE)->Disable();
    FindWindow(ID_RF_CHOICE)->Disable();
    FindWindow(ID_FU_CHOICE)->Disable();
    FindWindow(ID_PORT_CHOICE)->Disable();
    FindWindow(ID_INDEX_CHOICE)->Disable();
    FindWindow(ID_EXPRESSION)->Disable();
    FindWindow(ID_ADDRESS_SPACE)->Disable();

    // Enable widgets associated to the selected watch type.

    if (event.GetId() == ID_RB_MEMORY) {
        FindWindow(ID_LABEL_AS)->Enable();
        FindWindow(ID_LABEL_START_ADDRESS)->Enable();
        FindWindow(ID_LABEL_END_ADDRESS)->Enable();
        FindWindow(ID_START_ADDRESS)->Enable();
        FindWindow(ID_END_ADDRESS)->Enable();
    }
    if (event.GetId() == ID_RB_PORT) {
        FindWindow(ID_LABEL_UNIT)->Enable();
        FindWindow(ID_LABEL_PORT)->Enable();
        FindWindow(ID_FU_CHOICE)->Enable();
        FindWindow(ID_PORT_CHOICE)->Enable();
    }
    if (event.GetId() == ID_RB_REGISTER) {
        FindWindow(ID_LABEL_RF)->Enable();
        FindWindow(ID_LABEL_INDEX)->Enable();
        FindWindow(ID_RF_CHOICE)->Enable();
        FindWindow(ID_INDEX_CHOICE)->Enable();
    }
    if (event.GetId() == ID_RB_BUS) {
        FindWindow(ID_LABEL_BUS)->Enable();
        FindWindow(ID_BUS_CHOICE)->Enable();
    }
    if (event.GetId() == ID_RB_EXPRESSION) {
        FindWindow(ID_EXPRESSION)->Enable();
    }
}

/**
 * Sets the function unit port choicer items when the function unit selection
 * changes.
 */
void
AddWatchDialog::onFUChoice(wxCommandEvent&) {

    string fuName =
        WxConversion::toString(fuChoice_->GetStringSelection());

    const Machine::FunctionUnitNavigator fuNavigator =
        ProximToolbox::machine().functionUnitNavigator();

    FunctionUnit* fu = fuNavigator.item(fuName);

    assert(fu != NULL);

    // Set the port choicer to contain an item for each of the selected
    // function unit's ports.
    portChoice_->Clear();
    for (int i = 0; i < fu->portCount(); i++) {
        string portName = fu->port(i)->name();
        portChoice_->Append(WxConversion::toWxString(portName));
    }
    portChoice_->SetSelection(0);
}

/**
 * Sets the register index choicer items when the register file selection
 * changes.
 */
void
AddWatchDialog::onRFChoice(wxCommandEvent&) {

    string rfName =
        WxConversion::toString(rfChoice_->GetStringSelection());

    const Machine::RegisterFileNavigator rfNavigator =
        ProximToolbox::machine().registerFileNavigator();

    RegisterFile* rf = rfNavigator.item(rfName);

    assert(rf != NULL);

    // Set the index choicer to contain an item for each of the selected
    // register file's indexes.
    indexChoice_->Clear();
    for (int i = 0; i < rf->numberOfRegisters(); i++) {
        indexChoice_->Append(WxConversion::toWxString(i));
    }
    indexChoice_->SetSelection(0);
}

/**
 * Event handler for the address space choicer.
 *
 * Sets the minimum and maximum start/end address values of the memory
 * watch spin-buttons when the address space selection is changed.
 */
void
AddWatchDialog::onASChoice(wxCommandEvent&) {

    string asName =
        WxConversion::toString(asChoice_->GetStringSelection());

    const Machine::AddressSpaceNavigator asNavigator =
        ProximToolbox::machine().addressSpaceNavigator();

    AddressSpace* as = asNavigator.item(asName);

    assert(as != NULL);

    startAddressCtrl_->SetRange(as->start(), as->end());
    endAddressCtrl_->SetRange(as->start(), as->end());

    startAddressCtrl_->SetValue(as->start());
    endAddressCtrl_->SetValue(as->end());
}

/**
 * Event handler for the dialog OK-button.
 */
void
AddWatchDialog::onOK(wxCommandEvent&) {
    string expression;


    // Memory watch.
    if (dynamic_cast<wxRadioButton*>(FindWindow(ID_RB_MEMORY))->GetValue()) {

        string asName =
            WxConversion::toString(asChoice_->GetStringSelection());

        unsigned start = startAddressCtrl_->GetValue();
        unsigned end = endAddressCtrl_->GetValue();
        int count = end - start + 1;

        string startStr = Conversion::toString(start);
        string countStr = Conversion::toString(count);

        if (asName == "") {
            wxString message = _T("Invalid address space.");
            ErrorDialog error(this, message);
            error.ShowModal();
            return;
        }
        if (count < 1) {
            wxString message = _T("The memory watch end address must be\n");
            message.Append(_T("greater than the start address."));
            ErrorDialog error(this, message);
            error.ShowModal();
            return;
        }

        expression = "x " " /a " + asName + " /n " + countStr + " " + startStr;
    }

    // Port Watch.
    if (dynamic_cast<wxRadioButton*>(FindWindow(ID_RB_PORT))->GetValue()) {

        string fuName =
            WxConversion::toString(fuChoice_->GetStringSelection());
        string portName =
            WxConversion::toString(portChoice_->GetStringSelection());
        if (fuName == "" || portName == "") {
            wxString message = _T("Invalid port.");
            ErrorDialog error(this, message);
            error.ShowModal();
            return;
        }
        expression = "info ports " + fuName + " " + portName;
    }

    // Register watch.
    if (dynamic_cast<wxRadioButton*>(FindWindow(ID_RB_REGISTER))->GetValue()) {

        string rfName =
            WxConversion::toString(rfChoice_->GetStringSelection());
        string index = Conversion::toString(indexChoice_->GetSelection());

        if (rfName == "") {
            wxString message = _T("Invalid register.");
            ErrorDialog error(this, message);
            error.ShowModal();
            return;
        }
        expression = "info registers " + rfName + " " + index;
    }

    // Bus watch.
    if (dynamic_cast<wxRadioButton*>(FindWindow(ID_RB_BUS))->GetValue()) {

        string busName =
            WxConversion::toString(busChoice_->GetStringSelection());

        if (busName == "") {
            wxString message = _T("Invalid bus.");
            ErrorDialog error(this, message);
            error.ShowModal();
            return;
        }
        expression = "info segments " + busName;
    }

    // User defined expression watch.
    bool expressionWatch = 
        dynamic_cast<wxRadioButton*>(FindWindow(ID_RB_EXPRESSION))->GetValue();
    if (expressionWatch) {
        wxTextCtrl* expressionCtrl =
            dynamic_cast<wxTextCtrl*>(FindWindow(ID_EXPRESSION));

        expression = WxConversion::toString(
            expressionCtrl->GetValue().Trim(true).Trim(false));
    }

    if (expression == "") {
        wxString message = _T("No expression defined for the watch");
        ErrorDialog error(this, message);
        error.ShowModal();
        return;
    }

    // Test expression script.
    if (!ProximToolbox::testExpression(this, expression)) {
        // Invalid expression, error dialog is displayed by the toolbox.
        return;
    }

    ProximLineReader& lineReader = ProximToolbox::lineReader();
    lineReader.input(ProximConstants::SCL_ADD_WATCH);
    lineReader.input(expression);

    EndModal(wxID_OK);
}


/**
 * Creates the dialog widgets.
 *
 * Code generated by wxDesigner. Do not modify manually.
 *
 * @param parent Parent window for the dialog widgets.
 * @param call_fit If true, the dialog is resized to fit the widgets.
 * @param set_sizer If true, the created widgets are set as the dialog
 *                  contents.
 */
wxSizer*
AddWatchDialog::createContents(wxWindow *parent, bool call_fit, bool set_sizer) {

    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *item1 = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *item2 = new wxBoxSizer( wxHORIZONTAL );

    wxRadioButton *item3 = new wxRadioButton( parent, ID_RB_PORT, wxT("Port watch"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item4 = new wxBoxSizer( wxHORIZONTAL );

    item4->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item5 = new wxStaticText( parent, ID_LABEL_UNIT, wxT("Unit:"), wxDefaultPosition, wxDefaultSize, 0 );
    item4->Add( item5, 0, wxALIGN_CENTER|wxALL, 5 );

    wxString *strs6 = (wxString*) NULL;
    wxChoice *item6 = new wxChoice( parent, ID_FU_CHOICE, wxDefaultPosition, wxSize(100,-1), 0, strs6, 0 );
    item4->Add( item6, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item7 = new wxStaticText( parent, ID_LABEL_PORT, wxT("Port:"), wxDefaultPosition, wxDefaultSize, 0 );
    item4->Add( item7, 0, wxALIGN_CENTER|wxALL, 5 );

    wxString *strs8 = (wxString*) NULL;
    wxChoice *item8 = new wxChoice( parent, ID_PORT_CHOICE, wxDefaultPosition, wxSize(100,-1), 0, strs8, 0 );
    item4->Add( item8, 0, wxALIGN_CENTER|wxALL, 5 );

    item2->Add( item4, 0, wxALIGN_CENTER|wxALL, 5 );

    item1->Add( item2, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item9 = new wxBoxSizer( wxHORIZONTAL );

    wxRadioButton *item10 = new wxRadioButton( parent, ID_RB_REGISTER, wxT("Register watch"), wxDefaultPosition, wxDefaultSize, 0 );
    item9->Add( item10, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item11 = new wxBoxSizer( wxHORIZONTAL );

    item11->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item12 = new wxStaticText( parent, ID_LABEL_RF, wxT("Register file:"), wxDefaultPosition, wxDefaultSize, 0 );
    item11->Add( item12, 0, wxALIGN_CENTER|wxALL, 5 );

    wxString *strs13 = (wxString*) NULL;
    wxChoice *item13 = new wxChoice( parent, ID_RF_CHOICE, wxDefaultPosition, wxSize(100,-1), 0, strs13, 0 );
    item11->Add( item13, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item14 = new wxStaticText( parent, ID_LABEL_INDEX, wxT("Index:"), wxDefaultPosition, wxDefaultSize, 0 );
    item11->Add( item14, 0, wxALIGN_CENTER|wxALL, 5 );

    wxString *strs15 = (wxString*) NULL;
    wxChoice *item15 = new wxChoice( parent, ID_INDEX_CHOICE, wxDefaultPosition, wxSize(100,-1), 0, strs15, 0 );
    item11->Add( item15, 0, wxALIGN_CENTER|wxALL, 5 );

    item9->Add( item11, 0, wxALIGN_CENTER|wxALL, 5 );

    item1->Add( item9, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item16 = new wxBoxSizer( wxHORIZONTAL );

    wxRadioButton *item17 = new wxRadioButton( parent, ID_RB_BUS, wxT("Bus watch"), wxDefaultPosition, wxDefaultSize, 0 );
    item16->Add( item17, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item18 = new wxBoxSizer( wxHORIZONTAL );

    item18->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item19 = new wxStaticText( parent, ID_LABEL_BUS, wxT("Bus:"), wxDefaultPosition, wxDefaultSize, 0 );
    item18->Add( item19, 0, wxALIGN_CENTER|wxALL, 5 );

    wxString *strs20 = (wxString*) NULL;
    wxChoice *item20 = new wxChoice( parent, ID_BUS_CHOICE, wxDefaultPosition, wxSize(100,-1), 0, strs20, 0 );
    item18->Add( item20, 0, wxALIGN_CENTER|wxALL, 5 );

    item16->Add( item18, 0, wxALIGN_CENTER|wxALL, 5 );

    item1->Add( item16, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item21 = new wxBoxSizer( wxVERTICAL );

    wxRadioButton *item22 = new wxRadioButton( parent, ID_RB_MEMORY, wxT("Memory watch"), wxDefaultPosition, wxDefaultSize, 0 );
    item21->Add( item22, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item23 = new wxBoxSizer( wxHORIZONTAL );

    item23->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item24 = new wxStaticText( parent, ID_LABEL_AS, wxT("Address space:"), wxDefaultPosition, wxDefaultSize, 0 );
    item23->Add( item24, 0, wxALIGN_CENTER|wxALL, 5 );

    wxString *strs25 = (wxString*) NULL;
    wxChoice *item25 = new wxChoice( parent, ID_ADDRESS_SPACE, wxDefaultPosition, wxSize(100,-1), 0, strs25, 0 );
    item23->Add( item25, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item26 = new wxStaticText( parent, ID_LABEL_START_ADDRESS, wxT("Start address:"), wxDefaultPosition, wxDefaultSize, 0 );
    item23->Add( item26, 0, wxALIGN_CENTER|wxALL, 5 );

    wxSpinCtrl *item27 = new wxSpinCtrl( parent, ID_START_ADDRESS, wxT("0"), wxDefaultPosition, wxSize(100,-1), 0, 0, 100, 0 );
    item23->Add( item27, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item28 = new wxStaticText( parent, ID_LABEL_END_ADDRESS, wxT("End address:"), wxDefaultPosition, wxDefaultSize, 0 );
    item23->Add( item28, 0, wxALIGN_CENTER|wxALL, 5 );

    wxSpinCtrl *item29 = new wxSpinCtrl( parent, ID_END_ADDRESS, wxT("0"), wxDefaultPosition, wxSize(100,-1), 0, 0, 100, 0 );
    item23->Add( item29, 0, wxALIGN_CENTER|wxALL, 5 );

    item21->Add( item23, 0, wxALIGN_CENTER|wxALL, 5 );

    item1->Add( item21, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item30 = new wxBoxSizer( wxHORIZONTAL );

    wxRadioButton *item31 = new wxRadioButton( parent, ID_RB_EXPRESSION, wxT("Experession:"), wxDefaultPosition, wxDefaultSize, 0 );
    item30->Add( item31, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    wxTextCtrl *item32 = new wxTextCtrl( parent, ID_EXPRESSION, wxT(""), wxDefaultPosition, wxSize(500,-1), 0 );
    item30->Add( item32, 0, wxALIGN_CENTER|wxALL, 5 );

    item1->Add( item30, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item0->Add( item1, 0, wxGROW|wxALL, 5 );

    wxBoxSizer *item33 = new wxBoxSizer( wxVERTICAL );

    wxStaticLine *item34 = new wxStaticLine( parent, ID_LINE, wxDefaultPosition, wxSize(20,-1), wxLI_HORIZONTAL );
    item33->Add( item34, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item35 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item36 = new wxButton( parent, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item35->Add( item36, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item37 = new wxButton( parent, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item35->Add( item37, 0, wxALIGN_CENTER|wxALL, 5 );

    item33->Add( item35, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item0->Add( item33, 0, wxGROW|wxALIGN_BOTTOM|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;
}
