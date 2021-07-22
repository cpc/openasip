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
 * @file InputOperandDialog.cc
 *
 * Definition of InputOperandDialog class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red.
 */

#include <wx/valgen.h>
#include <string>
#include <boost/format.hpp>
#include <vector>

#include "InputOperandDialog.hh"
#include "WxConversion.hh"
#include "Conversion.hh"
#include "WidgetTools.hh"
#include "ObjectState.hh"
#include "OSEdConstants.hh"
#include "GUITextGenerator.hh"
#include "OSEdTextGenerator.hh"
#include "DialogPosition.hh"
#include "Operand.hh"
#include "SimValue.hh"

using std::set;
using std::string;
using boost::format;
using std::vector;

BEGIN_EVENT_TABLE(InputOperandDialog, wxDialog)
    EVT_LIST_ITEM_SELECTED(ID_SWAP_LIST, InputOperandDialog::onSelection)
    EVT_LIST_ITEM_DESELECTED(ID_SWAP_LIST, InputOperandDialog::onSelection)
    
    EVT_CHOICE(ID_OPERATION_INPUT_TYPES, InputOperandDialog::onType)
    EVT_SPINCTRL(ID_ELEMENT_WIDTH, InputOperandDialog::onElementWidth)
    EVT_CHOICE(ID_ELEMENT_COUNT, InputOperandDialog::onElementCount)

    EVT_BUTTON(ID_ADD_BUTTON, InputOperandDialog::onAddSwap)
    EVT_BUTTON(ID_DELETE_BUTTON, InputOperandDialog::onDeleteSwap)
    EVT_BUTTON(wxID_OK, InputOperandDialog::onOk)

END_EVENT_TABLE()

/**
 * Constructor.
 *
 * @param parent The parent window.
 * @param operand Operand to be added or modified.
 * @param numberOfOperands The number of input operands.
 */
InputOperandDialog::InputOperandDialog(
    wxWindow* parent,
    Operand* operand,
    int numberOfOperands,
    int operandIndex) :
    wxDialog(parent, -1, _T(""), 
             DialogPosition::getPosition(DialogPosition::DIALOG_INPUT_OPERAND)),
    operand_(operand), numberOfOperands_(numberOfOperands), 
    index_(operandIndex) {

    memAddress_ = operand_->isAddress();
    memData_ = operand_->isMemoryData();
    createContents(this, true, true);

    swapList_ = dynamic_cast<wxListCtrl*>(FindWindow(ID_SWAP_LIST));
    swapChoice_ = dynamic_cast<wxChoice*>(FindWindow(ID_OPERAND_CHOICE));

    inputTypesComboBox_ =
        dynamic_cast<wxChoice*>(FindWindow(ID_OPERATION_INPUT_TYPES));

    elementWidthSpinCtrl_ =
        dynamic_cast<wxSpinCtrl*>(FindWindow(ID_ELEMENT_WIDTH));

    elementCountChoice_ =
        dynamic_cast<wxChoice*>(FindWindow(ID_ELEMENT_COUNT));
    
    FindWindow(ID_MEM_ADDRESS)->SetValidator(wxGenericValidator(&memAddress_));
    FindWindow(ID_MEM_DATA)->SetValidator(wxGenericValidator(&memData_));

    FindWindow(wxID_OK)->SetFocus();

    canSwap_ = operand_->swap();

    inputTypes_.push_back(Operand::SINT_WORD_STRING);
    inputTypes_.push_back(Operand::UINT_WORD_STRING);
    inputTypes_.push_back(Operand::FLOAT_WORD_STRING);
    inputTypes_.push_back(Operand::DOUBLE_WORD_STRING);
    inputTypes_.push_back(Operand::HALF_FLOAT_WORD_STRING);
    inputTypes_.push_back(Operand::BOOL_STRING);
    inputTypes_.push_back(Operand::RAW_DATA_STRING);
    inputTypes_.push_back(Operand::SLONG_WORD_STRING);
    inputTypes_.push_back(Operand::ULONG_WORD_STRING);

    operandTypes_[0] = Operand::SINT_WORD;
    operandTypes_[1] = Operand::UINT_WORD;
    operandTypes_[2] = Operand::FLOAT_WORD;
    operandTypes_[3] = Operand::DOUBLE_WORD;
    operandTypes_[4] = Operand::HALF_FLOAT_WORD;
    operandTypes_[5] = Operand::BOOL;
    operandTypes_[6] = Operand::RAW_DATA;
    operandTypes_[7] = Operand::SLONG_WORD;
    operandTypes_[8] = Operand::ULONG_WORD;

    type_ = operand_->type();
    elemWidth_ = operand_->elementWidth();
    elemCount_ = operand_->elementCount();
    updateTypes();
    updateElementWidths();
    updateElementCounts();
    
    setTexts();
    }

