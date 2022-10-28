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
 * @file RFDialog.cc
 *
 * Definition of RFDialog class.
 *
 * @author Veli-Pekka Jääskeläinen 2004 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <wx/valgen.h>
#include <boost/format.hpp>

#include "RFDialog.hh"
#include "RFPortDialog.hh"
#include "ModelConstants.hh"
#include "ProDeConstants.hh"
#include "WxConversion.hh"
#include "Conversion.hh"
#include "WarningDialog.hh"
#include "InformationDialog.hh"
#include "Machine.hh"
#include "MachineTester.hh"
#include "RegisterFile.hh"
#include "UserManualCmd.hh"
#include "Port.hh"
#include "WidgetTools.hh"
#include "GUITextGenerator.hh"
#include "ProDeTextGenerator.hh"

using boost::format;
using std::string;
using namespace TTAMachine;

BEGIN_EVENT_TABLE(RFDialog, wxDialog)
    EVT_TEXT(ID_NAME, RFDialog::onName)
    EVT_BUTTON(ID_ADD_PORT, RFDialog::onAddPort)
    EVT_BUTTON(ID_DELETE_PORT, RFDialog::onDeletePort)
    EVT_BUTTON(ID_EDIT_PORT, RFDialog::onEditPort)
    EVT_MENU(ID_EDIT_PORT, RFDialog::onEditPort)
    EVT_MENU(ID_DELETE_PORT, RFDialog::onDeletePort)
    EVT_LIST_ITEM_ACTIVATED(ID_PORT_LIST, RFDialog::onActivatePort)
    EVT_LIST_ITEM_RIGHT_CLICK(ID_PORT_LIST, RFDialog::onPortRightClick)

    EVT_BUTTON(wxID_OK, RFDialog::onOK)

    EVT_LIST_ITEM_FOCUSED(ID_PORT_LIST, RFDialog::onPortSelection)
    EVT_LIST_DELETE_ITEM(ID_PORT_LIST, RFDialog::onPortSelection)
    EVT_LIST_ITEM_SELECTED(ID_PORT_LIST, RFDialog::onPortSelection)
    EVT_LIST_ITEM_DESELECTED(ID_PORT_LIST, RFDialog::onPortSelection)
END_EVENT_TABLE()


/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param registerFile Register file to add/edit.
 */
RFDialog::RFDialog(
    wxWindow* parent,
    RegisterFile* registerFile):
    wxDialog(parent, -1, _T(""), wxDefaultPosition),
    registerFile_(registerFile),
    name_(_T("")),
    size_(1),
    width_(ModelConstants::DEFAULT_WIDTH),
    maxReads_(0),
    maxWrites_(0),
    zeroRegister_(false),
    portListCtrl_(NULL),
    typeChoice_(NULL) {

    //registerFile_->updateMaxReadsAndWrites();
    createContents(this, true, true);

    FindWindow(wxID_OK)->Disable();
    FindWindow(ID_EDIT_PORT)->Disable();
    FindWindow(ID_DELETE_PORT)->Disable();

    FindWindow(ID_NAME)->SetValidator(wxTextValidator(wxFILTER_ASCII, &name_));

    portListCtrl_ = dynamic_cast<wxListCtrl*>(FindWindow(ID_PORT_LIST));
    typeChoice_ = dynamic_cast<wxChoice*>(FindWindow(ID_TYPE));

    // Set widget validators.
    FindWindow(ID_WIDTH)->SetValidator(wxGenericValidator(&width_));
    FindWindow(ID_SIZE)->SetValidator(wxGenericValidator(&size_));
    FindWindow(ID_GUARD_LATENCY)->SetValidator(
        wxGenericValidator(&guardLatency_));

    FindWindow(ID_ZERO_REGISTER)->SetValidator(
        wxGenericValidator(&zeroRegister_));

    // set widget texts
    setTexts();

    TransferDataToWindow();
}


/**
 * The Destructor.
 */
RFDialog::~RFDialog() {
}


/**
 * Sets texts for widgets.
 */
