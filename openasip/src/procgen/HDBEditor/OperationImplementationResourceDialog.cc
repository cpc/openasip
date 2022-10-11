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
 * @file FUGENResourceDialog.cc
 *
 * Implementation of FUGENResourceDialog.
 *
 * @author Lasse Lehtonen 2017 (lasse.lehtonen-no.spam-tut.fi)
 */

#include "OperationImplementationResourceDialog.hh"
#include "HDBEditor.hh"
#include "HDBManager.hh"
#include "OperationImplementationResource.hh"
#include "HDBBrowserWindow.hh"
#include "BlockImplementationFile.hh"


wxDEFINE_EVENT(BUTTON_SYN_VHDL, wxFileDirPickerEvent);
wxDEFINE_EVENT(BUTTON_SIM_VHDL, wxFileDirPickerEvent);
wxDEFINE_EVENT(BUTTON_SYN_VERILOG, wxFileDirPickerEvent);
wxDEFINE_EVENT(BUTTON_SIM_VERILOG, wxFileDirPickerEvent);
wxDEFINE_EVENT(BUTTON_IPXACT, wxFileDirPickerEvent);
wxDEFINE_EVENT(BUTTON_DEL, wxCommandEvent);

BEGIN_EVENT_TABLE(OperationImplementationResourceDialog, wxDialog)
    EVT_BUTTON(wxID_OK, OperationImplementationResourceDialog::onOK)
    EVT_FILEPICKER_CHANGED(BUTTON_SYN_VHDL,
        OperationImplementationResourceDialog::onAddVhdlSynFile)
    EVT_FILEPICKER_CHANGED(BUTTON_SIM_VHDL,
        OperationImplementationResourceDialog::onAddVhdlSimFile)
    EVT_FILEPICKER_CHANGED(BUTTON_SYN_VERILOG,
        OperationImplementationResourceDialog::onAddVerilogSynFile)
    EVT_FILEPICKER_CHANGED(BUTTON_SIM_VERILOG,
        OperationImplementationResourceDialog::onAddVerilogSimFile)
    EVT_BUTTON(wxID_DELETE,
        OperationImplementationResourceDialog::onDeleteListItem)
END_EVENT_TABLE()

