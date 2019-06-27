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
 * @file FUImplementationDialog.hh
 *
 * Declaration of FUImplementationDialog class.
 *
 * @author Veli-Pekka J��skel�inen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_FU_IMPLEMENTATION_DIALOG_HH
#define TTA_FU_IMPLEMENTATION_DIALOG_HH

#include <wx/listctrl.h>
#include "FUImplementation.hh"

// Remove this define when opcode editing should be disabled.
// Also edit hdb/HDBManager.cc not to write opcodes into hdb anymore.
#define ALLOW_OPCODE_EDITING

class wxListCtrl;

namespace TTAMachine {
    class FunctionUnit;
}

/**
 * Dialog for editing FU implementations.
 */
class FUImplementationDialog : public wxDialog {
public:
    FUImplementationDialog(
        wxWindow* parent, wxWindowID id,
        HDB::FUImplementation& implementation,
        const TTAMachine::FunctionUnit& architecture);

    virtual ~FUImplementationDialog();
private:

    void onOK(wxCommandEvent& event);
    void update(bool onInit = false);

    HDB::FUPortImplementation* selectedArchPort();
    HDB::FUExternalPort* selectedExternalPort();
    HDB::FUImplementation::Parameter selectedParameter();

    void onArchPortActivation(wxListEvent& event);
    void onArchPortSelection(wxListEvent& event);
    void onAddArchitecturePort(wxCommandEvent& event);
    void onEditArchitecturePort(wxCommandEvent& event);
    void onDeleteArchitecturePort(wxCommandEvent& event);

    void onExternalPortActivation(wxListEvent& event);
    void onExternalPortSelection(wxListEvent& event);
    void onAddExternalPort(wxCommandEvent& event);
    void onEditExternalPort(wxCommandEvent& event);
    void onDeleteExternalPort(wxCommandEvent& event);

    void onParameterActivation(wxListEvent& event);
    void onParameterSelection(wxListEvent& event);
    void onAddParameter(wxCommandEvent& event);
    void onEditParameter(wxCommandEvent& event);
    void onDeleteParameter(wxCommandEvent& event);

    void onSourceFileSelection(wxListEvent& event);
    void onAddSourceFile(wxCommandEvent& event);
    void onDeleteSourceFile(wxCommandEvent& event);

    void onMoveSourceFileUp(wxCommandEvent& event);
    void onMoveSourceFileDown(wxCommandEvent& event);

#ifdef ALLOW_OPCODE_EDITING
    void onOpcodeSelection(wxListEvent& event);
    void onSetOpcode(wxCommandEvent& event);
    void onClearOpcode(wxCommandEvent& event);
#endif

    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);

    /// Window indentifiers for dialog widgets.
    enum {
        ID_LABEL_NAME = 10000,
        ID_NAME,
        ID_LABEL_CLK_PORT,
        ID_CLK_PORT,
        ID_LABEL_RST_PORT,
        ID_RST_PORT,
        ID_LABEL_GLOCK_PORT,
        ID_GLOCK_PORT,
        ID_LABEL_GLOCK_REQ_PORT,
        ID_GLOCK_REQ_PORT,
        ID_OPCODE_LIST,
#ifdef ALLOW_OPCODE_EDITING
        ID_CLEAR_OPCODE,
        ID_SET_OPCODE,
        ID_OPCODE, 
#endif
        ID_EXTERNAL_PORT_LIST,
        ID_ADD_EXTERNAL_PORT,
        ID_EDIT_EXTERNAL_PORT,
        ID_DELETE_EXTERNAL_PORT,
        ID_SOURCE_LIST,
        ID_ADD_SOURCE,
        ID_DELETE_SOURCE,
        ID_PARAMETER_LIST,
        ID_EDIT_PARAMETER,
        ID_ADD_PARAMETER,
        ID_DELETE_PARAMETER,
        ID_LABEL_OPCODE_PORT_NAME,
        ID_OPCODE_PORT,
        ID_ARCH_PORT_LIST,
        ID_EDIT_ARCH_PORT,
        ID_LINE,
        ID_MOVE_SOURCE_UP,
        ID_MOVE_SOURCE_DOWN
    };
    
    /// FU Implementation to modify.
    HDB::FUImplementation& implementation_;
    /// Function unit architecture the FUImplementation implements.
    const TTAMachine::FunctionUnit& architecture_;

    bool opcodeWarningShowed_;

    /// Pointer to the architecture port list widget.
    wxListCtrl* archPortList_;
    /// Pointer to the external port list widget.
    wxListCtrl* externalPortList_;
    /// Pointer to the parameter list widget.
    wxListCtrl* parameterList_;
    /// Pointer to the opcode list widget.
    wxListCtrl* opcodeList_;
    /// Pointer to the source file list widget.
    wxListCtrl* sourceList_;

    wxString name_;
    wxString opcodePort_;
    wxString clkPort_;
    wxString rstPort_;
    wxString gLockPort_;
    wxString gLockReqPort_;

    DECLARE_EVENT_TABLE()
};

#endif
