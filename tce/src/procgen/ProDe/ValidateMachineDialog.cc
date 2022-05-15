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
 * @file ValidateMachineDialog.cc
 *
 * Definition of ValidateMachineDialog class.
 *
 * @author Veli-Pekka J��skel�inen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <string>
#include <wx/html/htmlwin.h>
#include <wx/statline.h>

#include "ProgrammabilityValidator.hh"
#include "ProgrammabilityValidatorResults.hh"
#include "ValidateMachineDialog.hh"
#include "WxConversion.hh"

using std::string;
using namespace TTAMachine;

BEGIN_EVENT_TABLE(ValidateMachineDialog, wxDialog)
EVT_BUTTON(ID_VALIDATE, ValidateMachineDialog::onValidate)
EVT_CHECKBOX(ID_CHECK_ANSI_C, ValidateMachineDialog::onCheck)
EVT_CHECKBOX(ID_CHECK_GLOBAL_CONN_REGISTER, ValidateMachineDialog::onCheck)
EVT_BUTTON(ID_CLOSE, ValidateMachineDialog::onClose)
END_EVENT_TABLE()

/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param machine Machine containing the address spaces.
 */
ValidateMachineDialog::ValidateMachineDialog(wxWindow *parent,
                                             const TTAMachine::Machine &machine)
    : wxDialog(parent, -1, _T("Validate Machine"), wxDefaultPosition),
      machine_(machine) {

  createContents(this, true, true);

  checkAnsiC_ = dynamic_cast<wxCheckBox *>(FindWindow(ID_CHECK_ANSI_C));
  checkGlobalConnReg_ =
      dynamic_cast<wxCheckBox *>(FindWindow(ID_CHECK_GLOBAL_CONN_REGISTER));

  FindWindow(ID_VALIDATE)->Disable();

  // Global connection register check in ProgrammabilityValidator
  // doesn't work for arbitrary machines yet. The global
  // connection register checkbox is therefore disabled.
  // Following disable-call can be removed when the
  // ProgrammabilityValiadtor works for all machines.
  // FindWindow(ID_CHECK_GLOBAL_CONN_REGISTER)->Disable();
}

/**
 * The Destructor.
 */
ValidateMachineDialog::~ValidateMachineDialog() {}

/**
 * Event handler for the checkbox check toggles.
 *
 * Disables the Validate-button if no checkboxes are checked.
 */
void ValidateMachineDialog::onCheck(wxCommandEvent &) {
  if (checkAnsiC_->IsChecked() || checkGlobalConnReg_->IsChecked()) {

    FindWindow(ID_VALIDATE)->Enable();
  } else {
    FindWindow(ID_VALIDATE)->Disable();
  }
}

/**
 * Event handler for the Validate-button.
 *
 * Validates the machine using a ProgrammabilityValidator.
 * Only checks that are selected with the checkboxes are ran.
 */
void ValidateMachineDialog::onValidate(wxCommandEvent &) {

  bool valid = true;

  wxString resultPage = _T("<html><body><small>");

  std::set<ProgrammabilityValidator::ErrorCode> checks;
  if (checkGlobalConnReg_->IsChecked()) {
    checks.insert(
        ProgrammabilityValidator::GLOBAL_CONNECTION_REGISTER_NOT_FOUND);
    checks.insert(ProgrammabilityValidator::MISSING_CONNECTION);
  }
  if (checkAnsiC_->IsChecked()) {
    checks.insert(ProgrammabilityValidator::
                      OPERATION_MISSING_FROM_THE_PRIMITIVE_OPERATION_SET);
  }

  assert(!checks.empty());

  try {
    ProgrammabilityValidator validator(machine_);

    ProgrammabilityValidatorResults *results = validator.validate(checks);
    for (int i = 0; i < results->errorCount(); i++) {
      valid = false;
      resultPage.Append(WxConversion::toWxString(results->error(i).second));
      resultPage.Append(_T("<hr>"));
    }
    delete results;
    results = NULL;
  } catch (IllegalMachine &e) {
    valid = false;
    resultPage.Append(WxConversion::toWxString(e.errorMessage()));
  }
  if (valid) {
    resultPage.Append(_T("<font color=#00c000>Validity checks passed!</font>"));
  }
  resultPage.Append(_T("</small></body></html>"));
  resultsWindow_->SetPage(resultPage);
}

/**
 * Event handler for the Close-button.
 *
 * Closes the dialog.
 */
void ValidateMachineDialog::onClose(wxCommandEvent &) { Close(); }

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
wxSizer *ValidateMachineDialog::createContents(wxWindow *parent, bool call_fit,
                                               bool set_sizer) {

  wxBoxSizer *item0 = new wxBoxSizer(wxVERTICAL);

  wxBoxSizer *item1 = new wxBoxSizer(wxVERTICAL);

  wxStaticBox *item3 = new wxStaticBox(parent, -1, wxT("Validate:"));
  wxStaticBoxSizer *item2 = new wxStaticBoxSizer(item3, wxVERTICAL);

  wxCheckBox *item4 =
      new wxCheckBox(parent, ID_CHECK_ANSI_C,
                     wxT("Check for operations needed to support ANSI C."),
                     wxDefaultPosition, wxDefaultSize, 0);
  item2->Add(item4, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

  wxCheckBox *item5 =
      new wxCheckBox(parent, ID_CHECK_GLOBAL_CONN_REGISTER,
                     wxT("Check for global connection register."),
                     wxDefaultPosition, wxDefaultSize, 0);
  item2->Add(item5, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

  wxButton *item6 = new wxButton(parent, ID_VALIDATE, wxT("Validate"),
                                 wxDefaultPosition, wxDefaultSize, 0);
  item2->Add(item6, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxALL, 5);

  item1->Add(item2, 0, wxGROW | wxALL, 5);

  wxStaticBox *item8 = new wxStaticBox(parent, -1, wxT("Results:"));
  wxStaticBoxSizer *item7 = new wxStaticBoxSizer(item8, wxVERTICAL);

  resultsWindow_ =
      new wxHtmlWindow(parent, ID_RESULTS, wxDefaultPosition, wxSize(250, 250));
  wxWindow *item9 = resultsWindow_;
  wxASSERT(item9);
  item7->Add(item9, 0, wxGROW | wxALL, 5);

  item1->Add(item7, 0, wxGROW | wxALL, 5);

  item0->Add(item1, 0, wxGROW | wxALL, 5);

  wxStaticLine *item10 = new wxStaticLine(parent, ID_LINE, wxDefaultPosition,
                                          wxSize(20, -1), wxLI_HORIZONTAL);
  item0->Add(item10, 0, wxGROW | wxALL, 5);

  wxButton *item11 = new wxButton(parent, ID_CLOSE, wxT("Close"),
                                  wxDefaultPosition, wxDefaultSize, 0);
  item0->Add(item11, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxALL, 5);

  if (set_sizer) {
    parent->SetSizer(item0);
    if (call_fit)
      item0->SetSizeHints(parent);
  }

  return item0;
}
