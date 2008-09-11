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
 * @file OperationDialog.hh
 *
 * Declaration of OperationDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel-no.spam-cs.tut.fi)
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
