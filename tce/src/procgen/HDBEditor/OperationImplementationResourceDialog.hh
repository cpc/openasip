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
 * @file FUGENResourceDialog.hh
 *
 * Declaration of FUGENResourceDialog.
 *
 * @author Lasse Lehtonen 2017 (lasse.lehtonen-no.spam-tut.fi)
 */
#pragma once

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/filepicker.h>

wxDECLARE_EVENT(BUTTON_SYN_VHDL, wxFileDirPickerEvent);
wxDECLARE_EVENT(BUTTON_SIM_VHDL, wxFileDirPickerEvent);
wxDECLARE_EVENT(BUTTON_SYN_VERILOG, wxFileDirPickerEvent);
wxDECLARE_EVENT(BUTTON_SIM_VERILOG, wxFileDirPickerEvent);
wxDECLARE_EVENT(BUTTON_IPXACT, wxFileDirPickerEvent);
wxDECLARE_EVENT(BUTTON_DEL, wxCommandEvent);

class OperationImplementationResourceDialog : public wxDialog {
public:
    OperationImplementationResourceDialog();
    virtual ~OperationImplementationResourceDialog() = default;

    void onOK(wxCommandEvent&);
    void onAddVhdlSynFile(wxFileDirPickerEvent&);
    void onAddVhdlSimFile(wxFileDirPickerEvent&);
    void onAddVerilogSynFile(wxFileDirPickerEvent&);
    void onAddVerilogSimFile(wxFileDirPickerEvent&);

    void onDeleteListItem(wxCommandEvent&);

private:
    wxListCtrl *fileList1_;
    wxTextCtrl *nameCtrl_;
    wxFilePickerCtrl *ipxactPick_;

    std::string sanitize(const wxString& path);

    DECLARE_EVENT_TABLE()
};
