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
 * @file TemplateListDialog.cc
 *
 * Implementation of TemplateListDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <boost/format.hpp>
#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/listctrl.h>

#include "TemplateListDialog.hh"
#include "Machine.hh"
#include "MachineTester.hh"
#include "InstructionTemplate.hh"
#include "Bus.hh"
#include "WxConversion.hh"
#include "TemplateSlotDialog.hh"
#include "WidgetTools.hh"
#include "InformationDialog.hh"
#include "WarningDialog.hh"
#include "GUITextGenerator.hh"
#include "ProDeTextGenerator.hh"

using std::string;
using boost::format;
using namespace TTAMachine;

// too long lines to keep doxygen quiet
BEGIN_EVENT_TABLE(TemplateListDialog, wxDialog)
    EVT_LIST_ITEM_SELECTED(ID_TEMPLATE_LIST, TemplateListDialog::onTemplateSelection)
    EVT_LIST_ITEM_DESELECTED(ID_TEMPLATE_LIST, TemplateListDialog::onTemplateSelection)
    EVT_LIST_ITEM_SELECTED(ID_SLOT_LIST, TemplateListDialog::onSlotSelection)
    EVT_LIST_ITEM_DESELECTED(ID_SLOT_LIST, TemplateListDialog::onSlotSelection)
    EVT_BUTTON(ID_ADD_TEMPLATE, TemplateListDialog::onAddTemplate)
    EVT_BUTTON(ID_DELETE_TEMPLATE, TemplateListDialog::onDeleteTemplate)
    EVT_BUTTON(ID_ADD_SLOT, TemplateListDialog::onAddSlot)
    EVT_BUTTON(ID_EDIT_SLOT, TemplateListDialog::onEditSlot)
    EVT_BUTTON(ID_DELETE_SLOT, TemplateListDialog::onDeleteSlot)
    EVT_TEXT(ID_NAME, TemplateListDialog::onTemplateName)
END_EVENT_TABLE()


/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param machine Parent machine of the instruction templates.
 */
TemplateListDialog::TemplateListDialog(
    wxWindow* parent,
    Machine* machine) :
    wxDialog(parent, -1, _T(""), wxDefaultPosition),
    machine_(machine),
    templateName_(_T("")) {

    createContents(this, true, true);

    templateList_ = dynamic_cast<wxListCtrl*>(FindWindow(ID_TEMPLATE_LIST));
    slotList_ = dynamic_cast<wxListCtrl*>(FindWindow(ID_SLOT_LIST));

    FindWindow(ID_NAME)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &templateName_));
    FindWindow(ID_ADD_TEMPLATE)->Disable();
    FindWindow(ID_DELETE_TEMPLATE)->Disable();
    FindWindow(ID_EDIT_SLOT)->Disable();

    // set widget texts
    setTexts();
}


/**
 * The Destructor.
 */
TemplateListDialog::~TemplateListDialog() {
}


/**
 * Sets texts for widgets.
 */
void
TemplateListDialog::setTexts() {
    GUITextGenerator* generator = GUITextGenerator::instance();
    ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();

    // Dialog title
    format fmt = prodeTexts->text(
        ProDeTextGenerator::TXT_TEMPLATES_DIALOG_TITLE);
    SetTitle(WxConversion::toWxString(fmt.str()));

    // buttons
    WidgetTools::setLabel(generator, FindWindow(wxID_OK),
                          GUITextGenerator::TXT_BUTTON_OK);

    WidgetTools::setLabel(generator, FindWindow(wxID_CANCEL),
                          GUITextGenerator::TXT_BUTTON_CANCEL);

    WidgetTools::setLabel(generator, FindWindow(ID_HELP),
                          GUITextGenerator::TXT_BUTTON_HELP);

    WidgetTools::setLabel(generator, FindWindow(ID_ADD_TEMPLATE),
                          GUITextGenerator::TXT_BUTTON_ADD);

    WidgetTools::setLabel(generator, FindWindow(ID_DELETE_TEMPLATE),
                          GUITextGenerator::TXT_BUTTON_DELETE);

    WidgetTools::setLabel(generator, FindWindow(ID_ADD_SLOT),
                          GUITextGenerator::TXT_BUTTON_ADD_DIALOG);

    WidgetTools::setLabel(generator, FindWindow(ID_EDIT_SLOT),
                          GUITextGenerator::TXT_BUTTON_EDIT_DIALOG);

    WidgetTools::setLabel(generator, FindWindow(ID_DELETE_SLOT),
                          GUITextGenerator::TXT_BUTTON_DELETE);

    // widget labels
    WidgetTools::setLabel(prodeTexts, FindWindow(ID_LABEL_NAME),
                          ProDeTextGenerator::TXT_LABEL_NAME);

    // box sizer label
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_TEMPLATES_TEMPLATES_BOX);
    WidgetTools::setWidgetLabel(templateSizer_, fmt.str());

    fmt = prodeTexts->text(ProDeTextGenerator::TXT_TEMPLATES_SLOTS_BOX);
    WidgetTools::setWidgetLabel(slotSizer_, fmt.str());

    // Create instruction template list columns.
    wxListCtrl* templateList =
        dynamic_cast<wxListCtrl*>(FindWindow(ID_TEMPLATE_LIST));
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_COLUMN_NAME);
    templateList->InsertColumn(0, WxConversion::toWxString(fmt.str()),
                               wxLIST_FORMAT_LEFT, 160);

    // Create template slot list columns.
    wxListCtrl* slotList =
        dynamic_cast<wxListCtrl*>(FindWindow(ID_SLOT_LIST));
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_COLUMN_SLOT);
    slotList->InsertColumn(0, WxConversion::toWxString(fmt.str()),
                           wxLIST_FORMAT_LEFT, 100);
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_COLUMN_DESTINATION);
    slotList->InsertColumn(1, WxConversion::toWxString(fmt.str()),
                           wxLIST_FORMAT_LEFT, 100);
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_COLUMN_WIDTH);
    slotList->InsertColumn(2, WxConversion::toWxString(fmt.str()),
                           wxLIST_FORMAT_LEFT, 50);

}


