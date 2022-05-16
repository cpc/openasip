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
 * @file ImmediateSlotDialog.cc
 *
 * Implementation of ImmediateSlotDialog class.
 *
 * @author Veli-Pekka J��skel�inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <boost/format.hpp>
#include <wx/statline.h>
#include <wx/listctrl.h>

#include "ImmediateSlotDialog.hh"
#include "ImmediateSlot.hh"
#include "Machine.hh"
#include "WxConversion.hh"
#include "WidgetTools.hh"
#include "ProDeTextGenerator.hh"
#include "GUITextGenerator.hh"
#include "MachineTester.hh"
#include "InformationDialog.hh"
#include "WarningDialog.hh"

using std::string;
using boost::format;
using namespace TTAMachine;

BEGIN_EVENT_TABLE(ImmediateSlotDialog, wxDialog)
    EVT_LIST_ITEM_SELECTED(ID_SLOT_LIST, ImmediateSlotDialog::onSlotSelection)
    EVT_LIST_ITEM_DESELECTED(ID_SLOT_LIST, ImmediateSlotDialog::onSlotSelection)
    EVT_BUTTON(ID_ADD_SLOT, ImmediateSlotDialog::onAddSlot)
    EVT_BUTTON(ID_DELETE_SLOT, ImmediateSlotDialog::onDeleteSlot)
    EVT_TEXT(ID_NAME, ImmediateSlotDialog::onSlotName)
END_EVENT_TABLE()


/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param machine Parent Machine of the immediate slots.
 */
ImmediateSlotDialog::ImmediateSlotDialog(
    wxWindow* parent,
    Machine* machine) :
    wxDialog(parent, -1, _T(""), wxDefaultPosition),
    machine_(machine) {

    createContents(this, true, true);

    slotList_ = dynamic_cast<wxListCtrl*>(FindWindow(ID_SLOT_LIST));

    FindWindow(ID_NAME)->SetValidator(
	wxTextValidator(wxFILTER_ASCII, &slotName_));

    // Disable conditional buttons.
    FindWindow(ID_ADD_SLOT)->Disable();
    FindWindow(ID_DELETE_SLOT)->Disable();

    setTexts();
}


/**
 * The Destructor.
 */
ImmediateSlotDialog::~ImmediateSlotDialog() {
}


/**
 * Sets widget texts.
 */
void
ImmediateSlotDialog::setTexts() {
    GUITextGenerator* generator = GUITextGenerator::instance();
    ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();

    // Dialog title.
    format fmt = prodeTexts->text(
	ProDeTextGenerator::TXT_IMMEDIATE_SLOT_DIALOG_TITLE);
    SetTitle(WxConversion::toWxString(fmt.str()));

    // Buttons
    WidgetTools::setLabel(generator, FindWindow(wxID_OK),
			  GUITextGenerator::TXT_BUTTON_OK);
    WidgetTools::setLabel(generator, FindWindow(wxID_CANCEL),
			  GUITextGenerator::TXT_BUTTON_CANCEL);
    WidgetTools::setLabel(generator, FindWindow(ID_HELP),
			  GUITextGenerator::TXT_BUTTON_HELP);
    WidgetTools::setLabel(generator, FindWindow(ID_ADD_SLOT),
			  GUITextGenerator::TXT_BUTTON_ADD);
    WidgetTools::setLabel(generator, FindWindow(ID_DELETE_SLOT),
			  GUITextGenerator::TXT_BUTTON_DELETE);

    WidgetTools::setLabel(generator, FindWindow(ID_NAME),
			  ProDeTextGenerator::TXT_LABEL_NAME);

    format fmtCol = prodeTexts->text(ProDeTextGenerator::TXT_COLUMN_NAME);
    slotList_->InsertColumn(0, WxConversion::toWxString(fmtCol.str()),
			    wxLIST_FORMAT_LEFT, 300);

}


/**
 * Transfers data from the machine model to the dialog widgets.
 */
bool
ImmediateSlotDialog::TransferDataToWindow() {

    // update slot list
    slotList_->DeleteAllItems();
    const Machine::ImmediateSlotNavigator navigator =
	machine_->immediateSlotNavigator();

    for (int i = 0; i < navigator.count(); i++) {
	slotList_->InsertItem(
	    i, WxConversion::toWxString(navigator.item(i)->name()));
    }

    return wxDialog::TransferDataToWindow();
}


/**
 * Enables and disables the delete button according to slot list selection.
 */
void
ImmediateSlotDialog::onSlotSelection(wxListEvent&) {
    if (slotList_->GetSelectedItemCount() == 1) {
	FindWindow(ID_DELETE_SLOT)->Enable();
    } else {
	FindWindow(ID_DELETE_SLOT)->Disable();
    }
}


/**
 * Deletes the selected immediate slot.
 */
