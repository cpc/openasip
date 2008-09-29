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
 * @file FUImplementationDialog.hh
 *
 * Declaration of FUImplementationDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_FU_IMPLEMENTATION_DIALOG_HH
#define TTA_FU_IMPLEMENTATION_DIALOG_HH

#include "FUImplementation.hh"

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
    void update();

    HDB::FUPortImplementation* selectedArchPort();
    HDB::FUExternalPort* selectedExternalPort();
    HDB::FUImplementation::Parameter selectedParameter();

    void onArchPortSelection(wxListEvent& event);
    void onAddArchitecturePort(wxCommandEvent& event);
    void onEditArchitecturePort(wxCommandEvent& event);
    void onDeleteArchitecturePort(wxCommandEvent& event);

    void onExternalPortSelection(wxListEvent& event);
    void onAddExternalPort(wxCommandEvent& event);
    void onEditExternalPort(wxCommandEvent& event);
    void onDeleteExternalPort(wxCommandEvent& event);

    void onParameterSelection(wxListEvent& event);
    void onAddParameter(wxCommandEvent& event);
    void onEditParameter(wxCommandEvent& event);
    void onDeleteParameter(wxCommandEvent& event);

    void onSourceFileSelection(wxListEvent& event);
    void onAddSourceFile(wxCommandEvent& event);
    void onDeleteSourceFile(wxCommandEvent& even);

    void onOpcodeSelection(wxListEvent& event);
    void onSetOpcode(wxCommandEvent& event);
    void onClearOpcode(wxCommandEvent& event);

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
        ID_CLEAR_OPCODE,
        ID_SET_OPCODE,
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
        ID_OPCODE,
        ID_LINE
    };

    /// FU Implementation to modify.
    HDB::FUImplementation& implementation_;
    /// Function unit architecture the FUImplementation implements.
    const TTAMachine::FunctionUnit& architecture_;

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
