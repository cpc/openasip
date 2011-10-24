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
 * @file OperationPropertyDialog.hh
 *
 * Declaration of OperationPropertyDialog class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Tero Ryynänen 2008 (tero.ryynanen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_OPERATION_PROPERTY_DIALOG_HH
#define TTA_OPERATION_PROPERTY_DIALOG_HH

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/choice.h>
#include <vector>
#include <string>

class Operation;
class Operand;
class OperationModule;
class ObjectState;

/**
 * Dialog which is used to create or modify an operation.
 */
class OperationPropertyDialog : public wxDialog {
public:
    OperationPropertyDialog(
        wxWindow* parent, 
        Operation* op, 
        OperationModule& module,
        const std::string& path);
    virtual ~OperationPropertyDialog();
    
    Operation* operation() const;

private:
    /// Copying not allowed.
    OperationPropertyDialog(const OperationPropertyDialog&);
    /// Assignment not allowed.
    OperationPropertyDialog& operator=(const OperationPropertyDialog&);

    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    wxBitmap createBitmaps(size_t index);
    virtual bool TransferDataToWindow();
    void updateOperands();
    void updateIndex();
    void updateDAG();
    void updateAffected();
    void updateTypes();
    void onSelection(wxListEvent& event);
    void onAddAffectedBy(wxCommandEvent& event);
    void onDeleteAffectedBy(wxCommandEvent& event);
    void onAddAffects(wxCommandEvent& event);
    void onDeleteAffects(wxCommandEvent& event);
    void onAddInputOperand(wxCommandEvent& event);
    void onModifyInputOperand(wxCommandEvent& event);
    void onDeleteInputOperand(wxCommandEvent& event);
    void onAddOutputOperand(wxCommandEvent& event);
    void onModifyOutputOperand(wxCommandEvent& event);
    void onDeleteOutputOperand(wxCommandEvent& event);
    void onMoveInputUp(wxCommandEvent& event);
    void onMoveInputDown(wxCommandEvent& event);
    void onMoveOutputUp(wxCommandEvent& event);
    void onMoveOutputDown(wxCommandEvent& event);
    void onOpen(wxCommandEvent& event);
    void onOpenDAG(wxCommandEvent& event);

    void onSaveDAG(wxCommandEvent& event);
    void onUndoDAG(wxCommandEvent& event);
    void onDeleteDAG(wxCommandEvent& event);
    void onNewDAG(wxCommandEvent& event);

    void onDAGChange(wxCommandEvent& event);
    void onComboBoxChange(wxCommandEvent& event);

    void moveUp(std::vector<Operand*>& ops, int id, wxListCtrl* list);
    void moveDown(std::vector<Operand*>& ops, int id, wxListCtrl* list);
    std::vector<std::string> getSelectedItems(wxListCtrl* listCtrl);
       
    void onOk(wxCommandEvent& event);
    ObjectState* saveOperation();
    void setTexts();
    void setBehaviorLabel();
    void launchEditor(const std::string& cmd);
    void updateSwapLists(std::vector<std::string> deletedOperands);

    /**
     * Component ids.
     */
    enum {
        ID_TEXT_NAME = 1000,
        ID_NAME,
        ID_READS_MEMORY,
        ID_WRITES_MEMORY,
        ID_CAN_TRAP,
        ID_HAS_SIDE_EFFECTS,
        ID_CLOCKED,
        ID_AFFECTED_BY,
        ID_OPERATION_AFFECTED_BY,
        ID_AFFECTED_ADD_BUTTON,
        ID_AFFECTED_DELETE_BUTTON,
        ID_INPUT_OPERANDS,
        ID_INPUT_UP_BUTTON,
        ID_INPUT_DOWN_BUTTON,
        ID_INPUT_ADD_BUTTON,
        ID_INPUT_MODIFY_BUTTON,
        ID_INPUT_DELETE_BUTTON,
        ID_AFFECTS,
        ID_OPERATION_AFFECTS,
        ID_AFFECTS_ADD_BUTTON,
        ID_AFFECTS_DELETE_BUTTON,
        ID_OUTPUT_OPERANDS,
        ID_OUTPUT_UP_BUTTON,
        ID_OUTPUT_DOWN_BUTTON,
        ID_OUTPUT_ADD_BUTTON,
        ID_OUTPUT_MODIFY_BUTTON,
        ID_OUTPUT_DELETE_BUTTON,
        ID_OPEN_BUTTON,
        ID_DAG_BUTTON,
        ID_OK_BUTTON,
        ID_TEXT_CTRL,
        ID_TEXT_OPEN,
        ID_EDIT_DESCRIPTION
    };

    /// Input operand sizer.
    wxStaticBoxSizer* inputSizer_;
    /// Output operand sizer.
    wxStaticBoxSizer* outputSizer_;
    /// Affects sizer.
    wxStaticBoxSizer* affectsSizer_;
    /// Affected by sizer.
    wxStaticBoxSizer* affectedBySizer_;

    /// List of affected by operations.
    wxListCtrl* affectedByList_;
    /// List of affects operations.
    wxListCtrl* affectsList_;
    /// List of input operands.
    wxListCtrl* inputOperandList_;
    /// List of output operands.
    wxListCtrl* outputOperandList_;
    /// Choice box for new affected by operations.
    wxComboBox* affectedByChoice_;
    /// Choice box for new affects operations.
    wxComboBox* affectsChoice_;
    /// Read memory check box.
    wxCheckBox* readMemoryCB_;
    /// Write memory check box.
    wxCheckBox* writeMemoryCB_;
    /// Can trap check box.
    wxCheckBox* canTrapCB_;
    /// Has side effects check box.
    wxCheckBox* sideEffectsCB_;
    /// Clocked check box.
    wxCheckBox* clockedCB_;
    /// Operation to be created or to be modified.
    Operation* operation_;
    /// Original operation's ObjectState tree
    ObjectState* orig_;
    
    /// Name of the operation.
    wxString name_;
    /// Module in which operation belongs to.
    OperationModule& module_;
    /// Path in which module belongs to.
    std::string path_;
    /// Does operation read memory?
    bool readMemory_;
    /// Does operation write memory?
    bool writeMemory_;
    /// Can operation trap?
    bool canTrap_;
    /// Does operation has side effects?
    bool hasSideEffects_;
    /// Is operation clocked?
    bool clocked_;
    /// Can operation can change program flow.
    bool controlFlow_;
    /// Operation performs function call.
    bool isCall_;
    /// Operation is branch.
    bool isBranch_;

    // Text box for editing operation description
    wxTextCtrl* editDescription_;

    /// Input operands.
    std::vector<Operand*> inputOperands_;
    /// Output operands.
    std::vector<Operand*> outputOperands_;
    /// Operations that affects this operation.
    std::vector<std::string> affects_;
    /// Operations that are affected by this operation.
    std::vector<std::string> affectedBy_;
    
    bool operationWasCreatedHere_;
    DECLARE_EVENT_TABLE()
};

#endif