void
RFDialog::setTexts() {
    GUITextGenerator* generator = GUITextGenerator::instance();
    ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();

    // Dialog title
    format fmt = prodeTexts->text(ProDeTextGenerator::TXT_RF_DIALOG_TITLE);
    SetTitle(WxConversion::toWxString(fmt.str()));

    // buttons
    WidgetTools::setLabel(generator, FindWindow(wxID_OK),
                          GUITextGenerator::TXT_BUTTON_OK);

    WidgetTools::setLabel(generator, FindWindow(wxID_CANCEL),
                          GUITextGenerator::TXT_BUTTON_CANCEL);

    WidgetTools::setLabel(generator, FindWindow(ID_HELP),
                          GUITextGenerator::TXT_BUTTON_HELP);

    WidgetTools::setLabel(generator, FindWindow(ID_ADD_PORT),
                          GUITextGenerator::TXT_BUTTON_ADD_DIALOG);

    WidgetTools::setLabel(generator, FindWindow(ID_EDIT_PORT),
                          GUITextGenerator::TXT_BUTTON_EDIT_DIALOG);

    WidgetTools::setLabel(generator, FindWindow(ID_DELETE_PORT),
                          GUITextGenerator::TXT_BUTTON_DELETE);

    // widget labels
    WidgetTools::setLabel(prodeTexts, FindWindow(ID_LABEL_NAME),
                          ProDeTextGenerator::TXT_LABEL_NAME);

    WidgetTools::setLabel(prodeTexts, FindWindow(ID_LABEL_TYPE),
                          ProDeTextGenerator::TXT_LABEL_TYPE);

    WidgetTools::setLabel(prodeTexts, FindWindow(ID_LABEL_SIZE),
                          ProDeTextGenerator::TXT_LABEL_SIZE);

    WidgetTools::setLabel(prodeTexts, FindWindow(ID_LABEL_WIDTH),
                          ProDeTextGenerator::TXT_LABEL_WIDTH);

    WidgetTools::setLabel(prodeTexts, FindWindow(ID_LABEL_MAX_READS),
                          ProDeTextGenerator::TXT_LABEL_MAX_READS);

    WidgetTools::setLabel(prodeTexts, FindWindow(ID_LABEL_MAX_WRITES),
                          ProDeTextGenerator::TXT_LABEL_MAX_WRITES);

    WidgetTools::setLabel(prodeTexts, FindWindow(ID_LABEL_GUARD_LATENCY),
                          ProDeTextGenerator::TXT_LABEL_GUARD_LATENCY);

    // choicer texts
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_RF_TYPE_NORMAL);
    typeChoice_->Append(WxConversion::toWxString(fmt.str()));
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_RF_TYPE_RESERVED);
    typeChoice_->Append(WxConversion::toWxString(fmt.str()));
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_RF_TYPE_VOLATILE);
    typeChoice_->Append(WxConversion::toWxString(fmt.str()));

    // box sizer label
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_RF_PORTS_BOX);
    WidgetTools::setWidgetLabel(portsSizer_, fmt.str());

    // Create port list column.
    wxListCtrl* portList =
        dynamic_cast<wxListCtrl*>(FindWindow(ID_PORT_LIST));
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_COLUMN_NAME);
    portList->InsertColumn(0, WxConversion::toWxString(fmt.str()),
                           wxLIST_FORMAT_LEFT, 200);
}

/**
 * Transfers data from the RegisterFile object to the dialog widgets.
 *
 * @return false, if an error occured in the transfer.
 */
bool
RFDialog::TransferDataToWindow() {

    updateWidgets();

    return wxWindow::TransferDataToWindow();
}


/**
 * Reads data form the RegisterFile and updates GUI.
 *
 */
void
RFDialog::updateWidgets() {

    //registerFile_->updateMaxReadsAndWrites();

    name_ = WxConversion::toWxString(registerFile_->name());
    size_ = registerFile_->numberOfRegisters();
    width_ = registerFile_->width();
    maxReads_ = registerFile_->maxReads();
    maxWrites_ = registerFile_->maxWrites();
    guardLatency_ = registerFile_->guardLatency();
    zeroRegister_ = registerFile_->zeroRegister();

    switch (registerFile_->type()) {
    case RegisterFile::NORMAL:
        typeChoice_->SetSelection(0);
        break;
    case RegisterFile::RESERVED:
        typeChoice_->SetSelection(1);
        break;
    case RegisterFile::VOLATILE:
        typeChoice_->SetSelection(2);
        break;
    default:
        assert(false);
    }

    updatePortList();

    // wxWidgets GTK1 version seems to bug with spincontrol validators.
    // The widget value has to be set manually.
    dynamic_cast<wxSpinCtrl*>(FindWindow(ID_SIZE))->SetValue(size_);
    dynamic_cast<wxSpinCtrl*>(FindWindow(ID_WIDTH))->SetValue(width_);
    dynamic_cast<wxStaticText*>(FindWindow(ID_MAX_READS))->SetLabel(
            WxConversion::toWxString(maxReads_));
    dynamic_cast<wxStaticText*>(FindWindow(ID_MAX_WRITES))->SetLabel(
            WxConversion::toWxString(maxWrites_));
    dynamic_cast<wxSpinCtrl*>(
        FindWindow(ID_GUARD_LATENCY))->SetValue(guardLatency_);
    dynamic_cast<wxCheckBox*>(
    FindWindow(ID_ZERO_REGISTER))->SetValue(zeroRegister_);
}

