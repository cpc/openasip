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
 * @file FUDialog.cc
 *
 * Definition of FUDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <set>
#include <string>
#include <wx/valtext.h>
#include <wx/valgen.h>
#include <boost/format.hpp>

#include "Application.hh"
#include "FUDialog.hh"
#include "FUPortDialog.hh"
#include "Model.hh"
#include "WxConversion.hh"
#include "ProDeConstants.hh"
#include "Conversion.hh"
#include "WarningDialog.hh"
#include "ErrorDialog.hh"
#include "InformationDialog.hh"
#include "Machine.hh"
#include "AddressSpace.hh"
#include "FunctionUnit.hh"
#include "FUPort.hh"
#include "UserManualCmd.hh"
#include "WidgetTools.hh"
#include "OperationDialog.hh"
#include "HWOperation.hh"
#include "ModelConstants.hh"
#include "MachineTester.hh"
#include "GUITextGenerator.hh"
#include "ProDeTextGenerator.hh"
#include "OpsetDialog.hh"
#include "ObjectState.hh"

using boost::format;
using std::string;
using namespace TTAMachine;

BEGIN_EVENT_TABLE(FUDialog, wxDialog)
    EVT_TEXT(ID_NAME, FUDialog::onName)
    EVT_CHOICE(ID_ADDRESS_SPACE, FUDialog::onSetAddressSpace)
    EVT_BUTTON(ID_ADD_PORT, FUDialog::onAddPort)
    EVT_BUTTON(ID_DELETE_PORT, FUDialog::onDeletePort)
    EVT_BUTTON(ID_EDIT_PORT, FUDialog::onEditPort)
    EVT_MENU(ID_EDIT_PORT, FUDialog::onEditPort)
    EVT_MENU(ID_DELETE_PORT, FUDialog::onDeletePort)
    EVT_MENU(ID_EDIT_OPERATION, FUDialog::onEditOperation)
    EVT_MENU(ID_DELETE_OPERATION, FUDialog::onDeleteOperation)

    EVT_LIST_ITEM_ACTIVATED(ID_PORT_LIST, FUDialog::onActivatePort)
    EVT_LIST_ITEM_RIGHT_CLICK(ID_PORT_LIST, FUDialog::onPortRightClick)
    EVT_LIST_ITEM_ACTIVATED(ID_OPERATION_LIST, FUDialog::onActivateOperation)
    EVT_LIST_ITEM_RIGHT_CLICK(ID_OPERATION_LIST, FUDialog::onOperationRightClick)

    EVT_BUTTON(ID_ADD_OPERATION, FUDialog::onAddOperation)
    EVT_BUTTON(ID_ADD_OPERATION_FROM_OPSET, FUDialog::onAddOperationFromOpset)
    EVT_BUTTON(ID_DELETE_OPERATION, FUDialog::onDeleteOperation)
    EVT_BUTTON(ID_EDIT_OPERATION, FUDialog::onEditOperation)

    EVT_BUTTON(wxID_OK, FUDialog::onOK)

    EVT_LIST_ITEM_FOCUSED(ID_PORT_LIST, FUDialog::onPortSelection)
    EVT_LIST_DELETE_ITEM(ID_PORT_LIST, FUDialog::onPortSelection)
    EVT_LIST_ITEM_SELECTED(ID_PORT_LIST, FUDialog::onPortSelection)
    EVT_LIST_ITEM_DESELECTED(ID_PORT_LIST, FUDialog::onPortSelection)

    EVT_LIST_ITEM_FOCUSED(ID_OPERATION_LIST, FUDialog::onOperationSelection)
    EVT_LIST_DELETE_ITEM(ID_OPERATION_LIST, FUDialog::onOperationSelection)
    EVT_LIST_ITEM_SELECTED(ID_OPERATION_LIST, FUDialog::onOperationSelection)
    EVT_LIST_ITEM_DESELECTED(ID_OPERATION_LIST, FUDialog::onOperationSelection)
END_EVENT_TABLE()


/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param functionUnit A function unit to be shown in the dialog.
 */
