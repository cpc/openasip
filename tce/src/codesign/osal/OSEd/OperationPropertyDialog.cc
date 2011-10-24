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
 * @file OperationPropertyDialog.cc
 *
 * Declaration of OperationProperty class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Mikael Lepistö 2007 (mikael.lepisto-no.spam-cs.tut.fi)
 * @author Tero Ryynänen 2008 (tero.ryynanen-no.spam-tut.fi)
 * @note rating: red
 */

#include <wx/valgen.h>
#include <boost/format.hpp>
#include <fstream>
#include <iostream>
#include <string>

#include "OperationPropertyDialog.hh"
#include "OperationDAGDialog.hh"
#include "OperationContainer.hh"
#include "WxConversion.hh"
#include "WidgetTools.hh"
#include "InputOperandDialog.hh"
#include "OutputOperandDialog.hh"
#include "OSEdConstants.hh"
#include "Application.hh"
#include "GUITextGenerator.hh"
#include "OSEdTextGenerator.hh"
#include "ErrorDialog.hh"
#include "ConfirmDialog.hh"
#include "CommandThread.hh"
#include "DialogPosition.hh"
#include "OSEd.hh"
#include "StringTools.hh"
#include "OperationIndex.hh"
#include "Operation.hh"
#include "Operand.hh"
#include "OperationModule.hh"
#include "ObjectState.hh"
#include "WarningDialog.hh"
#include "OperationDAG.hh"
#include "TCEString.hh"
#include "FileSystem.hh"

using std::string;
using std::vector;
using std::ifstream;
using boost::format;

BEGIN_EVENT_TABLE(OperationPropertyDialog, wxDialog)
    EVT_LIST_ITEM_SELECTED(ID_AFFECTED_BY, OperationPropertyDialog::onSelection)
    EVT_LIST_ITEM_DESELECTED(ID_AFFECTED_BY, OperationPropertyDialog::onSelection)

    EVT_LIST_ITEM_SELECTED(ID_AFFECTS, OperationPropertyDialog::onSelection)
    EVT_LIST_ITEM_DESELECTED(ID_AFFECTS, OperationPropertyDialog::onSelection)

    EVT_LIST_ITEM_SELECTED(ID_INPUT_OPERANDS, OperationPropertyDialog::onSelection)
    EVT_LIST_ITEM_DESELECTED(ID_INPUT_OPERANDS, OperationPropertyDialog::onSelection)

    EVT_LIST_ITEM_SELECTED(ID_OUTPUT_OPERANDS, OperationPropertyDialog::onSelection)
    EVT_LIST_ITEM_DESELECTED(ID_OUTPUT_OPERANDS, OperationPropertyDialog::onSelection)

    EVT_BUTTON(ID_AFFECTED_ADD_BUTTON, OperationPropertyDialog::onAddAffectedBy)
    EVT_BUTTON(ID_AFFECTED_DELETE_BUTTON, OperationPropertyDialog::onDeleteAffectedBy)

    EVT_BUTTON(ID_AFFECTS_ADD_BUTTON, OperationPropertyDialog::onAddAffects)
    EVT_BUTTON(ID_AFFECTS_DELETE_BUTTON, OperationPropertyDialog::onDeleteAffects)

    EVT_BUTTON(ID_INPUT_ADD_BUTTON, OperationPropertyDialog::onAddInputOperand)
    EVT_BUTTON(ID_INPUT_MODIFY_BUTTON, OperationPropertyDialog::onModifyInputOperand)
    EVT_BUTTON(ID_INPUT_DELETE_BUTTON, OperationPropertyDialog::onDeleteInputOperand)

    EVT_BUTTON(ID_OUTPUT_ADD_BUTTON, OperationPropertyDialog::onAddOutputOperand)
    EVT_BUTTON(ID_OUTPUT_MODIFY_BUTTON, OperationPropertyDialog::onModifyOutputOperand)
    EVT_BUTTON(ID_OUTPUT_DELETE_BUTTON, OperationPropertyDialog::onDeleteOutputOperand)

    EVT_BUTTON(ID_INPUT_UP_BUTTON, OperationPropertyDialog::onMoveInputUp)
    EVT_BUTTON(ID_INPUT_DOWN_BUTTON, OperationPropertyDialog::onMoveInputDown)

    EVT_BUTTON(ID_OUTPUT_UP_BUTTON, OperationPropertyDialog::onMoveOutputUp)
    EVT_BUTTON(ID_OUTPUT_DOWN_BUTTON, OperationPropertyDialog::onMoveOutputDown)

    EVT_BUTTON(ID_OPEN_BUTTON, OperationPropertyDialog::onOpen)
    EVT_BUTTON(ID_DAG_BUTTON, OperationPropertyDialog::onOpenDAG)
    EVT_BUTTON(ID_OK_BUTTON, OperationPropertyDialog::onOk)

END_EVENT_TABLE()

/**
 * Constructor.
 *
 * @param parent The parent window.
 * @param op Operation to be created or modified.
 * @param module Module in which operation belongs to.
 * @param path Path in which module belongs to.
 */
 OperationPropertyDialog::OperationPropertyDialog(
     wxWindow* parent,
     Operation* op, 
     OperationModule& module,
     const std::string& path) :
     wxDialog(
         parent, -1, _T(""), 
         DialogPosition::getPosition(DialogPosition::DIALOG_PROPERTIES), 
         wxDefaultSize, wxRESIZE_BORDER),
     operation_(op), name_(_T("")), module_(module), path_(path),
     operationWasCreatedHere_(false) {
    
    createContents(this, true, true);
    
    affectedByList_ = dynamic_cast<wxListCtrl*>(FindWindow(ID_AFFECTED_BY));
    affectsList_ = dynamic_cast<wxListCtrl*>(FindWindow(ID_AFFECTS));
    inputOperandList_ =
        dynamic_cast<wxListCtrl*>(FindWindow(ID_INPUT_OPERANDS));
    outputOperandList_ =
        dynamic_cast<wxListCtrl*>(FindWindow(ID_OUTPUT_OPERANDS));
    affectedByChoice_ =
        dynamic_cast<wxComboBox*>(FindWindow(ID_OPERATION_AFFECTED_BY));
    affectsChoice_ =
        dynamic_cast<wxComboBox*>(FindWindow(ID_OPERATION_AFFECTS));
    readMemoryCB_ = dynamic_cast<wxCheckBox*>(FindWindow(ID_READS_MEMORY));
    writeMemoryCB_ = dynamic_cast<wxCheckBox*>(FindWindow(ID_WRITES_MEMORY));
    canTrapCB_ = dynamic_cast<wxCheckBox*>(FindWindow(ID_CAN_TRAP));
    sideEffectsCB_ =
        dynamic_cast<wxCheckBox*>(FindWindow(ID_HAS_SIDE_EFFECTS));
    clockedCB_ =
        dynamic_cast<wxCheckBox*>(FindWindow(ID_CLOCKED));
    
    editDescription_ = dynamic_cast<wxTextCtrl*>(FindWindow(ID_EDIT_DESCRIPTION));

    FindWindow(ID_NAME)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &name_));

    FindWindow(ID_READS_MEMORY)->SetValidator(
        wxGenericValidator(&readMemory_));

    FindWindow(ID_WRITES_MEMORY)->SetValidator(
        wxGenericValidator(&writeMemory_));

    FindWindow(ID_CAN_TRAP)->SetValidator(wxGenericValidator(&canTrap_));

    FindWindow(ID_HAS_SIDE_EFFECTS)->SetValidator(
        wxGenericValidator(&hasSideEffects_));

    FindWindow(ID_CLOCKED)->SetValidator(
        wxGenericValidator(&clocked_));

    // set OK button as default choice
    FindWindow(ID_OK_BUTTON)->SetFocus();

    if (operation_ != NULL) {
        for (int i = 1; i <= operation_->numberOfInputs(); i++) {
            Operand* op = new Operand(operation_->operand(i));
            inputOperands_.push_back(op);
        }
        
        for (int i = operation_->numberOfInputs() + 1;
             i <= operation_->numberOfInputs() + operation_->numberOfOutputs(); i++) {
            
            Operand* op = new Operand(operation_->operand(i));
            outputOperands_.push_back(op);
        }
        
        for (int i = 0; i < operation_->affectsCount(); i++) {
            affects_.push_back(operation_->affects(i));
        }
        
        for (int i = 0; i < operation_->affectedByCount(); i++) {
            affectedBy_.push_back(operation_->affectedBy(i));
        }
    } else {
        operation_ = new Operation("", NullOperationBehavior::instance());
        operationWasCreatedHere_ = true;
    }

    orig_ = operation_->saveState(); // save original operation's state

    setTexts();
}

