/*
 Copyright (C) 2022 Tampere University.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 */
/**
 * @file OTAFormatListDialog.cc
 *
 * Implementation of OTAFormatListDialog class.
 *
 * @author Kari Hepola 2022
 * @note rating: red
 */

#include <boost/format.hpp>
#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/listctrl.h>

#include "OTAFormatListDialog.hh"
#include "Machine.hh"
#include "MachineTester.hh"
#include "OperationTriggeredFormat.hh"
#include "Bus.hh"
#include "WxConversion.hh"
#include "OTAOperationDialog.hh"
#include "WidgetTools.hh"
#include "InformationDialog.hh"
#include "WarningDialog.hh"
#include "GUITextGenerator.hh"
#include "ProDeTextGenerator.hh"

using std::string;
using boost::format;
using namespace TTAMachine;

// too long lines to keep doxygen quiet
BEGIN_EVENT_TABLE(OTAFormatListDialog, wxDialog)
    EVT_LIST_ITEM_SELECTED(ID_OTA_FORMAT_LIST, OTAFormatListDialog::onOTAFormatSelection)
    EVT_LIST_ITEM_DESELECTED(ID_OTA_FORMAT_LIST, OTAFormatListDialog::onOTAFormatSelection)
    EVT_LIST_ITEM_SELECTED(ID_OPERATION_LIST, OTAFormatListDialog::onOperationSelection)
    EVT_LIST_ITEM_DESELECTED(ID_OPERATION_LIST, OTAFormatListDialog::onOperationSelection)
    EVT_BUTTON(ID_ADD_OTA_FORMAT, OTAFormatListDialog::onAddOTAFormat)
    EVT_BUTTON(ID_DELETE_OTA_FORMAT, OTAFormatListDialog::onDeleteOTAFormat)
    EVT_BUTTON(ID_ADD_OPERATION, OTAFormatListDialog::onAddOperation)
    EVT_BUTTON(ID_DELETE_OPERATION, OTAFormatListDialog::onDeleteOperation)
    EVT_TEXT(ID_NAME, OTAFormatListDialog::onOTAFormatName)
END_EVENT_TABLE()


/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param machine Parent machine of the instruction OTAFormats.
 */
OTAFormatListDialog::OTAFormatListDialog(
    wxWindow* parent,
    Machine* machine) :
    wxDialog(parent, -1, _T(""), wxDefaultPosition),
    machine_(machine),
    OTAFormatName_(_T("")) {

    createContents(this, true, true);

    OTAFormatList_ = dynamic_cast<wxListCtrl*>(FindWindow(ID_OTA_FORMAT_LIST));
    operationList_ = dynamic_cast<wxListCtrl*>(FindWindow(ID_OPERATION_LIST));

    FindWindow(ID_NAME)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &OTAFormatName_));
    FindWindow(ID_ADD_OTA_FORMAT)->Disable();
    FindWindow(ID_DELETE_OTA_FORMAT)->Disable();
    // set widget texts
    setTexts();
}


/**
 * The Destructor.
 */
OTAFormatListDialog::~OTAFormatListDialog() {
}


/**
 * Sets texts for widgets.
 */