FUDialog::FUDialog(
    wxWindow* parent,
    FunctionUnit* functionUnit):
    wxDialog(parent, -1, _T(""), wxDefaultPosition),
    functionUnit_(functionUnit),
    name_(_T("")),
    portListCtrl_(NULL),
    operationListCtrl_(NULL),
    addressSpaceChoice_(NULL) {

    createContents(this, true, true);

    portListCtrl_ = dynamic_cast<wxListCtrl*>(FindWindow(ID_PORT_LIST));
    operationListCtrl_ =
        dynamic_cast<wxListCtrl*>(FindWindow(ID_OPERATION_LIST));
    addressSpaceChoice_ =
        dynamic_cast<wxChoice*>(FindWindow(ID_ADDRESS_SPACE));

    FindWindow(ID_NAME)->SetValidator(wxTextValidator(wxFILTER_ASCII, &name_));

    FindWindow(wxID_OK)->Disable();

    // set widget texts
    setTexts();

    TransferDataToWindow();
}


/**
 * The Destructor.
 */
FUDialog::~FUDialog() {
}


/**
 * Sets texts for widgets.
 */
void
FUDialog::setTexts() {
    GUITextGenerator* generator = GUITextGenerator::instance();
    ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();

    // Dialog title
    format fmt = prodeTexts->text(ProDeTextGenerator::TXT_FU_DIALOG_TITLE);
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

    WidgetTools::setLabel(generator, FindWindow(ID_ADD_OPERATION),
                          GUITextGenerator::TXT_BUTTON_ADD_DIALOG);

    WidgetTools::setLabel(generator, FindWindow(ID_EDIT_OPERATION),
                          GUITextGenerator::TXT_BUTTON_EDIT_DIALOG);

    WidgetTools::setLabel(generator, FindWindow(ID_DELETE_OPERATION),
                          GUITextGenerator::TXT_BUTTON_DELETE);

    // widget labels
    WidgetTools::setLabel(prodeTexts, FindWindow(ID_LABEL_NAME),
                          ProDeTextGenerator::TXT_LABEL_NAME);

    WidgetTools::setLabel(prodeTexts, FindWindow(ID_LABEL_AS),
                          ProDeTextGenerator::TXT_LABEL_ADDRESS_SPACE);

    // box sizer labels
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_FU_PORTS_BOX);
    WidgetTools::setWidgetLabel(portsSizer_, fmt.str());

    fmt = prodeTexts->text(ProDeTextGenerator::TXT_FU_OPERATIONS_BOX);
    WidgetTools::setWidgetLabel(operationsSizer_, fmt.str());

    // Create port list columns.
    wxListCtrl* portList =
        dynamic_cast<wxListCtrl*>(FindWindow(ID_PORT_LIST));
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_COLUMN_TRIGGERS);
    portList->InsertColumn(0, WxConversion::toWxString(fmt.str()),
                           wxLIST_FORMAT_CENTER, 30);
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_COLUMN_NAME);
    portList->InsertColumn(1, WxConversion::toWxString(fmt.str()),
                           wxLIST_FORMAT_LEFT, 110);
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_COLUMN_WIDTH);
    portList->InsertColumn(2, WxConversion::toWxString(fmt.str()),
                           wxLIST_FORMAT_LEFT, 110);

    // Create operation list column.
    wxListCtrl* operationList =
        dynamic_cast<wxListCtrl*>(FindWindow(ID_OPERATION_LIST));
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_COLUMN_NAME);
    operationList->InsertColumn(0, WxConversion::toWxString(fmt.str()),
                                wxLIST_FORMAT_LEFT, 250);

    Layout();
}

/**
 * Transfers data from the MOM to the dialog widgets.
 *
 * @return False, if an error occured in the transfer, true otherwise.
 */
bool
FUDialog::TransferDataToWindow() {

    name_ = WxConversion::toWxString(functionUnit_->name());
    updatePortList();
    updateOperationList();
    updateAddressSpaceChoice();

    return wxWindow::TransferDataToWindow();
}


/**
 * Validates input in the controls, and updates the FunctionUnit.
 */