/**
 * Destructor.
 */
OperationPropertyDialog::~OperationPropertyDialog() {

    for (unsigned int i = 0; i < inputOperands_.size(); i++) {
        delete inputOperands_[i];
    }

    for (unsigned int i = 0; i < outputOperands_.size(); i++) {
        delete outputOperands_[i];
    }

    int x, y;
    GetPosition(&x, &y);
    wxPoint point(x, y);
    DialogPosition::setPosition(DialogPosition::DIALOG_PROPERTIES, point);

    if (operationWasCreatedHere_) {
        delete operation_;
        operation_ = NULL;
    }
}

/**
 * Returns operation object of dialog.
 * 
 * @return Operation object of dialog.
 */
Operation* 
OperationPropertyDialog::operation() const {
    return operation_;
}

/**
 * Set texts to all widgets.
 */
void
OperationPropertyDialog::setTexts() {
	
    GUITextGenerator& guiText = *GUITextGenerator::instance();
    OSEdTextGenerator& osedText = OSEdTextGenerator::instance();

    // title
    format fmt = osedText.text(OSEdTextGenerator::TXT_PROPERTY_DIALOG_TITLE);
    SetTitle(WxConversion::toWxString(fmt.str()));

    WidgetTools::setLabel(&osedText, FindWindow(ID_TEXT_NAME),
                          OSEdTextGenerator::TXT_LABEL_OPERATION_NAME);

    // buttons
    WidgetTools::setLabel(&guiText, FindWindow(ID_OK_BUTTON),
                          GUITextGenerator::TXT_BUTTON_OK);

    WidgetTools::setLabel(&guiText, FindWindow(wxID_CANCEL),
                          GUITextGenerator::TXT_BUTTON_CANCEL);

    WidgetTools::setLabel(&guiText, FindWindow(ID_INPUT_ADD_BUTTON),
                          GUITextGenerator::TXT_BUTTON_ADD_DIALOG);

    WidgetTools::setLabel(&guiText, FindWindow(ID_INPUT_DELETE_BUTTON),
                          GUITextGenerator::TXT_BUTTON_DELETE);

    WidgetTools::setLabel(&guiText, FindWindow(ID_OUTPUT_ADD_BUTTON),
                          GUITextGenerator::TXT_BUTTON_ADD_DIALOG);

    WidgetTools::setLabel(&guiText, FindWindow(ID_OUTPUT_DELETE_BUTTON),
                          GUITextGenerator::TXT_BUTTON_DELETE);

    WidgetTools::setLabel(&guiText, FindWindow(ID_AFFECTS_ADD_BUTTON),
                          GUITextGenerator::TXT_BUTTON_ADD);
	
    WidgetTools::setLabel(&guiText, FindWindow(ID_AFFECTS_DELETE_BUTTON),
                          GUITextGenerator::TXT_BUTTON_DELETE);

    WidgetTools::setLabel(&guiText, FindWindow(ID_AFFECTED_ADD_BUTTON),
                          GUITextGenerator::TXT_BUTTON_ADD);

    WidgetTools::setLabel(&guiText, FindWindow(ID_AFFECTED_DELETE_BUTTON),
                          GUITextGenerator::TXT_BUTTON_DELETE);

    WidgetTools::setLabel(&osedText, FindWindow(ID_INPUT_MODIFY_BUTTON),
                          OSEdTextGenerator::TXT_BUTTON_MODIFY);

    WidgetTools::setLabel(&osedText, FindWindow(ID_OUTPUT_MODIFY_BUTTON),
                          OSEdTextGenerator::TXT_BUTTON_MODIFY);

    WidgetTools::setLabel(&osedText, FindWindow(ID_OPEN_BUTTON),
                              OSEdTextGenerator::TXT_BUTTON_OPEN);

	
    setBehaviorLabel();

    // column titles
    fmt = osedText.text(OSEdTextGenerator::TXT_COLUMN_OPERATION);
    affectedByList_->InsertColumn(
        0, WxConversion::toWxString(fmt.str()), wxLIST_FORMAT_LEFT,
        OSEdConstants::DEFAULT_COLUMN_WIDTH);

    affectsList_->InsertColumn(
        0, WxConversion::toWxString(fmt.str()), wxLIST_FORMAT_LEFT,
        OSEdConstants::DEFAULT_COLUMN_WIDTH);
    
    fmt = osedText.text(OSEdTextGenerator::TXT_COLUMN_TYPE);
	
    inputOperandList_->InsertColumn(
        0, WxConversion::toWxString(fmt.str()), wxLIST_FORMAT_LEFT,
        100);
    

    fmt = osedText.text(OSEdTextGenerator::TXT_COLUMN_OPERAND);

    inputOperandList_->InsertColumn(
        0, WxConversion::toWxString(fmt.str()), wxLIST_FORMAT_LEFT,
        OSEdConstants::DEFAULT_COLUMN_WIDTH);

    fmt = osedText.text(OSEdTextGenerator::TXT_COLUMN_TYPE);
	
    outputOperandList_->InsertColumn(
        0, WxConversion::toWxString(fmt.str()), wxLIST_FORMAT_LEFT,
        100);

    fmt = osedText.text(OSEdTextGenerator::TXT_COLUMN_OPERAND);
    
    outputOperandList_->InsertColumn(
        0, WxConversion::toWxString(fmt.str()), wxLIST_FORMAT_LEFT,
        OSEdConstants::DEFAULT_COLUMN_WIDTH);

    // check boxes
    WidgetTools::setLabel(&osedText, FindWindow(ID_READS_MEMORY),
                          OSEdTextGenerator::TXT_CHECKBOX_READS_MEMORY);

    WidgetTools::setLabel(&osedText, FindWindow(ID_WRITES_MEMORY),
                          OSEdTextGenerator::TXT_CHECKBOX_WRITES_MEMORY);

    WidgetTools::setLabel(&osedText, FindWindow(ID_CAN_TRAP),
                          OSEdTextGenerator::TXT_CHECKBOX_CAN_TRAP);

    WidgetTools::setLabel(&osedText, FindWindow(ID_HAS_SIDE_EFFECTS),
                          OSEdTextGenerator::TXT_CHECKBOX_HAS_SIDE_EFFECTS);

    WidgetTools::setLabel(&osedText, FindWindow(ID_CLOCKED),
                          OSEdTextGenerator::TXT_CHECKBOX_CLOCKED);
	
    // box sizers
    fmt = osedText.text(OSEdTextGenerator::TXT_BOX_AFFECTS);
    WidgetTools::setWidgetLabel(affectsSizer_, fmt.str());

    fmt = osedText.text(OSEdTextGenerator::TXT_BOX_AFFECTED_BY);
    WidgetTools::setWidgetLabel(affectedBySizer_, fmt.str());

    fmt = osedText.text(OSEdTextGenerator::TXT_BOX_INPUT_OPERANDS);
    WidgetTools::setWidgetLabel(inputSizer_, fmt.str());

    fmt = osedText.text(OSEdTextGenerator::TXT_BOX_OUTPUT_OPERANDS);
    WidgetTools::setWidgetLabel(outputSizer_, fmt.str());


}

