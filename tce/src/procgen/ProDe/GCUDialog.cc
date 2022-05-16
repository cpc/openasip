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
 * @file GCUDialog.cc
 *
 * Definition of GCUDialog class.
 *
 * @author Veli-Pekka Jääskeläinen 2004 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <string>
#include <wx/spinctrl.h>
#include <wx/listctrl.h>
#include <wx/statline.h>
#include <wx/valgen.h>
#include <boost/format.hpp>

#include "Application.hh"
#include "GCUDialog.hh"
#include "ModelConstants.hh"
#include "ProDeConstants.hh"
#include "WxConversion.hh"
#include "Conversion.hh"
#include "Machine.hh"
#include "MachineTester.hh"
#include "ControlUnit.hh"
#include "FUPort.hh"
#include "SpecialRegisterPort.hh"
#include "AddressSpace.hh"
#include "Socket.hh"
#include "InformationDialog.hh"
#include "GUITextGenerator.hh"
#include "ProDeTextGenerator.hh"
#include "WxConversion.hh"
#include "WidgetTools.hh"
#include "UserManualCmd.hh"
#include "OperationDialog.hh"
#include "HWOperation.hh"
#include "WarningDialog.hh"
#include "FUPortDialog.hh"
#include "SRPortDialog.hh"
#include "ObjectState.hh"

using std::string;
using boost::format;
using namespace TTAMachine;

// Too long lines to keep Doxygen quiet.
BEGIN_EVENT_TABLE(GCUDialog, wxDialog)
    EVT_TEXT(ID_NAME, GCUDialog::onName)
    EVT_BUTTON(wxID_OK, GCUDialog::onOK)

    EVT_BUTTON(ID_ADD_OPERATION, GCUDialog::onAddOperation)
    EVT_BUTTON(ID_EDIT_OPERATION, GCUDialog::onEditOperation)
    EVT_BUTTON(ID_DELETE_OPERATION, GCUDialog::onDeleteOperation)
    EVT_LIST_ITEM_SELECTED(ID_OPERATION_LIST, GCUDialog::onOperationSelection)
    EVT_LIST_ITEM_DESELECTED(ID_OPERATION_LIST, GCUDialog::onOperationSelection)
    EVT_LIST_ITEM_ACTIVATED(ID_OPERATION_LIST, GCUDialog::onActivateOperation)
    EVT_LIST_ITEM_RIGHT_CLICK(ID_OPERATION_LIST, GCUDialog::onOperationRightClick)
    EVT_MENU(ID_EDIT_OPERATION, GCUDialog::onEditOperation)
    EVT_MENU(ID_DELETE_OPERATION, GCUDialog::onDeleteOperation)

    EVT_BUTTON(ID_ADD_FU_PORT, GCUDialog::onAddFUPort)
    EVT_BUTTON(ID_ADD_SR_PORT, GCUDialog::onAddSRPort)
    EVT_BUTTON(ID_EDIT_PORT, GCUDialog::onEditPort)
    EVT_BUTTON(ID_DELETE_PORT, GCUDialog::onDeletePort)
    EVT_LIST_ITEM_SELECTED(ID_PORT_LIST, GCUDialog::onPortSelection)
    EVT_LIST_ITEM_DESELECTED(ID_PORT_LIST, GCUDialog::onPortSelection)
    EVT_LIST_ITEM_ACTIVATED(ID_PORT_LIST, GCUDialog::onActivatePort)
    EVT_LIST_ITEM_RIGHT_CLICK(ID_PORT_LIST, GCUDialog::onPortRightClick)
    EVT_MENU(ID_EDIT_PORT, GCUDialog::onEditPort)
    EVT_MENU(ID_DELETE_PORT, GCUDialog::onDeletePort)

    EVT_CHOICE(ID_RA_CHOICE, GCUDialog::onRAPortChoice)
END_EVENT_TABLE()


/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param gcu The Global Control Unit to be modified with the dialog.
 */
