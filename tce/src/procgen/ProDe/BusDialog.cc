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
 * @file BusDialog.cc
 *
 * Definition of BusDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/radiobox.h>
#include <wx/spinctrl.h>
#include <boost/format.hpp>

#include "BusDialog.hh"
#include "Bus.hh"
#include "Segment.hh"
#include "Guard.hh"
#include "Conversion.hh"
#include "WxConversion.hh"
#include "WarningDialog.hh"
#include "ErrorDialog.hh"
#include "InformationDialog.hh"
#include "Machine.hh"
#include "MachineTester.hh"
#include "UserManualCmd.hh"
#include "ProDeConstants.hh"
#include "FUPort.hh"
#include "RFGuardDialog.hh"
#include "FUGuardDialog.hh"
#include "ModelConstants.hh"
#include "WidgetTools.hh"
#include "GUITextGenerator.hh"
#include "ProDeTextGenerator.hh"

using boost::format;
using std::string;
using namespace TTAMachine;

BEGIN_EVENT_TABLE(BusDialog, wxDialog)
    EVT_TEXT(ID_SEGMENT_NAME, BusDialog::onSegmentName)
    EVT_TEXT(ID_BUS_NAME, BusDialog::onBusName)
    EVT_BUTTON(ID_ADD_SEGMENT, BusDialog::onAddSegment)
    EVT_BUTTON(ID_DELETE_SEGMENT, BusDialog::onDeleteSegment)
    EVT_BUTTON(ID_SEGMENT_UP, BusDialog::onSegmentUp)
    EVT_BUTTON(ID_SEGMENT_DOWN, BusDialog::onSegmentDown)
    EVT_MENU(ID_DELETE_SEGMENT, BusDialog::onDeleteSegment)
    EVT_LIST_ITEM_RIGHT_CLICK(ID_SEGMENT_LIST, BusDialog::onSegmentRightClick)

    EVT_BUTTON(wxID_OK, BusDialog::onOK)

    EVT_SPINCTRL(ID_BUS_WIDTH, BusDialog::onWidth)

    EVT_BUTTON(ID_ADD_RF_GUARD, BusDialog::onAddRFGuard)
    EVT_BUTTON(ID_DELETE_RF_GUARD, BusDialog::onDeleteRFGuard)
    EVT_BUTTON(ID_EDIT_RF_GUARD, BusDialog::onEditRFGuard)

    EVT_BUTTON(ID_ADD_FU_GUARD, BusDialog::onAddFUGuard)
    EVT_BUTTON(ID_DELETE_FU_GUARD, BusDialog::onDeleteFUGuard)
    EVT_BUTTON(ID_EDIT_FU_GUARD, BusDialog::onEditFUGuard)

    EVT_CHECKBOX(ID_FALSE_GUARD, BusDialog::onUnconditionalGuard)
    EVT_CHECKBOX(ID_TRUE_GUARD, BusDialog::onUnconditionalGuard)

    EVT_LIST_ITEM_FOCUSED(ID_SEGMENT_LIST, BusDialog::onSegmentSelection)
    EVT_LIST_DELETE_ITEM(ID_SEGMENT_LIST, BusDialog::onSegmentSelection)
    EVT_LIST_ITEM_SELECTED(ID_SEGMENT_LIST, BusDialog::onSegmentSelection)
    EVT_LIST_ITEM_DESELECTED(ID_SEGMENT_LIST, BusDialog::onSegmentSelection)

    EVT_LIST_ITEM_FOCUSED(ID_RF_GUARD_LIST, BusDialog::onRFGuardSelection)
    EVT_LIST_DELETE_ITEM(ID_RF_GUARD_LIST, BusDialog::onRFGuardSelection)
    EVT_LIST_ITEM_SELECTED(ID_RF_GUARD_LIST, BusDialog::onRFGuardSelection)
    EVT_LIST_ITEM_DESELECTED(ID_RF_GUARD_LIST, BusDialog::onRFGuardSelection)

    EVT_LIST_ITEM_FOCUSED(ID_FU_GUARD_LIST, BusDialog::onFUGuardSelection)
    EVT_LIST_DELETE_ITEM(ID_FU_GUARD_LIST, BusDialog::onFUGuardSelection)
    EVT_LIST_ITEM_SELECTED(ID_FU_GUARD_LIST, BusDialog::onFUGuardSelection)
    EVT_LIST_ITEM_DESELECTED(ID_FU_GUARD_LIST, BusDialog::onFUGuardSelection)

    EVT_LIST_ITEM_ACTIVATED(ID_RF_GUARD_LIST, BusDialog::onActivateRFGuard)
    EVT_LIST_ITEM_RIGHT_CLICK(ID_RF_GUARD_LIST, BusDialog::onRFGuardRightClick)
    EVT_LIST_ITEM_ACTIVATED(ID_FU_GUARD_LIST, BusDialog::onActivateFUGuard)
    EVT_LIST_ITEM_RIGHT_CLICK(ID_FU_GUARD_LIST, BusDialog::onFUGuardRightClick)

    EVT_MENU(ID_DELETE_RF_GUARD, BusDialog::onDeleteRFGuard)
    EVT_MENU(ID_EDIT_RF_GUARD, BusDialog::onEditRFGuard)
    EVT_MENU(ID_DELETE_FU_GUARD, BusDialog::onDeleteFUGuard)
    EVT_MENU(ID_EDIT_FU_GUARD, BusDialog::onEditFUGuard)
END_EVENT_TABLE()


/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param bus Transport bus to be modified with the dialog.
 */
