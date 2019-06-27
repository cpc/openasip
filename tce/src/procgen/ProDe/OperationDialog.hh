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
 * @file OperationDialog.hh
 *
 * Declaration of OperationDialog class.
 *
 * @author Veli-Pekka J��skel�inen 2004 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_OPERATION_DIALOG_HH
#define TTA_OPERATION_DIALOG_HH

#include <set>
#include <list>
#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <wx/grid.h>

namespace TTAMachine {
    class HWOperation;
}

/**
 * Dialog for editing HWOperation properties.
 */
class OperationDialog: public wxDialog {
public:
    OperationDialog(wxWindow* parent, TTAMachine::HWOperation* operation);
    ~OperationDialog();

private:
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    bool TransferDataToWindow();
    void initialize();
    void setTexts();

    void updateOperandList();
    void updateResourceGrid();
    void updateUsageGrid();
    void updateLatency();
    void setOperandType(int operand, bool read);
    void warnOnResourcesWithoutUsages();

    void onAddOperand(wxCommandEvent& event);
    void onDeleteOperand(wxCommandEvent& event);
    void onBindOperand(wxCommandEvent& event);

    void onAddResource(wxCommandEvent& event);
    void onDeleteResource(wxCommandEvent& event);
    void onOK(wxCommandEvent& event);

    void onName(wxCommandEvent& event);
    void onOperandSelection(wxListEvent& event);
    void onResourceSelection(wxGridRangeSelectEvent& event);
    void onResourceLClick(wxGridEvent& event);
    void onOperandLClick(wxGridEvent& event);
    void onGridLClick(wxGridEvent& event);
    void onGridRClick(wxGridEvent& event);

    std::string selectedResource();
    int operandRow(int operand);

    /// Operation to edit.
    TTAMachine::HWOperation* operation_;
    /// Name of the operation
    wxString name_;
    /// Name of resource to be added.
    wxString resourceName_;
    /// Bind list widget.
    wxListCtrl* bindList_;
    /// Spin button control for the bind operand.
    wxSpinCtrl* numberControl_;
    /// Resource list widget.
    wxGrid* resourceGrid_;
    /// Operand usage grid widget.
    wxGrid* usageGrid_;
    /// Choice widget for the port to bind.
    wxChoice* portChoice_;
    /// Static boxsizer for the resource grid widgets
    wxStaticBoxSizer* resourceSizer_;
    /// Static boxsizer for the operand list widgets
    wxStaticBoxSizer* operandSizer_;
    /// Static boxsizer for the operand usage grid widgets
    wxStaticBoxSizer* usageSizer_;

    /// List of unused resources.
    std::list<std::string> newResources_;

    /// List of unused operands.
    std::set<int> operands_;

    /// Static text control displaying the operation latency.
    wxStaticText* latencyText_;

    /// Width of the resource and operand usage grid columns.
    static const int GRID_COLUMN_WIDTH;
    /// Grid marker for resource use.
    static const wxString USE_MARK;
    /// Grid marker for a read.
    static const wxString READ_MARK;
    /// Grid marker for a write.
    static const wxString WRITE_MARK;

    // enumerated IDs for dialog widgets
    enum {
        ID_NAME = 10000,
        ID_LABEL_NAME,
        ID_BIND_LIST,
        ID_LABEL_PORT,
        ID_PORT,
        ID_LABEL_NUMBER,
        ID_ADD_OPERAND,
        ID_DELETE_OPERAND,
        ID_HELP,
        ID_LINE,
        ID_LABEL_OPERAND,
        ID_NUMBER,
        ID_RESOURCE_GRID,
        ID_OPERAND_GRID,
        ID_LABEL_RESOURCE_NAME,
        ID_RESOURCE_NAME,
        ID_ADD_RESOURCE,
        ID_DELETE_RESOURCE,
        ID_DELETE_RESOURCE_CYCLE,
        ID_MENU_FOO,
        ID_MENU_BAR,
        ID_LABEL_LATENCY,
        ID_LATENCY
    };

    DECLARE_EVENT_TABLE()
};

#endif