GCUDialog::GCUDialog(
    wxWindow* parent,
    ControlUnit* gcu):
    wxDialog(parent, -1, _T(""), wxDefaultPosition),
    gcu_(gcu),
    name_(_T("")),
    delaySlots_(0) {

    // Create dialog contents and set pointers to the dialog controls.
    createContents(this, true, true);
    addressSpaceChoice_ =
        dynamic_cast<wxChoice*>(FindWindow(ID_ADDRESS_SPACE));
    raPortChoice_ =
        dynamic_cast<wxChoice*>(FindWindow(ID_RA_CHOICE));
    operationList_ =
        dynamic_cast<wxListCtrl*>(FindWindow(ID_OPERATION_LIST));
    portList_ =
        dynamic_cast<wxListCtrl*>(FindWindow(ID_PORT_LIST));

    // Disable conditional controls initially.
    FindWindow(wxID_OK)->Disable();
    FindWindow(ID_EDIT_OPERATION)->Disable();
    FindWindow(ID_DELETE_OPERATION)->Disable();
    FindWindow(ID_EDIT_PORT)->Disable();
    FindWindow(ID_DELETE_PORT)->Disable();

    // set widget labels
    setLabels();

    // Set validators for dialog controls.
    FindWindow(ID_NAME)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &name_));
    FindWindow(ID_DELAY_SLOTS)->SetValidator(
        wxGenericValidator(&delaySlots_));
    //FindWindow(ID_GUARD_LATENCY)->SetValidator(
        //wxGenericValidator(&guardLatency_));

    TransferDataToWindow();
}


/**
 * The Destructor.
 */
GCUDialog::~GCUDialog() {
}


/**
 * Sets the widget labels to texts provided by the GUITextGenerator.
 */
void
GCUDialog::setLabels() {

    GUITextGenerator* generator = GUITextGenerator::instance();
    ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();

    // Dialog title
    format fmt = prodeTexts->text(ProDeTextGenerator::TXT_GCU_DIALOG_TITLE);
    string title = fmt.str();
    SetTitle(WxConversion::toWxString(title));

    // buttons
    WidgetTools::setLabel(generator, FindWindow(wxID_OK),
                          GUITextGenerator::TXT_BUTTON_OK);

    WidgetTools::setLabel(generator, FindWindow(wxID_CANCEL),
                          GUITextGenerator::TXT_BUTTON_CANCEL);

    WidgetTools::setLabel(generator, FindWindow(ID_ADD_OPERATION),
                          GUITextGenerator::TXT_BUTTON_ADD_DIALOG);

    WidgetTools::setLabel(generator, FindWindow(ID_EDIT_OPERATION),
                          GUITextGenerator::TXT_BUTTON_EDIT_DIALOG);

    WidgetTools::setLabel(generator, FindWindow(ID_DELETE_OPERATION),
                          GUITextGenerator::TXT_BUTTON_DELETE);

    WidgetTools::setLabel(generator, FindWindow(ID_EDIT_PORT),
                          GUITextGenerator::TXT_BUTTON_EDIT_DIALOG);

    WidgetTools::setLabel(generator, FindWindow(ID_DELETE_PORT),
                          GUITextGenerator::TXT_BUTTON_DELETE);

    WidgetTools::setLabel(generator, FindWindow(ID_HELP),
                          GUITextGenerator::TXT_BUTTON_HELP);

    WidgetTools::setLabel(prodeTexts, FindWindow(ID_ADD_FU_PORT),
                          ProDeTextGenerator::TXT_BUTTON_ADD_FU_PORT);

    WidgetTools::setLabel(prodeTexts, FindWindow(ID_ADD_SR_PORT),
                          ProDeTextGenerator::TXT_BUTTON_ADD_SR_PORT);

    // widget labels
    WidgetTools::setLabel(prodeTexts, FindWindow(ID_TEXT_NAME),
                          ProDeTextGenerator::TXT_LABEL_NAME);

    WidgetTools::setLabel(prodeTexts, FindWindow(ID_TEXT_AS),
                          ProDeTextGenerator::TXT_LABEL_ADDRESS_SPACE);

    WidgetTools::setLabel(prodeTexts, FindWindow(ID_TEXT_DS),
                          ProDeTextGenerator::TXT_GCU_DELAY_SLOTS);

    //WidgetTools::setLabel(
        //prodeTexts, FindWindow(ID_LABEL_GLOBAL_GUARD_LATENCY),
        //ProDeTextGenerator::TXT_LABEL_GLOBAL_GUARD_LATENCY);

    // box sizer labels
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_GCU_PORTS_BOX);
    WidgetTools::setWidgetLabel(portListSizer_, fmt.str());


    // Create operation list columns.
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_COLUMN_NAME);
    wxListCtrl* operationList =
        dynamic_cast<wxListCtrl*>(FindWindow(ID_OPERATION_LIST));
    operationList->InsertColumn(0, WxConversion::toWxString(fmt.str()),
                                wxLIST_FORMAT_LEFT, 260);

    // Create port list columns.
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_COLUMN_TRIGGERS);
    portList_->InsertColumn(0, WxConversion::toWxString(fmt.str()),
                            wxLIST_FORMAT_CENTER, 30);
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_COLUMN_SRP);
    portList_->InsertColumn(1, WxConversion::toWxString(fmt.str()),
                            wxLIST_FORMAT_CENTER, 40);
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_COLUMN_NAME);
    portList_->InsertColumn(2, WxConversion::toWxString(fmt.str()),
                            wxLIST_FORMAT_LEFT, 110);
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_COLUMN_WIDTH);
    portList_->InsertColumn(3, WxConversion::toWxString(fmt.str()),
                            wxLIST_FORMAT_LEFT, 110);
}


