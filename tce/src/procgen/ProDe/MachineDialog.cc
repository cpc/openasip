/*
 Copyright (c) 2002-2015 Tampere University.

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
/*
 * @file MachineDialog.cc
 *
 * Implementation of MachineDialog class.
 *
 * Created on: 6.2.2015
 * @author: Henry Linjamäki (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#include <wx/statline.h>

#include "MachineDialog.hh"

#include "Machine.hh"

BEGIN_EVENT_TABLE(MachineDialog, wxDialog)
    EVT_BUTTON(wxID_OK, MachineDialog::onOK)
    EVT_BUTTON(wxID_CANCEL, MachineDialog::onCancel)
END_EVENT_TABLE()

/// Indexes for endianess choices
enum EndianessChoice {BIGENDIAN=0, LITTLEENDIAN };

/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param machine The machine to modify.
 */
MachineDialog::MachineDialog(wxWindow* parent, TTAMachine::Machine& machine) :
        wxDialog(parent, -1, _T(""), wxDefaultPosition),
        machine_(machine) {

    createContents(this, true, true);
    SetTitle(wxT("Architecture Features"));
    endianessChoise_->Append(wxT("Big-endian"));
    endianessChoise_->Append(wxT("Little-endian"));
    TransferDataToWindow();
}


/**
 * The Destructor.
 */
MachineDialog::~MachineDialog() {

}


/**
 * Transfers data from the port object to the dialog widgets.
 *
 * @return False, if an error occured in the transfer.
 */
bool
MachineDialog::TransferDataToWindow() {
    assert(endianessChoise_->GetCount() > 1); // Assume
    if (machine_.isLittleEndian()) {
        endianessChoise_->SetSelection(LITTLEENDIAN);
    } else {
        endianessChoise_->SetSelection(BIGENDIAN);
    }

    return wxWindow::TransferDataToWindow();
}


/**
 * Event handler for Ok button press.
 */
void
MachineDialog::onOK(wxCommandEvent& /*event*/) {
    machine_.setLittleEndian(endianessChoise_->GetSelection() == LITTLEENDIAN);
    EndModal(wxID_OK);
}


/**
 * Event handler for Cancel button press.
 */
void
MachineDialog::onCancel(wxCommandEvent& /*event*/) {
    EndModal(wxID_CANCEL);
}


/**
 * Creates contents of the dialog window.
 *
 * @param parent Parent dialog of the contents.
 * @param call_fit If true, fits sizer in dialog window.
 * @param set_sizer If true, sets sizer as dialog's sizer.
 * @return Top level sizer of the contents.
 */
wxSizer*
MachineDialog::createContents(
    wxWindow *parent, bool call_fit, bool set_sizer) {

    wxBoxSizer *root = new wxBoxSizer(wxVERTICAL);
    wxFlexGridSizer *machSettings = new wxFlexGridSizer(2, 0, 0);

    // Global Endianess Setting //
    // Label
    wxStaticText *endianessLabel = new wxStaticText(parent, -1,
        wxT("Endianess:"), wxDefaultPosition, wxDefaultSize, 0 );
    machSettings->Add(endianessLabel,
        0, wxALIGN_RIGHT|wxALL, 5);
    // Choice
    endianessChoise_ = new wxChoice(parent, ID_ENDIANESS_CHOICE,
        wxDefaultPosition, wxDefaultSize);
    machSettings->Add(endianessChoise_,
        0, wxALL, 5);

    root->Add(machSettings, 0, wxALIGN_CENTER|wxALL, 5);

    // Buttons //
    wxStaticLine *horizLine = new wxStaticLine(parent, wxID_ANY,
        wxDefaultPosition, wxSize(20,-1), wxLI_HORIZONTAL);
    root->Add(horizLine, 0, wxGROW|wxALL, 5);
    wxBoxSizer *buttonBox = new wxBoxSizer(wxHORIZONTAL);
    // Ok button
    wxButton *okButton =
        new wxButton(parent, wxID_OK, wxT("&OK"), wxDefaultPosition,
            wxDefaultSize, 0);
    buttonBox->Add(okButton, 0, wxALIGN_CENTER|wxALL, 5);
    // Cancel Button
    wxButton *cancelButton =
        new wxButton(parent, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition,
            wxDefaultSize, 0);
    buttonBox->Add(cancelButton, 0, wxALIGN_CENTER|wxALL, 5);
    root->Add(buttonBox, 0, wxALIGN_CENTER|wxALL, 5);

    if (set_sizer) {
        parent->SetAutoLayout(TRUE);
        parent->SetSizer(root);
        if (call_fit)
        {
            root->Fit(parent);
            root->SetSizeHints(parent);
        }
    }
    return root;
}