/**
 * Sets the has behavior label.
 */
void
OperationPropertyDialog::setBehaviorLabel() {
    OSEdTextGenerator& osedText = OSEdTextGenerator::instance();
    if (module_.hasBehaviorSource()) {
        WidgetTools::setLabel(&osedText, FindWindow(ID_TEXT_OPEN),
                              OSEdTextGenerator::TXT_LABEL_HAS_BEHAVIOR);
    } else {
        WidgetTools::setLabel(&osedText, FindWindow(ID_TEXT_OPEN),
                              OSEdTextGenerator::TXT_LABEL_NO_BEHAVIOR);
    }
}

/**
 * Transfers data to window.
 *
 * @return True if transfer is successful.
 */
bool
OperationPropertyDialog::TransferDataToWindow() {
    if (operation_ != NULL) {
        name_ = WxConversion::toWxString(operation_->name());
        editDescription_->Clear();
        editDescription_->AppendText(WxConversion::toWxString(operation_->description()));
        readMemory_ = operation_->readsMemory();
        writeMemory_ = operation_->writesMemory();
        canTrap_ = operation_->canTrap();
        hasSideEffects_ = operation_->hasSideEffects();
        clocked_ = operation_->isClocked();
        controlFlow_ = operation_->isControlFlowOperation();
        isCall_ = operation_->isCall();
        isBranch_ = operation_->isBranch();	
        updateOperands();
        updateAffected();	
    } else {
        readMemory_ = false;
        writeMemory_ = false;
        canTrap_ = false;
        hasSideEffects_ = false;
        clocked_ = false;
        controlFlow_ = false;
        isCall_ = false;
        isBranch_ = false;
    }

    return wxWindow::TransferDataToWindow();
}

/**
 * Updates the operand lists.
 */
void
OperationPropertyDialog::updateOperands() {

    inputOperandList_->DeleteAllItems();
    outputOperandList_->DeleteAllItems();

    for (unsigned int i = 0; i < inputOperands_.size(); i++) {
        wxString id = WxConversion::toWxString(i + 1);
        inputOperandList_->InsertItem(i, id);
        std::string type = inputOperands_.at(i)->typeString();
        inputOperandList_->SetItem(i, 1, WxConversion::toWxString(type));
    }
    int k = 0;
    for (unsigned int i = inputOperands_.size() + 1;
         i <= inputOperands_.size() + outputOperands_.size(); i++) {
		
        wxString id = WxConversion::toWxString(i);
        outputOperandList_->InsertItem(k, id);
        std::string type = outputOperands_.at(k)->typeString();
        outputOperandList_->SetItem(k, 1, WxConversion::toWxString(type));
        k++;
    }

    wxListEvent dummy;
    onSelection(dummy);
}

/**
 * Updates the affects and affected by lists.
 */
void
OperationPropertyDialog::updateAffected() {
	
    affectedByList_->DeleteAllItems();
    affectsList_->DeleteAllItems();
    
    for (unsigned int i = 0; i < affectedBy_.size(); i++) {
        wxString oper = WxConversion::toWxString(affectedBy_[i]);
        affectedByList_->InsertItem(i, oper);
    }
	
    for (unsigned int i = 0; i < affects_.size(); i++) {
        wxString oper = WxConversion::toWxString(affects_[i]);
        affectsList_->InsertItem(i, oper);
    }
	
    OperationIndex& index = OperationContainer::operationIndex();
    affectsChoice_->Clear();
    affectedByChoice_->Clear();
    for (int i = 0; i < index.pathCount(); i++) {
        string path = index.path(i);
        int modules = index.moduleCount(path);
        for (int j = 0; j < modules; j++) {
            OperationModule& module = index.module(j, path);
            int operations = 0;
            try {
                operations = index.operationCount(module);
            } catch (const Exception& e) {
                continue;
            }
            for (int k = 0; k < operations; k++) {
                string name = index.operationName(k, module);
                if (affectsList_->
                    FindItem(-1, WxConversion::toWxString(name)) == -1) {
                    
                    affectsChoice_->Append(WxConversion::toWxString(name));
                }
                if (affectedByList_->
                    FindItem(-1, WxConversion::toWxString(name)) == -1) {
                    
                    affectedByChoice_->Append(WxConversion::toWxString(name));
                }
            }
        }
	}
    affectsChoice_->SetSelection(0);
    affectedByChoice_->SetSelection(0);
    
    if (affectsChoice_->GetCount() == 0) {
        FindWindow(ID_AFFECTS_ADD_BUTTON)->Disable();
    } else {
        FindWindow(ID_AFFECTS_ADD_BUTTON)->Enable();
    }

    if (affectedByChoice_->GetCount() == 0) {
        FindWindow(ID_AFFECTED_ADD_BUTTON)->Disable();
    } else {
        FindWindow(ID_AFFECTED_ADD_BUTTON)->Enable();
    }
	
    wxListEvent dummy;
    onSelection(dummy);
}


/**
 * Handles the event when an item is selected from a list.
 *
 * Item selection affects statuses of several buttons.
 */ 