void
OTAFormatListDialog::setTexts() {
    GUITextGenerator* generator = GUITextGenerator::instance();
    ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();

    // Dialog title
    format fmt = prodeTexts->text(
        ProDeTextGenerator::TXT_OTA_FORMATS_DIALOG_TITLE);
    SetTitle(WxConversion::toWxString(fmt.str()));

    // buttons
    WidgetTools::setLabel(generator, FindWindow(wxID_OK),
                          GUITextGenerator::TXT_BUTTON_OK);

    WidgetTools::setLabel(generator, FindWindow(wxID_CANCEL),
                          GUITextGenerator::TXT_BUTTON_CANCEL);

    WidgetTools::setLabel(generator, FindWindow(ID_HELP),
                          GUITextGenerator::TXT_BUTTON_HELP);

    WidgetTools::setLabel(generator, FindWindow(ID_ADD_OTA_FORMAT),
                          GUITextGenerator::TXT_BUTTON_ADD);

    WidgetTools::setLabel(generator, FindWindow(ID_DELETE_OTA_FORMAT),
                          GUITextGenerator::TXT_BUTTON_DELETE);

    WidgetTools::setLabel(generator, FindWindow(ID_ADD_OPERATION),
                          GUITextGenerator::TXT_BUTTON_ADD_DIALOG);

    WidgetTools::setLabel(generator, FindWindow(ID_DELETE_OPERATION),
                          GUITextGenerator::TXT_BUTTON_DELETE);

    // widget labels
    WidgetTools::setLabel(prodeTexts, FindWindow(ID_LABEL_NAME),
                          ProDeTextGenerator::TXT_LABEL_NAME);

    // box sizer label
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_OTA_FORMATS_BOX);
    WidgetTools::setWidgetLabel(OTAFormatSizer_, fmt.str());

    fmt = prodeTexts->text(ProDeTextGenerator::TXT_OTA_FORMATS_OPERATIONS_BOX);
    WidgetTools::setWidgetLabel(operationSizer_, fmt.str());

    // Create instruction OTAFormat list columns.
    wxListCtrl* OTAFormatList =
        dynamic_cast<wxListCtrl*>(FindWindow(ID_OTA_FORMAT_LIST));
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_COLUMN_NAME);
    OTAFormatList->InsertColumn(0, WxConversion::toWxString(fmt.str()),
                               wxLIST_FORMAT_LEFT, 160);

    // Create OTAFormat Operation list columns.
    wxListCtrl* OperationList =
        dynamic_cast<wxListCtrl*>(FindWindow(ID_OPERATION_LIST));
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_COLUMN_OTA_OPERATION);
    OperationList->InsertColumn(0, WxConversion::toWxString(fmt.str()),
                           wxLIST_FORMAT_LEFT, 100);
}


/**
 * Transfers data from the machine to the dialog widgets.
 */
bool
OTAFormatListDialog::TransferDataToWindow() {

    // update OTAFormat list
    OTAFormatList_->DeleteAllItems();
    Machine::OperationTriggeredFormatNavigator navigator =
        machine_->operationTriggeredFormatNavigator();
    for (int i = 0; i < navigator.count(); i++) {
        OTAFormatList_->InsertItem(
            i, WxConversion::toWxString(navigator.item(i)->name()));
    }
    updateOperationList();
    return wxDialog::TransferDataToWindow();
}


/**
 * Updates the OTAFormat Operation list.
 */
void
OTAFormatListDialog::updateOperationList() {


    operationList_->DeleteAllItems();
    operationList_->Enable();

    OperationTriggeredFormat* f = selectedOTAFormat();

    if (f == NULL) {
        FindWindow(ID_ADD_OPERATION)->Disable();
        return;
    }

    for (int i = 0; i < f->operationCount(); i++) {
        operationList_->InsertItem(
            0, WxConversion::toWxString(f->operation(i)));
    }

    FindWindow(ID_ADD_OPERATION)->Enable();

    wxListEvent dummy;
    onOperationSelection(dummy);
}


/**
 * Updates the Operation list when the OTAFormat selection changes.
 */
void
OTAFormatListDialog::onOTAFormatSelection(wxListEvent&) {
    if (OTAFormatList_->GetSelectedItemCount() == 0) {
        FindWindow(ID_DELETE_OTA_FORMAT)->Disable();
    } else {
        FindWindow(ID_DELETE_OTA_FORMAT)->Enable();
    }
    updateOperationList();
}


/**
 * Updates the edit/delete Operation buttons when the Operation selection changes.
 */
void
OTAFormatListDialog::onOperationSelection(wxListEvent&) {
    if (operationList_->GetSelectedItemCount() == 1) {
        FindWindow(ID_DELETE_OPERATION)->Enable();
    } else {
        FindWindow(ID_DELETE_OPERATION)->Disable();
    }
}


/**
 * Deletes the selected OTAFormat.
 */
void
OTAFormatListDialog::onDeleteOTAFormat(wxCommandEvent&) {
    delete selectedOTAFormat();
    TransferDataToWindow();
    wxListEvent dummy;
    onOTAFormatSelection(dummy);
}


