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
 * @file IUDialog.cc
 *
 * Definition of IUDialog class.
 *
 * @author Veli-Pekka J��skel�inen 2004 (vjaaskel-no.spam-cs.tut.fi)
 */

#include <wx/valgen.h>
#include <boost/format.hpp>

#include "ModelConstants.hh"
#include "IUDialog.hh"
#include "IUPortDialog.hh"
#include "ProDeConstants.hh"
#include "WxConversion.hh"
#include "Conversion.hh"
#include "WarningDialog.hh"
#include "InformationDialog.hh"
#include "Machine.hh"
#include "ImmediateUnit.hh"
#include "UserManualCmd.hh"
#include "Port.hh"
#include "MachineTester.hh"
#include "WidgetTools.hh"
#include "GUITextGenerator.hh"
#include "ProDeTextGenerator.hh"

using boost::format;
using std::string;
using namespace TTAMachine;

BEGIN_EVENT_TABLE(IUDialog, wxDialog)
    EVT_TEXT(ID_NAME, IUDialog::onName)
    EVT_BUTTON(ID_ADD_PORT, IUDialog::onAddPort)
    EVT_BUTTON(ID_DELETE_PORT, IUDialog::onDeletePort)
    EVT_BUTTON(ID_EDIT_PORT, IUDialog::onEditPort)
    EVT_MENU(ID_EDIT_PORT, IUDialog::onEditPort)
    EVT_MENU(ID_DELETE_PORT, IUDialog::onDeletePort)
    EVT_LIST_ITEM_ACTIVATED(ID_PORT_LIST, IUDialog::onActivatePort)
    EVT_LIST_ITEM_RIGHT_CLICK(ID_PORT_LIST, IUDialog::onPortRightClick)

    EVT_BUTTON(wxID_OK, IUDialog::onOK)

    EVT_LIST_ITEM_FOCUSED(ID_PORT_LIST, IUDialog::onPortSelection)
    EVT_LIST_DELETE_ITEM(ID_PORT_LIST, IUDialog::onPortSelection)
    EVT_LIST_ITEM_SELECTED(ID_PORT_LIST, IUDialog::onPortSelection)
    EVT_LIST_ITEM_DESELECTED(ID_PORT_LIST, IUDialog::onPortSelection)
END_EVENT_TABLE()


/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param immediateUnit Immediate unit to be modified with the dialog.
 */
IUDialog::IUDialog(
    wxWindow* parent,
    ImmediateUnit* immediateUnit):
    wxDialog(parent, -1, _T(""), wxDefaultPosition),
    immediateUnit_(immediateUnit),
    name_(_T("")),
    size_(ModelConstants::DEFAULT_IU_SIZE),
    width_(ModelConstants::DEFAULT_WIDTH),
    cycles_(ModelConstants::DEFAULT_IU_CYCLES),
    portList_(NULL),
    templateList_(NULL),
    extensionBox_(NULL) {

    createContents(this, true, true);

    portList_ = dynamic_cast<wxListCtrl*>(FindWindow(ID_PORT_LIST));
    templateList_ = dynamic_cast<wxListCtrl*>(FindWindow(ID_TEMPLATE_LIST));
    extensionBox_ = dynamic_cast<wxRadioBox*>(FindWindow(ID_EXTENSION));

    FindWindow(ID_SIZE)->SetValidator(wxGenericValidator(&size_));
    FindWindow(ID_WIDTH)->SetValidator(wxGenericValidator(&width_));
    FindWindow(ID_NAME)->SetValidator(wxTextValidator(wxFILTER_ASCII, &name_));

    // set OK button disabled initially
    FindWindow(wxID_OK)->Disable();
    FindWindow(ID_DELETE_PORT)->Disable();
    FindWindow(ID_EDIT_PORT)->Disable();

    // set widget texts
    setTexts();

    TransferDataToWindow();
}


/**
 * The Destructor.
 */
IUDialog::~IUDialog() {
}


/**
 * Sets texts for widgets.
 */
