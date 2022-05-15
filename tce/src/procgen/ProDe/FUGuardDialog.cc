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
 * @file FUGuardDialog.cc
 *
 * Implementation of FUGuardDialog class.
 *
 * @author Veli-Pekka J��skel�inen 2004 (vjaaskel-no.spam-cs.tut.fi)
 */

#include <boost/format.hpp>
#include <wx/statline.h>
#include <wx/valgen.h>
#include <wx/wx.h>

#include "FUGuardDialog.hh"
#include "FUPort.hh"
#include "FunctionUnit.hh"
#include "GUITextGenerator.hh"
#include "Guard.hh"
#include "InformationDialog.hh"
#include "Machine.hh"
#include "ProDeConstants.hh"
#include "ProDeTextGenerator.hh"
#include "WidgetTools.hh"
#include "WxConversion.hh"

using boost::format;
using std::string;
using namespace TTAMachine;

BEGIN_EVENT_TABLE(FUGuardDialog, wxDialog)
EVT_CHOICE(ID_FU_NAME, FUGuardDialog::onFUChoice)
EVT_BUTTON(wxID_OK, FUGuardDialog::onOK)
EVT_BUTTON(wxID_CANCEL, FUGuardDialog::onCancel)
END_EVENT_TABLE()

/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param guard Port guard to edit.
 */
FUGuardDialog::FUGuardDialog(wxWindow *parent, Bus *bus, PortGuard *guard)
    : wxDialog(parent, -1, _T(""), wxDefaultPosition), inverted_(false),
      newInverted_(false), port_(NULL), bus_(bus), adding_(false) {

  if (guard == NULL) {
    // adding a new guard
    adding_ = true;

    Machine::FunctionUnitNavigator navigator =
        bus_->machine()->functionUnitNavigator();

    for (int i = 0; i < navigator.count(); i++) {
      if (navigator.item(i)->operationPortCount() > 0) {
        port_ = navigator.item(i)->operationPort(0);
        break;
      }
    }

    assert(port_ != NULL);

  } else {
    // editing an old guard
    port_ = guard->port();
    inverted_ = guard->isInverted();
    newInverted_ = inverted_;
    bus_ = guard->parentBus();

    // The guard is temporarily deleted to simplify legality checks.
    delete guard;
    guard = NULL;
  }

  createContents(this, true, true);

  // set pointers to the dialog widgets
  nameChoice_ = dynamic_cast<wxChoice *>(FindWindow(ID_FU_NAME));
  portChoice_ = dynamic_cast<wxChoice *>(FindWindow(ID_FU_PORT));
  invertedBox_ = dynamic_cast<wxCheckBox *>(FindWindow(ID_INVERTED));

  invertedBox_->SetValidator(wxGenericValidator(&newInverted_));

  // set widget texts.
  setTexts();
}

/**
 * The Destructor.
 */
FUGuardDialog::~FUGuardDialog() {}

/**
 * Sets texts for widgets.
 */
void FUGuardDialog::setTexts() {
  GUITextGenerator *generator = GUITextGenerator::instance();
  ProDeTextGenerator *prodeTexts = ProDeTextGenerator::instance();

  // Dialog title
  format fmt = prodeTexts->text(ProDeTextGenerator::TXT_FU_GUARD_DIALOG_TITLE);
  SetTitle(WxConversion::toWxString(fmt.str()));

  // buttons
  WidgetTools::setLabel(generator, FindWindow(wxID_OK),
                        GUITextGenerator::TXT_BUTTON_OK);

  WidgetTools::setLabel(generator, FindWindow(wxID_CANCEL),
                        GUITextGenerator::TXT_BUTTON_CANCEL);

  WidgetTools::setLabel(generator, FindWindow(ID_HELP),
                        GUITextGenerator::TXT_BUTTON_HELP);

  // widget labels
  WidgetTools::setLabel(prodeTexts, FindWindow(ID_LABEL_NAME),
                        ProDeTextGenerator::TXT_LABEL_FU_NAME);

  WidgetTools::setLabel(prodeTexts, FindWindow(ID_LABEL_PORT),
                        ProDeTextGenerator::TXT_LABEL_PORT_NAME);

  WidgetTools::setLabel(prodeTexts, FindWindow(ID_INVERTED),
                        ProDeTextGenerator::TXT_LABEL_INVERTED);
}

