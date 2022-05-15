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
 * @file AddressSpaceDialog.cc
 *
 * Definition of AddressSpaceDialog class.
 *
 * @author Veli-Pekka J��skel�inen 2004 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <boost/format.hpp>
#include <string>
#include <wx/spinctrl.h>
#include <wx/statline.h>
#include <wx/valgen.h>

#include "AddressSpace.hh"
#include "AddressSpaceDialog.hh"
#include "Conversion.hh"
#include "GUITextGenerator.hh"
#include "InformationDialog.hh"
#include "Machine.hh"
#include "MachineTester.hh"
#include "MathTools.hh"
#include "ModelConstants.hh"
#include "NumberControl.hh"
#include "ProDeTextGenerator.hh"
#include "WarningDialog.hh"
#include "WidgetTools.hh"
#include "WxConversion.hh"

using boost::format;
using std::string;
using namespace TTAMachine;

BEGIN_EVENT_TABLE(AddressSpaceDialog, wxDialog)
EVT_TEXT(ID_NAME, AddressSpaceDialog::onName)
EVT_BUTTON(wxID_OK, AddressSpaceDialog::onOK)
EVT_TEXT(ID_MIN_ADDRESS, AddressSpaceDialog::onMinAddress)
EVT_TEXT(ID_MAX_ADDRESS, AddressSpaceDialog::onMaxAddress)
EVT_TEXT(ID_BIT_WIDTH, AddressSpaceDialog::onBitWidthText)
EVT_SPINCTRL(ID_BIT_WIDTH, AddressSpaceDialog::onBitWidth)

EVT_BUTTON(ID_ADD_ID, AddressSpaceDialog::onAddId)
EVT_BUTTON(ID_DELETE_ID, AddressSpaceDialog::onDeleteId)
EVT_SPINCTRL(ID_SPIN_ID, AddressSpaceDialog::onSpinId)
EVT_LIST_ITEM_FOCUSED(ID_ID_LIST, AddressSpaceDialog::onIdListSelection)
EVT_LIST_DELETE_ITEM(ID_ID_LIST, AddressSpaceDialog::onIdListSelection)
EVT_LIST_ITEM_SELECTED(ID_ID_LIST, AddressSpaceDialog::onIdListSelection)
EVT_LIST_ITEM_DESELECTED(ID_ID_LIST, AddressSpaceDialog::onIdListSelection)
END_EVENT_TABLE()

/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param addressSpace The address space to be modified with the dialog.
 */
AddressSpaceDialog::AddressSpaceDialog(wxWindow *parent,
                                       TTAMachine::Machine *machine,
                                       AddressSpace *addressSpace)
    : wxDialog(parent, -1, _T(""), wxDefaultPosition), machine_(machine),
      as_(addressSpace), name_(_T("")), width_(ModelConstants::DEFAULT_WIDTH),
      nameSizer_(NULL), minAddressSizer_(NULL), maxAddressSizer_(NULL),
      widthSizer_(NULL) {

  createContents(this, true, true);

  // disable conditional buttons initially
  FindWindow(wxID_OK)->Disable();
  FindWindow(ID_DELETE_ID)->Disable();

  // set validators for dialog controls
  FindWindow(ID_NAME)->SetValidator(wxTextValidator(wxFILTER_ASCII, &name_));
  FindWindow(ID_WIDTH)->SetValidator(wxGenericValidator(&width_));

  // set min and max adress spin button ranges
  minControl_ = dynamic_cast<NumberControl *>(FindWindow(ID_MIN_ADDRESS));
  maxControl_ = dynamic_cast<NumberControl *>(FindWindow(ID_MAX_ADDRESS));

  bitWidthSpinCtrl_ = dynamic_cast<wxSpinCtrl *>(FindWindow(ID_BIT_WIDTH));

  // set widget texts
  setTexts();

  TransferDataToWindow();

  FindWindow(ID_NAME)->SetFocus();
}

/**
 * The Destructor.
 */
AddressSpaceDialog::~AddressSpaceDialog() {}

/**
 * Sets texts for widgets.
 */