/**
 * Destructor.
 */
InputOperandDialog::~InputOperandDialog() {
    int x, y;
    GetPosition(&x, &y);
    wxPoint point(x, y);
    DialogPosition::setPosition(DialogPosition::DIALOG_INPUT_OPERAND, point);
}

/**
 * Set texts to widgets.
 */
void
InputOperandDialog::setTexts() {
	
    GUITextGenerator& guiText = *GUITextGenerator::instance();
    OSEdTextGenerator& osedText = OSEdTextGenerator::instance();
	
    // title
    format fmt = 
        osedText.text(OSEdTextGenerator::TXT_INPUT_OPERAND_DIALOG_TITLE);
    fmt % index_;
    SetTitle(WxConversion::toWxString(fmt.str()));

    // buttons
    WidgetTools::setLabel(&guiText, FindWindow(wxID_OK),
                          GUITextGenerator::TXT_BUTTON_OK);

    WidgetTools::setLabel(&guiText, FindWindow(wxID_CANCEL),
                          GUITextGenerator::TXT_BUTTON_CANCEL);

    WidgetTools::setLabel(&guiText, FindWindow(ID_ADD_BUTTON),
                          GUITextGenerator::TXT_BUTTON_ADD);

    WidgetTools::setLabel(&guiText, FindWindow(ID_DELETE_BUTTON),
                          GUITextGenerator::TXT_BUTTON_DELETE);

    // column names
    fmt = osedText.text(OSEdTextGenerator::TXT_COLUMN_OPERAND);
    swapList_->InsertColumn(
        0, WxConversion::toWxString(fmt.str()), wxLIST_FORMAT_LEFT,
        OSEdConstants::DEFAULT_COLUMN_WIDTH);

    // check boxes
    WidgetTools::setLabel(&osedText, FindWindow(ID_MEM_DATA),
                          OSEdTextGenerator::TXT_CHECKBOX_MEM_DATA);

    WidgetTools::setLabel(&osedText, FindWindow(ID_MEM_ADDRESS),
                          OSEdTextGenerator::TXT_CHECKBOX_MEM_ADDRESS);

    // sizer
    fmt = osedText.text(OSEdTextGenerator::TXT_BOX_CAN_SWAP);
    WidgetTools::setWidgetLabel(swapSizer_, fmt.str());
}

/**
 * Event handler for operand type choice box.
**/
void 
InputOperandDialog::onType(wxCommandEvent&) {

    type_ = inputTypesComboBox_->GetSelection();

    Operand::OperandType operType = static_cast<Operand::OperandType>(type_);
    elemWidth_ = Operand::defaultElementWidth(operType);
    elemCount_ = 1;
    updateElementWidths();
    updateElementCounts();
}

/**
 * Event handler for element width spin ctrl.
**/
void 
InputOperandDialog::onElementWidth(wxSpinEvent&) {
    elemWidth_ = elementWidthSpinCtrl_->GetValue();
    // update choice box list cells
    updateElementCounts();
}

/**
 * Event handler for element count choice box.
**/
void 
InputOperandDialog::onElementCount(wxCommandEvent&) {
    // get the current choice box value and convert it to integer
    int index = elementCountChoice_->GetSelection();
    wxString number = elementCountChoice_->GetString(index);
    long value;
    if(!number.ToLong(&value)) { 
        elemCount_ = 1;
        return;
    }

    // save current choice
    elemCount_ = static_cast<int>(value);
    // update spin ctrl range
    updateElementWidths();
}