void
IUDialog::setTexts() {
    GUITextGenerator* generator = GUITextGenerator::instance();
    ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();

    // Dialog title
    format fmt = prodeTexts->text(ProDeTextGenerator::TXT_IU_DIALOG_TITLE);
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

    WidgetTools::setLabel(prodeTexts, FindWindow(ID_LABEL_SIZE),
                          ProDeTextGenerator::TXT_LABEL_SIZE);

    WidgetTools::setLabel(prodeTexts, FindWindow(ID_LABEL_WIDTH),
                          ProDeTextGenerator::TXT_LABEL_WIDTH);

    WidgetTools::setLabel(prodeTexts, FindWindow(ID_EXTENSION),
                          ProDeTextGenerator::TXT_LABEL_EXTENSION);

    // box sizer label
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_IU_PORTS_BOX);
    WidgetTools::setWidgetLabel(portListSizer_, fmt.str());

    fmt = prodeTexts->text(ProDeTextGenerator::TXT_IU_TEMPLATES_BOX);
    WidgetTools::setWidgetLabel(templateListSizer_, fmt.str());

    // Radio button labels
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_RADIO_EXTENSION_ZERO);
    extensionBox_->SetString(0, WxConversion::toWxString(fmt.str()));
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_RADIO_EXTENSION_SIGN);
    extensionBox_->SetString(1, WxConversion::toWxString(fmt.str()));

    // Create list columns.
    wxListCtrl* portList =
        dynamic_cast<wxListCtrl*>(FindWindow(ID_PORT_LIST));
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_COLUMN_NAME);
    portList->InsertColumn(0, WxConversion::toWxString(fmt.str()),
                           wxLIST_FORMAT_LEFT, 240);
    wxListCtrl* templateList =
        dynamic_cast<wxListCtrl*>(FindWindow(ID_TEMPLATE_LIST));
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_COLUMN_NAME);
    templateList->InsertColumn(0, WxConversion::toWxString(fmt.str()),
                               wxLIST_FORMAT_LEFT, 240);

}


/**
 * Transfers data from the ImmediateUnit object to the dialog widgets.
 *
 * @return false, if an error occured in the transfer.
 */
bool
IUDialog::TransferDataToWindow() {

    name_ = WxConversion::toWxString(immediateUnit_->name());
    width_ = immediateUnit_->width();
    size_ = immediateUnit_->numberOfRegisters();
    cycles_ = immediateUnit_->latency();

    // transfer extension
    if (immediateUnit_->extensionMode() == Machine::ZERO) {
        wxString zero = ProDeConstants::EXTENSION_ZERO;
        extensionBox_->SetStringSelection(zero);
    } else if (immediateUnit_->extensionMode() == Machine::SIGN) {
        wxString sign = ProDeConstants::EXTENSION_SIGN;
        extensionBox_->SetStringSelection(sign);
    } else {
        assert(false);
    }

    updatePortList();
    updateTemplateList();

    // wxWidgets GTK1 version seems to bug with spincontrol validators.
    // The widget value has to be set manually.
    dynamic_cast<wxSpinCtrl*>(FindWindow(ID_WIDTH))->SetValue(width_);
    dynamic_cast<wxSpinCtrl*>(FindWindow(ID_SIZE))->SetValue(size_);

    return wxWindow::TransferDataToWindow();
}


/**
 * Validates input in the controls, and updates the ImmediateUnit.
 */
void
IUDialog::onOK(wxCommandEvent&) {

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

    if (trimmedName != immediateUnit_->name()) {

        Machine::ImmediateUnitNavigator navigator =
            immediateUnit_->machine()->immediateUnitNavigator();
        for (int i = 0; i < navigator.count(); i++) {
            ImmediateUnit* iu = navigator.item(i);
            if (trimmedName == iu->name()) {
                ProDeTextGenerator* prodeTexts =
                    ProDeTextGenerator::instance();
                format message =
                    prodeTexts->text(ProDeTextGenerator::MSG_ERROR_SAME_NAME);
                format an_iu = prodeTexts->text(
                    ProDeTextGenerator::COMP_AN_IMMEDIATE_UNIT);
                format machine =
                    prodeTexts->text(ProDeTextGenerator::COMP_MACHINE);
                format iu =
                    prodeTexts->text(ProDeTextGenerator::COMP_IMMEDIATE_UNIT);
                message % trimmedName % an_iu.str() % machine.str() % iu.str();
                WarningDialog warning(
                    this, WxConversion::toWxString(message.str()));
                warning.ShowModal();
                return;
            }
        }
    }

    // set IU attributes
    immediateUnit_->setName(trimmedName);
    immediateUnit_->setNumberOfRegisters(size_);
    immediateUnit_->setWidth(width_);

    wxString extension = extensionBox_->GetStringSelection();

    // set IU extension
    if (extension.IsSameAs(ProDeConstants::EXTENSION_ZERO)) {
        immediateUnit_->setExtensionMode(Machine::ZERO);
    } else if (extension.IsSameAs(ProDeConstants::EXTENSION_SIGN)) {
        immediateUnit_->setExtensionMode(Machine::SIGN);
    } else {
        assert(false);
    }

    EndModal(wxID_OK);
}


