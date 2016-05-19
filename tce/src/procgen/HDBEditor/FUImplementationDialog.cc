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
 * @file FUImplementationDialog.cc
 *
 * Implementation of FUImplementationDialog class.
 *
 * @author Veli-Pekka Jääskeläinen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/statline.h>
#include <wx/spinctrl.h>

#include <vector>
#include <iostream>

#include "FUImplementationDialog.hh"
#include "FUImplementation.hh"
#include "FUExternalPort.hh"
#include "FUPortImplementation.hh"
#include "BlockImplementationFile.hh"

#include "FUPortImplementationDialog.hh"
#include "ImplementationParameterDialog.hh"
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

#ifdef ALLOW_OPCODE_EDITING
    EVT_BUTTON(ID_SET_OPCODE, FUImplementationDialog::onSetOpcode)
    EVT_BUTTON(ID_CLEAR_OPCODE, FUImplementationDialog::onClearOpcode)
#endif

    EVT_BUTTON(ID_ADD_SOURCE, FUImplementationDialog::onAddSourceFile)
    EVT_BUTTON(ID_DELETE_SOURCE, FUImplementationDialog::onDeleteSourceFile)
    EVT_BUTTON(ID_MOVE_SOURCE_UP, FUImplementationDialog::onMoveSourceFileUp)
    EVT_BUTTON(ID_MOVE_SOURCE_DOWN, 
	       FUImplementationDialog::onMoveSourceFileDown)

    EVT_LIST_ITEM_SELECTED(
        ID_ARCH_PORT_LIST, FUImplementationDialog::onArchPortSelection)
    EVT_LIST_ITEM_DESELECTED(
        ID_ARCH_PORT_LIST, FUImplementationDialog::onArchPortSelection)
    EVT_LIST_ITEM_ACTIVATED(
        ID_ARCH_PORT_LIST, FUImplementationDialog::onArchPortActivation)

    EVT_LIST_ITEM_SELECTED(
        ID_EXTERNAL_PORT_LIST, 
        FUImplementationDialog::onExternalPortSelection)
    EVT_LIST_ITEM_ACTIVATED(
        ID_EXTERNAL_PORT_LIST, 
        FUImplementationDialog::onExternalPortActivation)
    EVT_LIST_ITEM_DESELECTED(
        ID_EXTERNAL_PORT_LIST, 
        FUImplementationDialog::onExternalPortSelection)

    EVT_LIST_ITEM_SELECTED(
        ID_PARAMETER_LIST, FUImplementationDialog::onParameterSelection)
    EVT_LIST_ITEM_ACTIVATED(
        ID_PARAMETER_LIST, FUImplementationDialog::onParameterActivation)
    EVT_LIST_ITEM_DESELECTED(
        ID_PARAMETER_LIST, FUImplementationDialog::onParameterSelection)
#ifdef ALLOW_OPCODE_EDITING
    EVT_LIST_ITEM_SELECTED(
        ID_OPCODE_LIST, FUImplementationDialog::onOpcodeSelection)
    EVT_LIST_ITEM_DESELECTED(
        ID_OPCODE_LIST, FUImplementationDialog::onOpcodeSelection)
#endif
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
    implementation_(implementation), architecture_(architecture),
    opcodeWarningShowed_(false) {

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
    // If implementation attributes are empty, use default values
    name_ = WxConversion::toWxString(implementation_.moduleName());
    opcodePort_ = WxConversion::toWxString(implementation_.opcodePort());
    if (implementation_.clkPort().empty()) {
        clkPort_ = WxConversion::toWxString("clk");
    } else {
        clkPort_ = WxConversion::toWxString(implementation_.clkPort());
    }
    if (implementation_.rstPort().empty()) {
        rstPort_ = WxConversion::toWxString("rstx");
    } else {
        rstPort_ = WxConversion::toWxString(implementation_.rstPort());
    }
    if (implementation_.glockPort().empty()) {
        gLockPort_ = WxConversion::toWxString("glock");
    } else {
        gLockPort_ = WxConversion::toWxString(implementation_.glockPort());
    }
    gLockReqPort_ = WxConversion::toWxString(implementation_.glockReqPort());


    // Set text field validators.
    FindWindow(ID_NAME)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &name_));
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
#ifdef ALLOW_OPCODE_EDITING
    FindWindow(ID_SET_OPCODE)->Disable();
    FindWindow(ID_CLEAR_OPCODE)->Disable();