/**
 * Transfers data from the guard object to the dialog widgets.
 *
 * @return true, if the transfer was succesful, false otherwise.
 */
bool FUGuardDialog::TransferDataToWindow() {
  nameChoice_->Clear();
  portChoice_->Clear();

  Machine::FunctionUnitNavigator navigator =
      bus_->machine()->functionUnitNavigator();

  // add function units and set selection
  for (int i = 0; i < navigator.count(); i++) {
    wxString name = WxConversion::toWxString(navigator.item(i)->name());
    nameChoice_->Append(name);
  }
  nameChoice_->SetStringSelection(
      WxConversion::toWxString(port_->parentUnit()->name()));

  // add function unit ports and set selection
  for (int i = 0; i < port_->parentUnit()->portCount(); i++) {

    // port must be output port
    if (port_->parentUnit()->port(i)->outputSocket() == NULL) {
      continue;
    }

    wxString name =
        WxConversion::toWxString(port_->parentUnit()->port(i)->name());

    portChoice_->Append(name);
  }
  portChoice_->SetStringSelection(WxConversion::toWxString(port_->name()));

  // set inverted checkbox
  invertedBox_->SetValue(newInverted_);

  return wxDialog::TransferDataToWindow();
}

/**
 * Updates the port choice when the function unit selection is changed.
 */
void FUGuardDialog::onFUChoice(wxCommandEvent &) {

  wxString selection = portChoice_->GetStringSelection();

  portChoice_->Clear();

  FunctionUnit *fu = selectedFU();
  for (int i = 0; i < fu->portCount(); i++) {
    wxString name = WxConversion::toWxString(fu->port(i)->name());
    portChoice_->Append(name);
  }

  if (portChoice_->FindString(selection) >= 0) {
    portChoice_->SetStringSelection(selection);
  } else {
    portChoice_->SetSelection(0);
  }
}

/**
 * Returns a pointer to the selected function unit.
 *
 * @return Pointer to the selected function unit.
 */
FunctionUnit *FUGuardDialog::selectedFU() const {
  string name = WxConversion::toString(nameChoice_->GetStringSelection());
  Machine::FunctionUnitNavigator navigator =
      bus_->machine()->functionUnitNavigator();
  FunctionUnit *fu = navigator.item(name);
  return fu;
}

/**
 * Returns a pointer to the selected function unit port.
 *
 * @return Pointer to the selected function unit port.
 */
FUPort *FUGuardDialog::selectedPort() const {
  string name = WxConversion::toString(portChoice_->GetStringSelection());

  if (name == "") {
    return NULL;
  }

  FUPort *port = selectedFU()->operationPort(name);
  return port;
}

/**
 * Updates the guard object when the OK-button is pressed.
 *
 * Closes the dialog.
 */
void FUGuardDialog::onOK(wxCommandEvent &) {
  FUPort *port = selectedPort();
  if (port == NULL) {
    ProDeTextGenerator *prodeTexts = ProDeTextGenerator::instance();
    format message = prodeTexts->text(ProDeTextGenerator::MSG_ERROR);
    InformationDialog dialog(
        this, WxConversion::toWxString(message.str() + "Select a port.\n"));
    dialog.ShowModal();
    return;
  }

  TransferDataFromWindow();

  try {
    new PortGuard(newInverted_, *port, *bus_);
  } catch (ComponentAlreadyExists &e) {
    ProDeTextGenerator *prodeTexts = ProDeTextGenerator::instance();
    format message =
        prodeTexts->text(ProDeTextGenerator::MSG_ERROR_GUARD_EXISTS);
    InformationDialog dialog(this, WxConversion::toWxString(message.str()));
    dialog.ShowModal();
    return;
  }
  EndModal(wxID_OK);
}