/**
 * Transfers data from the machine to the dialog widgets.
 */
bool
TemplateListDialog::TransferDataToWindow() {

    // update template list
    templateList_->DeleteAllItems();
    Machine::InstructionTemplateNavigator navigator =
        machine_->instructionTemplateNavigator();
    for (int i = 0; i < navigator.count(); i++) {
        templateList_->InsertItem(
            i, WxConversion::toWxString(navigator.item(i)->name()));
    }
    updateSlotList();
    return wxDialog::TransferDataToWindow();
}


/**
 * Updates the template slot list.
 */
void
TemplateListDialog::updateSlotList() {


    slotList_->DeleteAllItems();
    slotList_->Enable();

    InstructionTemplate* templ = selectedTemplate();

    if (templ == NULL) {
        FindWindow(ID_ADD_SLOT)->Disable();
        return;
    }

    FindWindow(ID_ADD_SLOT)->Enable();

    // Add bus slots.
    Machine::BusNavigator busNavigator = machine_->busNavigator();
    for (int i = 0; i < busNavigator.count(); i++) {
        if (templ->usesSlot(busNavigator.item(i)->name())) {

            Bus* slot = busNavigator.item(i);
            // Add slot name.
            slotList_->InsertItem(
                0, WxConversion::toWxString(slot->name()));

            // Add slot Destination.

            // Set slot destination.
            wxString name = WxConversion::toWxString(
                templ->destinationOfSlot(slot->name())->name());
            slotList_->SetItem(0, 1, name);

            // Set slot width.
            wxString width = WxConversion::toWxString(
                templ->supportedWidth(slot->name()));
            slotList_->SetItem(0, 2, width);
        }
    }

    // Add immediate slots.
    Machine::ImmediateSlotNavigator immsNavigator =
        machine_->immediateSlotNavigator();
    for (int i = 0; i < immsNavigator.count(); i++) {
        if (templ->usesSlot(immsNavigator.item(i)->name())) {

            ImmediateSlot* slot = immsNavigator.item(i);

            // Add slot name.
            slotList_->InsertItem(
                0, WxConversion::toWxString(slot->name()));

            // Set slot destination.
            wxString name = WxConversion::toWxString(
                templ->destinationOfSlot(slot->name())->name());
            slotList_->SetItem(0, 1, name);

            // Set slot width.
            wxString width = WxConversion::toWxString(
                templ->supportedWidth(slot->name()));
            slotList_->SetItem(0, 2, width);
        }
    }
    wxListEvent dummy;
    onSlotSelection(dummy);
}


/**
 * Updates the slot list when the template selection changes.
 */
void
TemplateListDialog::onTemplateSelection(wxListEvent&) {
    if (templateList_->GetSelectedItemCount() == 0) {
        FindWindow(ID_DELETE_TEMPLATE)->Disable();
    } else {
        FindWindow(ID_DELETE_TEMPLATE)->Enable();
    }
    updateSlotList();
}


/**
 * Updates the edit/delete slot buttons when the slot selection changes.
 */
void
TemplateListDialog::onSlotSelection(wxListEvent&) {
    if (slotList_->GetSelectedItemCount() == 1) {
        FindWindow(ID_DELETE_SLOT)->Enable();
        FindWindow(ID_EDIT_SLOT)->Enable();
    } else {
        FindWindow(ID_DELETE_SLOT)->Disable();
        FindWindow(ID_EDIT_SLOT)->Disable();
    }
}