void
OperationPropertyDialog::onSelection(wxListEvent&) {
	
    if (affectedByList_->GetSelectedItemCount() < 1) {
        FindWindow(ID_AFFECTED_DELETE_BUTTON)->Disable();
    } else {
        FindWindow(ID_AFFECTED_DELETE_BUTTON)->Enable();
    }
	
    if (affectsList_->GetSelectedItemCount() < 1) {
        FindWindow(ID_AFFECTS_DELETE_BUTTON)->Disable();
    } else {
        FindWindow(ID_AFFECTS_DELETE_BUTTON)->Enable();
    }
	
    if (inputOperandList_->GetSelectedItemCount() < 1) {
        FindWindow(ID_INPUT_DELETE_BUTTON)->Disable();
    } else {
        FindWindow(ID_INPUT_DELETE_BUTTON)->Enable();
    }
    
    if (outputOperandList_->GetSelectedItemCount() < 1) {
        FindWindow(ID_OUTPUT_DELETE_BUTTON)->Disable();
    } else {
        FindWindow(ID_OUTPUT_DELETE_BUTTON)->Enable();
    }
    
    if (inputOperandList_->GetSelectedItemCount() != 1) {
        FindWindow(ID_INPUT_MODIFY_BUTTON)->Disable();
        FindWindow(ID_INPUT_UP_BUTTON)->Disable();
        FindWindow(ID_INPUT_DOWN_BUTTON)->Disable();
    } else {
        FindWindow(ID_INPUT_MODIFY_BUTTON)->Enable();
        
        string idString = WidgetTools::lcStringSelection(inputOperandList_, 0);
        int id = Conversion::toInt(idString);
        
        if (id != 1) {
            FindWindow(ID_INPUT_UP_BUTTON)->Enable();
        } else {
            FindWindow(ID_INPUT_UP_BUTTON)->Disable();
        }
        
        if (id != static_cast<int>(inputOperands_.size())) {
            FindWindow(ID_INPUT_DOWN_BUTTON)->Enable();
        } else {
            FindWindow(ID_INPUT_DOWN_BUTTON)->Disable();
        }
    }
	
    if (outputOperandList_->GetSelectedItemCount() != 1) {
        FindWindow(ID_OUTPUT_MODIFY_BUTTON)->Disable();
        FindWindow(ID_OUTPUT_UP_BUTTON)->Disable();
        FindWindow(ID_OUTPUT_DOWN_BUTTON)->Disable();
    } else {
        FindWindow(ID_OUTPUT_MODIFY_BUTTON)->Enable();
        
        string idString = WidgetTools::lcStringSelection(outputOperandList_, 0);
        int id = Conversion::toInt(idString);
        
        if (id != static_cast<int>(inputOperands_.size()) + 1) {
            FindWindow(ID_OUTPUT_UP_BUTTON)->Enable();
        } else {
            FindWindow(ID_OUTPUT_UP_BUTTON)->Disable();
        }
        
        if (id != static_cast<int>(inputOperands_.size()) + 
            static_cast<int>(outputOperands_.size())) {
            FindWindow(ID_OUTPUT_DOWN_BUTTON)->Enable();
        } else {
            FindWindow(ID_OUTPUT_DOWN_BUTTON)->Disable();
        }
    }
}

/**
 * Handles adding new item to affected by list.
 */
void
OperationPropertyDialog::onAddAffectedBy(wxCommandEvent&) {
   
    wxString name = affectedByChoice_->GetValue();
    if (!OperationContainer::operationExists(WxConversion::toString(name))) {
        
        OSEdTextGenerator& texts = OSEdTextGenerator::instance();
        format fmt = texts.text(
            OSEdTextGenerator::TXT_ERROR_NON_EXISTING_OPERATION);
        fmt % name;
        ErrorDialog dialog(this, WxConversion::toWxString(fmt.str()));
        dialog.ShowModal();
    } else {
        affectedBy_.push_back(WxConversion::toString(name));
        updateAffected();
    }
}

/**
 * Handles deleting an item from affected by list.
 */
void
OperationPropertyDialog::onDeleteAffectedBy(wxCommandEvent&) {
   
    vector<string> toBeDeleted = getSelectedItems(affectedByList_);
 
    for (size_t i = 0; i < toBeDeleted.size(); i++) {
        vector<string>::iterator it = affectedBy_.begin();
        while (it != affectedBy_.end()) {
            if (*it == toBeDeleted[i]) {
                affectedBy_.erase(it);
                break;
            }
            it++;
        }
    }
    updateAffected();
}

/**
 * Handles event of adding new item to affects list.
 */
void
OperationPropertyDialog::onAddAffects(wxCommandEvent&) {
   
    wxString name = affectsChoice_->GetValue();
    if (!OperationContainer::operationExists(WxConversion::toString(name))) {
        
        OSEdTextGenerator& texts = OSEdTextGenerator::instance();
        format fmt = texts.text(
            OSEdTextGenerator::TXT_ERROR_NON_EXISTING_OPERATION);
        fmt % name;
        ErrorDialog dialog(this, WxConversion::toWxString(fmt.str()));
        dialog.ShowModal();
    } else {
        affects_.push_back(WxConversion::toString(name));
        updateAffected();
    }
}

/**
 * Handles event of deleting item from affects list.
 */
void
OperationPropertyDialog::onDeleteAffects(wxCommandEvent&) {
 
    vector<string> toBeDeleted = getSelectedItems(affectsList_);
 
    for (size_t i = 0; i < toBeDeleted.size(); i++) {
        vector<string>::iterator it = affects_.begin();
        while (it != affects_.end()) {
            if (*it == toBeDeleted[i]) {
                affects_.erase(it);
                break;
            }
            it++;
        }
    }
    updateAffected();
}

/**
 * Handles event for adding new operand to input operand list.
 */
void
OperationPropertyDialog::onAddInputOperand(wxCommandEvent&) {    
    Operand* input = 
        new Operand(true, inputOperands_.size() + 1, Operand::SINT_WORD);

    InputOperandDialog dialog(
        this, input, inputOperands_.size(), inputOperands_.size() + 1);

    if (dialog.ShowModal() == wxID_OK) {
        inputOperands_.push_back(input);
        updateOperands();
    } else {
        delete input;
    }
    updateOperands();
}

/**
 * Handles the event for modifying input operand.
 */
void
OperationPropertyDialog::onModifyInputOperand(wxCommandEvent&) {
    string idString = WidgetTools::lcStringSelection(inputOperandList_, 0);
    int id = Conversion::toInt(idString);
    Operand* operand = NULL;
    operand = inputOperands_[id - 1];
    assert(operand != NULL);
    InputOperandDialog dialog(this, operand, inputOperands_.size(), id);
    dialog.ShowModal();
    updateOperands();
}

/**
 * Handles the event of deleting input operand.
 */
void
OperationPropertyDialog::onDeleteInputOperand(wxCommandEvent&) {
   
    vector<string> toBeDeleted = getSelectedItems(inputOperandList_);
    size_t i = 0;
    vector<Operand*>::iterator it = inputOperands_.begin();
    int index = 1;
    
    updateSwapLists(toBeDeleted);
    while (it != inputOperands_.end() && i < toBeDeleted.size()) {
        int id = Conversion::toInt(toBeDeleted[i]);
        while (index < id) {
            it++;
            index++;
        }
        delete *it;
        it = inputOperands_.erase(it);
        index++;
        i++;
    }
    updateOperands();
}

/**
 * Handles the event of adding new output operand.
 */
void
OperationPropertyDialog::onAddOutputOperand(wxCommandEvent&) {
    unsigned int id = inputOperands_.size() + outputOperands_.size() + 1;
    Operand* output = new Operand(false, id, Operand::SINT_WORD);
    OutputOperandDialog dialog(this, output, id);
    if (dialog.ShowModal() == wxID_OK) {
        outputOperands_.push_back(output);
        updateOperands();
    } else {
        delete output;
    }
    updateOperands();
}

/**
 * Handles the event for mofifying output operand.
 */
void
OperationPropertyDialog::onModifyOutputOperand(wxCommandEvent&) {
    string idString = WidgetTools::lcStringSelection(outputOperandList_, 0);
    int realId = Conversion::toInt(idString);
    int id = realId - static_cast<int>(inputOperands_.size());
    Operand* operand = NULL;
    operand = outputOperands_[id - 1];
    assert(operand != NULL);
    OutputOperandDialog dialog(this, operand, realId);
    dialog.ShowModal();
    updateOperands();
}

