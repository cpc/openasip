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
 * @file FUImplementationDialog.cc
 *
 * Implementation of FUImplementationDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/statline.h>
#include <wx/spinctrl.h>

#include "FUImplementationDialog.hh"
#include "FUImplementation.hh"
#include "FUExternalPort.hh"
#include "FUPortImplementation.hh"
#include "BlockImplementationFile.hh"

#include "FUPortImplementationDialog.hh"
#include "FUImplementationParameterDialog.hh"
#include "FUExternalPortDialog.hh"

#include "FunctionUnit.hh"
#include "FUPort.hh"
#include "HWOperation.hh"

#include "BlockImplementationFile.hh"
#include "BlockImplementationFileDialog.hh"

#include "InformationDialog.hh"
#include "WxConversion.hh"
#include "WidgetTools.hh"
#include "Conversion.hh"
#include "ErrorDialog.hh"

using namespace HDB;
using namespace TTAMachine;

BEGIN_EVENT_TABLE(FUImplementationDialog, wxDialog)
    EVT_BUTTON(
        ID_EDIT_ARCH_PORT, FUImplementationDialog::onEditArchitecturePort)

    EVT_BUTTON(
        ID_ADD_EXTERNAL_PORT, FUImplementationDialog::onAddExternalPort)
    EVT_BUTTON(
        ID_EDIT_EXTERNAL_PORT, FUImplementationDialog::onEditExternalPort)
    EVT_BUTTON(
        ID_DELETE_EXTERNAL_PORT,
        FUImplementationDialog::onDeleteExternalPort)

    EVT_BUTTON(ID_ADD_PARAMETER, FUImplementationDialog::onAddParameter)
    EVT_BUTTON(ID_EDIT_PARAMETER, FUImplementationDialog::onEditParameter)
    EVT_BUTTON(ID_DELETE_PARAMETER, FUImplementationDialog::onDeleteParameter)

    EVT_BUTTON(ID_SET_OPCODE, FUImplementationDialog::onSetOpcode)
    EVT_BUTTON(ID_CLEAR_OPCODE, FUImplementationDialog::onClearOpcode)

    EVT_BUTTON(ID_ADD_SOURCE, FUImplementationDialog::onAddSourceFile)
    EVT_BUTTON(ID_DELETE_SOURCE, FUImplementationDialog::onDeleteSourceFile)

    EVT_LIST_ITEM_SELECTED(
        ID_ARCH_PORT_LIST, FUImplementationDialog::onArchPortSelection)
    EVT_LIST_ITEM_DESELECTED(
        ID_ARCH_PORT_LIST, FUImplementationDialog::onArchPortSelection)

    EVT_LIST_ITEM_SELECTED(
        ID_EXTERNAL_PORT_LIST, FUImplementationDialog::onExternalPortSelection)
    EVT_LIST_ITEM_DESELECTED(
        ID_EXTERNAL_PORT_LIST, FUImplementationDialog::onExternalPortSelection)

    EVT_LIST_ITEM_SELECTED(
        ID_PARAMETER_LIST, FUImplementationDialog::onParameterSelection)
    EVT_LIST_ITEM_DESELECTED(
        ID_PARAMETER_LIST, FUImplementationDialog::onParameterSelection)

    EVT_LIST_ITEM_SELECTED(
        ID_OPCODE_LIST, FUImplementationDialog::onOpcodeSelection)
    EVT_LIST_ITEM_DESELECTED(
        ID_OPCODE_LIST, FUImplementationDialog::onOpcodeSelection)

    EVT_BUTTON(wxID_OK, FUImplementationDialog::onOK)

    EVT_LIST_ITEM_SELECTED(
        ID_SOURCE_LIST, FUImplementationDialog::onSourceFileSelection)
    EVT_LIST_ITEM_DESELECTED(
        ID_SOURCE_LIST, FUImplementationDialog::onSourceFileSelection)

END_EVENT_TABLE()

/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param id Window identifier for the dialog window.
 * @param implementation FU implementation to modify.
 */
