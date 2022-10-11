/*
    Copyright (c) 2002-2017 Tampere University.

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
*@file OperationImplementationDialog.cc
*
*Implementation of OperationImplementationDialog.
*
*@author Lasse Lehtonen 2017 (lasse.lehtonen-no.spam-tut.fi)
*/

#include "OperationImplementationDialog.hh"
#include "BlockImplementationFile.hh"
#include "HDBBrowserWindow.hh"
#include "HDBEditor.hh"
#include "HDBManager.hh"
#include "OperationImplementation.hh"
#include "VariableDialog.hh"

wxDEFINE_EVENT(BUTTON_MODEL_VHDL, wxFileDirPickerEvent);
wxDEFINE_EVENT(BUTTON_MODEL_VERILOG, wxFileDirPickerEvent);
wxDEFINE_EVENT(BUTTON_POSTMODEL_VHDL, wxFileDirPickerEvent);
wxDEFINE_EVENT(BUTTON_POSTMODEL_VERILOG, wxFileDirPickerEvent);
wxDEFINE_EVENT(BUTTON_INITIAL_VHDL, wxFileDirPickerEvent);
wxDEFINE_EVENT(BUTTON_INITIAL_VERILOG, wxFileDirPickerEvent);
wxDEFINE_EVENT(BUTTON_IPXACT_ABSDEF, wxFileDirPickerEvent);
wxDEFINE_EVENT(BUTTON_ADD_VHDL_VAR, wxCommandEvent);
wxDEFINE_EVENT(BUTTON_ADD_VERILOG_VAR, wxCommandEvent);

BEGIN_EVENT_TABLE(OperationImplementationDialog, wxDialog)
EVT_BUTTON(wxID_OK, OperationImplementationDialog::onOK)
EVT_FILEPICKER_CHANGED(
    BUTTON_MODEL_VHDL, OperationImplementationDialog::onAddVhdlImplFile)
EVT_FILEPICKER_CHANGED(
    BUTTON_MODEL_VERILOG, OperationImplementationDialog::onAddVerilogImplFile)
EVT_BUTTON(wxID_ADD, OperationImplementationDialog::onAddListItem)
EVT_BUTTON(wxID_DELETE, OperationImplementationDialog::onDeleteListItem)
EVT_BUTTON(
    BUTTON_ADD_VHDL_VAR, OperationImplementationDialog::onAddVHDLVariable)
EVT_BUTTON(BUTTON_ADD_VERILOG_VAR,
    OperationImplementationDialog::onAddVerilogVariable)
END_EVENT_TABLE()

