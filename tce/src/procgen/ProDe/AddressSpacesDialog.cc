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
 * @file AddressSpacesDialog.cc
 *
 * Definition of AddressSpacesDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <string>
#include <wx/statline.h>
#include <wx/listctrl.h>
#include <boost/format.hpp>

#include "AddressSpacesDialog.hh"
#include "AddressSpaceDialog.hh"
#include "WxConversion.hh"
#include "InformationDialog.hh"
#include "Machine.hh"
#include "AddressSpace.hh"
#include "ControlUnit.hh"
#include "ModelConstants.hh"
#include "Machine.hh"
#include "ProDeConstants.hh"
#include "GUITextGenerator.hh"
#include "ProDeTextGenerator.hh"
#include "WidgetTools.hh"
#include "MathTools.hh"
#include "Conversion.hh"

using boost::format;
using std::string;
using namespace TTAMachine;

BEGIN_EVENT_TABLE(AddressSpacesDialog, wxDialog)
    EVT_BUTTON(ID_ADD, AddressSpacesDialog::onAdd)
    EVT_BUTTON(ID_EDIT, AddressSpacesDialog::onEdit)
    EVT_BUTTON(ID_DELETE, AddressSpacesDialog::onDelete)

    EVT_MENU(ID_EDIT, AddressSpacesDialog::onEdit)
    EVT_MENU(ID_DELETE, AddressSpacesDialog::onDelete)

    EVT_LIST_ITEM_FOCUSED(ID_LIST, AddressSpacesDialog::onASSelection)
    EVT_LIST_DELETE_ITEM(ID_LIST, AddressSpacesDialog::onASSelection)
    EVT_LIST_ITEM_SELECTED(ID_LIST, AddressSpacesDialog::onASSelection)
    EVT_LIST_ITEM_DESELECTED(ID_LIST, AddressSpacesDialog::onASSelection)
    EVT_LIST_ITEM_ACTIVATED(ID_LIST, AddressSpacesDialog::onActivateAS)
    EVT_LIST_ITEM_RIGHT_CLICK(ID_LIST, AddressSpacesDialog::onASRightClick)
END_EVENT_TABLE()


/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param machine Machine containing the address spaces.
 */
AddressSpacesDialog::AddressSpacesDialog(
    wxWindow* parent,
    Machine* machine):
    wxDialog(parent, -1, _T(""), wxDefaultPosition, wxSize(400,300)),
    machine_(machine) {

    createContents(this, true, true);

    asList_ = dynamic_cast<wxListCtrl*>(FindWindow(ID_LIST));

    FindWindow(ID_EDIT)->Disable();
    FindWindow(ID_DELETE)->Disable();

    // set texts to widgets
    setTexts();

    updateASList();
}


/**
 * The Destructor.
 */
AddressSpacesDialog::~AddressSpacesDialog() {
}


/**
 * Sets texts for widgets.
 */