/**
 * Updates the type lists.
**/
void
InputOperandDialog::updateTypes() {

    inputTypesComboBox_->Clear();
    
    for (unsigned int i = 0; i < inputTypes_.size(); i++) {
        wxString oper = WxConversion::toWxString(inputTypes_.at(i));
        inputTypesComboBox_->Append(oper);
    }
	
    inputTypesComboBox_->SetSelection(type_);
}

/**
 * Updates the element width choice box list.
**/
void
InputOperandDialog::updateElementWidths() {

    Operand::OperandType operType = static_cast<Operand::OperandType>(type_);

    if (operType == Operand::RAW_DATA) {
        // element width for raw data can be arbitrary up to the max width
        int elemWidth = 1;
        int lastValidWidth = 1;
        while (elemCount_*elemWidth <= SIMD_WORD_WIDTH) {
            lastValidWidth = elemWidth;
            elemWidth *= 2;
        }

        // degrade current element width if it is too big
        if (elemWidth_ > lastValidWidth) {
            elemWidth_ = lastValidWidth;
        }
        elementWidthSpinCtrl_->SetRange(1, lastValidWidth);
        elementWidthSpinCtrl_->SetValue(elemWidth_);
    } else {
        // element width for other types is their default type width
        elementWidthSpinCtrl_->SetRange(elemWidth_, elemWidth_);
        elementWidthSpinCtrl_->SetValue(elemWidth_);
    }
}

/**
 * Updates the element count choice box list.
**/
void
InputOperandDialog::updateElementCounts() {

    elementCountChoice_->Clear();
    
    // update the list so that only shorter or equal than SIMD_WORD_WIDTH 
    // width*count combinations are listed 
    int elemCount = 1;
    int elemCountIndex = 0;
    while (elemCount*elemWidth_ <= SIMD_WORD_WIDTH) {
        if (elemCount < elemCount_) {
            ++elemCountIndex;
        }
        elementCountChoice_->Append(WxConversion::toWxString(elemCount));
        elemCount *= 2;
    }
    elementCountChoice_->SetSelection(elemCountIndex);
}

/**
 * Transfers data to window.
 *
 * @return True if transfer is successful.
 */
bool
InputOperandDialog::TransferDataToWindow() {
    updateList();
    return wxWindow::TransferDataToWindow();
}

/**
 * Updates the list of can swap operands.
 */
void
InputOperandDialog::updateList() {

    swapList_->DeleteAllItems();
    swapChoice_->Clear();

    set<int>::iterator it = canSwap_.begin();
    int i = 0;
    while (it != canSwap_.end()) {
        wxString id = WxConversion::toWxString(*it);
        swapList_->InsertItem(i, id);
        i++;
        it++;
    }
	
    for (int i = 1; i <= numberOfOperands_; i++) {
        if (i != index_ && 
            swapList_->FindItem(-1, WxConversion::toWxString(i)) == -1) {
            
            swapChoice_->Append(WxConversion::toWxString(i));
        }
    }
    swapChoice_->SetSelection(0);
    if (swapChoice_->GetCount() == 0) {
        FindWindow(ID_ADD_BUTTON)->Disable();
    } else {
        FindWindow(ID_ADD_BUTTON)->Enable();
    }
    wxListEvent dummy;
    onSelection(dummy);
}

/**
 * Handles the event when id is added to can swap list.
 */
void
InputOperandDialog::onAddSwap(wxCommandEvent&) {
    wxString wxId = swapChoice_->GetStringSelection();
    string id = WxConversion::toString(wxId);
    canSwap_.insert(Conversion::toInt(id));
    updateList();
}

/**
 * Handles the event when id is deleted from can swap list.
 *
 * It is also possible to delete multible ids from the list.
 */
