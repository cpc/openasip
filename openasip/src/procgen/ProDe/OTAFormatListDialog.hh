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
 * @file OTAFormatListDialog.hh
 *
 * Declaration of OTAFormatListDialog class.
 *
 * @author Kari Hepola 2022
 * @note rating: red
 */

#ifndef TTA_OTA_FORMAT_LIST_DIALOG_HH
#define TTA_OTA_FORMAT_LIST_DIALOG_HH

#include <wx/wx.h>
#include <wx/listctrl.h>

class wxListCtrl;

namespace TTAMachine {
    class Machine;
    class OperationTriggeredFormat;
    class Bus;
}

/**
 * Dialog for listing and editing instruction OTAFormats in a machine.
 */
class OTAFormatListDialog : public wxDialog {
public:
    OTAFormatListDialog(wxWindow* parent, TTAMachine::Machine* machine);
    virtual ~OTAFormatListDialog();

private:
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    virtual bool TransferDataToWindow();
    void updateOperationList();
    void onOTAFormatSelection(wxListEvent& event);
    void onOperationSelection(wxListEvent& event);
    void onOTAFormatName(wxCommandEvent& event);
    void onAddOTAFormat(wxCommandEvent& event);
    void onDeleteOTAFormat(wxCommandEvent& event);
    void onAddOperation(wxCommandEvent& event);
    void onEditOperation(wxCommandEvent& event);
    void onDeleteOperation(wxCommandEvent& event);
    void setTexts();
    bool validFormatName() const;
    TTAMachine::OperationTriggeredFormat* selectedOTAFormat();
    std::string selectedOperation();

    /// Parent machine of the instruction OTAFormats.
    TTAMachine::Machine* machine_;
    /// Box sizer around the OTAFormat list.
    wxStaticBoxSizer* OTAFormatSizer_;
    /// Box sizer around the operation list.
    wxStaticBoxSizer* operationSizer_;
    /// Widget for list of OTAFormats.
    wxListCtrl* OTAFormatList_;
    /// Widget for list of operations in the selected OTAFormat.
    wxListCtrl* operationList_;
    /// Name of the new OTAFormat.
    wxString OTAFormatName_;

    // enumerated IDs for dialog widgets
    enum {
        ID_OTA_FORMAT_LIST = 10000,
        ID_OPERATION_LIST,
        ID_LINE,
        ID_HELP,
        ID_ADD_OTA_FORMAT,
        ID_DELETE_OTA_FORMAT,
        ID_ADD_OPERATION,
        ID_EDIT_OPERATION,
        ID_DELETE_OPERATION,
        ID_NAME,
        ID_LABEL_NAME
    };

    DECLARE_EVENT_TABLE()
};
#endif