/**
 * Cancels the dialog effects by creating the original port guard.
 *
 * Closes the Dialog.
 */
void FUGuardDialog::onCancel(wxCommandEvent &) {
  if (adding_ == false) {
    new PortGuard(inverted_, *port_, *bus_);
  }
  EndModal(wxID_CANCEL);
}

/**
 * Creates contents of the dialog window.
 *
 * Code generated with wxDesigner.
 *
 * @param parent Parent dialog of the contents.
 * @param call_fit If true, fits sizer in dialog window.
 * @param set_sizer If true, sets sizer as dialog's sizer.
 * @return Top level sizer of the contents.
 */
wxSizer *FUGuardDialog::createContents(wxWindow *parent, bool call_fit,
                                       bool set_sizer) {

  wxBoxSizer *mainCol = new wxBoxSizer(wxVERTICAL);

  wxGridSizer *item1 = new wxGridSizer(2, 0, 0);

  wxStaticText *item2 =
      new wxStaticText(parent, ID_LABEL_NAME, wxT("Function Unit Name:"),
                       wxDefaultPosition, wxDefaultSize, 0);
  item1->Add(item2, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

  wxString *strs3 = (wxString *)NULL;
  wxChoice *item3 = new wxChoice(parent, ID_FU_NAME, wxDefaultPosition,
                                 wxSize(100, -1), 0, strs3, 0);
  item1->Add(item3, 0, wxGROW | wxALL, 5);

  wxStaticText *item4 =
      new wxStaticText(parent, ID_LABEL_PORT, wxT("Port Name:"),
                       wxDefaultPosition, wxDefaultSize, 0);
  item1->Add(item4, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

  wxString *strs5 = (wxString *)NULL;
  wxChoice *item5 = new wxChoice(parent, ID_FU_PORT, wxDefaultPosition,
                                 wxSize(100, -1), 0, strs5, 0);
  item1->Add(item5, 0, wxGROW | wxALL, 5);

  mainCol->Add(item1, 0, wxALIGN_CENTER | wxALL, 5);

  wxCheckBox *item6 = new wxCheckBox(parent, ID_INVERTED, wxT("Inverted"),
                                     wxDefaultPosition, wxDefaultSize, 0);
  mainCol->Add(item6, 0, wxALL, 5);

  wxStaticLine *item7 = new wxStaticLine(parent, ID_LINE, wxDefaultPosition,
                                         wxSize(20, -1), wxLI_HORIZONTAL);
  mainCol->Add(item7, 0, wxGROW | wxALL, 5);

  wxBoxSizer *item8 = new wxBoxSizer(wxHORIZONTAL);

  wxButton *item9 = new wxButton(parent, ID_HELP, wxT("&Help"),
                                 wxDefaultPosition, wxDefaultSize, 0);
  item8->Add(item9, 0, wxALIGN_CENTER | wxALL, 5);

  wxButton *item10 = new wxButton(parent, wxID_OK, wxT("&OK"),
                                  wxDefaultPosition, wxDefaultSize, 0);
  item8->Add(item10, 0, wxALIGN_CENTER | wxALL, 5);

  wxButton *item11 = new wxButton(parent, wxID_CANCEL, wxT("&Cancel"),
                                  wxDefaultPosition, wxDefaultSize, 0);
  item8->Add(item11, 0, wxALIGN_CENTER | wxALL, 5);

  mainCol->Add(item8, 0, wxALIGN_CENTER | wxALL, 5);

  if (set_sizer) {
    parent->SetSizer(mainCol);
    if (call_fit)
      mainCol->SetSizeHints(parent);
  }

  return mainCol;
}