BusDialog::BusDialog(
    wxWindow* parent,
    Bus* bus):
    wxDialog(parent, -1, _T(""), wxDefaultPosition, wxSize(300, 300)),
    bus_(bus),
    name_(_T("")),
    width_(ModelConstants::DEFAULT_WIDTH),
    immWidth_(ModelConstants::DEFAULT_WIDTH),
    newSegName_(_T("")),
    alwaysTrueGuard_(NULL),
    alwaysFalseGuard_(NULL),
    immediateExtensionBox_(NULL),
    rfGuardList_(NULL),
    fuGuardList_(NULL) {

    createContents(this, true, true);

    FindWindow(wxID_OK)->Disable();
    FindWindow(ID_ADD_SEGMENT)->Disable();
    FindWindow(ID_DELETE_SEGMENT)->Disable();
    FindWindow(ID_SEGMENT_UP)->Disable();
    FindWindow(ID_SEGMENT_DOWN)->Disable();
    FindWindow(ID_EDIT_RF_GUARD)->Disable();
    FindWindow(ID_DELETE_RF_GUARD)->Disable();
    FindWindow(ID_EDIT_FU_GUARD)->Disable();
    FindWindow(ID_DELETE_FU_GUARD)->Disable();

    rfGuardList_ = dynamic_cast<wxListCtrl*>(FindWindow(ID_RF_GUARD_LIST));
    fuGuardList_ = dynamic_cast<wxListCtrl*>(FindWindow(ID_FU_GUARD_LIST));
    segList_ = dynamic_cast<wxListCtrl*>(FindWindow(ID_SEGMENT_LIST));

    immediateExtensionBox_ =
        dynamic_cast<wxRadioBox*>(FindWindow(ID_SI_EXTENSION));
    alwaysTrueGuard_ =
        dynamic_cast<wxCheckBox*>(FindWindow(ID_TRUE_GUARD));
    alwaysFalseGuard_ =
        dynamic_cast<wxCheckBox*>(FindWindow(ID_FALSE_GUARD));

    FindWindow(ID_BUS_NAME)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &name_));
    FindWindow(ID_SEGMENT_NAME)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &newSegName_));
    FindWindow(ID_BUS_WIDTH)->SetValidator(wxGenericValidator(&width_));
    FindWindow(ID_SI_WIDTH)->SetValidator(wxGenericValidator(&immWidth_));

    // set widget texts
    setTexts();

    TransferDataToWindow();

    FindWindow(ID_BUS_NAME)->SetFocus();

    // Editing segments is unsupported feature => hide segment ui
    segmentSizer_->Show(false);
    GetSizer()->SetSizeHints(this);
}



/**
 * The Destructor.
 */
BusDialog::~BusDialog() {
}


/**
 *
 */
void
BusDialog::setTexts() {
    GUITextGenerator* generator = GUITextGenerator::instance();
    ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();

    // Dialog title
    format fmt = prodeTexts->text(ProDeTextGenerator::TXT_BUS_DIALOG_TITLE);

    string title = fmt.str();
    SetTitle(WxConversion::toWxString(title));

    // buttons
    WidgetTools::setLabel(generator, FindWindow(wxID_OK),
                          GUITextGenerator::TXT_BUTTON_OK);

    WidgetTools::setLabel(generator, FindWindow(wxID_CANCEL),
                          GUITextGenerator::TXT_BUTTON_CANCEL);

    WidgetTools::setLabel(generator, FindWindow(ID_HELP),
                          GUITextGenerator::TXT_BUTTON_HELP);

    WidgetTools::setLabel(generator, FindWindow(ID_ADD_SEGMENT),
                          GUITextGenerator::TXT_BUTTON_ADD);

    WidgetTools::setLabel(generator, FindWindow(ID_DELETE_SEGMENT),
                          GUITextGenerator::TXT_BUTTON_DELETE);

    WidgetTools::setLabel(prodeTexts, FindWindow(ID_SEGMENT_UP),
                          ProDeTextGenerator::TXT_BUTTON_UP);

    WidgetTools::setLabel(prodeTexts, FindWindow(ID_SEGMENT_DOWN),
                          ProDeTextGenerator::TXT_BUTTON_DOWN);

    WidgetTools::setLabel(generator, FindWindow(ID_ADD_RF_GUARD),
                          GUITextGenerator::TXT_BUTTON_ADD_DIALOG);

    WidgetTools::setLabel(generator, FindWindow(ID_EDIT_RF_GUARD),
                          GUITextGenerator::TXT_BUTTON_EDIT_DIALOG);

    WidgetTools::setLabel(generator, FindWindow(ID_DELETE_RF_GUARD),
                          GUITextGenerator::TXT_BUTTON_DELETE);

    WidgetTools::setLabel(generator, FindWindow(ID_ADD_FU_GUARD),
                          GUITextGenerator::TXT_BUTTON_ADD_DIALOG);

    WidgetTools::setLabel(generator, FindWindow(ID_EDIT_FU_GUARD),
                          GUITextGenerator::TXT_BUTTON_EDIT_DIALOG);

    WidgetTools::setLabel(generator, FindWindow(ID_DELETE_FU_GUARD),
                          GUITextGenerator::TXT_BUTTON_DELETE);

    // widget labels
    WidgetTools::setLabel(prodeTexts, FindWindow(ID_LABEL_BUS_NAME),
                          ProDeTextGenerator::TXT_LABEL_NAME);

    WidgetTools::setLabel(prodeTexts, FindWindow(ID_LABEL_BUS_WIDTH),
                          ProDeTextGenerator::TXT_LABEL_WIDTH);

    WidgetTools::setLabel(prodeTexts, FindWindow(ID_LABEL_SEGMENT_NAME),
                          ProDeTextGenerator::TXT_LABEL_NAME);

    WidgetTools::setLabel(prodeTexts, FindWindow(ID_LABEL_SI_WIDTH),
                          ProDeTextGenerator::TXT_LABEL_WIDTH);

    WidgetTools::setLabel(prodeTexts, FindWindow(ID_SI_EXTENSION),
                          ProDeTextGenerator::TXT_LABEL_EXTENSION);

    WidgetTools::setLabel(prodeTexts, FindWindow(ID_TRUE_GUARD),
                          ProDeTextGenerator::TXT_LABEL_TRUE_GUARD);

    WidgetTools::setLabel(prodeTexts, FindWindow(ID_FALSE_GUARD),
                          ProDeTextGenerator::TXT_LABEL_FALSE_GUARD);

    // Radio button labels
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_RADIO_EXTENSION_ZERO);
    immediateExtensionBox_->SetString(0, WxConversion::toWxString(fmt.str()));
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_RADIO_EXTENSION_SIGN);
    immediateExtensionBox_->SetString(1, WxConversion::toWxString(fmt.str()));

    // box sizer labels
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_BUS_BUS_BOX);
    string label = fmt.str();
    WidgetTools::setWidgetLabel(busSizer_, label);

    fmt = prodeTexts->text(ProDeTextGenerator::TXT_BUS_SEGMENTS_BOX);
    label = fmt.str();
    WidgetTools::setWidgetLabel(segmentSizer_, label);

    fmt = prodeTexts->text(ProDeTextGenerator::TXT_BUS_SI_BOX);
    label = fmt.str();
    WidgetTools::setWidgetLabel(siSizer_, label);

    fmt = prodeTexts->
        text(ProDeTextGenerator::TXT_BUS_REGISTER_GUARD_BOX);
    label = fmt.str();
    WidgetTools::setWidgetLabel(registerGuardSizer_, label);

    fmt = prodeTexts->text(ProDeTextGenerator::TXT_BUS_PORT_GUARD_BOX);
    label = fmt.str();
    WidgetTools::setWidgetLabel(portGuardSizer_, label);

    // Create segment list columns.
    wxListCtrl* segmentList =
        dynamic_cast<wxListCtrl*>(FindWindow(ID_SEGMENT_LIST));
    wxListCtrl* rfGuardList =
        dynamic_cast<wxListCtrl*>(FindWindow(ID_RF_GUARD_LIST));
    wxListCtrl* fuGuardList =
        dynamic_cast<wxListCtrl*>(FindWindow(ID_FU_GUARD_LIST));

    fmt = prodeTexts->text(ProDeTextGenerator::TXT_COLUMN_INVERTED);
    string invLabel = fmt.str();
    
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_COLUMN_NAME);
    string nameLabel = fmt.str();
    
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_COLUMN_INDEX);
    string indexLabel = fmt.str();
        
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_COLUMN_PORT);
    string portLabel = fmt.str();

    segmentList->InsertColumn(0, WxConversion::toWxString(nameLabel),
                              wxLIST_FORMAT_LEFT, 280);
    
    rfGuardList->InsertColumn(0, WxConversion::toWxString(nameLabel),
                              wxLIST_FORMAT_LEFT, 170);
    rfGuardList->InsertColumn(1, WxConversion::toWxString(indexLabel),
                              wxLIST_FORMAT_LEFT, 70);
    rfGuardList->InsertColumn(2, WxConversion::toWxString(invLabel),
                              wxLIST_FORMAT_LEFT, 40);
    
    fuGuardList->InsertColumn(0, WxConversion::toWxString(invLabel),
                              wxLIST_FORMAT_LEFT, 40);
    fuGuardList->InsertColumn(1, WxConversion::toWxString(nameLabel),
                              wxLIST_FORMAT_LEFT, 170);
    fuGuardList->InsertColumn(2, WxConversion::toWxString(portLabel),
                              wxLIST_FORMAT_LEFT, 70);
}