#endif
    FindWindow(ID_DELETE_SOURCE)->Disable();
    FindWindow(ID_MOVE_SOURCE_UP)->Disable();
    FindWindow(ID_MOVE_SOURCE_DOWN)->Disable();

    update(true);
}

/**
 * The Destructor.
 */
FUImplementationDialog::~FUImplementationDialog() {
}

/**
 * Update handler
 * Updates UI values to reflect new state. If called from constructor, 
 * fills default values for opcodes.
 *
 * @param onInit Set true whe called from constructor
 */
void
FUImplementationDialog::update(bool onInit) {

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
    if (implementation_.implementationFileCount() != 0) {
        sourceList_->SetColumnWidth(0, wxLIST_AUTOSIZE);
    }

    // Update operation list.
    opcodeList_->DeleteAllItems();
    // arrange operations in alphabetical order
    std::set<std::string> opcodeSet;
    for (int i = 0; i < architecture_.operationCount(); i++) {
        std::string operation = architecture_.operation(i)->name();
        opcodeSet.insert(operation);
    }
    // operation opcodes should be generated according to the alphabetical
    // order
    int index = 0;
    for (std::set<std::string>::iterator i = opcodeSet.begin();
         i != opcodeSet.end(); i++) {
        std::string operation = *i;
        wxString opcode = _T("");
        if (implementation_.hasOpcode(operation)) {
            int opcodeIndex = implementation_.opcode(operation);
            if (opcodeIndex != index && !opcodeWarningShowed_) {
#ifdef ALLOW_OPCODE_EDITING
                // show warning about future incompatibility
                wxMessageDialog errorDialog(
                    this,
                    _T("Illegal opcodes. Design might not work in future. "
                       "Opcode IDs must be generated according to the "
                       "alphabetical order of operation names. Please fix "
                       "FU's vhdl code and HDB entry"),
                    _T("Illegal opcodes"), wxOK);
#else
                // operations must be in alphaberical order
                wxMessageDialog errorDialog(
                    this,
                    _T("Illegal opcodes. Design won't work. Opcode IDs must "
                       "be generated according to the alphabetical order of "
                       "operation names."),
                    _T("Illegal opcodes"), wxOK);
#endif
                opcodeWarningShowed_ = true;
                errorDialog.ShowModal();
            }
            opcode =
                WxConversion::toWxString(implementation_.opcode(operation));
        }
#ifndef ALLOW_OPCODE_EDITING
        else {
            // add default opcode (without ability to modify it)
            opcode = WxConversion::toWxString(index);
        }
#else
        else if (onInit) {
            // add default opcode
            opcode = WxConversion::toWxString(index);
            implementation_.setOpcode(operation, index);
        }
#endif
        wxString operationName = WxConversion::toWxString(operation);
        opcodeList_->InsertItem(index, operationName);
        opcodeList_->SetItem(index, 1, opcode);
        ++index;
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
 * Event handler for the architecture port list activateion
 *
 * Opens a FUPortImplementationdialog for modifying the selected architecure
 * port.
 */
void
FUImplementationDialog::onArchPortActivation(wxListEvent&) {
    FUPortImplementation* port = selectedArchPort();

    assert(port != NULL);
    
    const BaseFUPort& archPort =
        *architecture_.port(port->architecturePort());

    FUPortImplementationDialog dialog(this, -1, *port, archPort);

    dialog.ShowModal();
    update();
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
 * Event handler for the activate external port list item
 *
 * Opens a FUExternalPort for modifying the selected external port.
 */
void
FUImplementationDialog::onExternalPortActivation(wxListEvent&) {

    FUExternalPort* port = selectedExternalPort();
    assert(port != NULL);

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
 * Opens a ImplementationParameterDialog for adding a new parameter.
 */
void
FUImplementationDialog::onAddParameter(wxCommandEvent&) {

    FUImplementation::Parameter parameter;

    ImplementationParameterDialog dialog(this, -1, parameter);

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

    ImplementationParameterDialog dialog(this, -1, parameter);

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
 * Event handler for the parameter list activateion.
 *
 * Opens a FUImplementationParamaeterDialog for modifying the selected
 * parameter.
 */
void
FUImplementationDialog::onParameterActivation(wxListEvent&) {

    FUImplementation::Parameter parameter = selectedParameter();
    std::string oldName = parameter.name;

    ImplementationParameterDialog dialog(this, -1, parameter);

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

#ifdef ALLOW_OPCODE_EDITING
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
#endif

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

    FindWindow(ID_DELETE_SOURCE)->Disable();
    FindWindow(ID_MOVE_SOURCE_UP)->Disable();
    FindWindow(ID_MOVE_SOURCE_DOWN)->Disable();
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

    FindWindow(ID_DELETE_SOURCE)->Disable();
    FindWindow(ID_MOVE_SOURCE_UP)->Disable();
    FindWindow(ID_MOVE_SOURCE_DOWN)->Disable();
    
    update();
}

/**
 * Event handler for the move source file up button.
 *
 * Moves the selected source file up on the files list.
 */
void
FUImplementationDialog::onMoveSourceFileUp(wxCommandEvent&) {

    if (implementation_.implementationFileCount() > 1) {
        std::string fileName = 
            WidgetTools::lcStringSelection(sourceList_, 0);
        std::vector<std::string> pathToFileList;
        int originalImplementationFileCount = 
            implementation_.implementationFileCount();

        for (int i = 0; i < originalImplementationFileCount; i++) {
            HDB::BlockImplementationFile& file = implementation_.file(0);
            pathToFileList.push_back(file.pathToFile());
            implementation_.removeImplementationFile(file);
        }
        
        for (unsigned int i = 1; i < pathToFileList.size(); i++) {
            if (pathToFileList.at(i) == fileName) {
                pathToFileList.erase(pathToFileList.begin() + i);
                pathToFileList.insert(
                    pathToFileList.begin() + i - 1, fileName);
                break;
            }
        }
        
        for (unsigned int i = 0; i < pathToFileList.size(); i++) {
            BlockImplementationFile* file =
                new BlockImplementationFile(pathToFileList.at(i), 
                                            BlockImplementationFile::VHDL);
            implementation_.addImplementationFile(file);
        }
        
        pathToFileList.clear();
        update();

        for (int i = 0; i < implementation_.implementationFileCount(); i++) {
            if (implementation_.file(i).pathToFile() == fileName) {
                sourceList_->SetItemState(i, wxLIST_STATE_SELECTED, 
                                          wxLIST_STATE_SELECTED);
            }
        }
    }
}

/**
 * Event handler for the move source file down button.
 *
 * Moves the selected source file down on the files list.
 */
void
FUImplementationDialog::onMoveSourceFileDown(wxCommandEvent&) {
    if (implementation_.implementationFileCount() > 1) {
        std::string fileName = 
            WidgetTools::lcStringSelection(sourceList_, 0);
        std::vector<std::string> pathToFileList;
        
        int originalImplementationFileCount = 
            implementation_.implementationFileCount();
        
        for (int i = 0; i < originalImplementationFileCount; i++) {
            HDB::BlockImplementationFile& file = implementation_.file(0);
            pathToFileList.push_back(file.pathToFile());
            implementation_.removeImplementationFile(file);
        }
        
        for (unsigned int i = 0; i < (pathToFileList.size() - 1); i++) {
            if (pathToFileList.at(i) == fileName) {
                pathToFileList.erase(pathToFileList.begin() + i);
                pathToFileList.insert(
                    pathToFileList.begin() + i + 1, fileName);
                break;
            }
        }
                
        for (unsigned int i = 0; i < pathToFileList.size(); i++) {
            BlockImplementationFile* file =
                new BlockImplementationFile(pathToFileList.at(i), 
                                            BlockImplementationFile::VHDL);
            implementation_.addImplementationFile(file);
        }
        
        pathToFileList.clear();
        update();

        for (int i = 0; i < implementation_.implementationFileCount(); i++) {
            if (implementation_.file(i).pathToFile() == fileName) {
                sourceList_->SetItemState(i, wxLIST_STATE_SELECTED, 
                                          wxLIST_STATE_SELECTED);
            }
        }
    }
}

/**
 * Event handler for the source file list selection changes.
 *
 * Updates delete source file and move up/down buttons enabled/disabled 
 * states.
 */
void
FUImplementationDialog::onSourceFileSelection(wxListEvent&) {
    if (WidgetTools::lcStringSelection(sourceList_, 0)  == "") {
        FindWindow(ID_DELETE_SOURCE)->Disable();
        FindWindow(ID_MOVE_SOURCE_UP)->Disable();
        FindWindow(ID_MOVE_SOURCE_DOWN)->Disable();
    } else {
        FindWindow(ID_DELETE_SOURCE)->Enable();
        FindWindow(ID_MOVE_SOURCE_UP)->Enable();
        FindWindow(ID_MOVE_SOURCE_DOWN)->Enable();
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
        for (int k = i+1; k < implementation_.architecturePortCount(); k++) {
            if (implementation_.architecturePort(i).name() == 
                    implementation_.architecturePort(k).name()) {
                wxString message =
                _T("Ports '");
                std::string port =
                    implementation_.architecturePort(i).architecturePort();
                message.Append(WxConversion::toWxString(port));
                message.Append(_T("' and '"));
                port =
                    implementation_.architecturePort(k).architecturePort();
                message.Append(WxConversion::toWxString(port));
                message.Append(_T("' have the same name."));
                InformationDialog dialog(this, message);
                dialog.ShowModal();
                return;       
            }
        }
    }
#ifdef ALLOW_OPCODE_EDITING 
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
#endif

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

    wxFlexGridSizer *item17_1 = new wxFlexGridSizer( 2, 0, 0 );

    wxListCtrl *item17 = new wxListCtrl( parent, ID_SOURCE_LIST, wxDefaultPosition, wxSize(300,150), wxLC_REPORT|wxSUNKEN_BORDER );
    item17_1->Add( item17, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item17_3 = new wxBoxSizer( wxVERTICAL );
    
    wxButton *item60 = new wxButton( parent, ID_MOVE_SOURCE_UP, wxT("▴"), wxDefaultPosition, wxSize(20, 20), 0 );
    item17_3->Add( item60, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item61 = new wxButton( parent, ID_MOVE_SOURCE_DOWN, wxT("▾"), wxDefaultPosition, wxSize(20, 20), 0 );
    item17_3->Add( item61, 0, wxALIGN_CENTER|wxALL, 5 );
    
    item17_1->Add( item17_3, 0, wxALIGN_RIGHT|wxALIGN_CENTER, 5 );

    item15->Add( item17_1, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
    
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
#ifdef ALLOW_OPCODE_EDITING
    wxSpinCtrl *item30 = new wxSpinCtrl( parent, ID_OPCODE, wxT("0"), wxDefaultPosition, wxSize(60,-1), 0, 0, 100, 0 );
    item29->Add( item30, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item31 = new wxButton( parent, ID_SET_OPCODE, wxT("Set"), wxDefaultPosition, wxDefaultSize, 0 );
    item29->Add( item31, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item32 = new wxButton( parent, ID_CLEAR_OPCODE, wxT("Clear"), wxDefaultPosition, wxDefaultSize, 0 );
    item29->Add( item32, 0, wxALIGN_CENTER|wxALL, 5 );
#endif
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