/// Constructor
OperationImplementationDialog::OperationImplementationDialog()
    : wxDialog(NULL, -1, "Add Operation Implementation", wxDefaultPosition,
          wxSize(700, 900), wxRESIZE_BORDER) {
    wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* hbox1 = new wxBoxSizer(wxHORIZONTAL);

    wxButton* okButton = new wxButton(
        this, wxID_OK, wxT("Ok"), wxDefaultPosition, wxSize(70, 30));
    wxButton* closeButton = new wxButton(
        this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxSize(70, 30));

    wxButton* addVHDLVariableButton =
        new wxButton(this, BUTTON_ADD_VHDL_VAR, wxT("Add"));
    wxButton* addVerilogVariableButton =
        new wxButton(this, BUTTON_ADD_VERILOG_VAR, wxT("Add"));

    wxButton* addButton = new wxButton(this, wxID_ADD, wxT(""));
    wxButton* delButton = new wxButton(this, wxID_DELETE, wxT(""));

    hbox1->Add(okButton, 1);
    hbox1->Add(closeButton, 1, wxLEFT, 5);

    wxFlexGridSizer* flex1 = new wxFlexGridSizer(13, 2, 9, 25);

    wxStaticText* title1 = new wxStaticText(this, -1, wxT("Name"));
    wxStaticText* title2 =
        new wxStaticText(this, -1, wxT("VHDL Op Implementation file"));
    wxStaticText* title3 =
        new wxStaticText(this, -1, wxT("Verilog Op Implementation file"));
    wxStaticText* title21 =
        new wxStaticText(this, -1, wxT("VHDL Post-Op Implementation file"));
    wxStaticText* title31 =
        new wxStaticText(this, -1, wxT("Verilog Post-Op Implementation file"));
    wxStaticText* title_vhdl_init =
        new wxStaticText(this, -1, wxT("VHDL Initial Implementation file"));
    wxStaticText* title_vlog_init =
        new wxStaticText(this, -1, wxT("Verilog Initial Implementation file"));
    wxStaticText* title41 =
        new wxStaticText(this, -1, wxT("Optional IPXACT bus definition file"));
    wxStaticText* title4 =
        new wxStaticText(this, -1, wxT("Selected resources"));
    wxStaticText* title5 =
        new wxStaticText(this, -1, wxT("Selected resources"));
    wxStaticText* title6 =
        new wxStaticText(this, -1, wxT("Add VHDL Variable"));
    wxStaticText* title7 =
        new wxStaticText(this, -1, wxT("Add Verilog Variable"));
    wxStaticText* title_latency = new wxStaticText(this, -1, wxT("Latency"));

    nameCtrl_ = new wxTextCtrl(this, -1);

    pick1_ = new wxFilePickerCtrl(this, BUTTON_MODEL_VHDL, "",
        "Select VHDL Op Implementation File", "*.vhd;*.vhdl", wxDefaultPosition,
        wxDefaultSize, wxFLP_OPEN | wxFLP_FILE_MUST_EXIST | wxFLP_SMALL);
    pick2_ = new wxFilePickerCtrl(this, BUTTON_MODEL_VERILOG, "",
        "Select Verilog Op Implementation File", "*.vh;*.v", wxDefaultPosition,
        wxDefaultSize, wxFLP_OPEN | wxFLP_FILE_MUST_EXIST | wxFLP_SMALL);
    pick11_ = new wxFilePickerCtrl(this, BUTTON_POSTMODEL_VHDL, "",
        "Select VHDL Post-Op Implementation File", "*.vhd;*.vhdl", wxDefaultPosition,
        wxDefaultSize, wxFLP_OPEN | wxFLP_FILE_MUST_EXIST | wxFLP_SMALL);
    pick21_ = new wxFilePickerCtrl(this, BUTTON_POSTMODEL_VERILOG, "",
        "Select Verilog Post-Op Implementation File", "*.vh;*.v", wxDefaultPosition,
        wxDefaultSize, wxFLP_OPEN | wxFLP_FILE_MUST_EXIST | wxFLP_SMALL);
    pick_vhdl_init_ = new wxFilePickerCtrl(this, BUTTON_INITIAL_VHDL, "",
        "Select VHDL Initial Implementation File", "*.vhd;*.vhdl", wxDefaultPosition,
        wxDefaultSize, wxFLP_OPEN | wxFLP_FILE_MUST_EXIST | wxFLP_SMALL);
    pick_vlog_init_ = new wxFilePickerCtrl(this, BUTTON_INITIAL_VERILOG, "",
        "Select Verilog Initial Implementation File", "*.vh;*.v", wxDefaultPosition,
        wxDefaultSize, wxFLP_OPEN | wxFLP_FILE_MUST_EXIST | wxFLP_SMALL);
    pick31_ = new wxFilePickerCtrl(this, BUTTON_IPXACT_ABSDEF, "",
        "Select IPXACT Bus Abstraction Definition File", "*.xml", wxDefaultPosition,
        wxDefaultSize, wxFLP_OPEN | wxFLP_FILE_MUST_EXIST | wxFLP_SMALL);
    latencyWheel_ = new wxSpinCtrl(this);

    flex1->Add(title1);
    flex1->Add(nameCtrl_, 1, wxEXPAND);
    flex1->Add(title2);
    flex1->Add(pick1_, 1, wxEXPAND);
    flex1->Add(title3);
    flex1->Add(pick2_, 1, wxEXPAND);
    flex1->Add(title21);
    flex1->Add(pick11_, 1, wxEXPAND);
    flex1->Add(title31);
    flex1->Add(pick21_, 1, wxEXPAND);
    flex1->Add(title_vhdl_init);
    flex1->Add(pick_vhdl_init_, 1, wxEXPAND);
    flex1->Add(title_vlog_init);
    flex1->Add(pick_vlog_init_, 1, wxEXPAND);
    flex1->Add(title41);
    flex1->Add(pick31_, 1, wxEXPAND);
    flex1->Add(title6);
    flex1->Add(addVHDLVariableButton, 1, wxEXPAND);
    flex1->Add(title7);
    flex1->Add(addVerilogVariableButton, 1, wxEXPAND);
    flex1->Add(title4);
    flex1->Add(addButton, 1, wxEXPAND);
    flex1->Add(title5);
    flex1->Add(delButton, 1, wxEXPAND);
    flex1->Add(title_latency);
    flex1->Add(latencyWheel_, 1, wxEXPAND);

    addedResourceList_ = new wxListCtrl(
        this, -1, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
    addedResourceList_->AppendColumn(
        "Added Resource", wxLIST_FORMAT_LEFT, 400);
    addedResourceList_->AppendColumn("id", wxLIST_FORMAT_LEFT, 30);
    addedResourceList_->AppendColumn("Count", wxLIST_FORMAT_LEFT, 120);

    addedVariableList_ = new wxListCtrl(
        this, -1, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
    addedVariableList_->AppendColumn(
        "Added Variable", wxLIST_FORMAT_LEFT, 200);
    addedVariableList_->AppendColumn("width", wxLIST_FORMAT_LEFT, 120);
    addedVariableList_->AppendColumn("type", wxLIST_FORMAT_LEFT, 120);
    addedVariableList_->AppendColumn("language", wxLIST_FORMAT_LEFT, 120);

    availableResourceList_ = new wxListCtrl(
        this, -1, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
    availableResourceList_->AppendColumn(
        "Available Resource", wxLIST_FORMAT_LEFT, 170);
    availableResourceList_->AppendColumn("id", wxLIST_FORMAT_LEFT, 30);

    wxBoxSizer* hbox2 = new wxBoxSizer(wxHORIZONTAL);
    hbox2->Add(flex1, 1, wxALL | wxEXPAND, 15);
    hbox2->Add(availableResourceList_, 1, wxALL | wxEXPAND, 15);
    addAvailableResourcesToList();

    vbox->Add(hbox2, 1, wxALL | wxEXPAND, 15);
    vbox->Add(addedResourceList_, 1, wxALL | wxEXPAND, 15);
    vbox->Add(addedVariableList_, 1, wxALL | wxEXPAND, 15);
    vbox->Add(hbox1, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);

    SetSizer(vbox);
    Centre();
    ShowModal();
    Destroy();
}

void
OperationImplementationDialog::addAvailableResourcesToList() {
    HDB::HDBManager* manager = wxGetApp().mainFrame().hdbManager();
    std::set<RowID> resource_ids =
        manager->OperationImplementationResourceIDs();
    for (const auto id : resource_ids) {
        HDB::OperationImplementationResource resource =
            manager->OperationImplementationResourceByID(id);
        int idx = availableResourceList_->GetItemCount();
        availableResourceList_->InsertItem(idx, resource.name);
        availableResourceList_->SetItem(idx, 1, std::to_string(resource.id));
    }
}

std::string
OperationImplementationDialog::sanitize(const wxString& path) {
    HDB::HDBManager* manager = wxGetApp().mainFrame().hdbManager();
    std::string es(path);
    std::string s(manager->fileName());
    size_t i = s.rfind('/', s.length());
    if (i != std::string::npos) {
        s = s.substr(0, i + 1);
        if (es.find(s) == 0) {
            return es.substr(s.length());
        }
    }
    return es;
}

void
OperationImplementationDialog::onOK(wxCommandEvent&) {
    HDB::OperationImplementation operation;
    HDB::HDBManager* manager = wxGetApp().mainFrame().hdbManager();

    operation.name = nameCtrl_->GetLineText(0);
    if (operation.name.length() < 1) {
        wxMessageBox(
            wxT("Give the operation some name."), wxT("Error"), wxICON_ERROR);
        return;
    }

    operation.implFileVhdl = sanitize(pick1_->GetPath());
    if (operation.implFileVhdl.length() < 1) {
        wxMessageBox(wxT("Give the operation VHDL implementation file."),
            wxT("Error"), wxICON_ERROR);
        return;
    }

    operation.implFileVerilog = sanitize(pick2_->GetPath());

    operation.postOpImplFileVhdl = sanitize(pick11_->GetPath());
    operation.postOpImplFileVerilog = sanitize(pick21_->GetPath());
    operation.initialImplFileVhdl = sanitize(pick_vhdl_init_->GetPath());
    operation.initialImplFileVerilog = sanitize(pick_vlog_init_->GetPath());
    operation.absBusDefFile = sanitize(pick31_->GetPath());
    operation.latency = latencyWheel_->GetValue();

    long items = addedResourceList_->GetItemCount();
    for (long i = 0; i < items; ++i) {
        HDB::OperationImplementationResource resource;
        std::istringstream iss(
            addedResourceList_->GetItemText(i, 1).ToStdString());
        iss >> resource.id;
        std::istringstream iss2(
            addedResourceList_->GetItemText(i, 2).ToStdString());
        iss2 >> resource.count;
        operation.resources.emplace_back(resource);
    }

    items = addedVariableList_->GetItemCount();
    for (long i = 0; i < items; ++i) {
        std::string name =
            addedVariableList_->GetItemText(i, 0).ToStdString();
        std::string width =
            addedVariableList_->GetItemText(i, 1).ToStdString();
        std::string type =
            addedVariableList_->GetItemText(i, 2).ToStdString();
        std::string lang =
            addedVariableList_->GetItemText(i, 3).ToStdString();
        if (lang == "VHDL") {
            operation.vhdlVariables.emplace_back(
                HDB::Variable{name, width, type, true});
        } else if (lang == "Verilog") {
            operation.verilogVariables.emplace_back(
                HDB::Variable{name, width, type, true});
        } else {
            throw std::runtime_error("Unknown language");
        }
    }

    manager->addOperationImplementation(operation);
    wxGetApp().mainFrame().browser()->update();
    EndModal(wxID_OK);
}

void
OperationImplementationDialog::onAddVhdlImplFile(wxFileDirPickerEvent&) {}

void
OperationImplementationDialog::onAddVerilogImplFile(wxFileDirPickerEvent&) {}

void
OperationImplementationDialog::onAddVHDLVariable(wxCommandEvent&) {
    VariableDialog dialog(addedVariableList_, "VHDL");
}

void
OperationImplementationDialog::onAddVerilogVariable(wxCommandEvent&) {
    VariableDialog dialog(addedVariableList_, "Verilog");
}

void
OperationImplementationDialog::onDeleteListItem(wxCommandEvent&) {
    long item = -1;

    for (;;) {
        item = addedResourceList_->GetNextItem(
            item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        if (item == -1)
            break;
        addedResourceList_->DeleteItem(item);
        if (addedResourceList_->GetItemCount() == 0 ||
            item == addedResourceList_->GetItemCount())
            break;
    }
}

void
OperationImplementationDialog::onAddListItem(wxCommandEvent&) {
    long item = -1;

    for (;;) {
        item = availableResourceList_->GetNextItem(
            item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        if (item == -1)
            break;
        std::string name =
            availableResourceList_->GetItemText(item, 0).ToStdString();
        std::string id =
            availableResourceList_->GetItemText(item, 1).ToStdString();

        bool found = false;
        int idx = addedResourceList_->GetItemCount();
        for (int i = 0; i < idx; ++i) {
            if (addedResourceList_->GetItemText(i, 0).ToStdString() == name) {
                std::istringstream iss(
                    addedResourceList_->GetItemText(i, 2).ToStdString());
                int int_id;
                iss >> int_id;
                addedResourceList_->SetItem(i, 2, std::to_string(++int_id));
                found = true;
                break;
            }
        }
        if (!found) {
            addedResourceList_->InsertItem(idx, name);
            addedResourceList_->SetItem(idx, 1, id);
            addedResourceList_->SetItem(idx, 2, "1");
        }
    }
}