void
FUDialog::onOK(wxCommandEvent&) {

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

    if (trimmedName != functionUnit_->name()) {
        Machine::FunctionUnitNavigator navigator =
            functionUnit_->machine()->functionUnitNavigator();
        for (int i = 0; i < navigator.count(); i++) {
            FunctionUnit* fu = navigator.item(i);
            if (trimmedName == fu->name()) {
                ProDeTextGenerator* prodeTexts =
                    ProDeTextGenerator::instance();
                format message =
                    prodeTexts->text(ProDeTextGenerator::MSG_ERROR_SAME_NAME);
                format a_fu =
                    prodeTexts->text(ProDeTextGenerator::COMP_A_FUNCTION_UNIT);
                format machine =
                    prodeTexts->text(ProDeTextGenerator::COMP_MACHINE);
                format fu =
                    prodeTexts->text(ProDeTextGenerator::COMP_FUNCTION_UNIT);
                message % trimmedName % a_fu.str() % machine.str() % fu.str();
                WarningDialog warning(
                    this, WxConversion::toWxString(message.str()));
                warning.ShowModal();
                return;
            }
        }
    }
    functionUnit_->setName(trimmedName);
    EndModal(wxID_OK);
}

/**
 * Sets the function unit address space when user changes it.
 */
void
FUDialog::onSetAddressSpace(wxCommandEvent&) {

    string asName =
        WxConversion::toString(addressSpaceChoice_->GetStringSelection());

    if (asName == WxConversion::toString(ProDeConstants::NONE)) {
        functionUnit_->setAddressSpace(NULL);
    } else {

        Machine::AddressSpaceNavigator navigator =
            functionUnit_->machine()->addressSpaceNavigator();

        AddressSpace* as = navigator.item(asName);
        functionUnit_->setAddressSpace(as);
    }
}


/**
 * Creates and shows an empty Function Unit Port Dialog for adding ports.
 */
void
FUDialog::onAddPort(wxCommandEvent&) {

    // Generate name for the new port.
    int i = 1;
    string newName = ProDeConstants::COMP_NEW_NAME_PREFIX_PORT +
        Conversion::toString(i);
    while (functionUnit_->hasPort(newName)) {
        newName = ProDeConstants::COMP_NEW_NAME_PREFIX_PORT +
            Conversion::toString(i);
        i++;
    }

    FUPort* port = new FUPort(
        newName, ModelConstants::DEFAULT_WIDTH, *functionUnit_, false, false);
    FUPortDialog portDialog(this, port);
    if (portDialog.ShowModal() == wxID_CANCEL) {
        // adding port was cancelled
        delete port;
    }
    updatePortList();
}



/**
 * Deletes selected port from the port list.
 */
void
FUDialog::onDeletePort(wxCommandEvent&) {
    FUPort* selected = selectedPort();
    if (selected == NULL) {
        return;
    }
    assert(selected != NULL);
    delete selected;
    updatePortList();
}


/**
 * Returns a pointer to the function unit port which is selected.
 *
 * @return Pointer to the port selected in the port list.
 */
FUPort*
FUDialog::selectedPort() {
    string name = WidgetTools::lcStringSelection(portListCtrl_, 1);
    if (name == "") {
        return NULL;
    }
    FUPort* port = functionUnit_->operationPort(name);
    return port;
}


/**
 * Handles left mouse button double clicks on the port list.
 */
void
FUDialog::onActivatePort(wxListEvent&) {
    wxCommandEvent dummy;
    onEditPort(dummy);
}


/**
 * Handles the 'Edit Port' button event.
 *
 * Opens a FUPortDialog with the selected port set.
 */
