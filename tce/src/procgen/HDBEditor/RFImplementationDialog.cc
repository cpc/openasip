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
 * @file RFImplementationDialog.cc
 *
 * Implementation of RFImplementationDialog class.
 *
 * @author Veli-Pekka Jääskeläinen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/statline.h>

#include <vector>

#include "RFImplementationDialog.hh"
#include "RFImplementation.hh"
#include "RFPortImplementation.hh"
#include "BlockImplementationFile.hh"

#include "RFExternalPort.hh"
#include "RFExternalPortDialog.hh"

#include "RFPortImplementationDialog.hh"
#include "ImplementationParameterDialog.hh"

#include "BlockImplementationFile.hh"
#include "BlockImplementationFileDialog.hh"

#include "InformationDialog.hh"
#include "WxConversion.hh"
#include "WidgetTools.hh"
#include "ErrorDialog.hh"

using namespace HDB;

BEGIN_EVENT_TABLE(RFImplementationDialog, wxDialog)

    EVT_BUTTON(ID_ADD_PORT, RFImplementationDialog::onAddPort)
    EVT_BUTTON(ID_MODIFY_PORT, RFImplementationDialog::onModifyPort)
    EVT_BUTTON(ID_DELETE_PORT, RFImplementationDialog::onDeletePort)
    EVT_BUTTON(ID_ADD_SOURCE, RFImplementationDialog::onAddSourceFile)
    EVT_BUTTON(ID_DELETE_SOURCE, RFImplementationDialog::onDeleteSourceFile)
    EVT_BUTTON(ID_MOVE_SOURCE_UP, RFImplementationDialog::onMoveSourceFileUp)
    EVT_BUTTON(ID_MOVE_SOURCE_DOWN, 
               RFImplementationDialog::onMoveSourceFileDown)

    EVT_BUTTON(wxID_OK, RFImplementationDialog::onOK)

    EVT_LIST_ITEM_SELECTED(
        ID_PORT_LIST, RFImplementationDialog::onPortSelection)
    EVT_LIST_ITEM_DESELECTED(
        ID_PORT_LIST, RFImplementationDialog::onPortSelection)
    EVT_LIST_ITEM_SELECTED(
        ID_SOURCE_LIST, RFImplementationDialog::onSourceFileSelection)
    EVT_LIST_ITEM_DESELECTED(
        ID_SOURCE_LIST, RFImplementationDialog::onSourceFileSelection)

    EVT_BUTTON(
        ID_ADD_EXTERNAL_PORT, RFImplementationDialog::onAddExternalPort)
    EVT_BUTTON(
        ID_EDIT_EXTERNAL_PORT, RFImplementationDialog::onEditExternalPort)
    EVT_BUTTON(
        ID_DELETE_EXTERNAL_PORT,
        RFImplementationDialog::onDeleteExternalPort)

    EVT_LIST_ITEM_SELECTED(
        ID_EXTERNAL_PORT_LIST,
        RFImplementationDialog::onExternalPortSelection)
    EVT_LIST_ITEM_ACTIVATED(
        ID_EXTERNAL_PORT_LIST,
        RFImplementationDialog::onExternalPortActivation)
    EVT_LIST_ITEM_DESELECTED(
        ID_EXTERNAL_PORT_LIST,
        RFImplementationDialog::onExternalPortSelection)

    EVT_LIST_ITEM_SELECTED(
        ID_PARAMETER_LIST, RFImplementationDialog::onParameterSelection)
    EVT_LIST_ITEM_ACTIVATED(
        ID_PARAMETER_LIST, RFImplementationDialog::onParameterActivation)
    EVT_LIST_ITEM_DESELECTED(
        ID_PARAMETER_LIST, RFImplementationDialog::onParameterSelection)

    EVT_BUTTON(ID_ADD_PARAMETER, RFImplementationDialog::onAddParameter)
    EVT_BUTTON(ID_EDIT_PARAMETER, RFImplementationDialog::onEditParameter)
    EVT_BUTTON(ID_DELETE_PARAMETER, RFImplementationDialog::onDeleteParameter)