void AddressSpaceDialog::setTexts() {
  GUITextGenerator *generator = GUITextGenerator::instance();
  ProDeTextGenerator *prodeTexts = ProDeTextGenerator::instance();

  // Dialog title
  format fmt =
      prodeTexts->text(ProDeTextGenerator::TXT_ADDRESS_SPACE_DIALOG_TITLE);
  SetTitle(WxConversion::toWxString(fmt.str()));

  // buttons
  WidgetTools::setLabel(generator, FindWindow(wxID_OK),
                        GUITextGenerator::TXT_BUTTON_OK);

  WidgetTools::setLabel(generator, FindWindow(wxID_CANCEL),
                        GUITextGenerator::TXT_BUTTON_CANCEL);

  WidgetTools::setLabel(generator, FindWindow(ID_HELP),
                        GUITextGenerator::TXT_BUTTON_HELP);

  WidgetTools::setLabel(generator, FindWindow(ID_ADD_ID),
                        GUITextGenerator::TXT_BUTTON_ADD_DIALOG);

  WidgetTools::setLabel(generator, FindWindow(ID_DELETE_ID),
                        GUITextGenerator::TXT_BUTTON_DELETE);

  // box sizer label
  fmt = prodeTexts->text(ProDeTextGenerator::TXT_LABEL_NAME);
  WidgetTools::setWidgetLabel(nameSizer_, fmt.str());

  fmt = prodeTexts->text(ProDeTextGenerator::TXT_LABEL_MAU);
  WidgetTools::setWidgetLabel(widthSizer_, fmt.str());

  fmt = prodeTexts->text(ProDeTextGenerator::TXT_ADDRESS_SPACE_MIN_ADDRESS);
  WidgetTools::setWidgetLabel(minAddressSizer_, fmt.str());

  fmt = prodeTexts->text(ProDeTextGenerator::TXT_ADDRESS_SPACE_MAX_ADDRESS);
  WidgetTools::setWidgetLabel(maxAddressSizer_, fmt.str());
}

/**
 * Transfers data from the AddressSpace object to the dialog widgets.
 *
 * @return false, if an error occured in the transfer, true otherwise.
 */
bool AddressSpaceDialog::TransferDataToWindow() {

  name_ = WxConversion::toWxString(as_->name());
  width_ = as_->width();

  minControl_->setValue(as_->start());
  maxControl_->setValue(as_->end());
  // minControl_->setRange(0, as_->end() - 1);
  // maxControl_->setRange(as_->start() + 1, 0xFFFFFFFF);

  idNumbers_ = as_->numericalIds();
  updateIdLists();

  // wxWidgets GTK1 version seems to bug with spincontrol validators.
  // The widget value has to be set manually.
  dynamic_cast<wxSpinCtrl *>(FindWindow(ID_WIDTH))->SetValue(width_);

  wxCommandEvent dummy;
  onMaxAddress(dummy);
  onMinAddress(dummy);

  wxSpinEvent dummySpin;
  onSpinId(dummySpin);

  return wxWindow::TransferDataToWindow();
}

/**
 * Updates the id lists.
 */
void AddressSpaceDialog::updateIdLists() {
  assert(idListCtrl_ != NULL);
  idListCtrl_->DeleteAllItems();

  for (std::set<unsigned>::iterator it = idNumbers_.begin();
       it != idNumbers_.end(); ++it) {
    idListCtrl_->InsertItem(static_cast<long>(*it),
                            WxConversion::toWxString(*it));
  }
}

/**
 * Validates input in the controls, and updates the AddressSpace.
 */
void AddressSpaceDialog::onOK(wxCommandEvent &) {

  if (!Validate()) {
    return;
  }

  if (!TransferDataFromWindow()) {
    return;
  }

  string trimmedName = WxConversion::toString(name_.Trim(false).Trim(true));

  // Check the name validity.
  if (!MachineTester::isValidComponentName(trimmedName)) {
    ProDeTextGenerator *prodeTexts = ProDeTextGenerator::instance();
    format message =
        prodeTexts->text(ProDeTextGenerator::MSG_ERROR_ILLEGAL_NAME);
    InformationDialog warning(this, WxConversion::toWxString(message.str()));
    warning.ShowModal();
    return;
  }

  if (trimmedName != as_->name()) {

    // Check that the new address space name is unique among
    // all address spaces in the machine.
    Machine::AddressSpaceNavigator navigator =
        as_->machine()->addressSpaceNavigator();
    for (int i = 0; i < navigator.count(); i++) {
      string asName = navigator.item(i)->name();
      if (trimmedName == asName) {
        ProDeTextGenerator *prodeTexts = ProDeTextGenerator::instance();
        format message =
            prodeTexts->text(ProDeTextGenerator::MSG_ERROR_SAME_NAME);
        format an_as =
            prodeTexts->text(ProDeTextGenerator::COMP_AN_ADDRESS_SPACE);
        format machine = prodeTexts->text(ProDeTextGenerator::COMP_MACHINE);
        format as = prodeTexts->text(ProDeTextGenerator::COMP_ADDRESS_SPACE);
        message % trimmedName % an_as.str() % machine.str() % as.str();
        WarningDialog warning(this, WxConversion::toWxString(message.str()));
        warning.ShowModal();
        return;
      }
    }
  }

  as_->setName(trimmedName);
  as_->setWidth(width_);
  unsigned int minAddr = minControl_->unsignedValue();
  unsigned int maxAddr = maxControl_->unsignedValue();
  as_->setAddressBounds(minAddr, maxAddr);

  if (!as_->setNumericalIds(idNumbers_)) {
    ProDeTextGenerator *prodeTexts = ProDeTextGenerator::instance();
    format message = prodeTexts->text(ProDeTextGenerator::MSG_ERROR_ID_EXISTS);
    InformationDialog dialog(this, WxConversion::toWxString(message.str()));
    dialog.ShowModal();
  }

  EndModal(wxID_OK);
}