FUImplementationDialog::FUImplementationDialog(
    wxWindow* parent, wxWindowID id, FUImplementation& implementation,
    const TTAMachine::FunctionUnit& architecture) :
    wxDialog(parent, id, _T("Function Unit Implementation")),
    implementation_(implementation), architecture_(architecture) {

    createContents(this, true, true);

    // Initialize list widgets.
    archPortList_ = dynamic_cast<wxListCtrl*>(FindWindow(ID_ARCH_PORT_LIST));
    externalPortList_ =
        dynamic_cast<wxListCtrl*>(FindWindow(ID_EXTERNAL_PORT_LIST));
    parameterList_ = dynamic_cast<wxListCtrl*>(FindWindow(ID_PARAMETER_LIST));
    sourceList_ = dynamic_cast<wxListCtrl*>(FindWindow(ID_SOURCE_LIST));
    opcodeList_ = dynamic_cast<wxListCtrl*>(FindWindow(ID_OPCODE_LIST));

    // Create columns in list widgets.
    archPortList_->InsertColumn(0, _T("name"), wxLIST_FORMAT_LEFT, 130);

    archPortList_->InsertColumn(
        1, _T("architecture port"), wxLIST_FORMAT_LEFT, 130);

    externalPortList_->InsertColumn(0, _T("name"), wxLIST_FORMAT_LEFT, 260);

    parameterList_->InsertColumn(
        0, _T("parameter"), wxLIST_FORMAT_LEFT, 180);

    parameterList_->InsertColumn(
        1, _T("value"), wxLIST_FORMAT_LEFT, 80);

    opcodeList_->InsertColumn(
        0, _T("operation"), wxLIST_FORMAT_LEFT, 180);

    opcodeList_->InsertColumn(
        1, _T("opcode"), wxLIST_FORMAT_LEFT, 80);

    sourceList_->InsertColumn(
        0, _T("source file"), wxLIST_FORMAT_LEFT, 260);

    // Read string attributes from the FUImplementation object.
    name_ = WxConversion::toWxString(implementation_.moduleName());
    opcodePort_ = WxConversion::toWxString(implementation_.opcodePort());
    clkPort_ = WxConversion::toWxString(implementation_.clkPort());
    rstPort_ = WxConversion::toWxString(implementation_.rstPort());
    gLockPort_ = WxConversion::toWxString(implementation_.glockPort());
    gLockReqPort_ = WxConversion::toWxString(implementation_.glockReqPort());


    // Set text field validators.
    FindWindow(ID_NAME)->SetValidator(wxTextValidator(wxFILTER_ASCII, &name_));
    FindWindow(ID_OPCODE_PORT)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &opcodePort_));
    FindWindow(ID_CLK_PORT)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &clkPort_));
    FindWindow(ID_RST_PORT)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &rstPort_));
    FindWindow(ID_GLOCK_PORT)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &gLockPort_));
    FindWindow(ID_GLOCK_REQ_PORT)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &gLockReqPort_));


    // Disable conditional buttons initially.
    FindWindow(ID_EDIT_ARCH_PORT)->Disable();
    FindWindow(ID_EDIT_EXTERNAL_PORT)->Disable();
    FindWindow(ID_DELETE_EXTERNAL_PORT)->Disable();
    FindWindow(ID_EDIT_PARAMETER)->Disable();
    FindWindow(ID_DELETE_PARAMETER)->Disable();
    FindWindow(ID_SET_OPCODE)->Disable();
    FindWindow(ID_CLEAR_OPCODE)->Disable();
    FindWindow(ID_DELETE_SOURCE)->Disable();

    update();
}

/**
 * The Destructor.
 */
FUImplementationDialog::~FUImplementationDialog() {
}