void
FUDialog::onEditPort(wxCommandEvent&) {
    FUPort* port = selectedPort();
    if (port == NULL) {
        return;
    }
    FUPortDialog portDialog(this, port);
    portDialog.ShowModal();
    updatePortList();
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
FUDialog::onPortSelection(wxListEvent&) {
    if (portListCtrl_->GetSelectedItemCount() != 1) {
        FindWindow(ID_DELETE_PORT)->Disable();
        FindWindow(ID_EDIT_PORT)->Disable();
        return;
    }
    FindWindow(ID_DELETE_PORT)->Enable();
    FindWindow(ID_EDIT_PORT)->Enable();
}


/**
 * Creates and shows an empty Function Unit Operation Dialog for
 * adding operations.
 *
 * Not Implemented.
 */
void
FUDialog::onAddOperation(wxCommandEvent&) {
    ObjectState* fu = functionUnit_->saveState();

    // Generate name for the new operation.
    int i = 1;
    string newName = ProDeConstants::COMP_NEW_NAME_PREFIX_OPERATION +
        Conversion::toString(i);
    while (functionUnit_->hasOperation(newName)) {
        newName = ProDeConstants::COMP_NEW_NAME_PREFIX_OPERATION +
            Conversion::toString(i);
        i++;
    }
    HWOperation* operation = NULL;
    operation = new HWOperation(newName, *functionUnit_);

    OperationDialog dialog(this, operation);
    if (dialog.ShowModal() == wxID_OK) {
        delete fu;
    } else {
        functionUnit_->loadState(fu);
    }
    updateOperationList();
}

/**
 * Event handler for the add operation from opset button.
 */
void
FUDialog::onAddOperationFromOpset(wxCommandEvent&) {

    try {
        OpsetDialog opsetDialog(this);
        if (opsetDialog.ShowModal() != wxID_OK) {
            return;
        }

        HWOperation* operation = opsetDialog.createOperation(*functionUnit_);

        if (operation == NULL) {
            return;
        }

        OperationDialog operationDialog(this, operation);
        if (operationDialog.ShowModal() != wxID_OK) {
            delete operation;
        }

        updateOperationList();
    } catch (Exception & e) {
        wxString message = WxConversion::toWxString(e.errorMessage());
        ErrorDialog dialog(this, message);
        dialog.ShowModal();
        return;
    }
}

/**
 * Deletes selected operation from the operation list.
 */
void
FUDialog::onDeleteOperation(wxCommandEvent&) {
    string name = WidgetTools::lcStringSelection(operationListCtrl_, 0);
    if (name == "") {
        return;
    }
    delete functionUnit_->operation(name);
    updateOperationList();
}



/**
 * Handles the 'Edit Operation' button event.
 *
 * Opens a FUOperationDialog with the selected operation's attributes set.
 */
void
FUDialog::onEditOperation(wxCommandEvent&) {

    HWOperation* selected = selectedOperation();
    if (selected == NULL) {
        // No operation selected.
        return;
    }

    ObjectState* fu = functionUnit_->saveState();
    OperationDialog dialog(this, selected);
    if (dialog.ShowModal() == wxID_OK) {
        delete fu;
    } else {
        functionUnit_->loadState(fu);
    }
    updateOperationList();
}


/**
 * Checks whether name field is empty and disables OK button of the
 * dialog if it is.
 */
void
FUDialog::onName(wxCommandEvent&) {
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
 * Returns pointer to the operation, which is selected on the operation list.
 *
 * @return Pointer to the selected operation.
 */
HWOperation*
FUDialog::selectedOperation() {
    string name = WidgetTools::lcStringSelection(operationListCtrl_, 0);
    if (name == "") {
        return NULL;
    }
    return functionUnit_->operation(name);
}


/**
 * Disables and enables Edit and Delete buttons under the operation list.
 *
 * If a operation is selected, buttons are enabled. If no operation is
 * selected the buttons will be disabled.
 *
 * @param event ListEvent, which may have changed the selection.
 */
void
FUDialog::onOperationSelection(wxListEvent&) {
    if (operationListCtrl_->GetSelectedItemCount() != 1) {
        FindWindow(ID_DELETE_OPERATION)->Disable();
        FindWindow(ID_EDIT_OPERATION)->Disable();
        return;
    }
    FindWindow(ID_DELETE_OPERATION)->Enable();
    FindWindow(ID_EDIT_OPERATION)->Enable();
}


/**
 * Updates 'Address Space' choice control.
 *
 * Clears all items from the choicer and adds all address spaces plus
 * an item 'NONE' indicating empty selection.
 */
void
FUDialog::updateAddressSpaceChoice() {

    addressSpaceChoice_->Clear();
    addressSpaceChoice_->Append(ProDeConstants::NONE);

    Machine::AddressSpaceNavigator navigator =
        functionUnit_->machine()->addressSpaceNavigator();

    // put each address space of the Machine to the control.
    for (int i = 0; i < navigator.count(); i++) {
        AddressSpace* as = navigator.item(i);
        wxString name = WxConversion::toWxString(as->name());
        addressSpaceChoice_->Append(name);
    }

    // set address space selection
    AddressSpace* as = functionUnit_->addressSpace();
    if (as != NULL) {
        wxString name = WxConversion::toWxString(as->name());
        addressSpaceChoice_->SetStringSelection(name);
    } else {
        addressSpaceChoice_->SetStringSelection(ProDeConstants::NONE);
    }
}


/**
 * Updates 'Ports' list control.
 *
 * Clears all items from the choicer and adds all ports.
 */
void
FUDialog::updatePortList() {

    portListCtrl_->DeleteAllItems();

    for (int i = 0; i < functionUnit_->portCount(); i++) {

        FUPort* port = functionUnit_->operationPort(i);

        bool triggers = port->isTriggering();
        string width = Conversion::toString(port->width());
        string name = port->name();

        // set asterisk symbol in the "T" column if port triggers
        if (!triggers) {
            portListCtrl_->InsertItem(i, _T(""), 0);
        } else {
            portListCtrl_->InsertItem(i, _T("*"), 0);
        }

        portListCtrl_->SetItem(i, 1, WxConversion::toWxString(name));
        portListCtrl_->SetItem(i, 2, WxConversion::toWxString(width));

    }
    wxListEvent dummy;
    onPortSelection(dummy);
}


/**
 * Updates operation list.
 */
void
FUDialog::updateOperationList() {

    operationListCtrl_->DeleteAllItems();

    // Operation names are put in a set, which will sort the operation
    // names automatically.
    std::set<std::string> operations;
    for (int i=0; i < functionUnit_->operationCount(); i++) {
        operations.insert(functionUnit_->operation(i)->name());
    }

    std::set<std::string>::reverse_iterator iter = operations.rbegin();
    for (; iter != operations.rend(); iter++) {
        wxString name = WxConversion::toWxString(*iter);
        operationListCtrl_->InsertItem(0, name);
    }

    wxListEvent dummy;
    onOperationSelection(dummy);
}


/**
 * Opens a pop-up menu when right mouse button was pressed.
 *
 * @param event Information about right mouse click event.
 */
void
FUDialog::onPortRightClick(wxListEvent& event) {

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
 * Opens a pop-up menu when right mouse button was pressed on the
 * operation list.
 *
 * @param event Information about right mouse click event.
 */
void
FUDialog::onOperationRightClick(wxListEvent& event) {

    operationListCtrl_->SetItemState(event.GetIndex(), wxLIST_STATE_SELECTED,
                                wxLIST_STATE_SELECTED);

    wxMenu* contextMenu = new wxMenu();

    ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();
    format button = prodeTexts->text(
        ProDeTextGenerator::TXT_LABEL_BUTTON_EDIT);
    contextMenu->Append(
        ID_EDIT_OPERATION, WxConversion::toWxString(button.str()));
    button = prodeTexts->text(
        ProDeTextGenerator::TXT_LABEL_BUTTON_DELETE);
    contextMenu->Append(
        ID_DELETE_OPERATION, WxConversion::toWxString(button.str()));
    operationListCtrl_->PopupMenu(contextMenu, event.GetPoint());
}


/**
 * Handles left mouse button double clicks on the operation list.
 */
void
FUDialog::onActivateOperation(wxListEvent&) {
    wxCommandEvent dummy;
    onEditOperation(dummy);
}


/**
 * Creates the dialog window contents.
 *
 * This method was initially generated with wxDesigner, code will be
 * cleaned up later.
 *
 * @return Main sizer of the created contents.
 * @param parent The dialog window.
 * @param call_fit If true, fits the contents inside the dialog.
 * @param set_sizer If true, sets the main sizer as dialog contents.
 */
wxSizer*
FUDialog::createContents(wxWindow* parent, bool call_fit, bool set_sizer) {

    wxFlexGridSizer *item0 = new wxFlexGridSizer( 2, 0, 10 );

    wxBoxSizer *item1 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticText *item2 = new wxStaticText( parent, ID_LABEL_NAME, wxT("Name:"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item2, 0, wxALIGN_CENTER|wxALL, 5 );

    wxTextCtrl *item3 = new wxTextCtrl( parent, ID_NAME, wxT(""), wxDefaultPosition, wxSize(150,-1), 0 );
    item1->Add( item3, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item1, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item4 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticText *item5 = new wxStaticText( parent, ID_LABEL_AS, wxT("Address Space:"), wxDefaultPosition, wxDefaultSize, 0 );
    item4->Add( item5, 0, wxALIGN_CENTER|wxALL, 5 );

    wxString *strs6 = (wxString*) NULL;
    wxChoice *item6 = new wxChoice( parent, ID_ADDRESS_SPACE, wxDefaultPosition, wxSize(150,-1), 0, strs6, 0 );
    item4->Add( item6, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item4, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticBox *item8 = new wxStaticBox( parent, -1, wxT("Operations:") );
    wxStaticBoxSizer *item7 = new wxStaticBoxSizer( item8, wxVERTICAL );
    operationsSizer_ = item7;

    wxListCtrl *item9 = new wxListCtrl( parent, ID_OPERATION_LIST, wxDefaultPosition, wxSize(160,200), wxLC_REPORT|wxSUNKEN_BORDER );
    item7->Add( item9, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item10 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item11 = new wxButton( parent, ID_ADD_OPERATION, wxT("Add..."), wxDefaultPosition, wxDefaultSize, 0 );
    item10->Add( item11, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item12 = new wxButton( parent, ID_EDIT_OPERATION, wxT("Edit..."), wxDefaultPosition, wxDefaultSize, 0 );
    item10->Add( item12, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item13 = new wxButton( parent, ID_DELETE_OPERATION, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    item10->Add( item13, 0, wxALIGN_CENTER|wxALL, 5 );

    item7->Add( item10, 0, wxALIGN_CENTER, 5 );

    wxButton *item14 = new wxButton( parent, ID_ADD_OPERATION_FROM_OPSET, wxT(" Add from  Opset... "), wxDefaultPosition, wxDefaultSize, 0 );
    item7->Add( item14, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item0->Add( item7, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    wxStaticBox *item16 = new wxStaticBox( parent, -1, wxT("Ports:") );
    wxStaticBoxSizer *item15 = new wxStaticBoxSizer( item16, wxVERTICAL );
    portsSizer_ = item15;

    wxListCtrl *item17 = new wxListCtrl( parent, ID_PORT_LIST, wxDefaultPosition, wxSize(160,200), wxLC_REPORT|wxSUNKEN_BORDER );
    item15->Add( item17, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item18 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item19 = new wxButton( parent, ID_ADD_PORT, wxT("Add..."), wxDefaultPosition, wxDefaultSize, 0 );
    item18->Add( item19, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item20 = new wxButton( parent, ID_EDIT_PORT, wxT("Edit..."), wxDefaultPosition, wxDefaultSize, 0 );
    item18->Add( item20, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item21 = new wxButton( parent, ID_DELETE_PORT, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    item18->Add( item21, 0, wxALIGN_CENTER|wxALL, 5 );

    item15->Add( item18, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item15, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    wxBoxSizer *item25 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item26 = new wxButton( parent, ID_HELP, wxT("&Help"), wxDefaultPosition, wxDefaultSize, 0 );
    item25->Add( item26, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item25, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item27 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item28 = new wxButton( parent, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item27->Add( item28, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item29 = new wxButton( parent, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item27->Add( item29, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item27, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;
}