/**
 * Checks whether the name field is empty, and disables OK button of the
 * dialog if it is.
 */
void AddressSpaceDialog::onName(wxCommandEvent &) {
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
 * Sets the range of the MaxAddress spin-button.
 */
void AddressSpaceDialog::onMinAddress(wxCommandEvent &) {
  if (minControl_->unsignedValue() >= maxControl_->unsignedValue()) {
    minControl_->setValue(maxControl_->unsignedValue() - 1);
  }
}

/**
 * Sets the range of the MaxAddress spin-button.
 */
void AddressSpaceDialog::onMaxAddress(wxCommandEvent &) {
  if (maxControl_->unsignedValue() <= minControl_->unsignedValue()) {
    maxControl_->setValue(minControl_->unsignedValue() + 1);
  }
  int bitWidth = MathTools::requiredBits(maxControl_->unsignedValue());
  bitWidthSpinCtrl_->SetValue(bitWidth);
}

/**
 * Reads the range from bitWidth spin-button and updates min- and max ranges.
 */
void AddressSpaceDialog::onBitWidthText(wxCommandEvent &) {
  // wxSpinEvent dummy;
  // onBitWidth(dummy);
}

/**
 * Reads the range from bitWidth spin-button and updates min- and max ranges.
 */
void AddressSpaceDialog::onBitWidth(wxSpinEvent &) {

  unsigned maxAddress =
      static_cast<unsigned>(pow(2, bitWidthSpinCtrl_->GetValue()) - 1u);
  maxControl_->setValue(maxAddress);

  wxCommandEvent dummy;
  onMaxAddress(dummy);
  onMinAddress(dummy);
}

/**
 * Adds a new id number for the address space.
 */
void AddressSpaceDialog::onAddId(wxCommandEvent & /*event*/) {
  // current number in spin field
  unsigned spinId = static_cast<unsigned>(idSpinCtrl_->GetValue());

  // existing entry found from some address space
  if (!isFreeId(spinId)) {
    return;
  }

  // no conflict, make a new entry for the current value in the spin field
  idNumbers_.insert(spinId);
  updateIdLists();

  // disable add button for added id number
  FindWindow(ID_ADD_ID)->Disable();
}

/**
 * Deletes selected id number(s) from the address space.
 */
void AddressSpaceDialog::onDeleteId(wxCommandEvent & /*event*/) {
  long itemIndex = -1;

  // loop through all selected ids
  for (int i = 0; i < idListCtrl_->GetSelectedItemCount(); ++i) {
    itemIndex = idListCtrl_->GetNextItem(itemIndex, wxLIST_NEXT_ALL,
                                         wxLIST_STATE_SELECTED);

    // get numerical value of the id and erase it from the address space
    if (itemIndex >= 0) {
      wxString idText = idListCtrl_->GetItemText(itemIndex);
      long id;
      if (idText.ToLong(&id)) {
        idNumbers_.erase(id);
      }
    }
  }

  updateIdLists();

  wxSpinEvent dummy1;
  onSpinId(dummy1);
  wxListEvent dummy2;
  onIdListSelection(dummy2);
}

/**
 * Sets state of add button depending on whether the number in spin input
 * already exists in some address space
 */
void AddressSpaceDialog::onSpinId(wxSpinEvent & /*event*/) {
  // current number in spin field
  unsigned spinId = static_cast<unsigned>(idSpinCtrl_->GetValue());

  // no conflict found, enable add button
  if (isFreeId(spinId)) {
    FindWindow(ID_ADD_ID)->Enable();
    return;
  }

  // the number is already reserved, disable add button
  FindWindow(ID_ADD_ID)->Disable();
}

/**
 * Sets state of delete button depending on whether items are selected
 */
void AddressSpaceDialog::onIdListSelection(wxListEvent & /*event*/) {
  if (idListCtrl_->GetSelectedItemCount() < 1) {
    FindWindow(ID_DELETE_ID)->Disable();
  } else {
    FindWindow(ID_DELETE_ID)->Enable();
  }
}

/**
 * Checks if given id is already reserved by some address space
 *
 * @param id Number that should be checked against reserved ids
 */
bool AddressSpaceDialog::isFreeId(unsigned id) const {
  assert(machine_ != NULL);
  std::set<unsigned>::iterator it;

  // check id numbers reserved by this address space
  it = idNumbers_.find(id);
  if (it != idNumbers_.end()) {
    return false;
  }

  Machine::AddressSpaceNavigator asNavigator =
      machine_->addressSpaceNavigator();

  // check other address spaces
  for (int i = 0; i < asNavigator.count(); i++) {
    AddressSpace *as = asNavigator.item(i);

    // ignore address space owned by this because data might be old
    if (as != as_) {
      std::set<unsigned> ids = as->numericalIds();
      it = ids.find(id);
      if (it != ids.end()) {
        return false;
      }
    }
  }

  // the id is free
  return true;
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
wxSizer *AddressSpaceDialog::createContents(wxWindow *parent, bool call_fit,
                                            bool set_sizer) {

  wxBoxSizer *mainCol = new wxBoxSizer(wxVERTICAL);

  wxFlexGridSizer *mainFlex = new wxFlexGridSizer(2, 0, 0);

  wxStaticBox *nameLabel = new wxStaticBox(parent, -1, wxT("Name:"));
  wxStaticBoxSizer *nameCol = new wxStaticBoxSizer(nameLabel, wxVERTICAL);
  nameSizer_ = nameCol;

  wxTextCtrl *item4 = new wxTextCtrl(parent, ID_NAME, wxT(""),
                                     wxDefaultPosition, wxSize(160, -1), 0);
  nameCol->Add(item4, 0, wxGROW | wxALL, 5);

  mainFlex->Add(nameCol, 0, wxGROW | wxALL, 5);

  wxStaticBox *item6 = new wxStaticBox(parent, -1, wxT("Min-Address"));
  wxStaticBoxSizer *minAddressCol = new wxStaticBoxSizer(item6, wxVERTICAL);
  minAddressSizer_ = minAddressCol;

  NumberControl *item7 = new NumberControl(
      parent, ID_MIN_ADDRESS, wxDefaultPosition, wxSize(180, -1),
      (NumberControl::MODE_UNSIGNED | NumberControl::MODE_HEXADECIMAL));
  wxASSERT(item7);
  minAddressCol->Add(item7, 0, wxALIGN_CENTER | wxALL, 5);

  mainFlex->Add(minAddressCol, 0, wxGROW | wxALL, 5);

  wxStaticBox *item9 = new wxStaticBox(parent, -1, wxT("Width:"));
  wxStaticBoxSizer *item8 = new wxStaticBoxSizer(item9, wxVERTICAL);
  widthSizer_ = item8;

  wxSpinCtrl *item10 =
      new wxSpinCtrl(parent, ID_WIDTH, wxT("1"), wxDefaultPosition,
                     wxSize(100, -1), 0, 1, 10000, 1, wxT("Spin"));
  item8->Add(item10, 0, wxGROW | wxALL, 5);

  mainFlex->Add(item8, 0, wxGROW | wxALL, 5);

  wxStaticBox *item12 = new wxStaticBox(parent, -1, wxT("Max-Address"));
  wxStaticBoxSizer *item11 = new wxStaticBoxSizer(item12, wxVERTICAL);
  maxAddressSizer_ = item11;

  NumberControl *item13 = new NumberControl(
      parent, ID_MAX_ADDRESS, wxDefaultPosition, wxSize(180, -1),
      (NumberControl::MODE_UNSIGNED | NumberControl::MODE_HEXADECIMAL));
  wxASSERT(item13);
  item11->Add(item13, 0, wxALIGN_CENTER | wxALL, 5);

  wxBoxSizer *bitRow = new wxBoxSizer(wxHORIZONTAL);

  wxStaticText *bitText = new wxStaticText(parent, -1, wxT("Bits:"));
  bitRow->Add(bitText, 0, wxGROW | wxALL, 5);

  wxSpinCtrl *bitWidth = new wxSpinCtrl(
      parent, ID_BIT_WIDTH, wxT("1"), wxDefaultPosition, wxDefaultSize,
      wxSP_ARROW_KEYS, 1, 10000, 1); // last 3: min, max, initial
  bitRow->Add(bitWidth, 0, wxGROW | wxALL, 5);

  item11->Add(bitRow, 0, wxGROW | wxALL, 5);

  mainFlex->Add(item11, 0, wxGROW | wxALL, 5);

  // address space id box
  wxStaticBox *itemIdBox = new wxStaticBox(parent, -1, wxT("ID number:"));
  wxStaticBoxSizer *itemIdSizer = new wxStaticBoxSizer(itemIdBox, wxHORIZONTAL);
  idSizer_ = itemIdSizer;

  wxListCtrl *itemIdListCtrl =
      new wxListCtrl(parent, ID_ID_LIST, wxDefaultPosition, wxSize(100, 120),
                     wxLC_REPORT | wxSUNKEN_BORDER);
  idSizer_->Add(itemIdListCtrl, 0, wxALIGN_CENTER | wxALL, 5);
  idListCtrl_ = itemIdListCtrl;
  idListCtrl_->InsertColumn(0, _("ID"));

  wxBoxSizer *sizerOnRight = new wxBoxSizer(wxVERTICAL);

  wxSpinCtrl *itemIdSpinCtrl =
      new wxSpinCtrl(parent, ID_SPIN_ID, wxT("0"), wxDefaultPosition,
                     wxSize(100, -1), 0, 0, 2147483647, 1, wxT("Spin"));
  sizerOnRight->Add(itemIdSpinCtrl, 0, wxGROW | wxALL, 5);
  idSpinCtrl_ = itemIdSpinCtrl;

  wxButton *addButton = new wxButton(parent, ID_ADD_ID, wxT("Add"),
                                     wxDefaultPosition, wxDefaultSize, 0);
  sizerOnRight->Add(addButton, 0, wxALIGN_CENTER | wxALL, 5);

  wxButton *deleteButton = new wxButton(parent, ID_DELETE_ID, wxT("Delete"),
                                        wxDefaultPosition, wxDefaultSize, 0);
  sizerOnRight->Add(deleteButton, 0, wxALIGN_CENTER | wxALL, 5);

  idSizer_->Add(sizerOnRight, 0, wxALIGN_CENTER | wxALL, 5);
  mainFlex->Add(idSizer_, 0, wxGROW | wxALL, 5);

  assert(idSizer_ != NULL);
  assert(idListCtrl_ != NULL);
  assert(idSpinCtrl_ != NULL);
  // end of address space id box

  mainCol->Add(mainFlex, 0, wxALIGN_CENTER | wxALL, 5);

  wxStaticLine *item14 = new wxStaticLine(parent, ID_LINE, wxDefaultPosition,
                                          wxSize(20, -1), wxLI_HORIZONTAL);
  mainCol->Add(item14, 0, wxGROW | wxALL, 5);

  wxGridSizer *buttonFlex = new wxGridSizer(2, 0, 0);

  wxButton *item16 = new wxButton(parent, ID_HELP, wxT("&Help"),
                                  wxDefaultPosition, wxDefaultSize, 0);
  buttonFlex->Add(item16, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

  wxBoxSizer *okCancelRow = new wxBoxSizer(wxHORIZONTAL);

  wxButton *item18 = new wxButton(parent, wxID_OK, wxT("&OK"),
                                  wxDefaultPosition, wxDefaultSize, 0);
  okCancelRow->Add(item18, 0, wxALIGN_CENTER, 5);

  wxButton *item19 = new wxButton(parent, wxID_CANCEL, wxT("&Cancel"),
                                  wxDefaultPosition, wxDefaultSize, 0);
  okCancelRow->Add(item19, 0, wxALIGN_CENTER | wxALL, 5);

  buttonFlex->Add(okCancelRow, 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxALL, 5);

  mainCol->Add(buttonFlex, 0, wxGROW, 5);

  if (set_sizer) {
    parent->SetSizer(mainCol);
    if (call_fit)
      mainCol->SetSizeHints(parent);
  }

  return mainCol;
}
