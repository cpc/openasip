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
 * @file OutputOperandDialog.hh
 *
 * Declaration of OutputOperandDialog class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @author Tero Ryynänen 2008 (tero.ryynanen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_OUTPUT_OPERAND_DIALOG_HH
#define TTA_OUTPUT_OPERAND_DIALOG_HH

#include <wx/wx.h>
#include <map>

class Operand;

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