void
FUImplementationDialog::update() {

    // Update architecture port list.
    archPortList_->DeleteAllItems();
    for (int i = 0; i < implementation_.architecturePortCount(); i++) {

        wxString portName = WxConversion::toWxString(
            implementation_.architecturePort(i).name());

        std::string archPortName =
            implementation_.architecturePort(i).architecturePort();

        const FUPort* port = architecture_.operationPort(archPortName);

        // Append operation operands bound to this port to the
        // otherwise meaningless port name.
        archPortName += ":";
        for (int j = 0; j < architecture_.operationCount(); j++) {
            const HWOperation* operation = architecture_.operation(j);
            if (operation->isBound(*port)) {

                std::string io = Conversion::toString(operation->io(*port));
                std::string operationName = operation->name();
                archPortName =
                    archPortName + " " + operationName + "[" + io + "]";
            }
        }
        archPortList_->InsertItem(i, portName);
        archPortList_->SetItem(i, 1, WxConversion::toWxString(archPortName));
    }

    // Update external port list.
    externalPortList_->DeleteAllItems();
    for (int i = 0; i < implementation_.externalPortCount(); i++) {
        wxString portName = WxConversion::toWxString(
            implementation_.externalPort(i).name());
        externalPortList_->InsertItem(i, portName);
    }

    // Update parameter list.
    parameterList_->DeleteAllItems();
    for (int i = 0; i < implementation_.parameterCount(); i++) {
        const FUImplementation::Parameter parameter =
            implementation_.parameter(i);        
        wxString param = WxConversion::toWxString(parameter.type);
        param.Append(_T(": "));
        param.Append(WxConversion::toWxString(parameter.name));
        parameterList_->InsertItem(i, param);
        parameterList_->SetItem(
            i, 1, WxConversion::toWxString(parameter.value));
    }

    // Update source file list.
    sourceList_->DeleteAllItems();
    for (int i = 0; i < implementation_.implementationFileCount(); i++) {
        wxString fileName = WxConversion::toWxString(
            implementation_.file(i).pathToFile());
        sourceList_->InsertItem(i, fileName);
    }

    // Update operation list.
    opcodeList_->DeleteAllItems();
    for (int i = 0; i < architecture_.operationCount(); i++) {
        std::string operation = architecture_.operation(i)->name();
        wxString opcode = _T("");
        if (implementation_.hasOpcode(operation)) {
            opcode =
                WxConversion::toWxString(implementation_.opcode(operation));
        }
        wxString operationName = WxConversion::toWxString(operation);
        opcodeList_->InsertItem(i, operationName);
        opcodeList_->SetItem(i, 1, opcode);
    }

    wxListEvent dummy;
    onArchPortSelection(dummy);
    onExternalPortSelection(dummy);
    onParameterSelection(dummy);
}

/**
 * Event handler for the edit architecture port button.
 *
 * Opens a FUPortImplementationdialog for modifying the selected architecure
 * port.
 */
void
FUImplementationDialog::onEditArchitecturePort(wxCommandEvent&) {

    FUPortImplementation* port = selectedArchPort();

    if (port == NULL) {
        return;
    }

    const BaseFUPort& archPort =
        *architecture_.port(port->architecturePort());

    FUPortImplementationDialog dialog(this, -1, *port, archPort);

    dialog.ShowModal();
    update();
}


/**
 * Event handler for the architecture port list selection changes.
 *
 * Updates edit/delete architecture port enabeld/disabled states.
 */
void
FUImplementationDialog::onArchPortSelection(wxListEvent&) {
    if (selectedArchPort() == NULL) {
        FindWindow(ID_EDIT_ARCH_PORT)->Disable();
    } else {
        FindWindow(ID_EDIT_ARCH_PORT)->Enable();
    }
}


/**
 * Event handler for the add external port button.
 *
 * Opens a FUExternalPortDialog for adding a new external port.
 */
void
FUImplementationDialog::onAddExternalPort(wxCommandEvent&) {

    FUExternalPort* port = new FUExternalPort(
        "", IN, "", "", implementation_);

    FUExternalPortDialog dialog(this, -1, *port, implementation_);

    if (dialog.ShowModal() != wxID_OK) {
        implementation_.deleteExternalPort(port);
        return;
    }

    update();
}


/**
 * Event handler for the edit external port button.
 *
 * Opens a FUExternalPort for modifying the selected external port.
 */
void
FUImplementationDialog::onEditExternalPort(wxCommandEvent&) {

    FUExternalPort* port = selectedExternalPort();

    if (port == NULL) {
        return;
    }

    FUExternalPortDialog dialog(this, -1, *port, implementation_);

    dialog.ShowModal();
    update();
}


/**
 * Event handler for the delete external port button.
 *
 * Deletes the selected external port.
 */
void
FUImplementationDialog::onDeleteExternalPort(wxCommandEvent&) {

    FUExternalPort* port = selectedExternalPort();

    if (port == NULL) {
        return;
    }

    implementation_.deleteExternalPort(port);
    update();
}

/**
 * Event handler for the external port list selection changes.
 *
 * Updates edit/delete external port enabeld/disabled states.
 */
void
FUImplementationDialog::onExternalPortSelection(wxListEvent&) {
    if (selectedExternalPort() == NULL) {
        FindWindow(ID_DELETE_EXTERNAL_PORT)->Disable();
        FindWindow(ID_EDIT_EXTERNAL_PORT)->Disable();
    } else {
        FindWindow(ID_DELETE_EXTERNAL_PORT)->Enable();
        FindWindow(ID_EDIT_EXTERNAL_PORT)->Enable();
    }
}


