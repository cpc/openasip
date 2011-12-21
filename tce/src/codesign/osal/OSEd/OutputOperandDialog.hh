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
 * @file OutputOperandDialog.hh
 *
 * Declaration of OutputOperandDialog class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Tero Ryynänen 2008 (tero.ryynanen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_OUTPUT_OPERAND_DIALOG_HH
#define TTA_OUTPUT_OPERAND_DIALOG_HH

#include <wx/wx.h>
#include <map>

#include "Operand.hh"

typedef std::map <int, Operand::OperandType> operandList;
typedef std::pair <int, Operand::OperandType> operandPair;

/**
 * Class for adding or modifying output operands.
 */
class OutputOperandDialog : public wxDialog {
public:
    OutputOperandDialog(wxWindow* window, Operand* operand, int index);
    virtual ~OutputOperandDialog();

private:
    /// Copying not allowed.
    OutputOperandDialog(const OutputOperandDialog&);
    /// Assignment not allowed.
    OutputOperandDialog& operator=(const OutputOperandDialog&);

    wxSizer* createContents(wxWindow* window, bool call_fit, bool set_sizer);
    void onOk(wxCommandEvent& event);
    void updateTypes(Operand* operand);
    void setTexts();

    /**
     * Component ids.
     */
    enum {
        ID_MEM_DATA,           ///< Memory data choice.
        ID_OPERATION_OUTPUT_TYPES ///< Output Type ComboBox
    };

    /// Operand to be modified.
    Operand* operand_;
    /// Flag indicating whether operand is memory data or not.
    bool memData_;
    /// Index of the operand.
    int index_;
    /// Choice box for operation input types
    wxChoice* outputTypesComboBox_;
    
    /// Output types
    std::vector<std::string> outputTypes_;
    operandList operandTypes_;

    //Operand::OperandType operandTypes_[4];
    
    DECLARE_EVENT_TABLE()
};

#endif