/**
 * Returns pointer to the selected OperationTriggeredFormat.
 *
 * @return Pointer to the selected OperationTriggeredFormat.
 */
OperationTriggeredFormat*
OTAFormatListDialog::selectedOTAFormat() {
    long item = -1;
    item = OTAFormatList_->GetNextItem(
        item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

    if (item < 0) {
        return NULL;
    }

    OperationTriggeredFormat* f =
        machine_->operationTriggeredFormatNavigator().item(item);
    return f;
}


/**
 * Returns name of the selected Operation.
 *
 * @return Name of the selected Operation.
 */
string
OTAFormatListDialog::selectedOperation() {
    string name = WidgetTools::lcStringSelection(operationList_, 0);
    return name;
}

bool
OTAFormatListDialog::validFormatName() const {
    if (OTAFormatName_ == "riscv_r_type" ||
        OTAFormatName_ == "riscv_i_type" ||
        OTAFormatName_ == "riscv_s_type" ||
        OTAFormatName_ == "riscv_b_type" ||
        OTAFormatName_ == "riscv_u_type" ||
        OTAFormatName_ == "riscv_j_type") {
        return true;
    }
    return false;
}


/**
 * Handles the Add tempalte button event.
 *
 * Adds a new OTAFormat to the machine.
 */
void
OTAFormatListDialog::onAddOTAFormat(wxCommandEvent&) {
    if (!TransferDataFromWindow()) {
        assert(false);
    }
    string trimmedName =
        WxConversion::toString(OTAFormatName_.Trim(false).Trim(true));

    // Check the name validity.
    if (!validFormatName()) {
        ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();
        std::string message = "Format name is illegal. Legal names are:\n" \
        "riscv_r_type\n" \
        "riscv_i_type\n" \
        "riscv_s_type\n" \
        "riscv_b_type\n" \
        "riscv_u_type\n" \
        "riscv_j_type\n";
        InformationDialog warning(
            this, WxConversion::toWxString(message));
        warning.ShowModal();
        return;
    }

    Machine::OperationTriggeredFormatNavigator navigator =
        machine_->operationTriggeredFormatNavigator();

    if (navigator.hasItem(trimmedName)) {
        ProDeTextGenerator* prodeTexts =
            ProDeTextGenerator::instance();
        format message =
            prodeTexts->text(ProDeTextGenerator::MSG_ERROR_SAME_NAME);
        format a_tmplate =
            prodeTexts->text(ProDeTextGenerator::COMP_AN_OTA_FORMAT);
        format machine =
            prodeTexts->text(ProDeTextGenerator::COMP_MACHINE);
        format tmplate =
            prodeTexts->text(ProDeTextGenerator::COMP_OTA_FORMAT);
        message % trimmedName % a_tmplate.str() % machine.str() %
            tmplate.str();
        WarningDialog warning(this, WxConversion::toWxString(message.str()));
        warning.ShowModal();
        return;
    }
    new OperationTriggeredFormat(trimmedName, *machine_);
    OTAFormatName_ = _T("");
    TransferDataToWindow();
}


/**
 * Enables and disables the Add OTAFormat button when text is entered in the
 * OTAFormat name widget.
 */
void
OTAFormatListDialog::onOTAFormatName(wxCommandEvent&) {
    if (!TransferDataFromWindow()) {
        assert(false);
    }
    wxString trimmedName = OTAFormatName_.Trim(false).Trim(true);
    Machine::OperationTriggeredFormatNavigator nav =
    machine_->operationTriggeredFormatNavigator();
    if (trimmedName == _T("")) {
        FindWindow(ID_ADD_OTA_FORMAT)->Disable();
    //Only 6 formats in RISC-V
    } else if (nav.count() > 5) {
        FindWindow(ID_ADD_OTA_FORMAT)->Disable();
    } else {
        FindWindow(ID_ADD_OTA_FORMAT)->Enable();
    }
}


/**
 * Handles the add Operation button event.
 */
void
OTAFormatListDialog::onAddOperation(wxCommandEvent&) {

    if (selectedOTAFormat() == NULL) {
        assert(false);
    }
    OTAOperationDialog dialog(this, selectedOTAFormat());
    dialog.ShowModal();
    updateOperationList();
}


/**
 * Handles the delete Operation button event.
 */
void
OTAFormatListDialog::onDeleteOperation(wxCommandEvent&) {
    if (selectedOTAFormat() == NULL) {
        assert(false);
    }
    selectedOTAFormat()->removeOperation(selectedOperation());
    updateOperationList();
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
OTAFormatListDialog::createContents(
    wxWindow *parent, bool call_fit, bool set_sizer) {

    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *item1 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticBox *item3 = new wxStaticBox( parent, -1, wxT("Instruction OTAFormats:") );
    wxStaticBoxSizer *item2 = new wxStaticBoxSizer( item3, wxVERTICAL );
    OTAFormatSizer_ = item2;

    wxListCtrl *item4 = new wxListCtrl( parent, ID_OTA_FORMAT_LIST, wxDefaultPosition, wxSize(160,200), wxLC_REPORT|wxLC_SINGLE_SEL|wxSUNKEN_BORDER );
    item2->Add( item4, 0, wxGROW|wxALL, 5 );

    wxBoxSizer *item5 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticText *item6 = new wxStaticText( parent, ID_LABEL_NAME, wxT("Name:"), wxDefaultPosition, wxDefaultSize, 0 );
    item5->Add( item6, 0, wxALIGN_CENTER|wxALL, 5 );

    wxTextCtrl *item7 = new wxTextCtrl( parent, ID_NAME, wxT(""), wxDefaultPosition, wxSize(120,-1), 0 );
    item5->Add( item7, 0, wxALIGN_CENTER|wxALL, 5 );

    item2->Add( item5, 0, wxGROW|wxALL, 5 );

    wxBoxSizer *item8 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item9 = new wxButton( parent, ID_ADD_OTA_FORMAT, wxT("Add"), wxDefaultPosition, wxDefaultSize, 0 );
    item8->Add( item9, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item10 = new wxButton( parent, ID_DELETE_OTA_FORMAT, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    item8->Add( item10, 0, wxALIGN_CENTER|wxALL, 5 );

    item2->Add( item8, 0, wxALIGN_CENTER|wxALL, 5 );

    item1->Add( item2, 0, wxGROW|wxALL, 5 );

    wxStaticBox *item12 = new wxStaticBox( parent, -1, wxT("OTAFormat Operations:") );
    wxStaticBoxSizer *item11 = new wxStaticBoxSizer( item12, wxVERTICAL );
    operationSizer_ = item11;

    wxListCtrl *item13 = new wxListCtrl( parent, ID_OPERATION_LIST, wxDefaultPosition, wxSize(250,245), wxLC_REPORT|wxLC_SINGLE_SEL|wxSUNKEN_BORDER );
    item11->Add( item13, 0, wxGROW|wxALL, 5 );

    wxBoxSizer *item14 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item15 = new wxButton( parent, ID_ADD_OPERATION, wxT("Add..."), wxDefaultPosition, wxDefaultSize, 0 );
    item14->Add( item15, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item17 = new wxButton( parent, ID_DELETE_OPERATION, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    item14->Add( item17, 0, wxALIGN_CENTER|wxALL, 5 );

    item11->Add( item14, 0, wxALIGN_CENTER|wxALL, 5 );

    item1->Add( item11, 0, wxGROW|wxALL, 5 );

    item0->Add( item1, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticLine *item18 = new wxStaticLine( parent, ID_LINE, wxDefaultPosition, wxSize(20,-1), wxLI_HORIZONTAL );
    item0->Add( item18, 0, wxGROW|wxALL, 5 );

    wxGridSizer *item19 = new wxGridSizer( 2, 0, 0 );

    wxButton *item20 = new wxButton( parent, ID_HELP, wxT("&Help"), wxDefaultPosition, wxDefaultSize, 0 );
    item19->Add( item20, 0, wxALL, 5 );

    wxBoxSizer *item21 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item22 = new wxButton( parent, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item21->Add( item22, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item23 = new wxButton( parent, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item21->Add( item23, 0, wxALIGN_CENTER|wxALL, 5 );

    item19->Add( item21, 0, wxALL, 5 );

    item0->Add( item19, 0, wxGROW, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }

    return item0;
}