/**
 * Event handler for the add parameter button.
 *
 * Opens a FUImplementationParameterDialog for adding a new parameter.
 */
void
FUImplementationDialog::onAddParameter(wxCommandEvent&) {

    FUImplementation::Parameter parameter;

    FUImplementationParameterDialog dialog(this, -1, parameter);

    if (dialog.ShowModal() != wxID_OK) {
        return;
    }

    try {
        implementation_.addParameter(
            parameter.name, parameter.type, parameter.value);
    } catch (IllegalParameters& e) {
        wxString message = _T("FU implementation already contains ");
        message.Append(_T("a parameter\nwith name '"));
        message.Append(WxConversion::toWxString(parameter.name));
        message.Append(_T("'."));
        ErrorDialog dialog(this, message);
        dialog.ShowModal();
        return;
    }

    update();
}


/**
 * Event handler for the edit parameter button.
 *
 * Opens a FUImplementationParamaeterDialog for modifying the selected
 * parameter.
 */
void
FUImplementationDialog::onEditParameter(wxCommandEvent&) {

    FUImplementation::Parameter parameter = selectedParameter();
    std::string oldName = parameter.name;

    FUImplementationParameterDialog dialog(this, -1, parameter);

    if (dialog.ShowModal() == wxID_OK) {
        implementation_.removeParameter(oldName);
        implementation_.addParameter(
            parameter.name, parameter.type, parameter.value);

        // update external port parameter dependencies if needed
        for (int i = 0; i < implementation_.externalPortCount(); i++) {
            HDB::FUExternalPort& port = implementation_.externalPort(i);
            if (port.unsetParameterDependency(oldName)) {
                port.setParameterDependency(parameter.name);
            }
        }
    }

    update();
}


/**
 * Event handler for the delete parameter button.
 *
 * Deletes the selected parameter.
 */
void
FUImplementationDialog::onDeleteParameter(wxCommandEvent&) {

    FUImplementation::Parameter parameter = selectedParameter();

    implementation_.removeParameter(parameter.name);
    
    // update external port parameter dependencies
    for (int i = 0; i < implementation_.externalPortCount(); i++) {
        HDB::FUExternalPort& port = implementation_.externalPort(i);
        port.unsetParameterDependency(parameter.name);
    }

    update();
}

/**
 * Event handler for the parameter list selection changes.
 *
 * Updates edit/delete parameter enabled/disabled states.
 */
void
FUImplementationDialog::onParameterSelection(wxListEvent&) {
    if (selectedParameter().name == "") {
        FindWindow(ID_DELETE_PARAMETER)->Disable();
        FindWindow(ID_EDIT_PARAMETER)->Disable();
    } else {
        FindWindow(ID_DELETE_PARAMETER)->Enable();
        FindWindow(ID_EDIT_PARAMETER)->Enable();
    }
}


/**
 * Returns pointer to the architecture port selected in the architecture port
 * list.
 *
 * @return Selected architecture port.
 */
