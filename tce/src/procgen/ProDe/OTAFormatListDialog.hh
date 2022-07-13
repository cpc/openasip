/*
 Copyright (C) 2022 Tampere University.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
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