/**
 * Transfers data from the GCU object to the dialog widgets.
 *
 * @return false, if an error occured in the transfer.
 */
bool
GCUDialog::TransferDataToWindow() {
    name_ = WxConversion::toWxString(gcu_->name());
    delaySlots_ = gcu_->delaySlots();
    guardLatency_ = gcu_->globalGuardLatency();
    updateAddressSpaceChoice();
    updateRAPortChoice();
    updatePortList();
    updateOperationList();

    // wxWidgets GTK1 version seems to bug with spincontrol validators.
    // The widget value has to be set manually.
    dynamic_cast<wxSpinCtrl*>(
        FindWindow(ID_DELAY_SLOTS))->SetValue(delaySlots_);

    return wxWindow::TransferDataToWindow();
}


/**
 * Updates 'Address Space' choice control.
 *
 * Clears all items from the choicer and adds all address spaces plus
 * an item 'NONE' indicating empty selection.
 */
void
GCUDialog::updateAddressSpaceChoice() {

    addressSpaceChoice_->Clear();
    addressSpaceChoice_->Append(ProDeConstants::NONE);

    // Put each address space of the Machine to the control.
    Machine::AddressSpaceNavigator navigator =
        gcu_->machine()->addressSpaceNavigator();

    for (int i = 0;i < navigator.count(); i++) {
        AddressSpace* as = navigator.item(i);
        wxString name = WxConversion::toWxString(as->name());
        addressSpaceChoice_->Append(name);
    }

    if (gcu_->addressSpace() != NULL) {

	wxString asName =
	    WxConversion::toWxString(gcu_->addressSpace()->name());

	addressSpaceChoice_->SetStringSelection(asName);

    } else {
	addressSpaceChoice_->SetStringSelection(ProDeConstants::NONE);
    }
}


/**
 * Updates the Return address port choicer when the return address port
 * choicer selection is changed.
 */