void
AddressSpacesDialog::setTexts() {
    GUITextGenerator* generator = GUITextGenerator::instance();
    ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();

    // Dialog title
    format fmt = prodeTexts->text(
        ProDeTextGenerator::TXT_ADDRESS_SPACES_DIALOG_TITLE);
    SetTitle(WxConversion::toWxString(fmt.str()));

    // buttons
    WidgetTools::setLabel(generator, FindWindow(wxID_OK),
                          GUITextGenerator::TXT_BUTTON_OK);

    WidgetTools::setLabel(generator, FindWindow(wxID_CANCEL),
                          GUITextGenerator::TXT_BUTTON_CANCEL);

    WidgetTools::setLabel(generator, FindWindow(ID_HELP),
                          GUITextGenerator::TXT_BUTTON_HELP);

    WidgetTools::setLabel(generator, FindWindow(ID_ADD),
                          GUITextGenerator::TXT_BUTTON_ADD_DIALOG);

    WidgetTools::setLabel(generator, FindWindow(ID_EDIT),
                          GUITextGenerator::TXT_BUTTON_EDIT_DIALOG);

    WidgetTools::setLabel(generator, FindWindow(ID_DELETE),
                          GUITextGenerator::TXT_BUTTON_DELETE);

    // Create address space list column.
    wxListCtrl* portList =
        dynamic_cast<wxListCtrl*>(FindWindow(ID_LIST));
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_COLUMN_NAME);
    portList->InsertColumn(0, WxConversion::toWxString(fmt.str()),
                           wxLIST_FORMAT_LEFT, 100);
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_COLUMN_MAU);
    portList->InsertColumn(1, WxConversion::toWxString(fmt.str()),
                           wxLIST_FORMAT_LEFT, 40);
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_COLUMN_MIN_ADDRESS);
    portList->InsertColumn(2, WxConversion::toWxString(fmt.str()),
                           wxLIST_FORMAT_LEFT, 90);
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_COLUMN_MAX_ADDRESS);
    portList->InsertColumn(3, WxConversion::toWxString(fmt.str()),
                           wxLIST_FORMAT_LEFT, 90);
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_COLUMN_BIT_WIDTH);
    portList->InsertColumn(4, WxConversion::toWxString(fmt.str()),
                           wxLIST_FORMAT_LEFT, 80);
}

/**
 * Returns pointer to the AddressSpace, which is selected on the list.
 *
 * @return The selected address space, or NULL if no address space is selected.
 */