/// Constructor
OperationImplementationResourceDialog::
    OperationImplementationResourceDialog()
    : wxDialog(NULL, -1, "Add Operation Implementation Resource",
        wxDefaultPosition, wxSize(600, 480))
{
    wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *hbox1 = new wxBoxSizer(wxHORIZONTAL);

    wxButton *okButton = new wxButton(this, wxID_OK, wxT("Ok"),
    wxDefaultPosition, wxSize(70, 30));
    wxButton *closeButton = new wxButton(this, wxID_CANCEL, wxT("Cancel"),
    wxDefaultPosition, wxSize(70, 30));

    wxButton *delButton = new wxButton(this , wxID_DELETE, wxT(""));

    hbox1->Add(okButton, 1);
    hbox1->Add(closeButton, 1, wxLEFT, 5);

    wxFlexGridSizer *flex1 = new wxFlexGridSizer(7, 2, 9, 25);

    wxStaticText *title1 = new wxStaticText(
        this, -1, wxT("Name"));
    wxStaticText *title2 = new wxStaticText(
        this, -1, wxT("VHDL Synthesis file"));
    wxStaticText *title3 = new wxStaticText(
        this, -1, wxT("VHDL Simulation file"));
    wxStaticText *title4 = new wxStaticText(
        this, -1, wxT("Verilog Synthesis file"));
    wxStaticText *title5 = new wxStaticText(
        this, -1, wxT("Verilog Simulation file"));
    wxStaticText *title7 = new wxStaticText(
        this, -1, wxT("IP-XACT component file"));
    wxStaticText *title6 = new wxStaticText(
        this, -1, wxT("Selected files"));

    nameCtrl_ = new wxTextCtrl(this, -1);

    wxFilePickerCtrl *pick1 = new wxFilePickerCtrl(this, BUTTON_SYN_VHDL,
        "", "Select VHDL Synthesis File", "*.vhd;*.vhdl",
        wxDefaultPosition, wxDefaultSize,
        wxFLP_OPEN | wxFLP_FILE_MUST_EXIST | wxFLP_SMALL);
    wxFilePickerCtrl *pick2 = new wxFilePickerCtrl(this, BUTTON_SIM_VHDL,
        "", "Select VHDL Simulation File", "*.vhd;*.vhdl",
        wxDefaultPosition, wxDefaultSize,
        wxFLP_OPEN | wxFLP_FILE_MUST_EXIST | wxFLP_SMALL);
    wxFilePickerCtrl *pick3 = new wxFilePickerCtrl(this, BUTTON_SYN_VERILOG,
        "", "Select Verilog Synthesis File", "*.vh;*.v",
        wxDefaultPosition, wxDefaultSize,
         wxFLP_OPEN | wxFLP_FILE_MUST_EXIST | wxFLP_SMALL);
    wxFilePickerCtrl *pick4 = new wxFilePickerCtrl(this, BUTTON_SIM_VERILOG,
        "", "Select Verilog Simulation File", "*.vh;*.v",
        wxDefaultPosition, wxDefaultSize,
        wxFLP_OPEN | wxFLP_FILE_MUST_EXIST | wxFLP_SMALL);
    ipxactPick_ = new wxFilePickerCtrl(this, BUTTON_IPXACT,
        "", "Select IP-XACT component File", "*.xml",
        wxDefaultPosition, wxDefaultSize,
        wxFLP_OPEN | wxFLP_FILE_MUST_EXIST | wxFLP_SMALL);

    flex1->Add(title1);
    flex1->Add(nameCtrl_, 1, wxEXPAND);
    flex1->Add(title2);
    flex1->Add(pick1, 1, wxEXPAND);
    flex1->Add(title3);
    flex1->Add(pick2, 1, wxEXPAND);
    flex1->Add(title4);
    flex1->Add(pick3, 1, wxEXPAND);
    flex1->Add(title5);
    flex1->Add(pick4, 1, wxEXPAND);
    flex1->Add(title7);
    flex1->Add(ipxactPick_, 1, wxEXPAND);
    flex1->Add(title6);
    flex1->Add(delButton, 1, wxEXPAND);


    fileList1_ = new wxListCtrl(this, -1,
        wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
    fileList1_->AppendColumn("File", wxLIST_FORMAT_LEFT, 400);
    fileList1_->AppendColumn("Type", wxLIST_FORMAT_LEFT, 150);

    vbox->Add(flex1, 1, wxALL | wxEXPAND, 15);
    vbox->Add(fileList1_, 1, wxALL | wxEXPAND, 15);
    vbox->Add(hbox1, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);

    SetSizer(vbox);
    Centre();
    ShowModal();
    Destroy();
}

void
OperationImplementationResourceDialog::onOK(wxCommandEvent&) {
    HDB::OperationImplementationResource resource;
    HDB::HDBManager* manager = wxGetApp().mainFrame().hdbManager();

    resource.name = nameCtrl_->GetLineText(0);
    if (resource.name.length() < 1) {
        wxMessageBox( wxT("Give the resource a name."),
            wxT("Error"), wxICON_ERROR);
            return;
    }

    long items = fileList1_->GetItemCount();
    for (int i = 0; i < items; ++i) {
        std::string file = fileList1_->GetItemText(i, 0).ToStdString();
        std::string type = fileList1_->GetItemText(i, 1).ToStdString();
        if (type == manager->formatString(
                HDB::BlockImplementationFile::VHDL)) {
            resource.synFiles.emplace_back(file);
            resource.synFormats.emplace_back(type);
        } else if (type == manager->formatString(
                HDB::BlockImplementationFile::VHDLsim)) {
            resource.simFiles.emplace_back(file);
            resource.simFormats.emplace_back(type);
        }
        else if (type == manager->formatString(
                HDB::BlockImplementationFile::Verilog)) {
            resource.synFiles.emplace_back(file);
            resource.synFormats.emplace_back(type);
        }
        else if (type == manager->formatString(
                HDB::BlockImplementationFile::Verilogsim)) {
            resource.simFiles.emplace_back(file);
            resource.simFormats.emplace_back(type);
        } else {
            wxMessageBox( wxT("Unknown file type."),
            wxT("Error"), wxICON_ERROR);
            return;
        }
    }

    resource.ipxact = sanitize(ipxactPick_->GetPath());
    if (resource.ipxact.length() < 1) {
        wxMessageBox(wxT("Give the IP-XACT component file."),
            wxT("Error"), wxICON_ERROR);
        return;
    }

    manager->addOperationImplementationResource(resource);
    wxGetApp().mainFrame().browser()->update();
    EndModal(wxID_OK);
}

std::string
OperationImplementationResourceDialog::sanitize(const wxString& path) {
    HDB::HDBManager* manager = wxGetApp().mainFrame().hdbManager();
    std::string es(path);
    std::string s(manager->fileName());
    size_t i = s.rfind('/', s.length());
    if (i != std::string::npos) {
        s = s.substr(0, i+1);
        if (es.find(s) == 0) {
            return es.substr(s.length());
        }
    }
    return es;
}


void
OperationImplementationResourceDialog::onAddVhdlSynFile(
    wxFileDirPickerEvent& e) {
    HDB::HDBManager* manager = wxGetApp().mainFrame().hdbManager();
    int idx = fileList1_->GetItemCount();
    fileList1_->InsertItem(idx, sanitize(e.GetPath()));
    fileList1_->SetItem(idx, 1, manager->formatString(
                HDB::BlockImplementationFile::VHDL));
}

void
OperationImplementationResourceDialog::onAddVhdlSimFile(
    wxFileDirPickerEvent& e) {
    HDB::HDBManager* manager = wxGetApp().mainFrame().hdbManager();
    int idx = fileList1_->GetItemCount();
    fileList1_->InsertItem(idx, sanitize(e.GetPath()));
    fileList1_->SetItem(idx, 1, manager->formatString(
                HDB::BlockImplementationFile::VHDLsim));
}

void
OperationImplementationResourceDialog::onAddVerilogSynFile(
    wxFileDirPickerEvent& e) {
    HDB::HDBManager* manager = wxGetApp().mainFrame().hdbManager();
    int idx = fileList1_->GetItemCount();
    fileList1_->InsertItem(idx, sanitize(e.GetPath()));
    fileList1_->SetItem(idx, 1, manager->formatString(
                HDB::BlockImplementationFile::Verilog));
}

void
OperationImplementationResourceDialog::onAddVerilogSimFile(
    wxFileDirPickerEvent& e) {
    HDB::HDBManager* manager = wxGetApp().mainFrame().hdbManager();
    int idx = fileList1_->GetItemCount();
    fileList1_->InsertItem(idx, sanitize(e.GetPath()));
    fileList1_->SetItem(idx, 1, manager->formatString(
                HDB::BlockImplementationFile::Verilogsim));
}

void
OperationImplementationResourceDialog::onDeleteListItem(wxCommandEvent&) {
    long item = -1;

    for (;;)
    {
        item = fileList1_->GetNextItem(
            item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        if (item == -1)
            break;
        fileList1_->DeleteItem(item);
        if (fileList1_->GetItemCount() == 0 ||
            item == fileList1_->GetItemCount())
            break;
    }
}