/**
 * Transfers the data from the bus model to the dialog controls.
 *
 * @return false, if an error occured in the transfer.
 */
bool
BusDialog::TransferDataToWindow() {

    name_ = WxConversion::toWxString(bus_->name());
    width_ = bus_->width();
    immWidth_ = bus_->immediateWidth();


    // set extension
    if (bus_->signExtends()) {
        immediateExtensionBox_->SetStringSelection(
            ProDeConstants::EXTENSION_SIGN);
    } else {
        immediateExtensionBox_->SetStringSelection(
            ProDeConstants::EXTENSION_ZERO);
    }

    updateGuardLists();
    updateSegmentList();

    // wxWidgets GTK1 version seems to bug with spincontrol validators.
    // The widget value has to be set manually.
    dynamic_cast<wxSpinCtrl*>(FindWindow(ID_BUS_WIDTH))->SetValue(width_);
    dynamic_cast<wxSpinCtrl*>(FindWindow(ID_SI_WIDTH))->SetValue(immWidth_);

    wxSpinEvent dummy;
    onWidth(dummy);

    return wxWindow::TransferDataToWindow();
}

/**
 * Defines how SortItems() does comparison between two items to sort the list.
 *
 * Order: names in ascending order. Within same name, index numbers in 
 * ascending order. Within same name and index number, non-inverted guard 
 * comes before inverted.
 */
int wxCALLBACK 
ListCompareFunction(long item1, long item2, long WXUNUSED(sortData))
{
    // items are set with SetItemData to contain pointers to the rf guards
    RegisterGuard* rfGuard1 = (RegisterGuard*) item1;
    RegisterGuard* rfGuard2 = (RegisterGuard*) item2;
    assert (rfGuard1 != NULL);
    assert (rfGuard2 != NULL);

    string name1 = rfGuard1->registerFile()->name();
    int index1 = rfGuard1->registerIndex();
    bool inverted1 = rfGuard1->isInverted();

    string name2 = rfGuard2->registerFile()->name();
    int index2 = rfGuard2->registerIndex();
    bool inverted2 = rfGuard2->isInverted();
    
    if (name1 < name2) {
        return -1;
    } else if (name1 > name2) {
        return 1;
    } else if (index1 < index2) {
        return -1;
    } else if (index1 > index2) {
        return 1;
    } else if (!inverted1 && inverted2) {
        return -1;
    } else if (inverted1 && !inverted2) {
        return 1;
    }
    
    return 0;
}

/**
 * Updates the guard lists.
 */