/**
 * Handles the event of deleting output operand.
 */
void
OperationPropertyDialog::onDeleteOutputOperand(wxCommandEvent&) {
       
    vector<string> toBeDeleted = getSelectedItems(outputOperandList_);
    size_t i = 0;
    vector<Operand*>::iterator it = outputOperands_.begin();
    int index = inputOperands_.size() + 1;
   
    while (it != outputOperands_.end() && i < toBeDeleted.size()) {
        int id = Conversion::toInt(toBeDeleted[i]);
        while (index < id) {
            it++;
            index++;
        }
        delete *it;
        it = outputOperands_.erase(it);
        index++;
        i++;
    }
    updateOperands();
}

/**
 * Handles the event when input operand is moved up.
 */
void
OperationPropertyDialog::onMoveInputUp(wxCommandEvent&) {
    string idString = WidgetTools::lcStringSelection(inputOperandList_, 0);
    int id = Conversion::toInt(idString);
    moveUp(inputOperands_, id, inputOperandList_);
}

/**
 * Handles the event when input operand is moved down.
 */
void
OperationPropertyDialog::onMoveInputDown(wxCommandEvent&) {
    string idString = WidgetTools::lcStringSelection(inputOperandList_, 0);
    int id = Conversion::toInt(idString);
    moveDown(inputOperands_, id, inputOperandList_);
}

/**
 * Handles the event when output operand is moved up.
 */
void
OperationPropertyDialog::onMoveOutputUp(wxCommandEvent&) {
    string idString = WidgetTools::lcStringSelection(outputOperandList_, 0);
    int id = Conversion::toInt(idString) - 
        static_cast<int>(inputOperands_.size());
    moveUp(outputOperands_, id, outputOperandList_);
}

/**
 * Handles the event when output operand is moved down.
 */
void
OperationPropertyDialog::onMoveOutputDown(wxCommandEvent&) {
    string idString = WidgetTools::lcStringSelection(outputOperandList_, 0);
    int id = Conversion::toInt(idString) - 
        static_cast<int>(inputOperands_.size());
    moveDown(outputOperands_, id, outputOperandList_);
}

/**
 * Handles the event when Open button is pushed.
 */
void
OperationPropertyDialog::onOpen(wxCommandEvent&) {
	
    OSEdTextGenerator& texts = OSEdTextGenerator::instance();
    OSEdOptions* options = wxGetApp().options();
    string editor = options->editor();
    if (editor == "") {
        format fmt = texts.text(OSEdTextGenerator::TXT_ERROR_NO_EDITOR);
        ErrorDialog error(this, WxConversion::toWxString(fmt.str()));
        error.ShowModal();
    } else {
        if (FileSystem::fileExists(editor)) {
            if (module_.hasBehaviorSource()) {
                string code = module_.behaviorSourceModule();
                string cmd = editor + " " + code;
                
                launchEditor(cmd);
            } else {
                // new behavior is added for the module
                string code = module_.propertiesModule();
                size_t pos = code.find_last_of(".");
                code.erase(pos);
                code += ".cc";

                // copy template file as new file
                string dir = 
                    Environment::dataDirPath(
                        WxConversion::toString(
                            OSEdConstants::APPLICATION_NAME));

                string templateFile = dir + FileSystem::DIRECTORY_SEPARATOR +
                    OSEdConstants::BEHAVIOR_TEMPLATE_FILE_NAME;

                FileSystem::copy(templateFile, code);

                string cmd = editor + " " + code;
                launchEditor(cmd);
            }
        } else {
            format fmt = texts.text(OSEdTextGenerator::TXT_ERROR_OPEN);
            fmt % editor;
            ErrorDialog error(this, WxConversion::toWxString(fmt.str()));
            error.ShowModal();
        }
    }
    
    setBehaviorLabel();
}

/**
 * Handles the event when Open DAG button is pushed.
 */
void
OperationPropertyDialog::onOpenDAG(wxCommandEvent&) {
    OperationDAGDialog dialog(this, operation_);
    dialog.ShowModal();
}

/**
 * Moves operand up.
 *
 * @param list List from which operand is selected.
 * @param id Id of the operand.
 * @param ops Operands.
 */
