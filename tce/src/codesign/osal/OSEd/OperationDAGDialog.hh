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
 * @file OperationDAGDialog.hh
 *
 * Declaration of OperationDAGDialog class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @author Tero Ryynänen 2008 (tero.ryynanen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_OPERATION_DAG_DIALOG_HH
#define TTA_OPERATION_DAG_DIALOG_HH

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
class OperationDAGDialog : public wxDialog {
public:
    OperationDAGDialog(
        wxWindow* parent, 
        Operation* op);
    virtual ~OperationDAGDialog();
    
    Operation* operation() const;

private:
    /// Copying not allowed.
    OperationDAGDialog(const OperationDAGDialog&);
    /// Assignment not allowed.
    OperationDAGDialog& operator=(const OperationDAGDialog&);

    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    virtual bool TransferDataToWindow();
    void updateIndex();
    bool hasDot();
    void updateDAG();

    void onOK(wxCommandEvent& event);
    void onCancel(wxCommandEvent& event);

    void onSaveDAG(wxCommandEvent& event);
    void onUndoDAG(wxCommandEvent& event);
    void onDeleteDAG(wxCommandEvent& event);
    void onNewDAG(wxCommandEvent& event);

    void onDAGChange(wxCommandEvent& event);
    void onComboBoxChange(wxCommandEvent& event);

    ObjectState* saveOperation();

    void setTexts();
    void setBehaviorLabel();

    /**
     * Component ids.
     */
    enum {
        ID_EDIT_DAG = 1000,
        ID_SAVE_DAG_BUTTON,
        ID_UNDO_DAG_BUTTON,
        ID_DELETE_DAG_BUTTON,
        ID_NEW_DAG_BUTTON,
        ID_DAG_IMAGE,
        ID_INDEX_DAG,
        ID_OK_BUTTON,
        ID_CANCEL_BUTTON
    };

    /// Operation to be created or to be modified.
    Operation* operation_;
    /// Original operation's ObjectState tree
    ObjectState* orig_;
    
    // Text box for editing dag
    wxTextCtrl* dagEdit_;
    //
    // Text box for editing operation description
    wxTextCtrl* editDescription_;

    // ComboBox for selectind dag
    wxChoice* dagIndex_;

    // Dot generated image
    wxBitmap* dotImage_;

    // Bitmap for holding the dot image
    wxStaticBitmap* dagStaticBitmap_;
    wxStaticBoxSizer *dagImageStaticBoxSizer_;
    
    // Scrolled window for DAG
    wxScrolledWindow *dagWindow_;

    bool operationWasCreatedHere_;
    bool dotInstalled_;

    DECLARE_EVENT_TABLE()
};

#endif