void
BusDialog::updateGuardLists() {

    assert (rfGuardList_ != NULL);
    assert (fuGuardList_ != NULL);
    rfGuardList_->DeleteAllItems();
    fuGuardList_->DeleteAllItems();

    registerGuards_.clear();
    portGuards_.clear();

    alwaysTrueGuard_->SetValue(false);
    alwaysFalseGuard_->SetValue(false);

    // add guards
    for (int i = 0; i < bus_->guardCount(); i++) {

        Guard* guard = bus_->guard(i);

        UnconditionalGuard* unCondGuard =
            dynamic_cast<UnconditionalGuard*>(guard);

        RegisterGuard* rfGuard = dynamic_cast<RegisterGuard*>(guard);
        PortGuard* fuGuard = dynamic_cast<PortGuard*>(guard);

        if (unCondGuard != NULL) {
            // unconditional guard
            if (unCondGuard->isInverted()) {
                alwaysFalseGuard_->SetValue(true);
            } else {
                alwaysTrueGuard_->SetValue(true);
            }
        } else if(rfGuard != NULL) {
            // register guard
            int index = registerGuards_.size();
            registerGuards_.push_back(rfGuard);

            string name = rfGuard->registerFile()->name();
            int rfIndex = rfGuard->registerIndex();
            rfGuardList_->InsertItem(index, WxConversion::toWxString(name));
            rfGuardList_->SetItem(index, 1, WxConversion::toWxString(rfIndex));
            if (rfGuard->isInverted()) {
                rfGuardList_->SetItem(index, 2, _T("*"));
            } else {
                rfGuardList_->SetItem(index, 2, _T(" "));
            }

            // bind pointer to the guard as item data for future reference
            rfGuardList_->SetItemData(index, (long)rfGuard);
        } else if(fuGuard != NULL) {
            // port guard
            int index = portGuards_.size();
            portGuards_.push_back(fuGuard);

            if (fuGuard->isInverted()) {
                fuGuardList_->InsertItem(index, _T("*"));
            } else {
                fuGuardList_->InsertItem(index, _T(" "));
            }

            string name = fuGuard->port()->parentUnit()->name();
            fuGuardList_->SetItem(index, 1, WxConversion::toWxString(name));
            name = fuGuard->port()->name();
            fuGuardList_->SetItem(index, 2, WxConversion::toWxString(name));
        }
    }

    // sort guard list with given function, data parameter is not needed
    rfGuardList_->SortItems(ListCompareFunction, static_cast<long>(0));
}

/**
 * Updates the segment list control.
 */
void
BusDialog::updateSegmentList() {
    segList_->DeleteAllItems();

    for (int i = bus_->segmentCount()-1;i > -1;i--) {
        string segmentName = bus_->segment(i)->name();
        segList_->InsertItem(0, WxConversion::toWxString(segmentName));
    }
}

/**
 * Returns pointer to the selected segment.
 *
 * Returns NULL if no segment is selected.
 */
Segment*
BusDialog::selectedSegment() const {

    // search the selected segment in the segment list widget
    Segment* selected = NULL;
    for (int i = 0; i < segList_->GetItemCount(); i++) {
        // search selected list item
        if (segList_->GetItemState(i, wxLIST_STATE_SELECTED) != 0) {
            string name = WxConversion::toString(segList_->GetItemText(i));
            selected = bus_->segment(name);
        }
    }
    return selected;
}


/**
 * Returns pointer to the selected fu port guard.
 *
 * @return NULL if no port guard is selected.
 */