END_EVENT_TABLE()

/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param id Window identifier for the dialog window.
 * @param implementation RF implementation to modify.
 */
RFImplementationDialog::RFImplementationDialog(
    wxWindow* parent, wxWindowID id, RFImplementation& implementation):
    wxDialog(parent, id, _T("Register File Implementation")),
    implementation_(implementation) {

    createContents(this, true, true);

    // Initialize list widgets.
    portList_ = dynamic_cast<wxListCtrl*>(FindWindow(ID_PORT_LIST));
    sourceList_ = dynamic_cast<wxListCtrl*>(FindWindow(ID_SOURCE_LIST));
    externalPortList_ = dynamic_cast<wxListCtrl*>(FindWindow(
        ID_EXTERNAL_PORT_LIST));
    parameterList_ = dynamic_cast<wxListCtrl*>(FindWindow(ID_PARAMETER_LIST));

    portList_->InsertColumn(0, _T("name"), wxLIST_FORMAT_LEFT, 200);
    sourceList_->InsertColumn(
        0, _T("source file"), wxLIST_FORMAT_LEFT, 260);

    // Create columns in list widgets.
    externalPortList_->InsertColumn(0, _T("name"), wxLIST_FORMAT_LEFT, 260);
    parameterList_->InsertColumn(0, _T("parameter"), wxLIST_FORMAT_LEFT, 180);
    parameterList_->InsertColumn(1, _T("value"), wxLIST_FORMAT_LEFT, 80);

    // Read string attributes from the RFImplementation object.
    name_ = WxConversion::toWxString(implementation_.moduleName());
    clkPort_ = WxConversion::toWxString(implementation_.clkPort());
    rstPort_ = WxConversion::toWxString(implementation_.rstPort());
    gLockPort_ = WxConversion::toWxString(implementation_.glockPort());
    guardPort_ = WxConversion::toWxString(implementation_.guardPort());
    widthParam_ = WxConversion::toWxString(implementation_.widthParameter());
    sizeParam_ = WxConversion::toWxString(implementation_.sizeParameter());


    // Set text field validators.
    FindWindow(ID_NAME)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &name_));
    FindWindow(ID_CLK_PORT)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &clkPort_));
    FindWindow(ID_RESET_PORT)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &rstPort_));
    FindWindow(ID_GLOCK_PORT)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &gLockPort_));
    FindWindow(ID_GUARD_PORT)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &guardPort_));
    FindWindow(ID_WIDTH_PARAMETER)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &widthParam_));
    FindWindow(ID_SIZE_PARAMETER)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &sizeParam_));


    // Disable conditional buttons initially.
    FindWindow(ID_DELETE_PORT)->Disable();
    FindWindow(ID_MODIFY_PORT)->Disable();
    FindWindow(ID_DELETE_SOURCE)->Disable();
    FindWindow(ID_EDIT_EXTERNAL_PORT)->Disable();
    FindWindow(ID_DELETE_EXTERNAL_PORT)->Disable();
    FindWindow(ID_EDIT_PARAMETER)->Disable();
    FindWindow(ID_DELETE_PARAMETER)->Disable();

    update();
}

/**
 * The Destructor.
 */
RFImplementationDialog::~RFImplementationDialog() {
}


void
RFImplementationDialog::update() {

    // Update port list.
    portList_->DeleteAllItems();
    for (int i = 0; i < implementation_.portCount(); i++) {
        wxString portName = WxConversion::toWxString(
            implementation_.port(i).name());
        portList_->InsertItem(i, portName);
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
        const RFImplementation::Parameter parameter =
            implementation_.parameter(i);
        wxString param = WxConversion::toWxString(parameter.type);
        param.Append(_T(": "));
        param.Append(WxConversion::toWxString(parameter.name));
        parameterList_->InsertItem(i, param);
        parameterList_->SetItem(
            i, 1, WxConversion::toWxString(parameter.value));
    }

    wxListEvent dummy;
    onPortSelection(dummy);
    onExternalPortSelection(dummy);
    onParameterSelection(dummy);
}