/**
 * Deletes the selected template.
 */
void
TemplateListDialog::onDeleteTemplate(wxCommandEvent&) {
    delete selectedTemplate();
    TransferDataToWindow();
    wxListEvent dummy;
    onTemplateSelection(dummy);
}


/**
 * Returns pointer to the selected InstructionTemplate.
 *
 * @return Pointer to the selected InstructionTemplate.
 */
InstructionTemplate*
TemplateListDialog::selectedTemplate() {
    long item = -1;
    item = templateList_->GetNextItem(
        item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

    if (item < 0) {
        return NULL;
    }

    InstructionTemplate* temp =
        machine_->instructionTemplateNavigator().item(item);
    return temp;
}


/**
 * Returns name of the selected slot.
 *
 * @return Name of the selected slot.
 */
string
TemplateListDialog::selectedSlot() {
    string name = WidgetTools::lcStringSelection(slotList_, 0);
    return name;
}


/**
 * Handles the Add tempalte button event.
 *
 * Adds a new template to the machine.
 */
void
TemplateListDialog::onAddTemplate(wxCommandEvent&) {
    if (!TransferDataFromWindow()) {
        assert(false);
    }
    string trimmedName =
        WxConversion::toString(templateName_.Trim(false).Trim(true));

    // Check the name validity.
    if (!MachineTester::isValidComponentName(trimmedName)) {
        ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();
        format message =
            prodeTexts->text(ProDeTextGenerator::MSG_ERROR_ILLEGAL_NAME);
        InformationDialog warning(
            this, WxConversion::toWxString(message.str()));
        warning.ShowModal();
        return;
    }

    Machine::InstructionTemplateNavigator navigator =
        machine_->instructionTemplateNavigator();

    if (navigator.hasItem(trimmedName)) {
        ProDeTextGenerator* prodeTexts =
            ProDeTextGenerator::instance();
        format message =
            prodeTexts->text(ProDeTextGenerator::MSG_ERROR_SAME_NAME);
        format a_tmplate =
            prodeTexts->text(ProDeTextGenerator::COMP_A_TEMPLATE);
        format machine =
            prodeTexts->text(ProDeTextGenerator::COMP_MACHINE);
        format tmplate =
            prodeTexts->text(ProDeTextGenerator::COMP_TEMPLATE);
        message % trimmedName % a_tmplate.str() % machine.str() %
            tmplate.str();
        WarningDialog warning(this, WxConversion::toWxString(message.str()));
        warning.ShowModal();
        return;
    }
    new InstructionTemplate(trimmedName, *machine_);
    templateName_ = _T("");
    TransferDataToWindow();
}


/**
 * Enables and disables the Add template button when text is entered in the
 * template name widget.
 */
void
TemplateListDialog::onTemplateName(wxCommandEvent&) {
    if (!TransferDataFromWindow()) {
        assert(false);
    }
    wxString trimmedName = templateName_.Trim(false).Trim(true);
    if (trimmedName == _T("")) {
        FindWindow(ID_ADD_TEMPLATE)->Disable();
    } else {
        FindWindow(ID_ADD_TEMPLATE)->Enable();
    }
}


/**
 * Handles the add slot button event.
 */
void
TemplateListDialog::onAddSlot(wxCommandEvent&) {

    if (selectedTemplate() == NULL) {
        assert(false);
    }

    // Check that there is at least one immediate unit in the machine.
    const Machine::ImmediateUnitNavigator iuNavigator =
        machine_->immediateUnitNavigator();

    if (iuNavigator.count() == 0) {
        ProDeTextGenerator* generator = ProDeTextGenerator::instance();
        wxString message = WxConversion::toWxString(generator->text(
            ProDeTextGenerator::MSG_ERROR_NO_IMMEDIATE_UNITS).str());

        InformationDialog dialog(this, message);
        dialog.ShowModal();
        return;
    }

    // check that the template doesn't have all possible slots already
    bool available = false;

    // check buses
    const Machine::BusNavigator busNavigator = machine_->busNavigator();
    for (int i = 0; i < busNavigator.count(); i++) {
        if (!selectedTemplate()->usesSlot(busNavigator.item(i)->name())) {
            available = true;
            break;
        }
    }

    // check immediate slots
    const Machine::ImmediateSlotNavigator immsNavigator =
        machine_->immediateSlotNavigator();
    for (int i = 0; i < immsNavigator.count(); i++) {
        if (!selectedTemplate()->usesSlot(immsNavigator.item(i)->name())) {
            available = true;
            break;
        }
    }

    if (available == false) {
        ProDeTextGenerator* generator = ProDeTextGenerator::instance();
        wxString message = WxConversion::toWxString(generator->text(
            ProDeTextGenerator::MSG_ERROR_NO_SLOTS_AVAILABLE).str());
        InformationDialog dialog(this, message);
        dialog.ShowModal();
        return;
    }

    TemplateSlotDialog dialog(this, selectedTemplate());
    dialog.ShowModal();
    updateSlotList();
}


/**
 * Handles the delete slot button event.
 */
void
TemplateListDialog::onDeleteSlot(wxCommandEvent&) {
    selectedTemplate()->removeSlot(selectedSlot());
    updateSlotList();
}


/**
 * Handles the edit slot button event.
 */
void
TemplateListDialog::onEditSlot(wxCommandEvent&) {
    TemplateSlotDialog dialog(this, selectedTemplate(),
        selectedTemplate()->templateSlot(selectedSlot()) );
    dialog.ShowModal();
    updateSlotList();
}


/**
 * Creates the dialog window contents.
 *
 * This method was generated with wxDesigner.
 *
 * @return Main sizer of the created contents.
 * @param parent The dialog window.
 * @param call_fit If true, fits the contents inside the dialog.
 * @param set_sizer If true, sets the main sizer as dialog contents.
 */
wxSizer*
TemplateListDialog::createContents(
    wxWindow *parent, bool call_fit, bool set_sizer) {

    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *item1 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticBox *item3 = new wxStaticBox( parent, -1, wxT("Instruction Templates:") );
    wxStaticBoxSizer *item2 = new wxStaticBoxSizer( item3, wxVERTICAL );
    templateSizer_ = item2;

    wxListCtrl *item4 = new wxListCtrl( parent, ID_TEMPLATE_LIST, wxDefaultPosition, wxSize(160,200), wxLC_REPORT|wxLC_SINGLE_SEL|wxSUNKEN_BORDER );
    item2->Add( item4, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item5 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticText *item6 = new wxStaticText( parent, ID_LABEL_NAME, wxT("Name:"), wxDefaultPosition, wxDefaultSize, 0 );
    item5->Add( item6, 0, wxALIGN_CENTER|wxALL, 5 );

    wxTextCtrl *item7 = new wxTextCtrl( parent, ID_NAME, wxT(""), wxDefaultPosition, wxSize(120,-1), 0 );
    item5->Add( item7, 0, wxALIGN_CENTER|wxALL, 5 );

    item2->Add( item5, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item8 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item9 = new wxButton( parent, ID_ADD_TEMPLATE, wxT("Add"), wxDefaultPosition, wxDefaultSize, 0 );
    item8->Add( item9, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item10 = new wxButton( parent, ID_DELETE_TEMPLATE, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    item8->Add( item10, 0, wxALIGN_CENTER|wxALL, 5 );

    item2->Add( item8, 0, wxALIGN_CENTER|wxALL, 5 );

    item1->Add( item2, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    wxStaticBox *item12 = new wxStaticBox( parent, -1, wxT("Template Slots:") );
    wxStaticBoxSizer *item11 = new wxStaticBoxSizer( item12, wxVERTICAL );
    slotSizer_ = item11;

    wxListCtrl *item13 = new wxListCtrl( parent, ID_SLOT_LIST, wxDefaultPosition, wxSize(250,245), wxLC_REPORT|wxLC_SINGLE_SEL|wxSUNKEN_BORDER );
    item11->Add( item13, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item14 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item15 = new wxButton( parent, ID_ADD_SLOT, wxT("Add..."), wxDefaultPosition, wxDefaultSize, 0 );
    item14->Add( item15, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item16 = new wxButton( parent, ID_EDIT_SLOT, wxT("Edit..."), wxDefaultPosition, wxDefaultSize, 0 );
    item14->Add( item16, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item17 = new wxButton( parent, ID_DELETE_SLOT, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    item14->Add( item17, 0, wxALIGN_CENTER|wxALL, 5 );

    item11->Add( item14, 0, wxALIGN_CENTER|wxALL, 5 );

    item1->Add( item11, 0, wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    item0->Add( item1, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticLine *item18 = new wxStaticLine( parent, ID_LINE, wxDefaultPosition, wxSize(20,-1), wxLI_HORIZONTAL );
    item0->Add( item18, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxGridSizer *item19 = new wxGridSizer( 2, 0, 0 );

    wxButton *item20 = new wxButton( parent, ID_HELP, wxT("&Help"), wxDefaultPosition, wxDefaultSize, 0 );
    item19->Add( item20, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item21 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item22 = new wxButton( parent, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item21->Add( item22, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item23 = new wxButton( parent, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item21->Add( item23, 0, wxALIGN_CENTER|wxALL, 5 );

    item19->Add( item21, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item0->Add( item19, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }

    return item0;
}
