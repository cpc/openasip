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
 * @file OutputOperandDialog.cc
 *
 * Definition of OutputOperandDialog class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Tero Ryynänen 2008 (tero.ryynanen-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>
#include <wx/valgen.h>
#include <boost/format.hpp>

#include "OutputOperandDialog.hh"
#include "OSEdTextGenerator.hh"
#include "GUITextGenerator.hh"
#include "WxConversion.hh"
#include "WidgetTools.hh"
#include "DialogPosition.hh"
#include "Operand.hh"
#include "ObjectState.hh"
#include "SimValue.hh"

using std::string;
using boost::format;

BEGIN_EVENT_TABLE(OutputOperandDialog, wxDialog)
    EVT_BUTTON(wxID_OK, OutputOperandDialog::onOk)

    EVT_CHOICE(ID_OPERATION_OUTPUT_TYPES, OutputOperandDialog::onType)
    EVT_SPINCTRL(ID_ELEMENT_WIDTH, OutputOperandDialog::onElementWidth)
    EVT_CHOICE(ID_ELEMENT_COUNT, OutputOperandDialog::onElementCount)
END_EVENT_TABLE()

/**
 * Constructor.
 *
 * @param parent Parent window.
 * @param operand Operand to be added or modified.
 * @param index Index of the operand.
 */
OutputOperandDialog::OutputOperandDialog(
    wxWindow* parent, 
    Operand* operand,
    int index) :
    wxDialog(parent, -1, _T(""), 
             DialogPosition::getPosition(DialogPosition::DIALOG_OUTPUT_OPERAND)),
    operand_(operand), memData_(false), index_(index) {
    
    memData_ = operand_->isMemoryData();
    createContents(this, true, true);

    outputTypesComboBox_ =
        dynamic_cast<wxChoice*>(FindWindow(ID_OPERATION_OUTPUT_TYPES));

    elementWidthSpinCtrl_ =
        dynamic_cast<wxSpinCtrl*>(FindWindow(ID_ELEMENT_WIDTH));

    elementCountChoice_ =
        dynamic_cast<wxChoice*>(FindWindow(ID_ELEMENT_COUNT));

    FindWindow(ID_MEM_DATA)->SetValidator(wxGenericValidator(&memData_));

    outputTypes_.push_back(Operand::SINT_WORD_STRING);
    outputTypes_.push_back(Operand::UINT_WORD_STRING);
    outputTypes_.push_back(Operand::FLOAT_WORD_STRING);
    outputTypes_.push_back(Operand::DOUBLE_WORD_STRING);
    outputTypes_.push_back(Operand::HALF_FLOAT_WORD_STRING);
    outputTypes_.push_back(Operand::BOOL_STRING);
    outputTypes_.push_back(Operand::RAW_DATA_STRING);
    outputTypes_.push_back(Operand::SLONG_WORD_STRING);
    outputTypes_.push_back(Operand::ULONG_WORD_STRING);

    operandTypes_.insert(operandPair(0, Operand::SINT_WORD));
    operandTypes_.insert(operandPair(1, Operand::UINT_WORD));
    operandTypes_.insert(operandPair(2, Operand::FLOAT_WORD));
    operandTypes_.insert(operandPair(3, Operand::DOUBLE_WORD));
    operandTypes_.insert(operandPair(4, Operand::HALF_FLOAT_WORD));
    operandTypes_.insert(operandPair(5, Operand::BOOL));
    operandTypes_.insert(operandPair(6, Operand::RAW_DATA));
    operandTypes_.insert(operandPair(7, Operand::SLONG_WORD));
    operandTypes_.insert(operandPair(8, Operand::ULONG_WORD));

    FindWindow(wxID_OK)->SetFocus();

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
OutputOperandDialog::~OutputOperandDialog() {
    int x, y;
    GetPosition(&x, &y);
    wxPoint point(x, y);
    DialogPosition::setPosition(DialogPosition::DIALOG_OUTPUT_OPERAND, point);
}

/**
 * Event handler for operand type choice box.
**/
void 
OutputOperandDialog::onType(wxCommandEvent&) {

    type_ = outputTypesComboBox_->GetSelection();

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
OutputOperandDialog::onElementWidth(wxSpinEvent&) {
    elemWidth_ = elementWidthSpinCtrl_->GetValue();
    // update choice box list cells
    updateElementCounts();
}

/**
 * Event handler for element count choice box.
**/
void 
OutputOperandDialog::onElementCount(wxCommandEvent&) {
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
 */
void
OutputOperandDialog::updateTypes() {

    outputTypesComboBox_->Clear();
    
    for (unsigned int i = 0; i < outputTypes_.size(); i++) {
        wxString oper = WxConversion::toWxString(outputTypes_.at(i));
        outputTypesComboBox_->Append(oper);
    }

    outputTypesComboBox_->SetSelection(type_);

}

/**
 * Updates the element width choice box list.
**/
void
OutputOperandDialog::updateElementWidths() {

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
OutputOperandDialog::updateElementCounts() {

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
 * Set texts to widgets.
 */
void
OutputOperandDialog::setTexts() {
	
    GUITextGenerator& guiText = *GUITextGenerator::instance();
    OSEdTextGenerator& osedText = OSEdTextGenerator::instance();

    // title
    format fmt = 
        osedText.text(OSEdTextGenerator::TXT_OUTPUT_OPERAND_DIALOG_TITLE);
    fmt % index_;
    SetTitle(WxConversion::toWxString(fmt.str()));

    // buttons
    WidgetTools::setLabel(&guiText, FindWindow(wxID_OK),
                          GUITextGenerator::TXT_BUTTON_OK);

    WidgetTools::setLabel(&guiText, FindWindow(wxID_CANCEL),
                          GUITextGenerator::TXT_BUTTON_CANCEL);

    // check boxes
    WidgetTools::setLabel(&osedText, FindWindow(ID_MEM_DATA),
                          OSEdTextGenerator::TXT_CHECKBOX_MEM_DATA);
}

/**
 * Handles the event when OK button is pushed.
 */
void
OutputOperandDialog::onOk(wxCommandEvent&) {
    TransferDataFromWindow();
    ObjectState* root = new ObjectState("");
    root->setAttribute(Operand::OPRND_ID, operand_->index());

    int selected = outputTypesComboBox_->GetSelection();
    Operand::OperandType type = operandTypes_[selected];

    switch(type)
    {
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
            root->setAttribute(Operand::OPRND_TYPE, Operand::RAW_DATA_STRING);
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

    root->setAttribute(Operand::OPRND_MEM_ADDRESS, false);
    root->setAttribute(Operand::OPRND_MEM_DATA, memData_);
    
    operand_->loadState(root);
    delete root;
    EndModal(wxID_OK);
}

/**
 * Creates the contents of the dialog.
 *
 * NOTE! This function was generated by wxDesigner. This is why it may be 
 *       ugly.
 *
 * @param parent Parent window.
 * @param call_fit If true fits the contents inside the dialog. 
 * @param set_sizer If true, sets the main sizer as dialog contents.
 * @return The created sizer.
 */
wxSizer*
OutputOperandDialog::createContents(
    wxWindow *parent, 
    bool call_fit, 
    bool set_sizer) {

    wxBoxSizer *item0 = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer *item1 = new wxBoxSizer(wxHORIZONTAL);

    wxString strs9[] = 
        {
            wxT("id: 1")
        };
    
    // Choice for input operand types
    wxChoice *itemOutputTypes = new wxChoice(parent, ID_OPERATION_OUTPUT_TYPES, wxDefaultPosition, wxSize(100,-1), 1, strs9);
    item1->Add(itemOutputTypes, 0, wxALIGN_CENTER|wxALL, 5);

    wxStaticText *itemTextWidth = new wxStaticText(parent, ID_TEXT_WIDTH, wxT("Element width:"), wxDefaultPosition, wxDefaultSize, 0);
    item1->Add(itemTextWidth, 0, wxALIGN_CENTER|wxALL, 5);
    wxSpinCtrl *itemElemWidth = new wxSpinCtrl(parent, ID_ELEMENT_WIDTH, wxT(""), wxDefaultPosition, wxSize(70,-1), 1);
    item1->Add(itemElemWidth, 0, wxALIGN_CENTER|wxALL, 5);
    wxStaticText *itemTextCount = new wxStaticText(parent, ID_TEXT_COUNT, wxT("Element count:"), wxDefaultPosition, wxDefaultSize, 0);
    item1->Add(itemTextCount, 0, wxALIGN_CENTER|wxALL, 5);
    wxChoice *itemElemCount = new wxChoice(parent, ID_ELEMENT_COUNT, wxDefaultPosition, wxSize(70,-1), 1, strs9);
    item1->Add(itemElemCount, 0, wxALIGN_CENTER|wxALL, 5);

    wxBoxSizer *item1b = new wxBoxSizer(wxHORIZONTAL);
    wxCheckBox *item3 = new wxCheckBox(parent, ID_MEM_DATA, wxT("Memory data"), wxDefaultPosition, wxDefaultSize, 0);
    item1b->Add(item3, 0, wxALIGN_CENTER|wxALL, 5);

    item0->Add(item1, 0, wxALIGN_CENTER|wxALL, 5);
    item0->Add(item1b, 0, wxALIGN_CENTER|wxALL, 5);

    wxBoxSizer *item4 = new wxBoxSizer(wxHORIZONTAL);

    wxButton *item5 = new wxButton(parent, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0);
    item4->Add(item5, 0, wxALIGN_CENTER|wxALL, 5);

    wxButton *item6 = new wxButton(parent, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0);
    item4->Add(item6, 0, wxALIGN_CENTER|wxALL, 5);

    item0->Add(item4, 0, wxALIGN_CENTER|wxALL, 5);

    if (set_sizer)
    {
        parent->SetSizer(item0);
        if (call_fit)
            item0->SetSizeHints(parent);
    }
    
    return item0;
}