/**
 * Event handler for the add port button.
 *
 * Opens a RFPortImplementationdialog for adding a new port.
 */
void
RFImplementationDialog::onAddPort(wxCommandEvent&) {

    RFPortImplementation* port =
        new RFPortImplementation("", HDB::IN, "", "" , "", implementation_);

    RFPortImplementationDialog dialog(this, -1, *port);

    if (dialog.ShowModal() != wxID_OK) {
        implementation_.deletePort(port);
    }

    update();
}


/**
 * Event handler for the port list selection changes.
 *
 * Updates the delete port button enabled/disabled state.
 */
void
RFImplementationDialog::onPortSelection(wxListEvent&) {
    if (selectedPort() == NULL) {
        FindWindow(ID_DELETE_PORT)->Disable();
        FindWindow(ID_MODIFY_PORT)->Disable();
    } else {
        FindWindow(ID_DELETE_PORT)->Enable();
        FindWindow(ID_MODIFY_PORT)->Enable();
    }
}

/**
 * Event handler for the delete port button.
 *
 * Deletes the selected port.
 */
void
RFImplementationDialog::onDeletePort(wxCommandEvent&) {

    RFPortImplementation* port = selectedPort();

    if (port == NULL) {
        return;
    }

    implementation_.deletePort(port);
    update();
}

/**
 * Event handler for the modify port button.
 *
 * Modifies the selected port.
 */
void
RFImplementationDialog::onModifyPort(wxCommandEvent&) {

    RFPortImplementation* port = selectedPort();

    if (port == NULL) {
        return;
    }

    RFPortImplementationDialog dialog(this, -1, *port);
    dialog.ShowModal();
    update();
}

/**
 * Returns pointer to the port selected in the port list.
 *
 * @return Selected port.
 */
