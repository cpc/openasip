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
 * @file FUDialog.hh
 *
 * Declaration of FUDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel-no.spam-cs.tut.fi)
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
