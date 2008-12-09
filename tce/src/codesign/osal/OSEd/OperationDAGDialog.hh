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
 * @file OperationDAGDialog.hh
 *
 * Declaration of OperationDAGDialog class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Tero Ryynänen 2008 (tero.ryynanen-no.spam-tut.fi)
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