/**
 * Updates the port list widget.
 */
void
RFDialog::updatePortList() {
    portListCtrl_->DeleteAllItems();
    for (int i=0; i < registerFile_->portCount(); i++) {

        wxString name =
            WxConversion::toWxString(registerFile_->port(i)->name());

        portListCtrl_->InsertItem(i, name);
    }
}


/**
 * Validates input in the controls, and updates the RegisterFile.
 */
void
RFDialog::onOK(wxCommandEvent&) {
    if (!Validate()) {
        return;
    }

    if (!TransferDataFromWindow()) {
        return;
    }

    string trimmedName =
        WxConversion::toString(name_.Trim(false).Trim(true));

    // Check the name validity.
    if (!MachineTester::isValidComponentName(trimmedName)) {
        ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();
        format message =
            prodeTexts->text(ProDeTextGenerator::MSG_ERROR_ILLEGAL_NAME);
        InformationDialog warning(
            this, WxConversion::toWxString(message.str()));
        warning.ShowModal();
        return;
    }

    if (registerFile_->name() != trimmedName) {

        Machine::RegisterFileNavigator navigator =
            registerFile_->machine()->registerFileNavigator();

        for (int i = 0; i < navigator.count(); i++) {
            RegisterFile* rf = navigator.item(i);
            if (trimmedName == rf->name()) {
                ProDeTextGenerator* prodeTexts =
                    ProDeTextGenerator::instance();
                format message =
                    prodeTexts->text(ProDeTextGenerator::MSG_ERROR_SAME_NAME);
                format a_rf =
                    prodeTexts->text(ProDeTextGenerator::COMP_A_REGISTER_FILE);
                format machine =
                    prodeTexts->text(ProDeTextGenerator::COMP_MACHINE);
                format rf =
                    prodeTexts->text(ProDeTextGenerator::COMP_REGISTER_FILE);
                message % trimmedName % a_rf.str() % machine.str() % rf.str();
                WarningDialog warning(
                    this, WxConversion::toWxString(message.str()));
                warning.ShowModal();
                return;
            }
        }
    }

    try {
        registerFile_->setName(trimmedName);
        registerFile_->setWidth(width_);
        registerFile_->setNumberOfRegisters(size_);
        registerFile_->setGuardLatency(guardLatency_);
        registerFile_->setZeroRegister(zeroRegister_);
    } catch (Exception& e) {
        InformationDialog dialog(
            this, WxConversion::toWxString(e.errorMessage()));
        dialog.ShowModal();
        return;
    }

    // set register file type
    switch (typeChoice_->GetSelection()) {
    case 0:
        registerFile_->setType(RegisterFile::NORMAL);
        break;
    case 1:
        registerFile_->setType(RegisterFile::RESERVED);
        break;
    case 2:
        registerFile_->setType(RegisterFile::VOLATILE);
        break;
    default:
        assert(false);
    }

    //registerFile_->updateMaxReadsAndWrites();
    EndModal(wxID_OK);
}


/**
 * Checks whether name field is empty and disables OK button of the
 * dialog if it is.
 */
void
RFDialog::onName(wxCommandEvent&) {
    if (!TransferDataFromWindow()) {
        assert(false);
    }
    wxString trimmedName = name_.Trim(false).Trim(true);
    if (trimmedName == _T("")) {
        FindWindow(wxID_OK)->Disable();
    } else {
        FindWindow(wxID_OK)->Enable();
    }
}


/**
 * Returns pointer to the port which is selected on the port list.
 *
 * @return Pointer to the port which is selected on the port list.
 */