void
GCUDialog::updateRAPortChoice() {

    wxString selection = raPortChoice_->GetStringSelection();

    raPortChoice_->Clear();
    raPortChoice_->Append(ProDeConstants::NONE);
    raPortChoice_->SetSelection(0);

    for (int i = 0; i < gcu_->specialRegisterPortCount(); i++) {
	SpecialRegisterPort* port = gcu_->specialRegisterPort(i);
	string portName = port->name();
	raPortChoice_->Append(WxConversion::toWxString(portName));

	if (gcu_->hasReturnAddressPort() &&
	    port == gcu_->returnAddressPort()) {

	    raPortChoice_->SetStringSelection(
		WxConversion::toWxString(portName));
	}
    }
}


/**
 * Updates 'Ports' list control.
 *
 * Clears all items from the choicer and adds all ports.
 */
void
GCUDialog::updatePortList() {

    portList_->DeleteAllItems();

    // Current list index is stored in a separate variable because of
    // possible omitting of the return address port.
    int index = 0;
    for (int i = 0; i < gcu_->portCount(); i++) {

        BaseFUPort* port = gcu_->port(i);

        bool triggers = port->isTriggering();
        string width = Conversion::toString(port->width());
        string name = port->name();

        // set asterisk symbol in the "T" column if port triggers
        if (!triggers) {
            portList_->InsertItem(index, _T(""), 0);
        } else {
            portList_->InsertItem(index, _T("*"), 0);
        }

	// set asterisk symbol in the "SRP" column if the port is
	// a special register port
	if (gcu_->hasSpecialRegisterPort(port->name())) {
	    portList_->SetItem(index, 1, _T("*"));
	} else {
	    portList_->SetItem(index, 1, _T(""));
	}

        portList_->SetItem(index, 2, WxConversion::toWxString(name));
        portList_->SetItem(index, 3, WxConversion::toWxString(width));

        index++;
    }
    wxListEvent dummy;
    onPortSelection(dummy);
}


/**
 * Validates input in the controls, and updates the ControlUnit.
 */
void
GCUDialog::onOK(wxCommandEvent&) {

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

    // Check that the name is not reserved for another FU.
    if (trimmedName != gcu_->name()) {
        Machine::FunctionUnitNavigator navigator =
            gcu_->machine()->functionUnitNavigator();
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
                format gcu = prodeTexts->text(ProDeTextGenerator::COMP_GCU);
                message % trimmedName % a_fu.str() % machine.str() % gcu.str();
                WarningDialog warning(
                    this, WxConversion::toWxString(message.str()));
                warning.ShowModal();
                return;
            }
        }
    }

    try {
        gcu_->setName(trimmedName);
        gcu_->setDelaySlots(delaySlots_);
        gcu_->setGlobalGuardLatency(guardLatency_);
    } catch (Exception& e) {
        InformationDialog dialog(
            this, WxConversion::toWxString(e.errorMessage()));
        dialog.ShowModal();
        return;
    }

    // set GCU address space
    string asName =
	WxConversion::toString(addressSpaceChoice_->GetStringSelection());

    if (asName != WxConversion::toString(ProDeConstants::NONE)) {

	Machine::AddressSpaceNavigator asNavigator =
	    gcu_->machine()->addressSpaceNavigator();

	assert(asNavigator.hasItem(asName));
	AddressSpace* addressSpace = asNavigator.item(asName);
	gcu_->setAddressSpace(addressSpace);

    } else {
	// no address space set
	gcu_->setAddressSpace(NULL);
    }

    EndModal(wxID_OK);
}


/**
 * Checks whether the name field is empty, and disables OK button of the
 * dialog if it is.
 */
