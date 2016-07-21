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
 * @file OpsetDialog.hh
 *
 * Declaration of OpsetDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_OPSET_DIALOG_HH
#define TTA_OPSET_DIALOG_HH

#include <wx/wx.h>

namespace TTAMachine {
    class FunctionUnit;
    class HWOperation;
}

/**
 * Dialog for selecting and creating HWOperation from the operation pool.
 */
class OpsetDialog : public wxDialog {
public:
    OpsetDialog(wxWindow* parent);
    virtual ~OpsetDialog();
    TTAMachine::HWOperation* createOperation(TTAMachine::FunctionUnit& fu);
protected:
    virtual bool TransferDataToWindow();
    virtual bool TransferDataFromWindow();
private:
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    void onSelectOperation(wxCommandEvent& event);
    void onOK(wxCommandEvent& event);
    void onOperationFilterChange(wxCommandEvent& event);

    /// Chosen latency.
    int latency_;
    /// Name of the selected operation.
    std::string operation_;
    /// Operation list widget.
    wxListBox* operationList_;
    /// A string to filter opset list.
    std::string opNameFilter_ = "";

    /// Widget IDs.
    enum {
        ID_LIST = 20000,
        ID_OP_FILTER_LABEL,
        ID_OP_FILTER,
        ID_OP_DESCRIPTION,
        ID_OP_INPUTS,
        ID_OP_OUTPUTS,
        ID_LATENCY,
        ID_TEXT,
        ID_LINE
    };

    DECLARE_EVENT_TABLE()
};
#endif
