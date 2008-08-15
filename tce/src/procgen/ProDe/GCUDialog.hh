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
 * @file GCUDialog.hh
 *
 * Declaration of GCUDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_GCU_DIALOG_HH
#define TTA_GCU_DIALOG_HH

#include <wx/wx.h>
#include <wx/listctrl.h>

namespace TTAMachine {
    class ControlUnit;
    class HWOperation;
    class BaseFUPort;
}

/**
 * Dialog for adding a new Global Control Unit to the Machine and
 * editing existing Global Control Unit attributes.
 */
class GCUDialog : public wxDialog {
 public:
    GCUDialog(wxWindow* parent, TTAMachine::ControlUnit* gcu);
    virtual ~GCUDialog();

 private:
    virtual bool TransferDataToWindow();
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    void updateAddressSpaceChoice();
    void updateSocketChoicers();
    void updateOperationList();
    void updatePortList();
    void updateRAPortChoice();
    TTAMachine::HWOperation* selectedOperation();
    TTAMachine::BaseFUPort* selectedPort();
    void onName(wxCommandEvent& event);

    void onOperationSelection(wxListEvent& event);
    void onAddOperation(wxCommandEvent& event);
    void onEditOperation(wxCommandEvent& event);
    void onDeleteOperation(wxCommandEvent& event);
    void onActivateOperation(wxListEvent& event);
    void onOperationRightClick(wxListEvent& event);

    void onAddFUPort(wxCommandEvent& event);
    void onAddSRPort(wxCommandEvent& event);
    void onEditPort(wxCommandEvent& event);
    void onDeletePort(wxCommandEvent& event);
    void onPortSelection(wxListEvent& event);
    void onActivatePort(wxListEvent& event);
    void onPortRightClick(wxListEvent& event);

    void onRAPortChoice(wxCommandEvent& event);

    void onHelp(wxCommandEvent& event);
    void onOK(wxCommandEvent& event);
    void setLabels();

    /// The Global Control Unit to be modified with the dialog.
    TTAMachine::ControlUnit* gcu_;
    /// Name of the global control unit.
    wxString name_;
    /// Number of delay slots of the global control unit.
    int delaySlots_;
    /// Guard latency of the gcu.
    int guardLatency_;

    /// Address Space choice control.
    wxChoice* addressSpaceChoice_;
    /// Return address port choice  control.
    wxChoice* raPortChoice_;

    /// Operation list widget.
    wxListCtrl* operationList_;
    /// Port list widget.
    wxListCtrl* portList_;

    /// Box sizer for the operations list controls.
    wxStaticBoxSizer* operationsSizer_;
    /// Box sizer for the port list controls.
    wxStaticBoxSizer* portListSizer_;

    /// enumerated IDs for dialog controls
    enum {
        ID_NAME = 10000,
        ID_TEXT,
        ID_DELAY_SLOTS,
        //ID_GUARD_LATENCY,
        ID_ADDRESS_SPACE,
        ID_OPERATION_LIST,
        ID_PORT_LIST,
	ID_RA_CHOICE,
	ID_OP_CHOICE,
        ID_LINE,
        ID_ADD_OPERATION,
        ID_EDIT_OPERATION,
        ID_DELETE_OPERATION,
        ID_ADD_FU_PORT,
        ID_ADD_SR_PORT,
        ID_EDIT_PORT,
        ID_DELETE_PORT,
        ID_HELP,
        ID_TEXT_NAME,
        ID_TEXT_DS,
        ID_TEXT_AS,
        ID_TEXT_INPUT_SOCKET,
        ID_TEXT_OUTPUT_SOCKET,
        ID_TEXT_RA_WIDTH,
	ID_LABEL_RA_CHOICE,
	ID_LABEL_OP_CHOICE
        //ID_LABEL_GLOBAL_GUARD_LATENCY
    };
    DECLARE_EVENT_TABLE()
};

#endif
