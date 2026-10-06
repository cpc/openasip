/*
 Copyright (C) 2022-2026 Tampere University.

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
 * @file OTAOperationDialog.hh
 *
 * Declaration of OTAOperationDialog.
 *
 * @author Kari Hepola 2022
 * @note rating: red
 */

#ifndef TTA_TEMPLATE_SLOT_DIALOG_HH
#define TTA_TEMPLATE_SLOT_DIALOG_HH

#include <wx/wx.h>
#include <wx/spinctrl.h>
#include "TCEString.hh"
#include <set>

namespace TTAMachine {
    class OperationTriggeredFormat;
}

/**
 * Dialog for editing telplate slot properties.
 */
class OTAOperationDialog : public wxDialog {
public:
    OTAOperationDialog(
        wxWindow* parent,
        TTAMachine::OperationTriggeredFormat* format);

    virtual ~OTAOperationDialog();

private:
    wxSizer* createContents(wxWindow *parent, bool call_fit, bool set_sizer);
    virtual bool TransferDataToWindow();
    virtual bool TransferDataFromWindow();
    void onOK(wxCommandEvent& event);
    void onOperationFilterChange(wxCommandEvent& event);
    void onSelectOperation(wxCommandEvent& event);

    int numberOfInputs() const;
    int numberOfOutputs() const;

    bool validFormatName() const;
    std::set<TCEString> addRISCVBaseOperations(
        std::set<TCEString> opset) const;

    /// Name of the selected operation.
    TCEString operation_;
    /// Operation list widget.
    wxListBox* operationList_;
    /// A string to filter opset list.
    TCEString opNameFilter_ = "";

    TTAMachine::OperationTriggeredFormat* format_;

    // enumerated IDs for dialog widgets
    enum {
        ID_LABEL_OTA_OPERATION = 10000,
        ID_OTA_OPERATION,
        ID_LIST,
        ID_OP_FILTER,
        ID_LINE,
        ID_OP_FILTER_LABEL
    };

    DECLARE_EVENT_TABLE()
};
#endif
