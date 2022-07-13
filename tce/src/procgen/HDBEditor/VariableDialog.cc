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
* Implementation of VariableDialog.
*
*@author Lasse Lehtonen 2017 (lasse.lehtonen-no.spam-tut.fi)
*/

#include "VariableDialog.hh"

BEGIN_EVENT_TABLE(VariableDialog, wxDialog)
EVT_BUTTON(wxID_OK, VariableDialog::onOK)
END_EVENT_TABLE()

VariableDialog::VariableDialog(wxListCtrl* list, std::string language)
    : wxDialog(NULL, -1, "Add Variable for Operation Implementation",
          wxDefaultPosition, wxSize(360, 140), wxRESIZE_BORDER),
      varList_(list), language_(language) {

    wxButton* okButton = new wxButton(
        this, wxID_OK, wxT("Ok"), wxDefaultPosition, wxSize(70, 30));

    wxButton* closeButton = new wxButton(
        this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxSize(70, 30));

    wxBoxSizer* hbox1 = new wxBoxSizer(wxHORIZONTAL);
    hbox1->Add(okButton, 1);
    hbox1->Add(closeButton, 1, wxLEFT, 5);

    nameCtrl_ = new wxTextCtrl(this, -1);
    widthCtrl_ = new wxTextCtrl(this, -1);
    if (language_ == "VHDL") {
        wxString strs[] = {wxT("Logic"), wxT("Unsigned"), wxT("Signed")};
        typeCtrl_ = new wxComboBox(this, -1, wxT(""), wxDefaultPosition,
            wxDefaultSize, 3, strs, wxCB_READONLY);
    } else {
        wxString strs[] = {wxT("Unsigned"), wxT("Signed")};
        typeCtrl_ = new wxComboBox(this, -1, wxT(""), wxDefaultPosition,
            wxDefaultSize, 2, strs, wxCB_READONLY);
    }
    typeCtrl_->SetSelection(0);

    wxStaticText* title1 = new wxStaticText(this, -1, wxT("Name"));
    wxStaticText* title2 = new wxStaticText(this, -1, wxT("Width"));
    wxStaticText* title3 = new wxStaticText(this, -1, wxT("Type"));

    wxFlexGridSizer* flex1 = new wxFlexGridSizer(2, 3, 9, 25);
    flex1->Add(title1);
    flex1->Add(title2);
    flex1->Add(title3);
    flex1->Add(nameCtrl_);
    flex1->Add(widthCtrl_);
    flex1->Add(typeCtrl_);

    wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);
    vbox->Add(flex1, 0, wxALIGN_LEFT | wxLEFT | wxTOP | wxBOTTOM, 10);
    vbox->Add(hbox1, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);

    SetSizer(vbox);
    Centre();
    ShowModal();
    Destroy();
}

void
VariableDialog::onOK(wxCommandEvent&) {
    int idx = varList_->GetItemCount();
    varList_->InsertItem(idx, nameCtrl_->GetLineText(0));
    varList_->SetItem(idx, 1, widthCtrl_->GetLineText(0));
    varList_->SetItem(
        idx, 2, typeCtrl_->GetString(typeCtrl_->GetSelection()));
    varList_->SetItem(idx, 3, language_);
    EndModal(wxID_OK);
}