HDB::FUPortImplementation*
FUImplementationDialog::selectedArchPort() {

    long item = -1;
    item = archPortList_->GetNextItem(
        item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

    if (item == -1) {
        return NULL;
    }

    return &implementation_.architecturePort(item);
}

/**
 * Returns pointer to the external port selected in the external port list.
 *
 * @return Selected external port.
 */
HDB::FUExternalPort*
FUImplementationDialog::selectedExternalPort() {

    long item = -1;
    item = externalPortList_->GetNextItem(
        item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

    if (item == -1) {
        return NULL;
    }

    return &implementation_.externalPort(item);
}

/**
 * Returns copy of the selected parameter in the parameter list.
 *
 * @return Selected parameter.
 */
FUImplementation::Parameter
FUImplementationDialog::selectedParameter() {

    long item = -1;
    item = parameterList_->GetNextItem(
        item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

    if (item == -1) {
        FUImplementation::Parameter empty = {"", "", ""};
        return empty;
    }

    return implementation_.parameter(item);
}


/**
 * Event handler for the opcode list selection changes.
 *
 * Updates the enabled/disabled state of the set and clear opcode buttons.
 */
void
FUImplementationDialog::onOpcodeSelection(wxListEvent&) {
    std::string operation = WidgetTools::lcStringSelection(opcodeList_, 0);
    if (operation  == "") {
        FindWindow(ID_SET_OPCODE)->Disable();
        FindWindow(ID_CLEAR_OPCODE)->Disable();
    } else {
        FindWindow(ID_SET_OPCODE)->Enable();
        if (implementation_.hasOpcode(operation)) {
            FindWindow(ID_CLEAR_OPCODE)->Enable();
        } else {
            FindWindow(ID_CLEAR_OPCODE)->Disable();
        }
    }
}


/**
 * Event handler for the clear opcode button.
 *
 * Clears the opcode of the selected operation.
 */
void
FUImplementationDialog::onClearOpcode(wxCommandEvent&) {

    std::string operation = WidgetTools::lcStringSelection(opcodeList_, 0);

    if (operation == "") {
        return;
    }

    implementation_.unsetOpcode(operation);

    update();
}

/**
 * Event handler for the set opcode button.
 *
 * Sets the opcode for the selected operation.
 */
void
FUImplementationDialog::onSetOpcode(wxCommandEvent&) {

    std::string operation = WidgetTools::lcStringSelection(opcodeList_, 0);

    if (operation == "") {
        return;
    }

    int opcode = dynamic_cast<wxSpinCtrl*>(FindWindow(ID_OPCODE))->GetValue();

    for (int i = 0; i < architecture_.operationCount(); i++) {
        std::string operation = architecture_.operation(i)->name();
        if (implementation_.hasOpcode(operation) &&
            implementation_.opcode(operation) == opcode) {

            wxString message = _T("Opcode ");
            message.Append(WxConversion::toWxString(opcode));
            message.Append(_T(" is already used by operation "));
            message.Append(WxConversion::toWxString(operation));
            message.Append(_T("."));
            InformationDialog dialog(this, message);
            dialog.ShowModal();
            return;
        }
    }

    implementation_.setOpcode(operation, opcode);

    update();
}

/**
 * Event handler for the add source file button.
 *
 * Opens a source file dialog for adding a new implementation file to the list.
 */
void
FUImplementationDialog::onAddSourceFile(wxCommandEvent&) {

    BlockImplementationFile* file =
        new BlockImplementationFile("", BlockImplementationFile::VHDL);

    BlockImplementationFileDialog dialog(this, -1, *file);

    if (dialog.ShowModal() == wxID_OK) {
        implementation_.addImplementationFile(file);
        update();
    } else {
        delete file;
    }

}

/**
 * Event handler for the delete source file button.
 *
 * Removes the selected source file.
 */
void
FUImplementationDialog::onDeleteSourceFile(wxCommandEvent&) {

    std::string fileName = WidgetTools::lcStringSelection(sourceList_, 0);

    for (int i = 0; i < implementation_.implementationFileCount(); i++) {
        const HDB::BlockImplementationFile& file = implementation_.file(i);
        if (file.pathToFile() == fileName) {
            implementation_.removeImplementationFile(file);
        }
    }
    update();
}


/**
 * Event handler for the source file list selection changes.
 *
 * Updates delete source file button enabeld/disabled states.
 */
void
FUImplementationDialog::onSourceFileSelection(wxListEvent&) {
    if (WidgetTools::lcStringSelection(sourceList_, 0)  == "") {
        FindWindow(ID_DELETE_SOURCE)->Disable();
    } else {
        FindWindow(ID_DELETE_SOURCE)->Enable();
    }
}


/**
 * Event handler for the OK button.
 *
 * Validates the dialog data and updates the FUImplementation object string
 * attributes.
 */
void
FUImplementationDialog::onOK(wxCommandEvent&) {
    
    TransferDataFromWindow();

    name_ = name_.Trim(true).Trim(false);
    clkPort_ = clkPort_.Trim(true).Trim(false);
    rstPort_ = rstPort_.Trim(true).Trim(false);
    gLockPort_ = gLockPort_.Trim(true).Trim(false);
    gLockReqPort_ = gLockReqPort_.Trim(true).Trim(false);
    opcodePort_ = opcodePort_.Trim(true).Trim(false);

    if (name_.IsEmpty()) {
        wxString message = _T("Name field must not be empty.");
        InformationDialog dialog(this, message);
        dialog.ShowModal();
        return;
    }

    for (int i = 0; i < implementation_.architecturePortCount(); i++) {
        if (implementation_.architecturePort(i).name() == "") {
            wxString message =
                _T("No name specified for the architecture port '");
            std::string port =
                implementation_.architecturePort(i).architecturePort();
            message.Append(WxConversion::toWxString(port));
            message.Append(_T("'."));
            InformationDialog dialog(this, message);
            dialog.ShowModal();
            return;            
        }
    }

    if (architecture_.operationCount() > 1) {
        for (int i = 0; i < architecture_.operationCount(); i++) {
            if (!implementation_.hasOpcode(
                    architecture_.operation(i)->name())) {

                wxString message =
                    _T("No opcode specified for the operation '");
                std::string operation = architecture_.operation(i)->name();
                message.Append(WxConversion::toWxString(operation));
                message.Append(_T("'."));
                InformationDialog dialog(this, message);
                dialog.ShowModal();
                return;            
            }
        }
    }

    implementation_.setModuleName(WxConversion::toString(name_));
    implementation_.setClkPort(WxConversion::toString(clkPort_));
    implementation_.setRstPort(WxConversion::toString(rstPort_));
    implementation_.setOpcodePort(WxConversion::toString(opcodePort_));
    implementation_.setGlockPort(WxConversion::toString(gLockPort_));
    implementation_.setGlockReqPort(WxConversion::toString(gLockReqPort_));

    EndModal(wxID_OK);

}


/**
 * Creates the dialog contents.
 */
wxSizer*
FUImplementationDialog::createContents(
    wxWindow *parent, bool call_fit, bool set_sizer) {
    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxFlexGridSizer *item1 = new wxFlexGridSizer( 2, 0, 0 );

    wxFlexGridSizer *item2 = new wxFlexGridSizer( 2, 0, 0 );

    wxStaticText *item3 = new wxStaticText( parent, ID_LABEL_NAME, wxT("Name:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item3, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxTextCtrl *item4 = new wxTextCtrl( parent, ID_NAME, wxT(""), wxDefaultPosition, wxSize(160,-1), 0 );
    item2->Add( item4, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticText *item5 = new wxStaticText( parent, ID_LABEL_OPCODE_PORT_NAME, wxT("Opcode port:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item5, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxTextCtrl *item6 = new wxTextCtrl( parent, ID_OPCODE_PORT, wxT(""), wxDefaultPosition, wxSize(160,-1), 0 );
    item2->Add( item6, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticText *item7 = new wxStaticText( parent, ID_LABEL_CLK_PORT, wxT("Clock port:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item7, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxTextCtrl *item8 = new wxTextCtrl( parent, ID_CLK_PORT, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    item2->Add( item8, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticText *item9 = new wxStaticText( parent, ID_LABEL_GLOCK_PORT, wxT("Global lock port:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item9, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxTextCtrl *item10 = new wxTextCtrl( parent, ID_GLOCK_PORT, wxT(""), wxDefaultPosition, wxSize(150,-1), 0 );
    item2->Add( item10, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticText *item11 = new wxStaticText( parent, ID_LABEL_RST_PORT, wxT("Reset port:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item11, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxTextCtrl *item12 = new wxTextCtrl( parent, ID_RST_PORT, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    item2->Add( item12, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticText *item13 = new wxStaticText( parent, ID_LABEL_GLOCK_REQ_PORT, wxT("Global lock req. port:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item13, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxTextCtrl *item14 = new wxTextCtrl( parent, ID_GLOCK_REQ_PORT, wxT(""), wxDefaultPosition, wxSize(80,-1), 0 );
    item2->Add( item14, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item1->Add( item2, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticBox *item16 = new wxStaticBox( parent, -1, wxT("Source files:") );
    wxStaticBoxSizer *item15 = new wxStaticBoxSizer( item16, wxVERTICAL );

    wxListCtrl *item17 = new wxListCtrl( parent, ID_SOURCE_LIST, wxDefaultPosition, wxSize(160,120), wxLC_REPORT|wxSUNKEN_BORDER );
    item15->Add( item17, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item18 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item19 = new wxButton( parent, ID_ADD_SOURCE, wxT("Add..."), wxDefaultPosition, wxDefaultSize, 0 );
    item18->Add( item19, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxButton *item20 = new wxButton( parent, ID_DELETE_SOURCE, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    item18->Add( item20, 0, wxALIGN_CENTER|wxALL, 5 );

    item15->Add( item18, 0, wxALIGN_RIGHT|wxALIGN_BOTTOM, 5 );

    item1->Add( item15, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticBox *item22 = new wxStaticBox( parent, -1, wxT("Architecture ports:") );
    wxStaticBoxSizer *item21 = new wxStaticBoxSizer( item22, wxVERTICAL );

    wxListCtrl *item23 = new wxListCtrl( parent, ID_ARCH_PORT_LIST, wxDefaultPosition, wxSize(200,120), wxLC_REPORT|wxSUNKEN_BORDER );
    item21->Add( item23, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item24 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item25 = new wxButton( parent, ID_EDIT_ARCH_PORT, wxT("Edit..."), wxDefaultPosition, wxDefaultSize, 0 );
    item24->Add( item25, 0, wxALIGN_CENTER|wxALL, 5 );

    item21->Add( item24, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

    item1->Add( item21, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticBox *item27 = new wxStaticBox( parent, -1, wxT("Opcodes:") );
    wxStaticBoxSizer *item26 = new wxStaticBoxSizer( item27, wxVERTICAL );

    wxListCtrl *item28 = new wxListCtrl( parent, ID_OPCODE_LIST, wxDefaultPosition, wxSize(260,120), wxLC_REPORT|wxSUNKEN_BORDER );
    item26->Add( item28, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item29 = new wxBoxSizer( wxHORIZONTAL );

    wxSpinCtrl *item30 = new wxSpinCtrl( parent, ID_OPCODE, wxT("0"), wxDefaultPosition, wxSize(60,-1), 0, 0, 100, 0 );
    item29->Add( item30, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item31 = new wxButton( parent, ID_SET_OPCODE, wxT("Set"), wxDefaultPosition, wxDefaultSize, 0 );
    item29->Add( item31, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item32 = new wxButton( parent, ID_CLEAR_OPCODE, wxT("Clear"), wxDefaultPosition, wxDefaultSize, 0 );
    item29->Add( item32, 0, wxALIGN_CENTER|wxALL, 5 );

    item26->Add( item29, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

    item1->Add( item26, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticBox *item34 = new wxStaticBox( parent, -1, wxT("External ports:") );
    wxStaticBoxSizer *item33 = new wxStaticBoxSizer( item34, wxVERTICAL );

    wxListCtrl *item35 = new wxListCtrl( parent, ID_EXTERNAL_PORT_LIST, wxDefaultPosition, wxSize(200,120), wxLC_REPORT|wxSUNKEN_BORDER );
    item33->Add( item35, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item36 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item37 = new wxButton( parent, ID_ADD_EXTERNAL_PORT, wxT("Add..."), wxDefaultPosition, wxDefaultSize, 0 );
    item36->Add( item37, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxButton *item38 = new wxButton( parent, ID_EDIT_EXTERNAL_PORT, wxT("Edit..."), wxDefaultPosition, wxDefaultSize, 0 );
    item36->Add( item38, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item39 = new wxButton( parent, ID_DELETE_EXTERNAL_PORT, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    item36->Add( item39, 0, wxALIGN_CENTER|wxALL, 5 );

    item33->Add( item36, 0, wxALIGN_CENTER, 5 );

    item1->Add( item33, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticBox *item41 = new wxStaticBox( parent, -1, wxT("Parameters:") );
    wxStaticBoxSizer *item40 = new wxStaticBoxSizer( item41, wxVERTICAL );

    wxListCtrl *item42 = new wxListCtrl( parent, ID_PARAMETER_LIST, wxDefaultPosition, wxSize(260,120), wxLC_REPORT|wxSUNKEN_BORDER );
    item40->Add( item42, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item43 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item44 = new wxButton( parent, ID_ADD_PARAMETER, wxT("Add..."), wxDefaultPosition, wxDefaultSize, 0 );
    item43->Add( item44, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item45 = new wxButton( parent, ID_EDIT_PARAMETER, wxT("Edit..."), wxDefaultPosition, wxDefaultSize, 0 );
    item43->Add( item45, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item46 = new wxButton( parent, ID_DELETE_PARAMETER, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    item43->Add( item46, 0, wxALIGN_CENTER|wxALL, 5 );

    item40->Add( item43, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

    item1->Add( item40, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    item0->Add( item1, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticLine *item47 = new wxStaticLine( parent, ID_LINE, wxDefaultPosition, wxSize(20,-1), wxLI_HORIZONTAL );
    item0->Add( item47, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item48 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item49 = new wxButton( parent, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item48->Add( item49, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item50 = new wxButton( parent, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item48->Add( item50, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item48, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;
}
