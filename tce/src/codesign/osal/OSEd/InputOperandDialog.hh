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
 * @file InputOperandDialog.hh
 *
 * Declaration of InputOperandDialog class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_INPUT_OPERAND_DIALOG_HH
#define TTA_INPUT_OPERAND_DIALOG_HH

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/spinctrl.h>
#include <set>

class Operand;
#include "Operand.hh"

/**
 * Dialog for adding or modifying input operands.
 */
class InputOperandDialog : public wxDialog {
public:
    InputOperandDialog(
        wxWindow* parent,
        Operand* operand,
        int numberOfOperands,
        int operandIndex);
    
    virtual ~InputOperandDialog();
    
private:
    /// Copying not allowed.
    InputOperandDialog(const InputOperandDialog&);
    /// Assignment not allowed.
    InputOperandDialog& operator=(const InputOperandDialog&);
    
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    virtual bool TransferDataToWindow();
    void updateList();
    void onAddSwap(wxCommandEvent& event);
    void onDeleteSwap(wxCommandEvent& event);
    void onSelection(wxListEvent& event);
    void onOk(wxCommandEvent& event);
    void updateOperand();
    void onType(wxCommandEvent& event);
    void onElementWidth(wxSpinEvent& event);
    void onElementCount(wxCommandEvent& event);

    void updateTypes();
    void updateElementWidths();
    void updateElementCounts();
    void setTexts();
    
    /**
     * Component ids.
     */
    enum {
        ID_MEM_ADDRESS = 1000,
        ID_MEM_DATA,
        ID_SWAP_LIST,
        ID_OPERAND_CHOICE,
        ID_OPERATION_INPUT_TYPES, // Input Type ComboBox
        ID_ADD_BUTTON,
        ID_DELETE_BUTTON,
        ID_ELEMENT_WIDTH,
        ID_ELEMENT_COUNT,
        ID_TEXT_WIDTH,
        ID_TEXT_COUNT
    };

    /// List of can swap operands.
    wxListCtrl* swapList_;
    /// Choice list for can swap operands.
    wxChoice* swapChoice_;
    /// Pointer to can swap sizer.
    wxStaticBoxSizer* swapSizer_;
    /// Operand to be created or modified.
    Operand* operand_;
    /// Flag indicating if operand is memory address.
    bool memAddress_;
    /// Flag indicating if operand is memory data.
    bool memData_;
    /// Choice box for operation input types
    wxChoice* inputTypesComboBox_;
    /// Spin ctrl for operand element width.
    wxSpinCtrl* elementWidthSpinCtrl_;
    /// Choice box for operand element count.
    wxChoice* elementCountChoice_;
    /// Operands that can be swapped with this operand.
    std::set<int> canSwap_;
    /// Numberof input operands.
    int numberOfOperands_;
    /// Index of the input operand currently modified.
    int index_;
    /// Current operand type in choice box.
    int type_;
    /// Current element width in choice box.
    int elemWidth_;
    /// Current element count in choice box.
    int elemCount_;

    /// Input types
    std::vector<std::string> inputTypes_;

    Operand::OperandType operandTypes_[9];

    DECLARE_EVENT_TABLE()
};

#endif
