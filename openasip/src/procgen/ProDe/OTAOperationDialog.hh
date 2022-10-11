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
