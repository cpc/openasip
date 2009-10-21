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

using std::string;
using boost::format;

BEGIN_EVENT_TABLE(OutputOperandDialog, wxDialog)
    EVT_BUTTON(wxID_OK, OutputOperandDialog::onOk)
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
             DialogPosition::getPosition(DialogPosition::DIALOG_OUTPUT_OPERAND), 
             wxSize(350, 100)),
    operand_(operand), memData_(false), index_(index) {
    
    memData_ = operand_->isMemoryData();
    createContents(this, false, true);

    outputTypesComboBox_ =
        dynamic_cast<wxChoice*>(FindWindow(ID_OPERATION_OUTPUT_TYPES));

    FindWindow(ID_MEM_DATA)->SetValidator(wxGenericValidator(&memData_));

    outputTypes_.push_back(Operand::SINT_WORD_STRING);
    outputTypes_.push_back(Operand::UINT_WORD_STRING);
    outputTypes_.push_back(Operand::FLOAT_WORD_STRING);
    outputTypes_.push_back(Operand::DOUBLE_WORD_STRING);

    operandTypes_.insert(operandPair(0, Operand::SINT_WORD));
    operandTypes_.insert(operandPair(1, Operand::UINT_WORD));
    operandTypes_.insert(operandPair(2, Operand::FLOAT_WORD));
    operandTypes_.insert(operandPair(3, Operand::DOUBLE_WORD));

    FindWindow(wxID_OK)->SetFocus();

    setTexts();
    updateTypes(operand);
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
 * Updates the type lists.
 */
void
OutputOperandDialog::updateTypes(Operand* operand_) {

    outputTypesComboBox_->Clear();
    
    for (unsigned int i = 0; i < outputTypes_.size(); i++) {
        wxString oper = WxConversion::toWxString(outputTypes_.at(i));
        outputTypesComboBox_->Append(oper);
    }

    Operand::OperandType type = operand_->type();

    outputTypesComboBox_->SetSelection(type);

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
        default:
            root->setAttribute(Operand::OPRND_TYPE, Operand::SINT_WORD_STRING);
            break;
    }
    
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

    wxCheckBox *item3 = new wxCheckBox(parent, ID_MEM_DATA, wxT("Memory data"), wxDefaultPosition, wxDefaultSize, 0);
    item1->Add(item3, 0, wxALIGN_CENTER|wxALL, 5);

    item0->Add(item1, 0, wxALIGN_CENTER|wxALL, 5);

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