/**
 * Checks whether name field is empty and disables OK button of the
 * dialog if it is.
 */
void
IUDialog::onName(wxCommandEvent&) {
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
IUDialog::selectedPort() const {
    long item = -1;
    item = portList_->GetNextItem(
        item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

    if (item == -1) {
        return NULL;
    }

    string portName =
        WxConversion::toString(portList_->GetItemText(item));

    return immediateUnit_->port(portName);
}




/**
 * Creates and shows an empty IUPortDialog for adding ports.
 */
void
IUDialog::onAddPort(wxCommandEvent&) {

    // Generate name for the new port.
    int i = 1;
    string newName = ProDeConstants::COMP_NEW_NAME_PREFIX_PORT +
        Conversion::toString(i);
    while (immediateUnit_->hasPort(newName)) {
        newName = ProDeConstants::COMP_NEW_NAME_PREFIX_PORT +
            Conversion::toString(i);
        i++;
    }

    RFPort* port = new RFPort(newName, *immediateUnit_);
    IUPortDialog portDialog(this, port);
    if (portDialog.ShowModal() == wxID_OK) {
        updatePortList();
    } else {
        delete port;
    }
}


/**
 * Deletes selected port from the port list.
 */
void
IUDialog::onDeletePort(wxCommandEvent&) {
    Port* selected = selectedPort();
    assert(selected != NULL);
    delete selected;
    updatePortList();
}


/**
 * Handles left mouse button double click on the port list.
 */
void
IUDialog::onActivatePort(wxListEvent&) {
    wxCommandEvent dummy;
    onEditPort(dummy);
}


/**
 * Handles the 'Edit Port' button event.
 *
 * Opens a IUPortDialog with the selected port's attributes set.
 */
void
IUDialog::onEditPort(wxCommandEvent&) {

    Port* port = selectedPort();
    if (port == NULL) {
        // No port selected.
        return;
    }

    IUPortDialog portDialog(this, port);
    if (portDialog.ShowModal() == wxID_OK) {
        updatePortList();
    }
}

/**
 * Disables and enables Edit and Delete buttons under the port list.
 *
 * If a port is selected, buttons are enabled. If no port is selected the
 * buttons will be disabled.
 *
 * @param event ListEvent, which may have changed the selection.
 */
void
IUDialog::onPortSelection(wxListEvent&) {
    if (portList_->GetSelectedItemCount() != 1) {
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
IUDialog::onPortRightClick(wxListEvent& event) {

    portList_->SetItemState(event.GetIndex(), wxLIST_STATE_SELECTED,
                                wxLIST_STATE_SELECTED);

    wxMenu* contextMenu = new wxMenu();
    contextMenu->Append(ID_EDIT_PORT, _T("&Edit..."));
    contextMenu->Append(ID_DELETE_PORT, _T("&Delete"));
    portList_->PopupMenu(contextMenu, event.GetPoint());
}


/**
 * Updates 'Ports' list control.
 *
 * Clears all items from the choicer and adds all ports.
 */
void
IUDialog::updatePortList() {
    portList_->DeleteAllItems();
    for (int i=0; i < immediateUnit_->portCount(); i++) {

        wxString portName =
            WxConversion::toWxString(immediateUnit_->port(i)->name());

        portList_->InsertItem(i, portName);
    }
}


/**
 * Updates 'Templates' list control.
 */
void
IUDialog::updateTemplateList() {

    templateList_->DeleteAllItems();
    Machine::InstructionTemplateNavigator navigator =
        immediateUnit_->machine()->instructionTemplateNavigator();

    for (int i = 0; i < navigator.count(); i++) {
        if (navigator.item(i)->isOneOfDestinations(*immediateUnit_)) {
            wxString name =
                WxConversion::toWxString(navigator.item(i)->name());

            templateList_->InsertItem(0, name);
        }
    }
}


/**
 * Creates the dialog window contents.
 *
 * Code generated by wxDesigner.
 *
 * @return Main sizer of the created contents.
 * @param parent The dialog window.
 * @param call_fit If true, fits the contents inside the dialog.
 * @param set_sizer If true, sets the main sizer as dialog contents.
 */
wxSizer*
IUDialog::createContents(wxWindow *parent, bool call_fit, bool set_sizer) {

    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxFlexGridSizer *item1 = new wxFlexGridSizer( 2, 0, 0 );

    wxFlexGridSizer *item2 = new wxFlexGridSizer( 4, 0, 0 );

    wxStaticText *item3 = new wxStaticText( parent, ID_LABEL_NAME, wxT("Name:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add(item3, 0, wxALIGN_RIGHT | wxALL, 5);

    wxTextCtrl *item4 = new wxTextCtrl( parent, ID_NAME, wxT(""), wxDefaultPosition, wxSize(120,-1), 0 );
    item2->Add(item4, 0, wxGROW | wxALL, 5);

    wxStaticText *item5 = new wxStaticText( parent, ID_LABEL_SIZE, wxT("Size:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add(item5, 0, wxALIGN_RIGHT | wxALL, 5);

    wxSpinCtrl *item6 = new wxSpinCtrl( parent, ID_SIZE, wxT("1"), wxDefaultPosition, wxSize(60,-1), 0, 1, 10000, 1 );
    item2->Add(item6, 0, wxGROW | wxALL, 5);

    wxStaticText *item7 = new wxStaticText( parent, ID_LABEL_WIDTH, wxT("Width:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add(item7, 0, wxALIGN_RIGHT | wxALL, 5);

    wxSpinCtrl *item8 = new wxSpinCtrl( parent, ID_WIDTH, wxT("1"), wxDefaultPosition, wxSize(100,-1), 0, 1, 10000, 1 );
    item2->Add( item8, 0, wxGROW|wxALL, 5 );

    item1->Add( item2, 0, wxALIGN_CENTER_VERTICAL, 5 );

    wxString strs11[] =
    {
        wxT("Zero"),
        wxT("Sign")
    };
    wxRadioBox *item11 = new wxRadioBox( parent, ID_EXTENSION, wxT("Extension"), wxDefaultPosition, wxDefaultSize, 2, strs11, 1, wxRA_SPECIFY_COLS );
    item1->Add( item11, 0, wxGROW|wxALL, 5 );

    wxStaticBox *item13 = new wxStaticBox( parent, -1, wxT("Templates:") );
    wxStaticBoxSizer *item12 = new wxStaticBoxSizer( item13, wxVERTICAL );
    templateListSizer_ = item12;

    wxListCtrl *item14 = new wxListCtrl( parent, ID_TEMPLATE_LIST, wxDefaultPosition, wxSize(200,200), wxLC_REPORT|wxLC_SINGLE_SEL|wxSUNKEN_BORDER );
    item12->Add(item14, 0, wxGROW | wxALL, 5);

    item1->Add(item12, 0, wxGROW | wxALL, 5);

    wxStaticBox *item16 = new wxStaticBox( parent, -1, wxT("Ports:") );
    wxStaticBoxSizer *item15 = new wxStaticBoxSizer( item16, wxVERTICAL );
    portListSizer_ = item15;

    wxListCtrl *item17 = new wxListCtrl( parent, ID_PORT_LIST, wxDefaultPosition, wxSize(240,150), wxLC_REPORT|wxLC_SINGLE_SEL|wxSUNKEN_BORDER );
    item15->Add(item17, 0, wxGROW | wxALL, 5);

    wxBoxSizer *item18 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item19 = new wxButton( parent, ID_ADD_PORT, wxT("Add..."), wxDefaultPosition, wxDefaultSize, 0 );
    item18->Add( item19, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item20 = new wxButton( parent, ID_EDIT_PORT, wxT("Edit..."), wxDefaultPosition, wxDefaultSize, 0 );
    item18->Add( item20, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item21 = new wxButton( parent, ID_DELETE_PORT, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    item18->Add( item21, 0, wxALIGN_CENTER|wxALL, 5 );

    item15->Add( item18, 0, wxALIGN_CENTER|wxALL, 5 );

    item1->Add(item15, 0, wxGROW | wxALL, 5);

    item0->Add(item1, 0, wxALL, 5);

    wxStaticLine *item22 = new wxStaticLine( parent, ID_LINE, wxDefaultPosition, wxSize(20,-1), wxLI_HORIZONTAL );
    item0->Add(item22, 0, wxGROW | wxALL, 5);

    wxGridSizer *item23 = new wxGridSizer( 2, 0, 0 );

    wxButton *item24 = new wxButton( parent, ID_HELP, wxT("&Help"), wxDefaultPosition, wxDefaultSize, 0 );
    item23->Add( item24, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item25 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item26 = new wxButton( parent, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item25->Add( item26, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item27 = new wxButton( parent, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item25->Add( item27, 0, wxALIGN_CENTER|wxALL, 5 );

    item23->Add(item25, 0, wxALIGN_RIGHT | wxALL, 5);

    item0->Add(item23, 0, wxGROW, 5);

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }

    return item0;
}
