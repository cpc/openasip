/*
    Copyright (c) 2002-2017 Tampere University.

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
 * @file OperationImplementationDialog.hh
 *
 * Declaration of OperationImplementationDialog.
 *
 * @author Lasse Lehtonen 2017 (lasse.lehtonen-no.spam-tut.fi)
 */
#pragma once

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/filepicker.h>
#include <wx/spinctrl.h>

wxDECLARE_EVENT(BUTTON_ADD_VHDL_VAR, wxCommandEvent);
wxDECLARE_EVENT(BUTTON_ADD_VERILOG_VAR, wxCommandEvent);

class OperationImplementationDialog : public wxDialog {
public:
    OperationImplementationDialog();
    virtual ~OperationImplementationDialog() = default;

    void onOK(wxCommandEvent&);
    void onAddVhdlImplFile(wxFileDirPickerEvent&);
    void onAddVerilogImplFile(wxFileDirPickerEvent&);

    void onDeleteListItem(wxCommandEvent&);
    void onAddListItem(wxCommandEvent&);

    void onAddVHDLVariable(wxCommandEvent&);
    void onAddVerilogVariable(wxCommandEvent&);

private:
    wxListCtrl *availableResourceList_;
    wxListCtrl *addedResourceList_;
    wxListCtrl *addedVariableList_;
    wxTextCtrl *nameCtrl_;
    wxFilePickerCtrl *pick1_;
    wxFilePickerCtrl *pick2_;
    wxFilePickerCtrl *pick11_;
    wxFilePickerCtrl *pick21_;
    wxFilePickerCtrl *pick_vhdl_init_;
    wxFilePickerCtrl *pick_vlog_init_;
    wxFilePickerCtrl *pick31_;
    wxSpinCtrl *latencyWheel_;

    void addAvailableResourcesToList();
    std::string sanitize(const wxString& path);

    DECLARE_EVENT_TABLE()
};