HDB::RFPortImplementation*
RFImplementationDialog::selectedPort() {

    long item = -1;
    item = portList_->GetNextItem(
        item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

    if (item == -1) {
        return NULL;
    }

    return &implementation_.port(item);
}


/**
 * Returns pointer to the external port selected in the external port list.
 *
 * @return Selected external port.
 */
HDB::RFExternalPort*
RFImplementationDialog::selectedExternalPort() {

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
HDB::RFImplementation::Parameter
RFImplementationDialog::selectedParameter() {
    long item = -1;
    item = parameterList_->GetNextItem(
        item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

    if (item == -1) {
        RFImplementation::Parameter empty = {"", "", ""};
        return empty;
    }

    return implementation_.parameter(item);
}


/**
 * Event handler for the activate external port list item
 *
 * Opens a RFExternalPort for modifying the selected external port.
 */
void RFImplementationDialog::onExternalPortActivation(wxListEvent&) {
    RFExternalPort* port = selectedExternalPort();
    assert(port != NULL);

    RFExternalPortDialog dialog(this, -1, *port, implementation_);

    dialog.ShowModal();
    update();
}


/**
 * Event handler for the external port list selection changes.
 *
 * Updates edit/delete external port enabeld/disabled states.
 */
void RFImplementationDialog::onExternalPortSelection(wxListEvent&) {
    if (selectedExternalPort() == NULL) {
        FindWindow(ID_DELETE_EXTERNAL_PORT)->Disable();
        FindWindow(ID_EDIT_EXTERNAL_PORT)->Disable();
    } else {
        FindWindow(ID_DELETE_EXTERNAL_PORT)->Enable();
        FindWindow(ID_EDIT_EXTERNAL_PORT)->Enable();
    }
}


/**
 * Event handler for the add external port button.
 *
 * Opens a RFExternalPortDialog for adding a new external port.
 */
void RFImplementationDialog::onAddExternalPort(wxCommandEvent&) {

    RFExternalPort* port = new RFExternalPort(
        "", IN, "", "", implementation_);

    RFExternalPortDialog dialog(this, -1, *port, implementation_);

    if (dialog.ShowModal() != wxID_OK) {
        implementation_.deleteExternalPort(port);
        return;
    }

    update();
}


/**
 * Event handler for the edit external port button.
 *
 * Opens a RFExternalPort for modifying the selected external port.
 */
void RFImplementationDialog::onEditExternalPort(wxCommandEvent&) {

    RFExternalPort* port = selectedExternalPort();

    if (port == NULL) {
        return;
    }

    RFExternalPortDialog dialog(this, -1, *port, implementation_);

    dialog.ShowModal();
    update();
}


/**
 * Event handler for the delete external port button.
 *
 * Deletes the selected external port.
 */
void RFImplementationDialog::onDeleteExternalPort(wxCommandEvent&) {

    RFExternalPort* port = selectedExternalPort();

    if (port == NULL) {
        return;
    }

    implementation_.deleteExternalPort(port);
    update();
}


/**
 * Event handler for the parameter list activateion.
 *
 * Opens a RFImplementationParamaeterDialog for modifying the selected
 * parameter.
 */
void
RFImplementationDialog::onParameterActivation(wxListEvent&) {
    RFImplementation::Parameter parameter = selectedParameter();
    std::string oldName = parameter.name;

    ImplementationParameterDialog dialog(this, -1, parameter);

    if (dialog.ShowModal() == wxID_OK) {
        implementation_.removeParameter(oldName);
        implementation_.addParameter(
            parameter.name, parameter.type, parameter.value);

        // update external port parameter dependencies if needed
        for (int i = 0; i < implementation_.externalPortCount(); i++) {
            HDB::RFExternalPort& port = implementation_.externalPort(i);
            if (port.unsetParameterDependency(oldName)) {
                port.setParameterDependency(parameter.name);
            }
        }
    }

    update();
}


/**
 * Event handler for the parameter list selection changes.
 *
 * Updates edit/delete parameter enabled/disabled states.
 */
void
RFImplementationDialog::onParameterSelection(wxListEvent&) {
    if (selectedParameter().name == "") {
        FindWindow(ID_DELETE_PARAMETER)->Disable();
        FindWindow(ID_EDIT_PARAMETER)->Disable();
    } else {
        FindWindow(ID_DELETE_PARAMETER)->Enable();
        FindWindow(ID_EDIT_PARAMETER)->Enable();
    }
}


/**
 * Event handler for the add parameter button.
 *
 * Opens a ImplementationParameterDialog for adding a new parameter.
 */
void
RFImplementationDialog::onAddParameter(wxCommandEvent&) {

    RFImplementation::Parameter parameter;

    ImplementationParameterDialog dialog(this, -1, parameter);

    if (dialog.ShowModal() != wxID_OK) {
        return;
    }

    try {
        implementation_.addParameter(
            parameter.name, parameter.type, parameter.value);
    } catch (IllegalParameters& e) {
        wxString message = _T("RF implementation already contains ");
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
 * Opens a ImplementationParamaeterDialog for modifying the selected
 * parameter.
 */
void
RFImplementationDialog::onEditParameter(wxCommandEvent&) {

    FUImplementation::Parameter parameter = selectedParameter();
    std::string oldName = parameter.name;

    ImplementationParameterDialog dialog(this, -1, parameter);

    if (dialog.ShowModal() == wxID_OK) {
        implementation_.removeParameter(oldName);
        implementation_.addParameter(
            parameter.name, parameter.type, parameter.value);

        // update external port parameter dependencies if needed
        for (int i = 0; i < implementation_.externalPortCount(); i++) {
            HDB::RFExternalPort& port = implementation_.externalPort(i);
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
void RFImplementationDialog::onDeleteParameter(wxCommandEvent&) {
    RFImplementation::Parameter parameter = selectedParameter();

    implementation_.removeParameter(parameter.name);

    // update external port parameter dependencies
    for (int i = 0; i < implementation_.externalPortCount(); i++) {
        HDB::RFExternalPort& port = implementation_.externalPort(i);
        port.unsetParameterDependency(parameter.name);
    }

    update();
}



/**
 * Event handler for the add source file button.
 *
 * Opens a source file dialog for adding a new implementation file 
 * to the list.
 */
void
RFImplementationDialog::onAddSourceFile(wxCommandEvent&) {

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
RFImplementationDialog::onDeleteSourceFile(wxCommandEvent&) {

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
 * Event handler for the move source file up button.
 *
 * Moves the selected source file up on the files list.
 */
void
RFImplementationDialog::onMoveSourceFileUp(wxCommandEvent&) {

    if (implementation_.implementationFileCount() > 1) {
        std::string fileName = WidgetTools::lcStringSelection(sourceList_, 0);
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
RFImplementationDialog::onMoveSourceFileDown(wxCommandEvent&) {
    if (implementation_.implementationFileCount() > 1) {
        std::string fileName = WidgetTools::lcStringSelection(sourceList_, 0);
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
 * Updates delete source file button enabeld/disabled states.
 */
void
RFImplementationDialog::onSourceFileSelection(wxListEvent&) {
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
 * Validates the dialog data and updates the RFImplementation object string
 * attributes.
 */
void
RFImplementationDialog::onOK(wxCommandEvent&) {
    
    TransferDataFromWindow();

    name_ = name_.Trim(true).Trim(false);
    clkPort_ = clkPort_.Trim(true).Trim(false);
    rstPort_ = rstPort_.Trim(true).Trim(false);
    gLockPort_ = gLockPort_.Trim(true).Trim(false);
    guardPort_ = guardPort_.Trim(true).Trim(false);
    widthParam_ = widthParam_.Trim(true).Trim(false);
    sizeParam_ = sizeParam_.Trim(true).Trim(false);

    if (name_.IsEmpty()) {
        wxString message = _T("Name field must not be empty.");
        InformationDialog dialog(this, message);
        dialog.ShowModal();
        return;
    }

    implementation_.setModuleName(WxConversion::toString(name_));
    implementation_.setClkPort(WxConversion::toString(clkPort_));
    implementation_.setRstPort(WxConversion::toString(rstPort_));
    implementation_.setGlockPort(WxConversion::toString(gLockPort_));
    implementation_.setGuardPort(WxConversion::toString(guardPort_));
    implementation_.setWidthParameter(WxConversion::toString(widthParam_));
    implementation_.setSizeParameter(WxConversion::toString(sizeParam_));

    EndModal(wxID_OK);

}


/**
 * Creates the dialog contents.
 */
wxSizer*
RFImplementationDialog::createContents(
    wxWindow *parent, bool call_fit, bool set_sizer) {

    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *item1 = new wxBoxSizer( wxVERTICAL );

    wxFlexGridSizer *item2 = new wxFlexGridSizer( 2, 0, 0 );

    wxStaticText *item3 = new wxStaticText( parent, ID_LABEL_NAME,
        wxT("Name:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item3, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxTextCtrl *item4 = new wxTextCtrl( parent, ID_NAME, wxT(""),
        wxDefaultPosition, wxSize(200,-1), 0 );
    item2->Add( item4, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item5 = new wxStaticText( parent, ID_LABEL_CLK_PORT,
        wxT("Clock port:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item5, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxTextCtrl *item6 = new wxTextCtrl( parent, ID_CLK_PORT, wxT(""),
        wxDefaultPosition, wxSize(80,-1), 0 );
    item2->Add( item6, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticText *item7 = new wxStaticText( parent, ID_LABEL_RESET_PORT,
        wxT("Reset port:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item7, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxTextCtrl *item8 = new wxTextCtrl( parent, ID_RESET_PORT, wxT(""),
        wxDefaultPosition, wxSize(80,-1), 0 );
    item2->Add( item8, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticText *item9 = new wxStaticText( parent, ID_LABEL_GLOCK_PORT,
        wxT("Global lock port:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item9, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxTextCtrl *item10 = new wxTextCtrl( parent, ID_GLOCK_PORT, wxT(""),
        wxDefaultPosition, wxSize(80,-1), 0 );
    item2->Add( item10, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticText *item11 = new wxStaticText( parent, ID_LABEL_GUARD_PORT,
        wxT("Guard port:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item11, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxTextCtrl *item12 = new wxTextCtrl( parent, ID_GUARD_PORT, wxT(""),
        wxDefaultPosition, wxSize(80,-1), 0 );
    item2->Add( item12, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticText *item13 = new wxStaticText( parent, ID_LABEL_SIZE_PARAMETER,
        wxT("Size parameter:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item13, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxTextCtrl *item14 = new wxTextCtrl( parent, ID_SIZE_PARAMETER, wxT(""),
        wxDefaultPosition, wxSize(80,-1), 0 );
    item2->Add( item14, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticText *item15 = new wxStaticText( parent, ID_LABEL_WIDTH_PARAMETER,
        wxT("Width parameter:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item15, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxTextCtrl *item16 = new wxTextCtrl( parent, ID_WIDTH_PARAMETER, wxT(""),
        wxDefaultPosition, wxSize(80,-1), 0 );
    item2->Add( item16, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item1->Add( item2, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    wxBoxSizer *item17 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticBox *item19 = new wxStaticBox( parent, -1, wxT("Ports:") );
    wxStaticBoxSizer *item18 = new wxStaticBoxSizer( item19, wxVERTICAL );

    wxListCtrl *item20 = new wxListCtrl( parent, ID_PORT_LIST,
        wxDefaultPosition, wxSize(220,160), wxLC_REPORT|wxSUNKEN_BORDER );
    item18->Add( item20, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item21 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item22 = new wxButton( parent, ID_ADD_PORT,
        wxT("Add..."), wxDefaultPosition, wxDefaultSize, 0 );
    item21->Add( item22, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item23 = new wxButton( parent, ID_MODIFY_PORT,
        wxT("Modify..."), wxDefaultPosition, wxDefaultSize, 0 );
    item21->Add( item23, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item24 = new wxButton( parent, ID_DELETE_PORT,
        wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    item21->Add( item24, 0, wxALIGN_CENTER|wxALL, 5 );

    item18->Add( item21, 0, wxALIGN_CENTER, 5 );

    item17->Add( item18, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticBox *item26 = new wxStaticBox( parent, -1, wxT("Source files:") );
    wxStaticBoxSizer *item25 = new wxStaticBoxSizer( item26, wxVERTICAL );

    wxFlexGridSizer *item27_1 = new wxFlexGridSizer( 2, 0, 0 );

    wxListCtrl *item27 = new wxListCtrl( parent, ID_SOURCE_LIST,
        wxDefaultPosition, wxSize(300, 150), wxLC_REPORT|wxSUNKEN_BORDER );
    item27_1->Add( item27, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item27_2 = new wxBoxSizer( wxVERTICAL );
    
    wxButton *item60 = new wxButton( parent, ID_MOVE_SOURCE_UP, wxT("▴"),
        wxDefaultPosition, wxSize(20, 20), 0 );
    item27_2->Add( item60, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item61 = new wxButton( parent, ID_MOVE_SOURCE_DOWN, wxT("▾"),
        wxDefaultPosition, wxSize(20, 20), 0 );
    item27_2->Add( item61, 0, wxALIGN_CENTER|wxALL, 5 );
    
    item27_1->Add( item27_2, 0, wxALIGN_RIGHT|wxALIGN_CENTER, 5 );

    item25->Add( item27_1, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item28 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item29 = new wxButton( parent, ID_ADD_SOURCE, wxT("Add..."),
        wxDefaultPosition, wxDefaultSize, 0 );
    item28->Add( item29, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxButton *item30 = new wxButton( parent, ID_DELETE_SOURCE, wxT("Delete"),
        wxDefaultPosition, wxDefaultSize, 0 );
    item28->Add( item30, 0, wxALIGN_CENTER|wxALL, 5 );

    item25->Add( item28, 0, wxALIGN_RIGHT|wxALIGN_BOTTOM, 5 );

    item17->Add( item25, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    item1->Add( item17, 0, wxALIGN_CENTER|wxALL, 5 );

    // External ports Box //
    wxBoxSizer *extPortAndParamSizer = new wxBoxSizer( wxHORIZONTAL );

    wxStaticBox *extPortBox = new wxStaticBox(parent, -1,
        wxT("External ports:"));
    wxStaticBoxSizer *extPortBoxSizer = new wxStaticBoxSizer(extPortBox,
        wxVERTICAL);
    wxListCtrl *item35 = new wxListCtrl( parent, ID_EXTERNAL_PORT_LIST,
        wxDefaultPosition, wxSize(200,120), wxLC_REPORT|wxSUNKEN_BORDER );
    extPortBoxSizer->Add( item35, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
    wxBoxSizer *extPortButtons = new wxBoxSizer( wxHORIZONTAL );
    wxButton *addExtPort = new wxButton( parent, ID_ADD_EXTERNAL_PORT,
        wxT("Add..."), wxDefaultPosition, wxDefaultSize, 0);
    extPortButtons->Add( addExtPort, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
    wxButton *editExtPort = new wxButton( parent, ID_EDIT_EXTERNAL_PORT,
        wxT("Edit..."), wxDefaultPosition, wxDefaultSize, 0);
    extPortButtons->Add(editExtPort, 0, wxALIGN_CENTER|wxALL, 5);
    wxButton *deleteExtPort = new wxButton( parent, ID_DELETE_EXTERNAL_PORT,
        wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0);
    extPortButtons->Add(deleteExtPort, 0, wxALIGN_CENTER|wxALL, 5);
    extPortBoxSizer->Add(extPortButtons, 0, wxALIGN_CENTER, 5);
    extPortAndParamSizer->Add(extPortBoxSizer, 0,
        wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // Parameter box //
    wxStaticBox *paramBox = new wxStaticBox(parent, -1, wxT("Parameters:"));
    wxStaticBoxSizer *paramBoxSizer = new wxStaticBoxSizer(paramBox,
        wxVERTICAL);

    wxListCtrl *item42 = new wxListCtrl( parent, ID_PARAMETER_LIST,
        wxDefaultPosition, wxSize(300,120), wxLC_REPORT|wxSUNKEN_BORDER );
    paramBoxSizer->Add( item42, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item43 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item44 = new wxButton( parent, ID_ADD_PARAMETER, wxT("Add..."),
        wxDefaultPosition, wxDefaultSize, 0 );
    item43->Add( item44, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item45 = new wxButton( parent, ID_EDIT_PARAMETER, wxT("Edit..."),
        wxDefaultPosition, wxDefaultSize, 0 );
    item43->Add( item45, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item46 = new wxButton( parent, ID_DELETE_PARAMETER,
        wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    item43->Add( item46, 0, wxALIGN_CENTER|wxALL, 5 );

    paramBoxSizer->Add( item43, 0,
        wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

    extPortAndParamSizer->Add( paramBoxSizer, 0,
        wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    item1 ->Add(extPortAndParamSizer, 0,
        wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    item0->Add( item1, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticLine *item31 = new wxStaticLine( parent, ID_LINE,
        wxDefaultPosition, wxSize(20,-1), wxLI_HORIZONTAL );
    item0->Add( item31, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item32 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item33 = new wxButton( parent, wxID_OK, wxT("&OK"),
        wxDefaultPosition, wxDefaultSize, 0 );
    item32->Add( item33, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item34 = new wxButton( parent, wxID_CANCEL, wxT("&Cancel"),
        wxDefaultPosition, wxDefaultSize, 0 );
    item32->Add( item34, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item32, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;
}