Port*
RFDialog::selectedPort() const {
    long item = -1;
    item = portListCtrl_->GetNextItem(
        item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

    if (item == -1) {
        return NULL;
    }

    string portName =
        WxConversion::toString(portListCtrl_->GetItemText(item));

    return registerFile_->port(portName);
}


/**
 * Creates and shows an empty RFPortDialog for adding ports.
 */
void
RFDialog::onAddPort(wxCommandEvent&) {

    // Generate name for the new port.
    int i = 1;
    string newName = ProDeConstants::COMP_NEW_NAME_PREFIX_PORT +
        Conversion::toString(i);
    while (registerFile_->hasPort(newName)) {
        newName = ProDeConstants::COMP_NEW_NAME_PREFIX_PORT +
            Conversion::toString(i);
        i++;
    }

    RFPort* port = new RFPort(newName, *registerFile_);
    RFPortDialog portDialog(this, port);
    if (portDialog.ShowModal() == wxID_OK) {
        updatePortList();
    } else {
        delete port;
    }

    updateWidgets();

}


/**
 * Deletes selected port from the port list.
 */
void
RFDialog::onDeletePort(wxCommandEvent&) {
    Port* selected = selectedPort();
    assert(selected != NULL);
    delete selected;
    updatePortList();

    updateWidgets();
}


/**
 * Handles left mouse button double clicks on the port list.
 */
void
RFDialog::onActivatePort(wxListEvent&) {
    wxCommandEvent dummy;
    onEditPort(dummy);
}


/**
 * Handles the 'Edit Port' button event.
 *
 * Opens a RFPortDialog with the selected port's attributes set.
 */
void
RFDialog::onEditPort(wxCommandEvent&) {

    Port* port = selectedPort();
    if (port == NULL) {
        // No port selected.
        return;
    }

    RFPortDialog portDialog(this, port);
    if (portDialog.ShowModal() == wxID_OK) {
        updatePortList();
    }
}

/**
 * Disables and enables Edit and Delete buttons under the port list.
 *
 * If a port is selected, buttons are enabled. If no port is selected the
 * buttons will be disabled.
 */
void
RFDialog::onPortSelection(wxListEvent&) {
    if (portListCtrl_->GetSelectedItemCount() != 1) {
        FindWindow(ID_DELETE_PORT)->Disable();
        FindWindow(ID_EDIT_PORT)->Disable();
        return;
    }
    FindWindow(ID_DELETE_PORT)->Enable();
    FindWindow(ID_EDIT_PORT)->Enable();
}


/**
 * Opens a pop-up menu when right mouse button was pressed.
 *
 * @param event Information about right mouse click event.
 */
void
RFDialog::onPortRightClick(wxListEvent& event) {

    portListCtrl_->SetItemState(event.GetIndex(), wxLIST_STATE_SELECTED,
                                wxLIST_STATE_SELECTED);

    wxMenu* contextMenu = new wxMenu();

    ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();
    format button = prodeTexts->text(
        ProDeTextGenerator::TXT_LABEL_BUTTON_EDIT);
    contextMenu->Append(
        ID_EDIT_PORT, WxConversion::toWxString(button.str()));
    button = prodeTexts->text(
        ProDeTextGenerator::TXT_LABEL_BUTTON_DELETE);
    contextMenu->Append(
        ID_DELETE_PORT, WxConversion::toWxString(button.str()));
    portListCtrl_->PopupMenu(contextMenu, event.GetPoint());
}


/**
 * Creates the dialog window contents.
 *
 * This method was generated with wxDesigner.
 *
 * @return Main sizer of the created contents.
 * @param parent The dialog window.
 * @param call_fit If true, fits the contents inside the dialog.
 * @param set_sizer If true, sets the main sizer as dialog contents.
 */
wxSizer*
RFDialog::createContents(wxWindow *parent, bool call_fit, bool set_sizer) {

    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxFlexGridSizer *item1 = new wxFlexGridSizer( 2, 0, 0 );

    wxStaticText *item2 = new wxStaticText( parent, ID_LABEL_NAME, wxT("Name:"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item2, 0, wxALIGN_RIGHT|wxALL, 5 );

    wxTextCtrl *item3 = new wxTextCtrl( parent, ID_NAME, wxT(""), wxDefaultPosition, wxSize(200,-1), 0 );
    item1->Add( item3, 0, wxGROW|wxALL, 5 );

    wxStaticText *item4 = new wxStaticText( parent, ID_LABEL_TYPE, wxT("Type:"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item4, 0, wxALIGN_RIGHT|wxALL, 5 );

    wxString *strs5 = (wxString*) NULL;
    wxChoice *item5 = new wxChoice( parent, ID_TYPE, wxDefaultPosition, wxSize(100,-1), 0, strs5, 0 );
    item1->Add( item5, 0, wxGROW|wxALL, 5 );

    wxStaticText *item6 = new wxStaticText( parent, ID_LABEL_SIZE, wxT("Size:"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item6, 0, wxALIGN_RIGHT|wxALL, 5 );

    wxSpinCtrl *item7 = new wxSpinCtrl( parent, ID_SIZE, wxT("1"), wxDefaultPosition, wxSize(-1,-1), 0, 1, 10000, 1 );
    item1->Add( item7, 0, wxGROW|wxALL, 5 );

    wxStaticText *item8 = new wxStaticText( parent, ID_LABEL_WIDTH, wxT("Width:"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item8, 0, wxALIGN_RIGHT|wxALL, 5 );

    wxSpinCtrl *item9 = new wxSpinCtrl( parent, ID_WIDTH, wxT("1"), wxDefaultPosition, wxSize(-1,-1), 0, 1, 10000, 1 );
    item1->Add( item9, 0, wxGROW|wxALL, 5 );

    wxStaticText *item10 = new wxStaticText( parent, ID_LABEL_MAX_READS, wxT("Max-Reads:"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item10, 0, wxALIGN_RIGHT|wxALL, 5 );

    wxStaticText *item11 = new wxStaticText( parent, ID_MAX_READS, wxT("0"), wxDefaultPosition, wxSize(100,-1), 0);
    item1->Add( item11, 0, wxGROW|wxALL, 5 );

    wxStaticText *item12 = new wxStaticText( parent, ID_LABEL_MAX_WRITES, wxT("Max-Writes:"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item12, 0, wxALIGN_RIGHT|wxALL, 5 );

    wxStaticText *item13 = new wxStaticText( parent, ID_MAX_WRITES, wxT("0"), wxDefaultPosition, wxSize(100,-1), 0);
    item1->Add( item13, 0, wxGROW|wxALL, 5 );

    wxStaticText *item14 = new wxStaticText( parent, ID_LABEL_GUARD_LATENCY, wxT("Guard Latency:"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item14, 0, wxALIGN_RIGHT|wxALL, 5 );

    wxSpinCtrl *item15 = new wxSpinCtrl( parent, ID_GUARD_LATENCY, wxT("0"), wxDefaultPosition, wxSize(-1,-1), 0, 0, 1, 0 );
    item1->Add( item15, 0, wxGROW|wxALL, 5 );

    item0->Add( item1, 0, wxGROW|wxALL, 5 );

    wxStaticBox *item17 = new wxStaticBox( parent, -1, wxT("Ports:") );
    wxStaticBoxSizer *item16 = new wxStaticBoxSizer( item17, wxVERTICAL );
    portsSizer_ = item16;

    wxListCtrl *item18 = new wxListCtrl( parent, ID_PORT_LIST, wxDefaultPosition, wxSize(160,120), wxLC_REPORT|wxSUNKEN_BORDER );
    item16->Add( item18, 0, wxGROW|wxALL, 5 );

    wxBoxSizer *item19 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item20 = new wxButton( parent, ID_ADD_PORT, wxT("&Add.."), wxDefaultPosition, wxDefaultSize, 0 );
    item19->Add( item20, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item21 = new wxButton( parent, ID_EDIT_PORT, wxT("&Edit..."), wxDefaultPosition, wxDefaultSize, 0 );
    item19->Add( item21, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item22 = new wxButton( parent, ID_DELETE_PORT, wxT("&Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    item19->Add( item22, 0, wxALIGN_CENTER|wxALL, 5 );

    item16->Add( item19, 0, wxALIGN_CENTER|wxLEFT|wxRIGHT, 5 );

    item0->Add( item16, 0, wxALIGN_CENTER|wxALL, 5 );

    wxBoxSizer *item23 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item24 = new wxButton( parent, ID_HELP, wxT("&Help"), wxDefaultPosition, wxDefaultSize, 0 );
    item23->Add( item24, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item25 = new wxButton( parent, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item23->Add( item25, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item26 = new wxButton( parent, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item23->Add( item26, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item23, 0, wxALIGN_CENTER|wxALL, 5 );

    wxCheckBox *item27 = new wxCheckBox( parent, ID_ZERO_REGISTER, wxT("Zero register"), wxDefaultPosition, wxDefaultSize, 0);
    item1->Add( item27, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;
}
