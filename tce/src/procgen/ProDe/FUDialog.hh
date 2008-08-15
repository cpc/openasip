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
 * @file FUDialog.hh
 *
 * Declaration of FUDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_FU_DIALOG_HH
#define TTA_FU_DIALOG_HH

#include <string>
#include <wx/wx.h>
#include <wx/listctrl.h>

namespace TTAMachine {
    class FunctionUnit;
    class FUPort;
    class HWOperation;
}

/**
 * Dialog for querying FunctionUnit parameters from the user.
 */
class FUDialog : public wxDialog {
public:
    FUDialog(wxWindow* parent, TTAMachine::FunctionUnit* functionUnit);
    virtual ~FUDialog();

protected:
    virtual bool TransferDataToWindow();
private:
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);

    TTAMachine::FUPort* selectedPort();
    TTAMachine::HWOperation* selectedOperation();
    void onOK(wxCommandEvent& event);
    void onHelp(wxCommandEvent& event);
    void onName(wxCommandEvent& event);
    void onPortSelection(wxListEvent& event);
    void onPortRightClick(wxListEvent& event);
    void onAddPort(wxCommandEvent& event);
    void onDeletePort(wxCommandEvent& event);
    void onEditPort(wxCommandEvent& event);
    void onActivatePort(wxListEvent& event);
    void onOperationSelection(wxListEvent& event);
    void onAddOperation(wxCommandEvent& event);
    void onAddOperationFromOpset(wxCommandEvent& event);
    void onDeleteOperation(wxCommandEvent& event);
    void onEditOperation(wxCommandEvent& event);
    void onSetAddressSpace(wxCommandEvent& event);
    void onActivateOperation(wxListEvent& event);
    void onOperationRightClick(wxListEvent& event);
    void updateAddressSpaceChoice();
    void updatePortList();
    void updateOperationList();
    void setTexts();

    /// FunctionUnit to modify.
    TTAMachine::FunctionUnit* functionUnit_;
    /// Name of the function unit.
    wxString name_;

    /// Port list control.
    wxListCtrl* portListCtrl_;
    /// Operation list control.
    wxListCtrl* operationListCtrl_;
    /// Address Space choice control.
    wxChoice* addressSpaceChoice_;

    /// Sizer for the port list controls.
    wxStaticBoxSizer* portsSizer_;
    /// Sizer for the operation list controls.
    wxStaticBoxSizer* operationsSizer_;

    enum {
	ID_NAME=10000,
	ID_ADDRESS_SPACE,
	ID_OPERATION_LIST,
	ID_ADD_OPERATION,
	ID_EDIT_OPERATION,
        ID_ADD_OPERATION_FROM_OPSET,
	ID_DELETE_OPERATION,
        ID_PORT_LIST,
	ID_ADD_PORT,
	ID_EDIT_PORT,
	ID_DELETE_PORT,
	ID_HELP,
        ID_LABEL_NAME,
        ID_LABEL_AS
    };

    /// The event table for the controls of the dialog.
    DECLARE_EVENT_TABLE()
};
#endif