PortGuard*
BusDialog::selectedFUGuard() const {
    long item = -1;
    item = fuGuardList_->GetNextItem(
        item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

    if (item == -1) {
        return NULL;
    }

    assert (item < int(portGuards_.size()));

    return portGuards_[item];
}


/**
 * Returns pointer to the selected register file guard.
 *
 * @return NULL if no register guard is selected.
 */
RegisterGuard*
BusDialog::selectedRFGuard() const {
    long item = -1;
    item = rfGuardList_->GetNextItem(
        item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

    if (item == -1) {
        return NULL;
    }

    assert (item < int(registerGuards_.size()));

    long itemData = rfGuardList_->GetItemData(item);
    RegisterGuard* rfGuard = (RegisterGuard*)itemData;
    assert (rfGuard != NULL);
    return rfGuard;
}




/**
 * Adds a new segment to the segment list.
 */
void
BusDialog::onAddSegment(wxCommandEvent&) {

    if (!TransferDataFromWindow()) {
        return;
    }
    assert(newSegName_ != _T(""));

    string trimmedName =
        WxConversion::toString(newSegName_.Trim(false).Trim(true));

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

    // Check that the new segment's name is unique among the segments of
    // the bus.
    if (bus_->hasSegment(trimmedName)) {
        ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();
        format message =
            prodeTexts->text(ProDeTextGenerator::MSG_ERROR_SAME_NAME);
        format component =
            prodeTexts->text(ProDeTextGenerator::COMP_A_SEGMENT);
        format bus = prodeTexts->text(ProDeTextGenerator::COMP_BUS);
        message % trimmedName % component.str() % bus.str();
        component = prodeTexts->text(ProDeTextGenerator::COMP_SEGMENT);
        message % component.str();
        WarningDialog dialog(this, WxConversion::toWxString(message.str()));
        dialog.ShowModal();
        return;
    }
    new Segment(trimmedName, *bus_);
    updateSegmentList();

    // clear the segment name control
    dynamic_cast<wxTextCtrl*>(
        FindWindow(ID_SEGMENT_NAME))->SetValue(_T(""));
}


/**
 * Enables and disables OK button based on input in the bus name widget.
 */
void
BusDialog::onBusName(wxCommandEvent&) {
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
 * Handles the unconditional guard deletion/creation when
 * true/false guard checkbox values are toggled.
 */
void
BusDialog::onUnconditionalGuard(wxCommandEvent& event) {

    bool inverted = false;
    wxCheckBox* checkbox = alwaysTrueGuard_;

    if (event.GetId() == ID_FALSE_GUARD) {
        inverted = true;
        checkbox = alwaysFalseGuard_;
    }

    if (!checkbox->GetValue()) {
        for (int i = 0; i < bus_->guardCount(); i++) {
            Guard* guard = bus_->guard(i);
            UnconditionalGuard* unCondGuard =
                dynamic_cast<UnconditionalGuard*>(guard);
            if (unCondGuard != NULL &&
                unCondGuard->isInverted() == inverted) {

                delete unCondGuard;
            }
        }
    } else {
        try {
            new UnconditionalGuard(inverted, *bus_);
        } catch (Exception& e) {
            ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();
            format text = prodeTexts->text(ProDeTextGenerator::MSG_ERROR);
            wxString message = WxConversion::toWxString(text.str());
            message.Append(WxConversion::toWxString(e.errorMessage()));
            ErrorDialog dialog(this, message);
            dialog.ShowModal();
            return;
        }
    }
}

/**
 * Enables and disables add segment button based on input in the segment
 * name widget.
 */
void
BusDialog::onSegmentName(wxCommandEvent&) {
    if (!TransferDataFromWindow()) {
        assert(false);
    }
    wxString trimmedName = newSegName_.Trim(false).Trim(true);
    if (trimmedName == _T("")) {
        FindWindow(ID_ADD_SEGMENT)->Disable();
    } else {
        FindWindow(ID_ADD_SEGMENT)->Enable();
    }
}


/**
 * Disables segment deletion and move buttons if no segment is selected.
 */
void
BusDialog::onSegmentSelection(wxListEvent&) {
    if (segList_->GetSelectedItemCount() != 1) {
        FindWindow(ID_DELETE_SEGMENT)->Disable();
        FindWindow(ID_SEGMENT_UP)->Disable();
        FindWindow(ID_SEGMENT_DOWN)->Disable();
        return;
    }
    FindWindow(ID_DELETE_SEGMENT)->Enable();
    FindWindow(ID_SEGMENT_UP)->Enable();
    FindWindow(ID_SEGMENT_DOWN)->Enable();
}


/**
 * Opens a pop-up menu when right mouse button was pressed.
 *
 * @param event Information about right mouse click event.
 */
void
BusDialog::onSegmentRightClick(wxListEvent& event) {

    segList_->SetItemState(event.GetIndex(), wxLIST_STATE_SELECTED,
                           wxLIST_STATE_SELECTED);

    wxMenu* contextMenu = new wxMenu();

    ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();
    format button = prodeTexts->text(
        ProDeTextGenerator::TXT_LABEL_BUTTON_DELETE);
    contextMenu->Append(
        ID_DELETE_SEGMENT, WxConversion::toWxString(button.str()));
    segList_->PopupMenu(contextMenu, event.GetPoint());
}


/**
 * Deletes the selected segment from the bus.
 *
 * The Segments before and after the deleted segments are connected to each
 * other.
 */
void
BusDialog::onDeleteSegment(wxCommandEvent&) {

    Segment* selected = selectedSegment();
    assert(selected != NULL);

    delete selected;
    updateSegmentList();

    wxListEvent dummy;
    onSegmentSelection(dummy);
}


/**
 * Moves selected segment one position up in the segment chain.
 */
void
BusDialog::onSegmentUp(wxCommandEvent&) {
    Segment* selected = selectedSegment();
    assert (selected != NULL);

    if (!selected->hasSourceSegment()) {
        // Selected segment was first in the chain, do nothing.
        return;
    }

    selected->moveBefore(*(selected->sourceSegment()));
    updateSegmentList();

    // set the moved segment as selected
    long i = segList_->FindItem(0, WxConversion::toWxString(selected->name()));
    segList_->SetItemState(i,wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
}



/**
 * Moves selected segment one position down in the segment chain.
 */
void
BusDialog::onSegmentDown(wxCommandEvent&) {
    Segment* selected = selectedSegment();
    assert (selected != NULL);

    if (!selected->hasDestinationSegment()) {
        // Selected segment was first in the chain, do nothing.
        return;
    }

    selected->moveAfter(*(selected->destinationSegment()));
    updateSegmentList();

    // set the moved segment as selected
    long i = segList_->FindItem(0, WxConversion::toWxString(selected->name()));
    segList_->SetItemState(i,wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
}


/**
 * Validates input in the controls, and updates the Bus object.
 */
void
BusDialog::onOK(wxCommandEvent&) {

    if (!Validate()) {
        return;
    }

    if (!TransferDataFromWindow()) {
        return;
    }

    // Check that the machine doesn't contain instruction templates with
    // slots that are wider than the new bus width.
    Machine::InstructionTemplateNavigator navigator =
        bus_->machine()->instructionTemplateNavigator();
    for (int i = 0; i < navigator.count(); i++) {
        int slotWidth = navigator.item(i)->supportedWidth(bus_->name());
        if (slotWidth != 0 && slotWidth > width_) {
            ProDeTextGenerator* generator =
                ProDeTextGenerator::instance();
            format message = generator->text(
                ProDeTextGenerator::MSG_ERROR_BUS_IT_SLOT_WIDTH);
            message % navigator.item(i)->name();
            message % slotWidth;
            InformationDialog dialog(
                this, WxConversion::toWxString(message.str()));
            dialog.ShowModal();
            return;
        }
    }

    // Check the name validity.
    string trimmedName =
        WxConversion::toString(name_.Trim(false).Trim(true));
    if (!MachineTester::isValidComponentName(trimmedName)) {
        ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();
        format message =
            prodeTexts->text(ProDeTextGenerator::MSG_ERROR_ILLEGAL_NAME);
        InformationDialog warning(
            this, WxConversion::toWxString(message.str()));
        warning.ShowModal();
        return;
    }

    // Check that the new name is not reserved.
    if (trimmedName != bus_->name()) {
        const Machine::BusNavigator busNavigator =
	    bus_->machine()->busNavigator();
	if (busNavigator.hasItem(trimmedName)) {
	    ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();
	    format message =
		prodeTexts->text(ProDeTextGenerator::MSG_ERROR_SAME_NAME);
	    message % trimmedName;
	    message % prodeTexts->text(ProDeTextGenerator::COMP_A_BUS).str();
	    message % prodeTexts->text(ProDeTextGenerator::COMP_MACHINE).str();
	    message % prodeTexts->text(ProDeTextGenerator::COMP_BUS).str();
	    WarningDialog warning(
		this, WxConversion::toWxString(message.str()));
	    warning.ShowModal();
	    return;
	}
	// Immediate slots share namespace with busses, check that an immediate
	// slot with the same name does not exist.
        const Machine::ImmediateSlotNavigator immSlotNavigator =
	    bus_->machine()->immediateSlotNavigator();
	if (immSlotNavigator.hasItem(trimmedName)) {
	    ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();
	    format message =
		prodeTexts->text(ProDeTextGenerator::MSG_ERROR_SAME_NAME);
	    message % trimmedName;
	    message % prodeTexts->text(
		ProDeTextGenerator::COMP_AN_IMM_SLOT).str();
	    message % prodeTexts->text(ProDeTextGenerator::COMP_MACHINE).str();
	    message % prodeTexts->text(ProDeTextGenerator::COMP_BUS).str();
	    WarningDialog warning(
		this, WxConversion::toWxString(message.str()));
	    warning.ShowModal();
	    return;
	}
    }

    bus_->setWidth(width_);
    bus_->setImmediateWidth(immWidth_);
    bus_->setName(trimmedName);

    if (immediateExtensionBox_->GetStringSelection().IsSameAs(
            ProDeConstants::EXTENSION_ZERO)) {

        bus_->setZeroExtends();
    } else {
        bus_->setSignExtends();
    }

    EndModal(wxID_OK);
}


/**
 * Creates and shows a register file guard dialog for
 * adding register file guards.
 */
void
BusDialog::onAddRFGuard(wxCommandEvent&) {
    Machine::RegisterFileNavigator navigator =
        bus_->machine()->registerFileNavigator();

    if (navigator.count() < 1) {
        ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();
        format message =
            prodeTexts->text(ProDeTextGenerator::MSG_ERROR_CANNOT_RF_GUARD);
        InformationDialog dialog(this, WxConversion::toWxString(message.str()));
        dialog.ShowModal();
        return;
    }

    RFGuardDialog dialog(this, bus_);
    dialog.ShowModal();
    updateGuardLists();
}



/**
 * Deletes selected register file guards from the register file guards list.
 */
void
BusDialog::onDeleteRFGuard(wxCommandEvent&) {
    long item = -1;

    // loop selected guards and delete them
    for (int i = 0; i < rfGuardList_->GetSelectedItemCount(); ++i) {
        item = rfGuardList_->GetNextItem(
            item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        
        assert (item < static_cast<int>(registerGuards_.size()));

        // retrieve item data (guard pointer) that is bound with the item  
        if (item >= 0) {
            long itemData = rfGuardList_->GetItemData(item);
            RegisterGuard* rfGuard = (RegisterGuard*)itemData;
            if (rfGuard != NULL) {
                delete rfGuard;
                rfGuard = NULL;
            }
        }
    }
    
    updateGuardLists();
    wxListEvent dummy;
    onRFGuardSelection(dummy); 
    
    return;
}



/**
 * Handles the 'Edit RFGuard' button event.
 *
 * Opens a RFGuardDialog with the selected guard's attributes set.
 */
void
BusDialog::onEditRFGuard(wxCommandEvent&) {
    RegisterGuard* guard = selectedRFGuard();
    if (guard == NULL) {
        // No guard selected.
        return;
    }
    RFGuardDialog dialog(this, bus_, guard);
    dialog.ShowModal();
    updateGuardLists();
}


/**
 * Disables and enables Edit and Delete buttons under the register
 * file guard list.
 *
 * If a guard is selected, buttons are enabled. If no guard is
 * selected the buttons will be disabled.
 */
void
BusDialog::onRFGuardSelection(wxListEvent&) {
    if (rfGuardList_->GetSelectedItemCount() != 1) {
        FindWindow(ID_EDIT_RF_GUARD)->Disable();
    } else {
        FindWindow(ID_EDIT_RF_GUARD)->Enable();
    }

    if (rfGuardList_->GetSelectedItemCount() < 1) {
        FindWindow(ID_DELETE_RF_GUARD)->Disable();
    } else {
        FindWindow(ID_DELETE_RF_GUARD)->Enable();
    }
}


/**
 * Creates and shows an empty  function unit port guard dialog for
 * adding  function unit port guards.
 */
void
BusDialog::onAddFUGuard(wxCommandEvent&) {
    Machine::FunctionUnitNavigator navigator =
        bus_->machine()->functionUnitNavigator();

    bool portExists = false;
    for (int i = 0; i < navigator.count(); i++) {
        if (navigator.item(i)->portCount() > 0) {
            portExists = true;
            break;
        }
    }

    if (!portExists) {
        ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();
        format message =
            prodeTexts->text(ProDeTextGenerator::MSG_ERROR_CANNOT_FU_GUARD);
        InformationDialog dialog(
            this, WxConversion::toWxString(message.str()));
        dialog.ShowModal();
        return;
    }

    FUGuardDialog dialog(this, bus_);
    dialog.ShowModal();
    updateGuardLists();
}



/**
 * Deletes selected function unit port guards from the function unit
 * port guards list.
 */
void
BusDialog::onDeleteFUGuard(wxCommandEvent&) {
    delete selectedFUGuard();
    updateGuardLists();
    wxListEvent dummy;
    onFUGuardSelection(dummy);
}



/**
 * Handles the 'Edit FUGuard' button event.
 *
 * Opens a FUGuardDialog with the selected guard's attributes set.
 */
void
BusDialog::onEditFUGuard(wxCommandEvent&) {
    PortGuard* guard = selectedFUGuard();
    if (guard == NULL) {
        // No guard selected.
        return;
    }
    FUGuardDialog dialog(this, bus_, guard);
    dialog.ShowModal();
    updateGuardLists();
}


/**
 * Sets the immediate width range when the bus width is modified.
 */
void
BusDialog::onWidth(wxSpinEvent&) {
    wxSpinCtrl* width = dynamic_cast<wxSpinCtrl*>(FindWindow(ID_BUS_WIDTH));
    wxSpinCtrl* siWidth = dynamic_cast<wxSpinCtrl*>(FindWindow(ID_SI_WIDTH));
    if (siWidth->GetValue() > width->GetValue()) {
        siWidth->SetValue(width->GetValue());
    }
    siWidth->SetRange(0, width->GetValue());
}


/**
 * Disables and enables Edit and Delete buttons under the register
 * file guard list.
 *
 * If a guard is selected, buttons are enabled. If no guard is
 * selected the buttons will be disabled.
 */
void
BusDialog::onFUGuardSelection(wxListEvent&) {
    if (fuGuardList_->GetSelectedItemCount() != 1) {
        FindWindow(ID_DELETE_FU_GUARD)->Disable();
        FindWindow(ID_EDIT_FU_GUARD)->Disable();
        return;
    }
    FindWindow(ID_DELETE_FU_GUARD)->Enable();
    FindWindow(ID_EDIT_FU_GUARD)->Enable();
}


/**
 * Opens a pop-up menu when right mouse button was pressed on the
 * register file guard list.
 *
 * @param event Information about right mouse click event.
 */
void
BusDialog::onRFGuardRightClick(wxListEvent& event) {

    rfGuardList_->SetItemState(event.GetIndex(), wxLIST_STATE_SELECTED,
                                wxLIST_STATE_SELECTED);

    wxMenu* contextMenu = new wxMenu();

    ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();
    format button = prodeTexts->text(
        ProDeTextGenerator::TXT_LABEL_BUTTON_EDIT);
    contextMenu->Append(
        ID_EDIT_RF_GUARD, WxConversion::toWxString(button.str()));
    button = prodeTexts->text(
        ProDeTextGenerator::TXT_LABEL_BUTTON_DELETE);
    contextMenu->Append(
        ID_DELETE_RF_GUARD, WxConversion::toWxString(button.str()));
    rfGuardList_->PopupMenu(contextMenu, event.GetPoint());
}


/**
 * Handles left mouse button double clicks on the RFGuard list.
 */
void
BusDialog::onActivateRFGuard(wxListEvent&) {
    wxCommandEvent dummy;
    onEditRFGuard(dummy);
}


/**
 * Opens a pop-up menu when right mouse button was pressed on the
 * FU guard list.
 *
 * @param event Information about right mouse click event.
 */
void
BusDialog::onFUGuardRightClick(wxListEvent& event) {

    fuGuardList_->SetItemState(event.GetIndex(), wxLIST_STATE_SELECTED,
                                wxLIST_STATE_SELECTED);

    wxMenu* contextMenu = new wxMenu();

    ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();
    format button = prodeTexts->text(
        ProDeTextGenerator::TXT_LABEL_BUTTON_EDIT);
    contextMenu->Append(
        ID_EDIT_FU_GUARD, WxConversion::toWxString(button.str()));
    button = prodeTexts->text(
        ProDeTextGenerator::TXT_LABEL_BUTTON_DELETE);
    contextMenu->Append(
        ID_DELETE_FU_GUARD, WxConversion::toWxString(button.str()));
    fuGuardList_->PopupMenu(contextMenu, event.GetPoint());
}


/**
 * Handles left mouse button double clicks on the FUGuard list.
 */
void
BusDialog::onActivateFUGuard(wxListEvent&) {
    wxCommandEvent dummy;
    onEditFUGuard(dummy);
}


/**
 * Creates the dialog window contents.
 *
 * This method was generated with wxDesigner. Don't modify manually,
 * all chnages will be lost if the code is regenerated.
 *
 * @return Main sizer of the created contents.
 * @param parent The dialog window.
 * @param call_fit If true, fits the contents inside the dialog.
 * @param set_sizer If true, sets the main sizer as dialog contents.
 */
wxSizer*
BusDialog::createContents(wxWindow *parent, bool call_fit, bool set_sizer) {

    wxFlexGridSizer *item0 = new wxFlexGridSizer( 2, 0, 0 );

    wxBoxSizer *item1 = new wxBoxSizer( wxVERTICAL );

    wxStaticBox *item3 = new wxStaticBox( parent, -1, wxT("Bus:") );
    wxStaticBoxSizer *item2 = new wxStaticBoxSizer( item3, wxVERTICAL );
    busSizer_ = item2;

    wxBoxSizer *item4 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticText *item5 = new wxStaticText( parent, ID_LABEL_BUS_NAME, wxT("Name:"), wxDefaultPosition, wxDefaultSize, 0 );
    item4->Add( item5, 0, wxALIGN_CENTER|wxALL, 5 );

    wxTextCtrl *item6 = new wxTextCtrl( parent, ID_BUS_NAME, wxT(""), wxDefaultPosition, wxSize(200,-1), 0 );
    item4->Add( item6, 0, wxALIGN_CENTER|wxALL, 5 );

    item2->Add( item4, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item7 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticText *item8 = new wxStaticText( parent, ID_LABEL_BUS_WIDTH, wxT("Width:"), wxDefaultPosition, wxDefaultSize, 0 );
    item7->Add( item8, 0, wxALIGN_CENTER|wxALL, 5 );

    wxSpinCtrl *item9 = new wxSpinCtrl( parent, ID_BUS_WIDTH, wxT("1"), wxDefaultPosition, wxSize(100,-1), 0, 1, 1024, 1 );
    item7->Add( item9, 0, wxALIGN_CENTER|wxALL, 5 );

    item2->Add( item7, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item0->Add( item2, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
    //item1->Add( item2, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticBox *item11 = new wxStaticBox( parent, -1, wxT("Short Immediate:") );
    wxStaticBoxSizer *item10 = new wxStaticBoxSizer( item11, wxVERTICAL );
    siSizer_ = item10;

    wxBoxSizer *item12 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticText *item13 = new wxStaticText( parent, ID_LABEL_SI_WIDTH, wxT("Width:"), wxDefaultPosition, wxDefaultSize, 0 );
    item12->Add( item13, 0, wxALIGN_CENTER|wxALL, 5 );

    wxSpinCtrl *item14 = new wxSpinCtrl( parent, ID_SI_WIDTH, wxT("0"), wxDefaultPosition, wxSize(100,-1), 0, 0, 1000, 0 );
    item12->Add( item14, 0, wxALIGN_CENTER|wxALL, 5 );

    item10->Add( item12, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxString strs15[] =
    {
        wxT("Zero"),
        wxT("Sign")
    };
    wxRadioBox *item15 = new wxRadioBox( parent, ID_SI_EXTENSION, wxT("Extension"), wxDefaultPosition, wxDefaultSize, 2, strs15, 1, wxRA_SPECIFY_ROWS );
    item10->Add( item15, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item0->Add( item10, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
    //item1->Add( item10, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxCheckBox *item16 = new wxCheckBox( parent, ID_TRUE_GUARD, wxT("Always true guard"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item16, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxCheckBox *item17 = new wxCheckBox( parent, ID_FALSE_GUARD, wxT("Always false guard"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item17, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item0->Add( item1, 0, wxGROW|wxALL, 5 );

    wxStaticBox *item19 = new wxStaticBox( parent, -1, wxT("Segements:") );
    wxStaticBoxSizer *item18 = new wxStaticBoxSizer( item19, wxVERTICAL );
    segmentSizer_ = item18;

    wxListCtrl *item20 = new wxListCtrl( parent, ID_SEGMENT_LIST, wxDefaultPosition, wxSize(300,160), wxLC_REPORT|wxLC_SINGLE_SEL|wxSUNKEN_BORDER );
    item18->Add( item20, 0, wxALIGN_CENTER|wxALL, 5 );

    wxBoxSizer *item21 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticText *item22 = new wxStaticText( parent, ID_LABEL_SEGMENT_NAME, wxT("Name:"), wxDefaultPosition, wxDefaultSize, 0 );
    item21->Add( item22, 0, wxALIGN_CENTER|wxALL, 5 );

    wxTextCtrl *item23 = new wxTextCtrl( parent, ID_SEGMENT_NAME, wxT(""), wxDefaultPosition, wxSize(100,-1), 0 );
    item21->Add( item23, 0, wxALIGN_CENTER|wxALL, 5 );

    wxBoxSizer *item24 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item25 = new wxButton( parent, ID_SEGMENT_UP, wxT("Up"), wxDefaultPosition, wxSize(40,-1), 0 );
    item24->Add( item25, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item26 = new wxButton( parent, ID_SEGMENT_DOWN, wxT("Down"), wxDefaultPosition, wxSize(50,-1), 0 );
    item24->Add( item26, 0, wxALIGN_CENTER|wxALL, 5 );

    item21->Add( item24, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    item18->Add( item21, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item27 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item28 = new wxButton( parent, ID_ADD_SEGMENT, wxT("Add"), wxDefaultPosition, wxDefaultSize, 0 );
    item27->Add( item28, 0, wxALIGN_CENTER|wxALL, 5 );

    item27->Add( 20, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item29 = new wxButton( parent, ID_DELETE_SEGMENT, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    item27->Add( item29, 0, wxALIGN_CENTER|wxALL, 5 );

    item18->Add( item27, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item18, 0, wxALL, 5 );

    wxStaticBox *item31 = new wxStaticBox( parent, -1, wxT("Register File Guards:") );
    wxStaticBoxSizer *item30 = new wxStaticBoxSizer( item31, wxVERTICAL );
    registerGuardSizer_ = item30;

    wxListCtrl *item32 = new wxListCtrl( parent, ID_RF_GUARD_LIST, wxDefaultPosition, wxSize(300,120), wxLC_REPORT|wxSUNKEN_BORDER );
    item30->Add( item32, 0, wxALIGN_CENTER|wxALL, 5 );

    wxBoxSizer *item33 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item34 = new wxButton( parent, ID_ADD_RF_GUARD, wxT("Add..."), wxDefaultPosition, wxDefaultSize, 0 );
    item33->Add( item34, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item35 = new wxButton( parent, ID_EDIT_RF_GUARD, wxT("Edit..."), wxDefaultPosition, wxDefaultSize, 0 );
    item35->Enable( false );
    item33->Add( item35, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item36 = new wxButton( parent, ID_DELETE_RF_GUARD, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    item36->Enable( false );
    item33->Add( item36, 0, wxALIGN_CENTER|wxALL, 5 );

    item30->Add( item33, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item30, 0, wxGROW|wxALL, 5 );

    wxStaticBox *item38 = new wxStaticBox( parent, -1, wxT("Function Unit Guards") );
    wxStaticBoxSizer *item37 = new wxStaticBoxSizer( item38, wxVERTICAL );
    portGuardSizer_ = item37;

    wxListCtrl *item39 = new wxListCtrl( parent, ID_FU_GUARD_LIST, wxDefaultPosition, wxSize(300,120), wxLC_REPORT|wxLC_SINGLE_SEL|wxSUNKEN_BORDER );
    item37->Add( item39, 0, wxALIGN_CENTER|wxALL, 5 );

    wxBoxSizer *item40 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item41 = new wxButton( parent, ID_ADD_FU_GUARD, wxT("Add..."), wxDefaultPosition, wxDefaultSize, 0 );
    item40->Add( item41, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item42 = new wxButton( parent, ID_EDIT_FU_GUARD, wxT("Edit..."), wxDefaultPosition, wxDefaultSize, 0 );
    item42->Enable( false );
    item40->Add( item42, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item43 = new wxButton( parent, ID_DELETE_FU_GUARD, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    item43->Enable( false );
    item40->Add( item43, 0, wxALIGN_CENTER|wxALL, 5 );

    item37->Add( item40, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item37, 0, wxGROW|wxALL, 5 );

    wxButton *item44 = new wxButton( parent, ID_HELP, wxT("&Help"), wxDefaultPosition, wxDefaultSize, 0 );
    item0->Add( item44, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item45 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item46 = new wxButton( parent, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item45->Add( item46, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item47 = new wxButton( parent, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item45->Add( item47, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item45, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }

    return item0;
}