void
InputOperandDialog::onDeleteSwap(wxCommandEvent&) {

    vector<string> toBeDeleted;
    long item = -1;
    for (;;) {
        item = swapList_->GetNextItem(
            item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    
        if (item == -1) {
            break;
        }
        
        wxListItem info;
        info.SetId(item);
        info.SetColumn(0);
        swapList_->GetItem(info);
        toBeDeleted.push_back(WxConversion::toString(info.GetText()));
    }

    for (size_t i = 0; i < toBeDeleted.size(); i++) {
        int id = Conversion::toInt(toBeDeleted[i]);
        set<int>::iterator it = canSwap_.begin();
        while (it != canSwap_.end()) {
            if (*it == id) {
                canSwap_.erase(it);
                break;
            }
            it++;
        }
    }

    updateList();
}

/**
 * Handles the event when OK button is pushed.
 */
void
InputOperandDialog::onOk(wxCommandEvent&) {
    TransferDataFromWindow();
    updateOperand();
    EndModal(wxID_OK);
}

/**
 * Loads the properties to the modified/created operand.
 */
void
InputOperandDialog::updateOperand() {
    ObjectState* root = new ObjectState("");
    root->setAttribute(Operand::OPRND_ID, numberOfOperands_ + 1);
    
    int selected = inputTypesComboBox_->GetSelection();
    Operand::OperandType type = operandTypes_[selected];
    

    switch(type) {
        case Operand::SINT_WORD:
            root->setAttribute(Operand::OPRND_TYPE, Operand::SINT_WORD_STRING);
            break;
        case Operand::UINT_WORD:
            root->setAttribute(Operand::OPRND_TYPE, Operand::UINT_WORD_STRING);
            break;
        case Operand::FLOAT_WORD:
            root->setAttribute(Operand::OPRND_TYPE, Operand::FLOAT_WORD_STRING);
            break;
        case Operand::DOUBLE_WORD:
            root->setAttribute(Operand::OPRND_TYPE, Operand::DOUBLE_WORD_STRING);
            break;
        case Operand::HALF_FLOAT_WORD:
            root->setAttribute(
                Operand::OPRND_TYPE, Operand::HALF_FLOAT_WORD_STRING);
            break;
        case Operand::BOOL:
            root->setAttribute(Operand::OPRND_TYPE, Operand::BOOL_STRING);
            break;
        case Operand::RAW_DATA:
            root->setAttribute(
                Operand::OPRND_TYPE, Operand::RAW_DATA_STRING);
            break;
        case Operand::SLONG_WORD:
            root->setAttribute(
                Operand::OPRND_TYPE, Operand::SLONG_WORD_STRING);
            break;
        case Operand::ULONG_WORD:
            root->setAttribute(
                Operand::OPRND_TYPE, Operand::ULONG_WORD_STRING);
            break;
        default:
            root->setAttribute(Operand::OPRND_TYPE, Operand::SINT_WORD_STRING);
            break;
    }

    root->setAttribute(Operand::OPRND_ELEM_WIDTH, elemWidth_);
    root->setAttribute(Operand::OPRND_ELEM_COUNT, elemCount_);
    
    root->setAttribute(Operand::OPRND_MEM_ADDRESS, memAddress_);
    root->setAttribute(Operand::OPRND_MEM_DATA, memData_);
    
    if (canSwap_.size() > 0) {
        ObjectState* swap = new ObjectState(Operand::OPRND_CAN_SWAP);
        set<int>::iterator it = canSwap_.begin();
        while (it != canSwap_.end()) {
            ObjectState* swapChild = new ObjectState(Operand::OPRND_IN);
            swapChild->setAttribute(Operand::OPRND_ID, *it);
            swap->addChild(swapChild);
            it++;
        }
        root->addChild(swap);
    }
    
    operand_->loadState(root);
    delete root;
}

/**
 *
 */
void
InputOperandDialog::onSelection(wxListEvent&) {
    if (swapList_->GetSelectedItemCount() == 0) {
        FindWindow(ID_DELETE_BUTTON)->Disable();
    } else {
        FindWindow(ID_DELETE_BUTTON)->Enable();
    }
}

/**
 * Creates the contents of dialog.
 *
 * NOTE! This function is generated by wxDesigner, that is why it is ugly.
 *
 * @param parent The parent window.
 * @param call_fir If true, fits the contents of the dialog inside dialog.
 * @param set_sizer If true, sets the main sizer as the contents of the dialog.
 * @return The created sizer.
 */
wxSizer*
InputOperandDialog::createContents(wxWindow *parent, bool call_fit, bool set_sizer)
{
    wxBoxSizer *item0 = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer *item1 = new wxBoxSizer(wxHORIZONTAL);

    wxString strs9[] = {
            wxT("id: 1")
    };


    // ComboBox for input operand types
    wxChoice *itemInputTypes = new wxChoice(parent, ID_OPERATION_INPUT_TYPES, wxDefaultPosition, wxSize(100,-1), 1, strs9);
    item1->Add(itemInputTypes, 0, wxALIGN_CENTER|wxALL, 5);

    wxStaticText *itemTextWidth = new wxStaticText(parent, ID_TEXT_WIDTH, wxT("Element width:"), wxDefaultPosition, wxDefaultSize, 0);
    item1->Add(itemTextWidth, 0, wxALIGN_CENTER|wxALL, 5);
    wxSpinCtrl *itemElemWidth = new wxSpinCtrl(parent, ID_ELEMENT_WIDTH, wxT(""), wxDefaultPosition, wxSize(70,-1), 1);
    item1->Add(itemElemWidth, 0, wxALIGN_CENTER|wxALL, 5);
    wxStaticText *itemTextCount = new wxStaticText(parent, ID_TEXT_COUNT, wxT("Element count:"), wxDefaultPosition, wxDefaultSize, 0);
    item1->Add(itemTextCount, 0, wxALIGN_CENTER|wxALL, 5);
    wxChoice *itemElemCount = new wxChoice(parent, ID_ELEMENT_COUNT, wxDefaultPosition, wxSize(70,-1), 1, strs9);
    item1->Add(itemElemCount, 0, wxALIGN_CENTER|wxALL, 5);

    wxBoxSizer *item1b = new wxBoxSizer(wxHORIZONTAL);

    wxCheckBox *item2 = new wxCheckBox(parent, ID_MEM_ADDRESS, wxT("Memory address"), wxDefaultPosition, wxDefaultSize, 0);
    item1b->Add(item2, 0, wxALIGN_CENTER|wxALL, 5);

    wxCheckBox *item3 = new wxCheckBox(parent, ID_MEM_DATA, wxT("Memory data"), wxDefaultPosition, wxDefaultSize, 0);
    item1b->Add(item3, 0, wxALIGN_CENTER|wxALL, 5);

    item0->Add(item1, 0, wxALIGN_CENTER|wxALL, 5);
    item0->Add(item1b, 0, wxALIGN_CENTER|wxALL, 5);

    wxStaticBox *item5 = new wxStaticBox(parent, -1, wxT("Can swap"));
    wxStaticBoxSizer *item4 = new wxStaticBoxSizer(item5, wxVERTICAL);
    swapSizer_ = item4;

    wxListCtrl *item6 = new wxListCtrl(parent, ID_SWAP_LIST, wxDefaultPosition, wxSize(160,120), wxLC_REPORT|wxSUNKEN_BORDER);
    item4->Add(item6, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer *item7 = new wxBoxSizer(wxHORIZONTAL);
    
    wxChoice *item8 = new wxChoice(parent, ID_OPERAND_CHOICE, wxDefaultPosition, wxSize(100,-1), 1, strs9, 0);
    item7->Add(item8, 0, wxALIGN_CENTER|wxALL, 5);

    wxButton *item9 = new wxButton(parent, ID_ADD_BUTTON, wxT("Add"), wxDefaultPosition, wxDefaultSize, 0);
    item7->Add(item9, 0, wxALIGN_CENTER|wxALL, 5);

    wxButton *item10 = new wxButton(parent, ID_DELETE_BUTTON, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0);
    item7->Add(item10, 0, wxALIGN_CENTER|wxALL, 5);

    item4->Add(item7, 0, wxALIGN_CENTER|wxALL, 5);

    item0->Add(item4, 0, wxALIGN_CENTER|wxALL, 5);

    wxGridSizer *item11 = new wxGridSizer(2, 0, 0);

    item11->Add(20, 20, 0, wxALIGN_CENTER|wxALL, 5);

    wxBoxSizer *item12 = new wxBoxSizer(wxHORIZONTAL);

    wxButton *item13 = new wxButton(parent, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0);
    item12->Add(item13, 0, wxALIGN_CENTER|wxALL, 5);

    wxButton *item14 = new wxButton(parent, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0);
    item12->Add(item14, 0, wxALIGN_CENTER|wxALL, 5);

    item11->Add(item12, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    item0->Add(item11, 0, wxALIGN_CENTER|wxALL, 5);

    if (set_sizer) {
        parent->SetSizer(item0);
        if (call_fit)
            item0->SetSizeHints(parent);
    }
    
    return item0;
}