void
ImmediateSlotDialog::onDeleteSlot(wxCommandEvent&) {

    // Check which slot is selected.
    long item = -1;
    item = slotList_->GetNextItem(
	item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

    if (item < 0) {
	return;
    }

    ImmediateSlot* slot =
	machine_->immediateSlotNavigator().item(item);

    delete slot;

    TransferDataToWindow();
    wxListEvent dummy;
    onSlotSelection(dummy);
}


/**
 * Adds a new slot to the machine when "Add" button is pressed.
 */
void
ImmediateSlotDialog::onAddSlot(wxCommandEvent&) {

    if (!TransferDataFromWindow()) {
	return;
    }

    string trimmedName =
	WxConversion::toString(slotName_.Trim(false).Trim(true));

    // Check name validity.
    if (!MachineTester::isValidComponentName(trimmedName)) {
	ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();
	format message =
	    prodeTexts->text(ProDeTextGenerator::MSG_ERROR_ILLEGAL_NAME);
	InformationDialog warning(
	    this, WxConversion::toWxString(message.str()));
	warning.ShowModal();
	return;
    }

    if (machine_->immediateSlotNavigator().hasItem(trimmedName)) {
	ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();
	format message =
	    prodeTexts->text(ProDeTextGenerator::MSG_ERROR_SAME_NAME);
	message % trimmedName;
	message % prodeTexts->text(ProDeTextGenerator::COMP_AN_IMM_SLOT).str();
	message % prodeTexts->text(ProDeTextGenerator::COMP_MACHINE).str();
	message % prodeTexts->text(ProDeTextGenerator::COMP_IMM_SLOT).str();
	WarningDialog warning(this, WxConversion::toWxString(message.str()));
	warning.ShowModal();
	return;
    }

    // Buses share namespace with immediate slots. Check that a bus with the
    // same name does not exist.
    if (machine_->busNavigator().hasItem(trimmedName)) {
	ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();
	format message =
	    prodeTexts->text(ProDeTextGenerator::MSG_ERROR_SAME_NAME);
	message % trimmedName;
	message % prodeTexts->text(ProDeTextGenerator::COMP_A_BUS).str();
	message % prodeTexts->text(ProDeTextGenerator::COMP_MACHINE).str();
	message % prodeTexts->text(ProDeTextGenerator::COMP_IMM_SLOT).str();
	WarningDialog warning(this, WxConversion::toWxString(message.str()));
	warning.ShowModal();
	return;
    }

    new ImmediateSlot(trimmedName, *machine_);
    slotName_ = _T("");
    TransferDataToWindow();
}


/**
 * Enables and disables the "Add" button when text is entered in the
 * slot name widget.
 */
void
ImmediateSlotDialog::onSlotName(wxCommandEvent&) {

    if (!TransferDataFromWindow()) {
	return;
    }

    wxString trimmedName = slotName_.Trim(false).Trim(true);
    if (trimmedName == _T("")) {
	FindWindow(ID_ADD_SLOT)->Disable();
    } else {
	FindWindow(ID_ADD_SLOT)->Enable();
    }
}


/**
 * Creates the dialog contents.
 *
 * @param parent Parent dialog of the contents.
 * @param call_fit If true, fits the contents inside the dialog.
 * @param set_sizer If true, sets the main sizer as dialog contents.
 * @return Top level sizer of the dialog contents.
 */
wxSizer*
ImmediateSlotDialog::createContents(
    wxWindow *parent, bool call_fit, bool set_sizer) {


    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxListCtrl *item1 = new wxListCtrl( parent, ID_SLOT_LIST, wxDefaultPosition, wxSize(300,200), wxLC_REPORT|wxLC_SINGLE_SEL|wxSUNKEN_BORDER );
    item0->Add(item1, 0, wxGROW | wxALL, 5);

    wxBoxSizer *item2 = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *item3 = new wxBoxSizer( wxHORIZONTAL );

    wxBoxSizer *item4 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticText *item5 = new wxStaticText( parent, ID_LABEL_NAME, wxT("Name:"), wxDefaultPosition, wxDefaultSize, 0 );
    item4->Add( item5, 0, wxALIGN_CENTER|wxALL, 5 );

    wxTextCtrl *item6 = new wxTextCtrl( parent, ID_NAME, wxT(""), wxDefaultPosition, wxSize(120,-1), 0 );
    item4->Add( item6, 0, wxALIGN_CENTER|wxALL, 5 );

    item3->Add(item4, 0, wxGROW | wxALL, 5);

    wxBoxSizer *item7 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item8 = new wxButton( parent, ID_ADD_SLOT, wxT("Add"), wxDefaultPosition, wxDefaultSize, 0 );
    item7->Add( item8, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item9 = new wxButton( parent, ID_DELETE_SLOT, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    item7->Add( item9, 0, wxALIGN_CENTER|wxALL, 5 );

    item3->Add( item7, 0, wxALIGN_CENTER, 5 );

    item2->Add( item3, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add(item2, 0, wxGROW | wxALL, 5);

    wxStaticLine *item10 = new wxStaticLine( parent, ID_LINE, wxDefaultPosition, wxSize(20,-1), wxLI_HORIZONTAL );
    item0->Add(item10, 0, wxGROW | wxALL, 5);

    wxGridSizer *item11 = new wxGridSizer( 2, 0, 0 );

    wxButton *item12 = new wxButton( parent, ID_HELP, wxT("&Help"), wxDefaultPosition, wxDefaultSize, 0 );
    item11->Add( item12, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item13 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item14 = new wxButton( parent, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item13->Add( item14, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item15 = new wxButton( parent, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item13->Add( item15, 0, wxALIGN_CENTER|wxALL, 5 );

    item11->Add(item13, 0, wxALIGN_RIGHT, 5);

    item0->Add(item11, 0, wxGROW, 5);

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }

    return item0;
}
