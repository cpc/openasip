/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file OutputOperandDialog.cc
 *
 * Definition of OutputOperandDialog class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @author Tero Ryynänen 2008 (tero.ryynanen@tut.fi)
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

    operandList::iterator i = operandTypes_.find(type);

    outputTypesComboBox_->SetSelection(0);

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