void
OperationPropertyDialog::moveUp(
    std::vector<Operand*>& ops, 
    int id,
    wxListCtrl* list) {
    
    if (id != 1) {
        Operand* temp = ops[id - 2];
        ops[id - 2] = ops[id - 1];
        ops[id - 1] = temp;
        long item = -1;
        item = list->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        list->SetItemState(
            item - 1, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
        list->SetItemState(item, 0, wxLIST_STATE_SELECTED);
    }
}

/**
 * Moves operand down.
 *
 * @param list List from which operand is selected.
 * @param ops Operands.
 * @param id Id of the operand.
 */
void
OperationPropertyDialog::moveDown(
    std::vector<Operand*>& ops, 
    int id,
    wxListCtrl* list) {
    
    if (static_cast<unsigned int>(id) != ops.size()) {
        Operand* temp = ops[id - 1];
        ops[id - 1] = ops[id];
        ops[id] = temp;
        long item = -1;
        item = list->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        list->SetItemState(
            item + 1, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
        list->SetItemState(item, 0, wxLIST_STATE_SELECTED);
    }
}

/**
 * Returns all the selected items of the list control.
 *
 * @param listCtrl List control which is investigated.
 * @return All selected items as a vector.
 */
vector<string>
OperationPropertyDialog::getSelectedItems(wxListCtrl* listCtrl) {
    vector<string> selected;
    long item = -1;
    for (;;) {
        item = listCtrl->GetNextItem(
            item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        if (item == -1) {
            break;
        }
        wxListItem info;
        info.SetId(item);
        info.SetColumn(0);
        listCtrl->GetItem(info);
        selected.push_back(WxConversion::toString(info.GetText()));
    }
    return selected;
}

/**
 * Handles the event when OK button is pushed.
 */
void
OperationPropertyDialog::onOk(wxCommandEvent&) {
    TransferDataFromWindow();
    string opName = 
        StringTools::stringToUpper(WxConversion::toString(name_));

    OSEdTextGenerator& texts = OSEdTextGenerator::instance();
    
    if (name_ == _T("") && operation_->name() == "") {
        format fmt = texts.text(OSEdTextGenerator::TXT_ERROR_NO_NAME);
        fmt % "operation";
        WarningDialog dialog(this, WxConversion::toWxString(fmt.str()));
        dialog.ShowModal();
    } else {
                      
        // let's check there isn't already an operation by the same name
        if (operation_ != NULL and operation_->name() != opName) {
            Operation* op = 
                OperationContainer::operation(path_, module_.name(), opName);

            if (op != NULL) {
                format fmt = 
                    texts.text(OSEdTextGenerator::TXT_ERROR_OPERATION_EXISTS);
                fmt % WxConversion::toString(name_);
                WarningDialog dialog(this, WxConversion::toWxString(fmt.str()));
                dialog.ShowModal();
                return;
            }
        }
        
        ObjectState* mod = saveOperation(); // load modified operation's settings from gui
        ObjectState* orig = orig_;

        if (*orig != *mod) {
            format fmt = texts.text(
                OSEdTextGenerator::TXT_QUESTION_SAVE_PROPERTIES);
            ConfirmDialog dialog(this, WxConversion::toWxString(fmt.str()));
            int ans = dialog.ShowModal();
            if (ans == wxID_YES) {
                try {
                    operation_->loadState(mod);
                } catch (ObjectStateLoadingException& e) {
                    std::cerr << "Exception caught: " << e.errorMessage() << std::endl; 
                    assert(false);
                }
            }
            delete orig;
            delete mod;
            if (ans == wxID_YES) {
                EndModal(wxID_OK);
            } else if (ans == wxID_NO) {
                EndModal(wxID_CANCEL);
            }
            
        } else {
            delete orig;
            delete mod;
            EndModal(wxID_OK);
        }
    }
}

/**
 * Returns the operation properties as ObjectState tree.
 *
 * @return Operation properties as ObjectState tree.
 */
ObjectState*
OperationPropertyDialog::saveOperation() {
	
    ObjectState* root = new ObjectState(Operation::OPRN_OPERATION);
    root->setAttribute(Operation::OPRN_NAME, WxConversion::toString(name_));

    std::string description("");
    wxString wxTemp;

    for (int i = 0; i < editDescription_->GetNumberOfLines(); ++i)
    {
        wxTemp = editDescription_->GetLineText(i);
#if wxCHECK_VERSION(2, 6, 0)
        std::string stdTemp(wxTemp.mb_str());
#else
        std::string stdTemp(wxTemp.c_str());
#endif
        if (!StringTools::endsWith(stdTemp, "\n"))
            stdTemp = stdTemp + "\n";
        description += stdTemp;
    }

    description = StringTools::trim(description);

    root->setAttribute(Operation::OPRN_DESCRIPTION, description);

    int inputs = inputOperands_.size();
    int outputs = outputOperands_.size();

    root->setAttribute(Operation::OPRN_INPUTS, inputs);
    root->setAttribute(Operation::OPRN_OUTPUTS, outputs);

    root->setAttribute(Operation::OPRN_TRAP, canTrap_);
    root->setAttribute(Operation::OPRN_SIDE_EFFECTS, hasSideEffects_);
    root->setAttribute(Operation::OPRN_CLOCKED, clocked_);
    root->setAttribute(Operation::OPRN_READS_MEMORY, readMemory_);
    root->setAttribute(Operation::OPRN_WRITES_MEMORY, writeMemory_);
    root->setAttribute(Operation::OPRN_CONTROL_FLOW, controlFlow_);
    root->setAttribute(Operation::OPRN_ISCALL, isCall_);    
    root->setAttribute(Operation::OPRN_ISBRANCH, isBranch_);    

    if (affectedBy_.size() > 0) {
        ObjectState* affectedBy = new ObjectState(Operation::OPRN_AFFECTED_BY);
        for (unsigned int i = 0; i < affectedBy_.size(); i++) {
            ObjectState* affectedByChild = 
                new ObjectState(Operation::OPRN_OPERATION);
            affectedByChild->setAttribute(Operation::OPRN_NAME, affectedBy_[i]);
            affectedBy->addChild(affectedByChild);
        }
        root->addChild(affectedBy);
    }

    if (affects_.size() > 0) {
        ObjectState* affects = new ObjectState(Operation::OPRN_AFFECTS);
        for (unsigned int i = 0; i < affects_.size(); i++) {
            ObjectState* affectsChild = 
                new ObjectState(Operation::OPRN_OPERATION);
            affectsChild->setAttribute(Operation::OPRN_NAME, affects_[i]);
            affects->addChild(affectsChild);
        }
        root->addChild(affects);
    }
    
    for (unsigned int i = 0; i < inputOperands_.size(); i++) {
        ObjectState* operand = inputOperands_[i]->saveState();
        operand->setName(Operation::OPRN_IN);
        int index = i + 1;
        operand->setAttribute(Operand::OPRND_ID, index);
        std::string type = inputOperands_.at(i)->typeString();
        operand->setAttribute(Operand::OPRND_TYPE, type);
        root->addChild(operand);
    }

    for (unsigned int i = 0; i < outputOperands_.size(); i++) {
        ObjectState* operand = outputOperands_[i]->saveState();
        operand->setName(Operation::OPRN_OUT);
        int index = inputOperands_.size() + i + 1;
        operand->setAttribute(Operand::OPRND_ID, index);
        std::string type = outputOperands_.at(i)->typeString();
        operand->setAttribute(Operand::OPRND_TYPE, type);
        root->addChild(operand);
    }

    // copy dags from operation to objectState tree
    for (int i = 0; i < operation_->dagCount(); i++) {     
        ObjectState* trigger = new ObjectState(Operation::OPRN_TRIGGER);
        trigger->setValue(operation_->dagCode(i));
        root->addChild(trigger);
    }

    return root;
}

/**
 * Launches editor for editing operation behavior.
 *
 * @param cmd Command for launching the editor.
 */
void
OperationPropertyDialog::launchEditor(const std::string& cmd) {
    CommandThread* thread = new CommandThread(cmd);
    thread->Create();
    thread->Run();
}

/**
 * Deletes deleted operands from swap lists.
 *
 * @param deletedOperands Deleted operands.
 */
void
OperationPropertyDialog::updateSwapLists(vector<string> deletedOperands) {
    
    for (size_t i = 0; i < inputOperands_.size(); i++) {
        Operand* operand = inputOperands_[i];
        ObjectState* state = operand->saveState();
        for (int j = 0; j < state->childCount(); j++) {
            if (state->child(j)->name() == Operand::OPRND_CAN_SWAP) {
                ObjectState* canSwap = state->child(j);
             
                int k = 0;
                while (k < canSwap->childCount()) {
                    ObjectState* child = canSwap->child(k);
                    int id = child->intAttribute(Operand::OPRND_ID);
                    string idString = Conversion::toString(id);
                    
                    bool deleted = false;
                    for (size_t r = 0; r < deletedOperands.size(); r++) {
                        if (deletedOperands[r] == idString) {
                            delete child;
                            child = NULL;
                            k--;
                            deleted = true;
                            break;
                        }
                    }
                    
                    if (!deleted) {
                        int dec = 0;
                        for (size_t r = 0; r < deletedOperands.size(); r++) {
                            int delId = Conversion::toInt(deletedOperands[r]);
                            if (delId < id) {
                                dec++;
                            }
                        }
                        id = id - dec;
                        child->setAttribute(Operand::OPRND_ID, id);
                    }
                    k++;
                }
            }
        }
        operand->loadState(state);
    }
}

/**
 * Creates the contents of the dialog.
 *
 * NOTE! This function was generated by wxDesigner, that's why it is so ugly.
 *
 * @param parent Parent window.
 * @param call_fit If true, fits the contents inside the dialog.
 * @param set_sizer If true, sets the main sizer as dialog contents.
 * @return The created sizer.
 */
wxSizer*
OperationPropertyDialog::createContents(
    wxWindow* parent, 
    bool call_fit, 
    bool set_sizer) {

    wxBoxSizer *item0 = new wxBoxSizer(wxVERTICAL);

    wxGridSizer *item1 = new wxGridSizer(2, 0, 0);

    wxBoxSizer *opHeaderSizer = new wxBoxSizer(wxHORIZONTAL);

    wxBoxSizer *nameSizer = new wxBoxSizer(wxHORIZONTAL);

    wxStaticText *item2 = new wxStaticText(parent, ID_TEXT_NAME, wxT("Name:"), wxDefaultPosition, wxDefaultSize, 0);
    nameSizer->Add(item2, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxTextCtrl *item3 = new wxTextCtrl(parent, ID_NAME, wxT(""), wxDefaultPosition, wxSize(200,-1), 0);
    nameSizer->Add(item3, 0, wxALIGN_LEFT|wxALL, 5);

    wxStaticBox *opProperties = new wxStaticBox(parent, -1, wxT("Operation properties"));
    wxStaticBoxSizer *opPropertiesContainer = new wxStaticBoxSizer(opProperties, wxVERTICAL);
    //wxStaticBoxSizer *opPropertiesContainer = new wxStaticBoxSizer(wxGROW, opProperties);

    opPropertiesContainer->Add(nameSizer, 0, wxGROW, 5);
    opPropertiesContainer->Add(item1, 0, wxGROW, 5);

    wxStaticBox *opDescription = new wxStaticBox(parent, -1, wxT("Operation description"));
    wxTextCtrl* editDescription = new wxTextCtrl(parent, ID_EDIT_DESCRIPTION, wxT(""), wxDefaultPosition, wxSize(250,50), wxTE_MULTILINE);
    wxStaticBoxSizer *opDescriptionContainer = new wxStaticBoxSizer(opDescription, wxGROW);

    opDescriptionContainer->Add(editDescription, 0, wxALL|wxEXPAND, 5);

    opHeaderSizer->Add(opPropertiesContainer, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    opHeaderSizer->Add(opDescriptionContainer, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxGROW, 5);

    wxCheckBox *item4 = new wxCheckBox(parent, ID_READS_MEMORY, wxT("Reads memory"), wxDefaultPosition, wxDefaultSize, 0);
    item1->Add(item4, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxCheckBox *item5 = new wxCheckBox(parent, ID_WRITES_MEMORY, wxT("Writes memory"), wxDefaultPosition, wxDefaultSize, 0);
    item1->Add(item5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxCheckBox *item6 = new wxCheckBox(parent, ID_CAN_TRAP, wxT("Can trap"), wxDefaultPosition, wxDefaultSize, 0);
    item1->Add(item6, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxCheckBox *item7 = new wxCheckBox(parent, ID_HAS_SIDE_EFFECTS, wxT("Has side effects"), wxDefaultPosition, wxDefaultSize, 0);
    item1->Add(item7, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxCheckBox *clocked = new wxCheckBox(parent, ID_CLOCKED, wxT("Clocked"), wxDefaultPosition, wxDefaultSize, 0);
    item1->Add(clocked, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    item0->Add(opHeaderSizer, 0, wxALIGN_CENTER|wxALL, 5);

    wxGridSizer *item8 = new wxGridSizer(2, 0, 0);

    wxStaticBox *item10 = new wxStaticBox(parent, -1, wxT("Affected by"));
    wxStaticBoxSizer *item9 = new wxStaticBoxSizer(item10, wxVERTICAL);
    affectedBySizer_ = item9;

    wxListCtrl *item11 = new wxListCtrl(parent, ID_AFFECTED_BY, wxDefaultPosition, wxSize(160,120), wxLC_REPORT|wxSUNKEN_BORDER);
    item9->Add(item11, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 10);

    wxBoxSizer *item12 = new wxBoxSizer(wxHORIZONTAL);

    wxString strs13[] = 
    {
        wxT("ComboItem")
    };
    wxComboBox *item13 = new wxComboBox(parent, ID_OPERATION_AFFECTED_BY, wxT(""), wxDefaultPosition, wxSize(100,-1), 1, strs13, wxCB_DROPDOWN);
    item12->Add(item13, 0, wxALIGN_CENTER|wxALL, 5);

    wxButton *item14 = new wxButton(parent, ID_AFFECTED_ADD_BUTTON, wxT("Add"), wxDefaultPosition, wxDefaultSize, 0);
    item12->Add(item14, 0, wxALIGN_CENTER|wxALL, 5);

    wxButton *item15 = new wxButton(parent, ID_AFFECTED_DELETE_BUTTON, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0);
    item12->Add(item15, 0, wxALIGN_CENTER|wxALL, 5);

    item9->Add(item12, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    item8->Add(item9, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // Operation inputs
    wxStaticBox *item17 = new wxStaticBox(parent, -1, wxT("Input operands"));
    wxStaticBoxSizer *item16 = new wxStaticBoxSizer(item17, wxVERTICAL);
    inputSizer_ = item16;

    wxBoxSizer *item18 = new wxBoxSizer(wxHORIZONTAL);

    wxListCtrl *item19 = new wxListCtrl(parent, ID_INPUT_OPERANDS, wxDefaultPosition, wxSize(335,120), wxLC_REPORT|wxSUNKEN_BORDER);
    item18->Add(item19, 0, wxALIGN_CENTER|wxALL, 5);

    // BoxSizer for up and down buttons
    wxBoxSizer *item20 = new wxBoxSizer(wxVERTICAL);

    // Move up and down buttons
    wxBitmapButton *item21 = new wxBitmapButton(parent, ID_INPUT_UP_BUTTON, createBitmaps(0), wxDefaultPosition, wxDefaultSize);
    item20->Add(item21, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBitmapButton *item22 = new wxBitmapButton(parent, ID_INPUT_DOWN_BUTTON, createBitmaps(1), wxDefaultPosition, wxDefaultSize);
    item20->Add(item22, 0, wxALIGN_CENTER|wxALL, 5);

    item18->Add(item20, 0, wxALIGN_CENTER|wxALL, 5);

    item16->Add(item18, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer *item23 = new wxBoxSizer(wxHORIZONTAL);

    wxButton *item24 = new wxButton(parent, ID_INPUT_ADD_BUTTON, wxT("Add..."), wxDefaultPosition, wxDefaultSize, 0);
    item23->Add(item24, 0, wxALIGN_CENTER|wxALL, 5);

    wxButton *item25 = new wxButton(parent, ID_INPUT_MODIFY_BUTTON, wxT("Modify..."), wxDefaultPosition, wxDefaultSize, 0);
    item23->Add(item25, 0, wxALIGN_CENTER|wxALL, 5);

    wxButton *item26 = new wxButton(parent, ID_INPUT_DELETE_BUTTON, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0);
    item23->Add(item26, 0, wxALIGN_CENTER|wxALL, 5);

    item16->Add(item23, 0, wxALIGN_CENTER|wxALL, 5);

    item8->Add(item16, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticBox *item28 = new wxStaticBox(parent, -1, wxT("Affects"));
    wxStaticBoxSizer *item27 = new wxStaticBoxSizer(item28, wxVERTICAL);
    affectsSizer_ = item27;

    wxListCtrl *item29 = new wxListCtrl(parent, ID_AFFECTS, wxDefaultPosition, wxSize(160,120), wxLC_REPORT|wxSUNKEN_BORDER);
    item27->Add(item29, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 10);

    wxBoxSizer *item30 = new wxBoxSizer(wxHORIZONTAL);

    wxString strs31[] = 
    {
        wxT("ComboItem")
    };
    wxComboBox *item31 = new wxComboBox(parent, ID_OPERATION_AFFECTS, wxT(""), wxDefaultPosition, wxSize(100,-1), 1, strs31, wxCB_DROPDOWN);
    item30->Add(item31, 0, wxALIGN_CENTER|wxALL, 5);

    wxButton *item32 = new wxButton(parent, ID_AFFECTS_ADD_BUTTON, wxT("Add"), wxDefaultPosition, wxDefaultSize, 0);
    item30->Add(item32, 0, wxALIGN_CENTER|wxALL, 5);

    wxButton *item33 = new wxButton(parent, ID_AFFECTS_DELETE_BUTTON, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0);
    item30->Add(item33, 0, wxALIGN_CENTER|wxALL, 5);

    item27->Add(item30, 0, wxGROW|wxALIGN_CENTER|wxALL, 5);

    item8->Add(item27, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticBox *item35 = new wxStaticBox(parent, -1, wxT("Output operands"));
    wxStaticBoxSizer *item34 = new wxStaticBoxSizer(item35, wxVERTICAL);
    outputSizer_ = item34;

    wxBoxSizer *item36 = new wxBoxSizer(wxHORIZONTAL);

    wxListCtrl *item37 = new wxListCtrl(parent, ID_OUTPUT_OPERANDS, wxDefaultPosition, wxSize(335,120), wxLC_REPORT|wxSUNKEN_BORDER);
    item36->Add(item37, 0, wxALIGN_CENTER|wxALL, 5);

    wxBoxSizer *item38 = new wxBoxSizer(wxVERTICAL);

    wxBitmapButton *item39 = new wxBitmapButton(parent, ID_OUTPUT_UP_BUTTON, createBitmaps(0), wxDefaultPosition, wxDefaultSize);
    item38->Add(item39, 0, wxALIGN_CENTER|wxALL, 5);

    wxBitmapButton *item40 = new wxBitmapButton(parent, ID_OUTPUT_DOWN_BUTTON, createBitmaps(1), wxDefaultPosition, wxDefaultSize);
    item38->Add(item40, 0, wxALIGN_CENTER|wxALL, 5);

    item36->Add(item38, 0, wxALIGN_CENTER|wxALL, 5);

    item34->Add(item36, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer *item41 = new wxBoxSizer(wxHORIZONTAL);

    wxButton *item42 = new wxButton(parent, ID_OUTPUT_ADD_BUTTON, wxT("Add..."), wxDefaultPosition, wxDefaultSize, 0);
    item41->Add(item42, 0, wxALIGN_CENTER|wxALL, 5);

    wxButton *item43 = new wxButton(parent, ID_OUTPUT_MODIFY_BUTTON, wxT("Modify..."), wxDefaultPosition, wxDefaultSize, 0);
    item41->Add(item43, 0, wxALIGN_CENTER|wxALL, 5);

    wxButton *item44 = new wxButton(parent, ID_OUTPUT_DELETE_BUTTON, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0);
    item41->Add(item44, 0, wxALIGN_CENTER|wxALL, 5);

    item34->Add(item41, 0, wxALIGN_CENTER|wxALL, 5);

    item8->Add(item34, 0, wxALIGN_CENTER|wxALL, 5);

    wxBoxSizer *pageSizer = new wxBoxSizer(wxHORIZONTAL);
    pageSizer->Add(item8, 0, wxALIGN_TOP|wxALL, 5);

    item0->Add(pageSizer, 0, wxALIGN_CENTER|wxALL, 5);

    wxGridSizer *item45 = new wxGridSizer(2, 0, 0);

    wxBoxSizer *item46 = new wxBoxSizer(wxHORIZONTAL);

    wxStaticText *item47 = NULL;
    if (module_.hasBehaviorSource()) {
        item47 = new wxStaticText(parent, ID_TEXT_OPEN, _T("Operation behavior module defined."), wxDefaultPosition, wxDefaultSize, 0);
    } else {
        item47 = new wxStaticText(parent, ID_TEXT_OPEN, wxT("Operation behavior module not defined."), wxDefaultPosition, wxDefaultSize, 0);
    }
    item46->Add(item47, 0, wxALIGN_CENTER|wxALL, 5);

    wxButton *item48 = new wxButton(parent, ID_OPEN_BUTTON, wxT("Open"), wxDefaultPosition, wxDefaultSize, 0);
    item46->Add(item48, 0, wxALIGN_CENTER|wxALL, 5);

    wxButton *OpenDAG = new wxButton(parent, ID_DAG_BUTTON, wxT("Open DAG"), wxDefaultPosition, wxDefaultSize, 0);
    item46->Add(OpenDAG, 0, wxALIGN_CENTER|wxALL, 5);

    item45->Add(item46, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer *item49 = new wxBoxSizer(wxHORIZONTAL);

    wxButton *item50 = new wxButton(parent, ID_OK_BUTTON, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0);
    item49->Add(item50, 0, wxALIGN_CENTER|wxALL, 5);

    wxButton *item51 = new wxButton(parent, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0);
    item49->Add(item51, 0, wxALIGN_CENTER|wxALL, 5);

    item45->Add(item49, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    item0->Add(item45, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    if (set_sizer)
    {
        parent->SetSizer(item0);
        if (call_fit)
            item0->SetSizeHints(parent);
    }
    
    return item0;
}

/**
 * Creates the bitmaps.
 *
 * NOTE! This function was created by wxDesigner.
 * 
 * @param index Index of the bitmap.
 * @return The bitmap.
 */
wxBitmap
OperationPropertyDialog::createBitmaps(size_t index) {
    if (index == 0)
    {
        /* XPM */
        static const char *xpm_data[] = {
            /* columns rows colors chars-per-pixel */
            "16 30 2 1",
            "  c None",
            "a c Black",
            /* pixels */
            "                ",
            "                ",
            "                ",
            "       aa       ",
            "      aaaa      ",
            "     aaaaaa     ",
            "    aa aa aa    ",
            "   aa  aa  aa   ",
            "  aa   aa   aa  ",
            " aa    aa    aa ",
            " a     aa     a ",
            "       aa       ",
            "       aa       ",
            "       aa       ",
            "       aa       ",
            "       aa       ",
            "       aa       ",
            "       aa       ",
            "       aa       ",
            "       aa       ",
            "       aa       ",
            "       aa       ",
            "       aa       ",
            "       aa       ",
            "       aa       ",
            "       aa       ",
            "       aa       ",
            "       aa       ",
            "       aa       ",
            "                "
        };
        wxBitmap bitmap(xpm_data);
        return bitmap;
    }
    if (index == 1)
    {
        /* XPM */
        static const char *xpm_data[] = {
            /* columns rows colors chars-per-pixel */
            "16 30 2 1",
            "  c None",
            "a c Black",
            /* pixels */
            "                ",
            "       aa       ",
            "       aa       ",
            "       aa       ",
            "       aa       ",
            "       aa       ",
            "       aa       ",
            "       aa       ",
            "       aa       ",
            "       aa       ",
            "       aa       ",
            "       aa       ",
            "       aa       ",
            "       aa       ",
            "       aa       ",
            "       aa       ",
            "       aa       ",
            "       aa       ",
            "       aa       ",
            "       aa       ",
            "  a    aa    a  ",
            "  aa   aa   aa  ",
            "   aa  aa  aa   ",
            "    aa aa aa    ",
            "     aaaaaa     ",
            "      aaaa      ",
            "       aa       ",
            "                ",
            "                ",
            "                "
        };
        wxBitmap bitmap(xpm_data);
        return bitmap;
    }
    return wxNullBitmap;
}