AddressSpace*
AddressSpacesDialog::selectedAS() {

    long item = -1;
    item = asList_->GetNextItem(
	item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

    if (item == -1) {
	return NULL;
    }

    string asName = WxConversion::toString(asList_->GetItemText(item));
    Machine::AddressSpaceNavigator navigator =
	machine_->addressSpaceNavigator();

    if (!navigator.hasItem(asName)) {
        return NULL;
    }

    return navigator.item(asName);

}


/**
 * Creates and shows an address space dialog with default values set
 * for adding an address space.
 */
void
AddressSpacesDialog::onAdd(wxCommandEvent&) {

    // Generate name for the new AS.
    Machine::AddressSpaceNavigator navigator =
        machine_->addressSpaceNavigator();
    int i = 1;
    string newName = ProDeConstants::COMP_NEW_NAME_PREFIX_AS +
        Conversion::toString(i);
    while (navigator.hasItem(newName)) {
        newName = ProDeConstants::COMP_NEW_NAME_PREFIX_AS +
            Conversion::toString(i);
        i++;
    }

    AddressSpace* newAS =
	new AddressSpace(newName,
			 ModelConstants::DEFAULT_AS_MAU_WIDTH,
			 ModelConstants::DEFAULT_AS_MIN_ADDRESS,
			 ModelConstants::DEFAULT_AS_MAX_ADDRESS,
			 *machine_);

    AddressSpaceDialog dialog(this, machine_, newAS);

    if (dialog.ShowModal() == wxID_OK) {
	updateASList();
    } else {
	// The dialog was cancelled.
	delete newAS;
    }
}



/**
 * Handles left mouse button double click on the address space list.
 *
 * Opens the clicked address space in an AddressSpaceDialog.
 */
void
AddressSpacesDialog::onActivateAS(wxListEvent&) {
    wxCommandEvent dummy;
    onEdit(dummy);
}


/**
 * Handles the 'Edit' button event.
 *
 * Opens an AddressSpaceDialog with the selected address space's attributes
 * set.
 */
void
AddressSpacesDialog::onEdit(wxCommandEvent&) {

    AddressSpace* selected = selectedAS();
    if (selected == NULL) {
        // No address space selected.
        return;
    }

    AddressSpaceDialog dialog(this, machine_, selectedAS());

    dialog.ShowModal();
    updateASList();
}


/**
 * Handles the 'Delete' button event.
 *
 * Deletes the selected address space.
 */
void
AddressSpacesDialog::onDelete(wxCommandEvent&) {
    delete selectedAS();
    updateASList();
}


/**
 * Disables and enables Edit and Delete buttons under the address space list.
 *
 * If an address space is selected, buttons are enabled. If no address space
 * is selected the buttons will be disabled.
 */
void
AddressSpacesDialog::onASSelection(wxListEvent&) {
    if (asList_->GetSelectedItemCount() != 1) {
        FindWindow(ID_DELETE)->Disable();
        FindWindow(ID_EDIT)->Disable();
        return;
    }
    FindWindow(ID_DELETE)->Enable();
    FindWindow(ID_EDIT)->Enable();
}


/**
 * Updates the address space list.
 */
void
AddressSpacesDialog::updateASList() {

    Machine::AddressSpaceNavigator asNavigator =
	machine_->addressSpaceNavigator();

    asList_->DeleteAllItems();

    for (int i = 0; i < asNavigator.count(); i++) {
        AddressSpace* as = asNavigator.item(i);
        int bitWidth = MathTools::requiredBits(as->end());
        asList_->InsertItem(i, WxConversion::toWxString(as->name()));
        asList_->SetItem(i, 1, WxConversion::toWxString(as->width()));
        asList_->SetItem(i, 2, WxConversion::toWxString(as->start()));
        asList_->SetItem(i, 3, WxConversion::toWxString(as->end()));
        asList_->SetItem(i, 4, WxConversion::toWxString(bitWidth));
    }
}


/**
 * Opens a pop-up menu when right mouse button is pressed on the list.
 *
 * @param event Information about right mouse click event.
 */
void
AddressSpacesDialog::onASRightClick(wxListEvent& event) {

    asList_->SetItemState(event.GetIndex(), wxLIST_STATE_SELECTED,
                          wxLIST_STATE_SELECTED);

    wxMenu* contextMenu = new wxMenu();

    ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();
    format button = prodeTexts->text(
        ProDeTextGenerator::TXT_LABEL_BUTTON_EDIT);
    contextMenu->Append(
        ID_EDIT, WxConversion::toWxString(button.str()));
    button = prodeTexts->text(
        ProDeTextGenerator::TXT_LABEL_BUTTON_DELETE);
    contextMenu->Append(
        ID_DELETE, WxConversion::toWxString(button.str()));
    asList_->PopupMenu(contextMenu, event.GetPoint());
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
wxSizer*
AddressSpacesDialog::createContents( wxWindow *parent, bool call_fit, bool set_sizer )
{
    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxListCtrl *item1 = new wxListCtrl( parent, ID_LIST, wxDefaultPosition, wxSize(400,200), wxLC_REPORT|wxLC_SINGLE_SEL );
    item0->Add( item1, 0, wxALIGN_CENTER|wxALL, 5 );

    wxGridSizer *item2 = new wxGridSizer( 3, 0, 0 );

    wxButton *item3 = new wxButton( parent, ID_ADD, wxT("&Add..."), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item3, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item4 = new wxButton( parent, ID_EDIT, wxT("&Edit..."), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item4, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item5 = new wxButton( parent, ID_DELETE, wxT("&Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item5, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item2, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxStaticLine *item6 = new wxStaticLine( parent, ID_LINE, wxDefaultPosition, wxSize(20,-1), wxLI_HORIZONTAL );
    item0->Add( item6, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxGridSizer *item7 = new wxGridSizer( 2, 0, 0 );

    wxButton *item8 = new wxButton( parent, ID_HELP, wxT("&Help"), wxDefaultPosition, wxDefaultSize, 0 );
    item7->Add( item8, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item9 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item10 = new wxButton( parent, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item9->Add( item10, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item11 = new wxButton( parent, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item9->Add( item11, 0, wxALIGN_CENTER|wxALL, 5 );

    item7->Add( item9, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item0->Add( item7, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }

    return item0;
}