void
GCUDialog::onName(wxCommandEvent&) {
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
 * Creates and shows an empty Operation Dialog for adding operations.
 */
void
GCUDialog::onAddOperation(wxCommandEvent&) {
    ObjectState* gcu = gcu_->saveState();

    // Generate name for the new operation.
    int i = 1;
    string newName = ProDeConstants::COMP_NEW_NAME_PREFIX_OPERATION +
        Conversion::toString(i);
    while (gcu_->hasOperation(newName)) {
        newName = ProDeConstants::COMP_NEW_NAME_PREFIX_OPERATION +
            Conversion::toString(i);
        i++;
    }

    HWOperation* operation = new HWOperation(newName, *gcu_);
    OperationDialog dialog(this, operation);
    if (dialog.ShowModal() == wxID_OK) {
        delete gcu;
    } else {
        gcu_->loadState(gcu);
    }
    updateOperationList();
}


/**
 * Deletes selected operation from the operation list.
 */
void
GCUDialog::onDeleteOperation(wxCommandEvent&) {
    string name = WidgetTools::lcStringSelection(operationList_, 0);
    if (name == "") {
        return;
    }
    delete gcu_->operation(name);
    updateOperationList();
}


/**
 * Handles the 'Edit Operation' button event.
 *
 * Opens a OperationDialog with the selected operation's attributes set.
 */
void
GCUDialog::onEditOperation(wxCommandEvent&) {

    HWOperation* selected = selectedOperation();
    if (selected == NULL) {
        // No operation selected.
        return;
    }

    ObjectState* gcu = gcu_->saveState();
    OperationDialog dialog(this, selected);
    if (dialog.ShowModal() == wxID_OK) {
        delete gcu;
    } else {
        gcu_->loadState(gcu);
    }
    updateOperationList();
}


/**
 * Returns pointer to the operation, which is selected on the operation list.
 *
 * @return Pointer to the selected operation.
 */
HWOperation*
GCUDialog::selectedOperation() {
    string name = WidgetTools::lcStringSelection(operationList_, 0);
    if (name == "") {
        return NULL;
    }
    return gcu_->operation(name);
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
GCUDialog::onOperationSelection(wxListEvent&) {
    if (operationList_->GetSelectedItemCount() != 1) {
        FindWindow(ID_DELETE_OPERATION)->Disable();
        FindWindow(ID_EDIT_OPERATION)->Disable();
        return;
    }
    FindWindow(ID_DELETE_OPERATION)->Enable();
    FindWindow(ID_EDIT_OPERATION)->Enable();
}


/**
 * Updates the operation list.
 */
void
GCUDialog::updateOperationList() {
    operationList_->DeleteAllItems();
    for (int i=0; i < gcu_->operationCount(); i++) {
        wxString name = WxConversion::toWxString(gcu_->operation(i)->name());
        operationList_->InsertItem(0, name);
    }
    wxListEvent dummy;
    onOperationSelection(dummy);
}


/**
 * Opens a pop-up menu when right mouse button was pressed on the
 * operation list.
 *
 * @param event Information about right mouse click event.
 */
void
GCUDialog::onOperationRightClick(wxListEvent& event) {

    operationList_->SetItemState(event.GetIndex(), wxLIST_STATE_SELECTED,
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
    operationList_->PopupMenu(contextMenu, event.GetPoint());
}


/**
 * Handles left mouse button double clicks on the operation list.
 */
void
GCUDialog::onActivateOperation(wxListEvent&) {
    wxCommandEvent dummy;
    onEditOperation(dummy);
}



/**
 * Creates and shows an empty Function Unit Port Dialog for adding ports.
 */
void
GCUDialog::onAddFUPort(wxCommandEvent&) {

    // Generate name for the new port.
    int i = 1;
    string newName = ProDeConstants::COMP_NEW_NAME_PREFIX_PORT +
        Conversion::toString(i);
    while (gcu_->hasPort(newName)) {
        newName = ProDeConstants::COMP_NEW_NAME_PREFIX_PORT +
            Conversion::toString(i);
        i++;
    }

    FUPort* port = new FUPort(
        newName, ModelConstants::DEFAULT_WIDTH, *gcu_, false, false);
    FUPortDialog portDialog(this, port);
    if (portDialog.ShowModal() == wxID_CANCEL) {
        // adding port was cancelled
        delete port;
    }
    updatePortList();
}


/**
 * Creates and shows an empty Special Register Port Dialog for adding ports.
 */
void
GCUDialog::onAddSRPort(wxCommandEvent&) {

    // Generate name for the new port.
    int i = 1;
    string newName = ProDeConstants::COMP_NEW_NAME_PREFIX_PORT +
        Conversion::toString(i);
    while (gcu_->hasPort(newName)) {
        newName = ProDeConstants::COMP_NEW_NAME_PREFIX_PORT +
            Conversion::toString(i);
        i++;
    }

    SpecialRegisterPort* port = new SpecialRegisterPort(
	newName, ModelConstants::DEFAULT_WIDTH, *gcu_);

    SRPortDialog portDialog(this, port);
    if (portDialog.ShowModal() == wxID_CANCEL) {
        // adding port was cancelled
        delete port;
    }
    updatePortList();
    updateRAPortChoice();
}


/**
 * Deletes selected port from the port list.
 */
void
GCUDialog::onDeletePort(wxCommandEvent&) {
    Port* selected = selectedPort();
    if (selected == NULL) {
        return;
    }
    delete selected;
    updatePortList();
    updateRAPortChoice();
}


/**
 * Returns a pointer to the port selected in the port list.
 *
 * @return Pointer to the port selected in the port list.
 */
BaseFUPort*
GCUDialog::selectedPort() {
    string name = WidgetTools::lcStringSelection(portList_, 2);
    if (name == "") {
        return NULL;
    }
    BaseFUPort* port = gcu_->port(name);
    return port;
}


/**
 * Handles the 'Edit Port' button event.
 *
 * Opens a FUPortDialog with the selected port set.
 */
void
GCUDialog::onEditPort(wxCommandEvent&) {
    BaseFUPort* selected = selectedPort();
    if (selected == NULL) {
        return;
    }

    if (gcu_->hasSpecialRegisterPort(selected->name())) {
	SpecialRegisterPort* port =
	    gcu_->specialRegisterPort(selected->name());
	SRPortDialog portDialog(this, port);
	portDialog.ShowModal();
    } else if (gcu_->hasOperationPort(selected->name())) {
	FUPort* port = gcu_->operationPort(selected->name());
	FUPortDialog portDialog(this, port);
	portDialog.ShowModal();
    }
    updatePortList();
    updateRAPortChoice();
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
GCUDialog::onPortSelection(wxListEvent&) {
    if (portList_->GetSelectedItemCount() != 1) {
        FindWindow(ID_DELETE_PORT)->Disable();
        FindWindow(ID_EDIT_PORT)->Disable();
        return;
    }
    FindWindow(ID_DELETE_PORT)->Enable();
    FindWindow(ID_EDIT_PORT)->Enable();
}


/**
 * Opens a pop-up menu when right mouse button was pressed on the port list.
 *
 * @param event Information about right mouse click event.
 */
void
GCUDialog::onPortRightClick(wxListEvent& event) {

    portList_->SetItemState(event.GetIndex(), wxLIST_STATE_SELECTED,
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
    portList_->PopupMenu(contextMenu, event.GetPoint());
}


/**
 * Handles left mouse button double clicks on the port list.
 */
void
GCUDialog::onActivatePort(wxListEvent&) {
    wxCommandEvent dummy;
    onEditPort(dummy);
}


/**
 * Sets the return address port when return address port choicer selection
 * changes.
 */
void
GCUDialog::onRAPortChoice(wxCommandEvent&) {

    if (raPortChoice_->GetStringSelection() == ProDeConstants::NONE) {
	gcu_->unsetReturnAddressPort();
	return;
    }

    string selection =
        WxConversion::toString(raPortChoice_->GetStringSelection());

    SpecialRegisterPort* port = gcu_->specialRegisterPort(selection);

    assert(port != NULL);

    gcu_->setReturnAddressPort(*port);
    updateRAPortChoice();
}


/**
 * Creates the dialog window contents.
 *
 * This method was generated with wxDesigner, thus the ugly code and
 * too long lines.
 *
 * @return Main sizer of the created contents.
 * @param parent The dialog window.
 * @param call_fit If true, fits the contents inside the dialog.
 * @param set_sizer If true, sets the main sizer as dialog contents.
 */
wxSizer*
GCUDialog::createContents(wxWindow *parent, bool call_fit, bool set_sizer) {

    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *item1 = new wxBoxSizer( wxHORIZONTAL );

    wxBoxSizer *item2 = new wxBoxSizer( wxVERTICAL );

    wxFlexGridSizer *item3 = new wxFlexGridSizer( 2, 0, 0 );

    wxStaticText *item4 = new wxStaticText( parent, ID_TEXT_NAME, wxT("Name:"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add(item4, 0, wxALIGN_RIGHT | wxALL, 5);

    wxTextCtrl *item5 = new wxTextCtrl( parent, ID_NAME, wxT(""), wxDefaultPosition, wxSize(140,-1), 0 );
    item3->Add(item5, 0, wxGROW | wxALL, 5);

    wxStaticText *item6 = new wxStaticText( parent, ID_TEXT_DS, wxT("Delay slots:"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add(item6, 0, wxALIGN_RIGHT | wxALL, 5);

    wxSpinCtrl *item7 =
        new wxSpinCtrl(parent, ID_DELAY_SLOTS, wxT("1"), wxDefaultPosition,
                       wxDefaultSize, 0, 0, 10000, 1);
    item3->Add(item7, 0, wxGROW | wxALL, 5);

    // global guard latency currently fixed at 1.
    // wxStaticText *item8 = new wxStaticText( parent,
    // ID_LABEL_GLOBAL_GUARD_LATENCY, wxT("Guard Latency:"), wxDefaultPosition,
    // wxDefaultSize, 0 ); item3->Add( item8, 0, wxALIGN_RIGHT|wxALL, 5 );

    // wxSpinCtrl *item9 = new wxSpinCtrl( parent, ID_GUARD_LATENCY, wxT("0"),
    // wxDefaultPosition, wxSize(100,-1), 0, 0, 1000, 0 ); item3->Add( item9, 0,
    // wxGROW|wxALL, 5 );

    wxStaticText *item10 = new wxStaticText( parent, ID_TEXT_AS, wxT("Address Space:"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add(item10, 0, wxALIGN_RIGHT | wxALL, 5);

    wxString *strs11 = (wxString*) NULL;
    wxChoice *item11 = new wxChoice( parent, ID_ADDRESS_SPACE, wxDefaultPosition, wxSize(100,-1), 0, strs11, 0 );
    item3->Add(item11, 0, wxGROW | wxALL, 5);

    item2->Add(item3, 0, wxALL, 5);

    wxStaticBox *item13 = new wxStaticBox( parent, -1, wxT("Operations:") );
    wxStaticBoxSizer *item12 = new wxStaticBoxSizer( item13, wxVERTICAL );
    operationsSizer_ = item12;

    wxListCtrl *item14 = new wxListCtrl( parent, ID_OPERATION_LIST, wxDefaultPosition, wxSize(300,200), wxLC_REPORT|wxLC_SINGLE_SEL|wxSUNKEN_BORDER );
    item12->Add(item14, 0, wxGROW | wxALL, 5);

    wxBoxSizer *item15 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item16 = new wxButton( parent, ID_ADD_OPERATION, wxT("&Add..."), wxDefaultPosition, wxDefaultSize, 0 );
    item15->Add( item16, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item17 = new wxButton( parent, ID_EDIT_OPERATION, wxT("&Edit..."), wxDefaultPosition, wxDefaultSize, 0 );
    item15->Add( item17, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item18 = new wxButton( parent, ID_DELETE_OPERATION, wxT("&Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    item15->Add( item18, 0, wxALIGN_CENTER|wxALL, 5 );

    item12->Add( item15, 0, wxALIGN_CENTER|wxALL, 5 );

    item2->Add(item12, 0, wxGROW | wxALL, 5);

    item1->Add( item2, 0, wxGROW|wxALL, 5 );

    wxStaticBox *item20 = new wxStaticBox( parent, -1, wxT("Ports:") );
    wxStaticBoxSizer *item19 = new wxStaticBoxSizer( item20, wxVERTICAL );
    portListSizer_ = item19;

    wxBoxSizer *item21 = new wxBoxSizer( wxVERTICAL );

    wxListCtrl *item22 = new wxListCtrl( parent, ID_PORT_LIST, wxDefaultPosition, wxSize(300,180), wxLC_REPORT|wxSUNKEN_BORDER );
    item21->Add(item22, 0, wxGROW | wxALL, 5);

    wxBoxSizer *item23 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item24 = new wxButton( parent, ID_EDIT_PORT, wxT("Edit..."), wxDefaultPosition, wxDefaultSize, 0 );
    item23->Add( item24, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item25 = new wxButton( parent, ID_DELETE_PORT, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    item23->Add( item25, 0, wxALIGN_CENTER|wxALL, 5 );

    item21->Add(item23, 0, wxALIGN_RIGHT, 5);

    wxBoxSizer *item26 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item27 = new wxButton( parent, ID_ADD_FU_PORT, wxT("Add operation port..."), wxDefaultPosition, wxDefaultSize, 0 );
    item26->Add( item27, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item28 = new wxButton( parent, ID_ADD_SR_PORT, wxT("Add special register port..."), wxDefaultPosition, wxDefaultSize, 0 );
    item26->Add( item28, 0, wxALIGN_CENTER|wxALL, 5 );

    item21->Add( item26, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticLine *item29 = new wxStaticLine( parent, ID_LINE, wxDefaultPosition, wxSize(20,-1), wxLI_HORIZONTAL );
    item21->Add(item29, 0, wxGROW | wxALL, 5);

    wxFlexGridSizer *item30 = new wxFlexGridSizer( 2, 0, 0 );

    wxStaticText *item31 = new wxStaticText( parent, ID_LABEL_RA_CHOICE, wxT("Return address port:"), wxDefaultPosition, wxDefaultSize, 0 );
    item30->Add( item31, 0, wxALIGN_CENTER|wxALL, 5 );

    wxString strs32[] = 
    {
        wxT("NONE")
    };
    wxChoice *item32 = new wxChoice( parent, ID_RA_CHOICE, wxDefaultPosition, wxSize(120,-1), 1, strs32, 0 );
    item30->Add( item32, 0, wxALIGN_CENTER|wxALL, 5 );

    item21->Add(item30, 0, wxGROW | wxALL, 5);

    wxStaticLine *item35 = new wxStaticLine( parent, ID_LINE, wxDefaultPosition, wxSize(20,-1), wxLI_HORIZONTAL );
    item21->Add(item35, 0, wxGROW | wxALL, 5);

    item19->Add(item21, 0, wxGROW | wxALL, 5);

    item1->Add(item19, 0, wxGROW | wxALL, 5);

    item0->Add( item1, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticLine *item36 = new wxStaticLine( parent, ID_LINE, wxDefaultPosition, wxSize(20,-1), wxLI_HORIZONTAL );
    item0->Add(item36, 0, wxGROW | wxALL, 5);

    wxGridSizer *item37 = new wxGridSizer( 2, 0, 0 );

    wxButton *item38 = new wxButton( parent, ID_HELP, wxT("&Help"), wxDefaultPosition, wxDefaultSize, 0 );
    item37->Add( item38, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item39 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item40 = new wxButton( parent, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item39->Add( item40, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item41 = new wxButton( parent, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item39->Add( item41, 0, wxALIGN_CENTER|wxALL, 5 );

    item37->Add(item39, 0, wxALIGN_RIGHT | wxALL, 5);

    item0->Add(item37, 0, wxGROW | wxLEFT | wxRIGHT, 5);

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;
}
