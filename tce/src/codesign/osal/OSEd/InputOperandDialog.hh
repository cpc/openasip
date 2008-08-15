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
 * @file InputOperandDialog.hh
 *
 * Declaration of InputOperandDialog class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_INPUT_OPERAND_DIALOG_HH
#define TTA_INPUT_OPERAND_DIALOG_HH

#include <wx/wx.h>
#include <wx/listctrl.h>
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
    void updateTypes();
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
        ID_DELETE_BUTTON
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
    /// Operands that can be swapped with this operand.
    std::set<int> canSwap_;
    /// Numberof input operands.
    int numberOfOperands_;
    /// Index of the input operand currently modified.
    int index_;

    /// Input types
    std::vector<std::string> inputTypes_;

    Operand::OperandType operandTypes_[4];
    
    DECLARE_EVENT_TABLE()
};

#endif
