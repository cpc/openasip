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
 * @file OTAOperationDialog.cc
 *
 * Implementation of OTAOperationDialog class.
 *
 * @author Kari Hepola 2022
 * @note rating: red
 */

#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <wx/statline.h>
#include <wx/valgen.h>
#include <boost/format.hpp>

#include "OTAOperationDialog.hh"
#include "Machine.hh"
#include "OperationTriggeredFormat.hh"
#include "WxConversion.hh"
#include "AssocTools.hh"
#include "WidgetTools.hh"
#include "InformationDialog.hh"
#include "ModelConstants.hh"
#include "WidgetTools.hh"
#include "GUITextGenerator.hh"
#include "ProDeTextGenerator.hh"
#include "Application.hh"
#include "FunctionUnit.hh"
#include "HWOperation.hh"
#include "ErrorDialog.hh"
#include "OperationPool.hh"
#include "Operation.hh"
#include "RISCVFields.hh"
#include "MapTools.hh"

using boost::format;
using std::string;
using namespace TTAMachine;

BEGIN_EVENT_TABLE(OTAOperationDialog, wxDialog)
    EVT_LISTBOX(ID_LIST, OTAOperationDialog::onSelectOperation)
    EVT_BUTTON(wxID_OK, OTAOperationDialog::onOK)
    EVT_TEXT(ID_OP_FILTER, OTAOperationDialog::onOperationFilterChange)
END_EVENT_TABLE()

/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param format OperationTriggeredFormat format to edit.
 * @param operation Operation to edit, NULL if a new operation is being added.
 */
OTAOperationDialog::OTAOperationDialog(
    wxWindow* parent,
    TTAMachine::OperationTriggeredFormat* format) :
    wxDialog(parent, -1, _T("Choose operation"), wxDefaultPosition),
    operation_(""),
    format_(format) {

    createContents(this, true, true);
    operationList_ = dynamic_cast<wxListBox*>(FindWindow(ID_LIST));
    FindWindow(wxID_OK)->Disable();
}


/**
 * The Destructor.
 */
OTAOperationDialog::~OTAOperationDialog() {
}

int
OTAOperationDialog::numberOfInputs() const {
    if (format_->name() == RISCVFields::RISCV_R_TYPE_NAME ||
        format_->name() == RISCVFields::RISCV_I_TYPE_NAME) {
        return 2;
    }
    if (format_->name() == RISCVFields::RISCV_S_TYPE_NAME ||
        format_->name() == RISCVFields::RISCV_B_TYPE_NAME) {
        return 3;
    }
    if (format_->name() == RISCVFields::RISCV_U_TYPE_NAME   ||
        format_->name() == RISCVFields::RISCV_J_TYPE_NAME   ||
        format_->name() == RISCVFields::RISCV_R1R_TYPE_NAME ||
        format_->name() == RISCVFields::RISCV_R1_TYPE_NAME) {
        return 1;
    }
    if (format_->name() == RISCVFields::RISCV_R3R_TYPE_NAME) {
        return 3;
    }
    return 0;
}

int
OTAOperationDialog::numberOfOutputs() const {
    if (format_->name() == RISCVFields::RISCV_R_TYPE_NAME   ||
        format_->name() == RISCVFields::RISCV_R1R_TYPE_NAME ||
        format_->name() == RISCVFields::RISCV_I_TYPE_NAME   ||
        format_->name() == RISCVFields::RISCV_U_TYPE_NAME   ||
        format_->name() == RISCVFields::RISCV_R3R_TYPE_NAME ||
        format_->name() == RISCVFields::RISCV_J_TYPE_NAME) {
        return 1;
    }
    return 0;
}

bool
OTAOperationDialog::validFormatName() const {
    if (format_->name() == RISCVFields::RISCV_R_TYPE_NAME   ||
        format_->name() == RISCVFields::RISCV_R1_TYPE_NAME  ||
        format_->name() == RISCVFields::RISCV_R1R_TYPE_NAME ||
        format_->name() == RISCVFields::RISCV_R3R_TYPE_NAME ||
        format_->name() == RISCVFields::RISCV_I_TYPE_NAME   ||
        format_->name() == RISCVFields::RISCV_S_TYPE_NAME   ||
        format_->name() == RISCVFields::RISCV_B_TYPE_NAME   ||
        format_->name() == RISCVFields::RISCV_U_TYPE_NAME   ||
        format_->name() == RISCVFields::RISCV_J_TYPE_NAME) {
        return true;
    }
    return false;
}

std::set<TCEString>
OTAOperationDialog::addRISCVBaseOperations(std::set<TCEString> opset) const {
    std::map<std::string, int> ops;
    if (format_->name() == RISCVFields::RISCV_R_TYPE_NAME) {
        ops = RISCVFields::RISCVRTypeOperations;
    } else if (format_->name() == RISCVFields::RISCV_I_TYPE_NAME) {
        ops = RISCVFields::RISCVITypeOperations;
    } else if (format_->name() == RISCVFields::RISCV_S_TYPE_NAME) {
        ops = RISCVFields::RISCVSTypeOperations;
    } else if (format_->name() == RISCVFields::RISCV_B_TYPE_NAME) {
        ops = RISCVFields::RISCVBTypeOperations;
    } else if (format_->name() == RISCVFields::RISCV_U_TYPE_NAME) {
        ops = RISCVFields::RISCVUTypeOperations;
    } else if (format_->name() == RISCVFields::RISCV_J_TYPE_NAME) {
        ops = RISCVFields::RISCVJTypeOperations;
    }
    for (const auto& op : ops) {
        const std::string opName = op.first;
        if (!opNameFilter_.empty() &&
            opName.find(opNameFilter_) == std::string::npos) {
            continue;
        } else if (format_->hasOperation(opName)) {
            continue;
        } else if (op.first == "lui") {
            opset.insert(opName);
            continue;
        }
        const std::string OAName = RISCVFields::RISCVOperationNameTable.at(opName);
        if (format_->machine()->hasOperation(OAName)) {
            opset.insert(opName);
        }
    }
    return opset;
}


/**
 * Transfers data to the opset list.
 */
bool
OTAOperationDialog::TransferDataToWindow() {

    operationList_->Clear();

    std::set<TCEString> opset;
    if (format_->name() == RISCVFields::RISCV_R_TYPE_NAME   ||
        format_->name() == RISCVFields::RISCV_R3R_TYPE_NAME ||
        format_->name() == RISCVFields::RISCV_R1R_TYPE_NAME ||
        format_->name() == RISCVFields::RISCV_R1_TYPE_NAME) {
        Machine::FunctionUnitNavigator nav =
            format_->machine()->functionUnitNavigator();
        for (int i = 0; i < nav.count(); i++) {
            FunctionUnit* fu = nav.item(i);
            for (int j = 0; j < fu->operationCount(); j++) {
                HWOperation* op = fu->operation(j);
                TCEString opName =op->name();
                opName = opName.lower();
                if (!opNameFilter_.empty() &&
                    opName.find(opNameFilter_) == std::string::npos) {
                    continue;
                } else if (format_->hasOperation(opName)) {
                    continue;
                } else if (op->numberOfInputs() != numberOfInputs()) {
                    continue;
                } else if (op->numberOfOutputs() != numberOfOutputs()) {
                    continue;
                } else if (MapTools::containsValue(
                    RISCVFields::RISCVOperationNameTable, opName)) {
                        continue;
                }
                opset.insert(opName);
            }
        }
    }

    opset = addRISCVBaseOperations(opset);


    for (const auto& opName : opset) {
        operationList_->Append(WxConversion::toWxString(opName));
    }

    return true;
}

/**
 * Reads user choices from the dialog widgets.
 */
bool
OTAOperationDialog::TransferDataFromWindow() {
    operation_ = WxConversion::toString(operationList_->GetStringSelection());
    return true;
}

/**
 * Event handler for the OK button.
 */
void
OTAOperationDialog::onOK(wxCommandEvent&) {
    if (operationList_->GetSelection() == wxNOT_FOUND) {
        wxString message = _T("No operation selected.");
        ErrorDialog dialog(this, message);
        dialog.ShowModal();
        return;
    }
    if (!format_->hasOperation(operation_)) {
        format_->addOperation(operation_);
    }
    TransferDataFromWindow();
    EndModal(wxID_OK);
}

/**
 * Event handler for the operation list selections.
 *
 * Enables and disables the OK button.
 * Displays operation description and ports count.
 */
void
OTAOperationDialog::onSelectOperation(wxCommandEvent&) {
    FindWindow(wxID_OK)->Enable(operationList_->GetSelection() != wxNOT_FOUND);

    operation_ = WxConversion::toString(operationList_->GetStringSelection());
}


/**
 * Event handler for opset filtering.
 */
void
OTAOperationDialog::onOperationFilterChange(wxCommandEvent& event) {
    std::string pattern(event.GetString().mb_str());
    std::string::iterator it;
    it = std::remove_if(pattern.begin(), pattern.end(), [](const char& c) {
        return c == ' ';
    });
    pattern.erase(it, pattern.end());
    for (auto& c : pattern) c = toupper(c);
    opNameFilter_ = pattern;
    opNameFilter_ = opNameFilter_.lower();
    OTAOperationDialog::TransferDataToWindow();
}

/**
 * Creates the dialog widgets.
 *
 * @param parent Parent window of the widgets.
 */
wxSizer*
OTAOperationDialog::createContents(wxWindow *parent, bool call_fit, bool set_sizer) {

    wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
    // Sizer for leftSizer and rightSizer
    wxBoxSizer *upperSizer = new wxBoxSizer(wxHORIZONTAL);

    // Sizer for oplistbox, filterlabel and filterinput
    wxBoxSizer *leftSizer = new wxBoxSizer(wxVERTICAL);
    // List of operations
    wxListBox *opListBox = new wxListBox(parent, ID_LIST, wxDefaultPosition,
        wxSize(210, 150), 0, NULL, wxLB_SINGLE|wxLB_SORT);
    leftSizer->Add(opListBox, 0, wxEXPAND|wxALL, 5);

    // Sizer for opNameFilterLabel and opNameFilter
    wxBoxSizer *filterSizer = new wxBoxSizer(wxHORIZONTAL);
    // TextLabel "Filter:"
    wxStaticText *opNameFilterLabel = new wxStaticText(parent,
        ID_OP_FILTER_LABEL, wxT("Filter:"), wxDefaultPosition, wxDefaultSize,
        0);
    // Operation filter input
    wxTextCtrl *opNameFilter = new wxTextCtrl(parent, ID_OP_FILTER, wxT(""),
        wxDefaultPosition, wxDefaultSize, 0);
    filterSizer->Add(opNameFilterLabel, 0, 0);
    filterSizer->Add(opNameFilter, 1, wxEXPAND);
    leftSizer->Add(filterSizer, 0, wxEXPAND|wxALL, 5);

    upperSizer->Add(leftSizer, 0, wxALL, 5);

    mainSizer->Add(upperSizer, 1, wxEXPAND);

    // Static line
    wxStaticLine *horisontalLine = new wxStaticLine(parent, ID_LINE,
        wxDefaultPosition, wxSize(20,-1), wxLI_HORIZONTAL);
    mainSizer->Add(horisontalLine, 0, wxEXPAND|wxALL, 5);

    // Sizer for Cancel and OK buttons
    wxBoxSizer *buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
    // Cancel button
    wxButton *cancelButton = new wxButton(parent, wxID_CANCEL, wxT("&Cancel"),
        wxDefaultPosition, wxDefaultSize, 0);
    buttonsSizer->Add(cancelButton, 0, wxALIGN_CENTER|wxALL, 5);
    // OK button
    wxButton *okButton = new wxButton(parent, wxID_OK, wxT("&OK"),
        wxDefaultPosition, wxDefaultSize, 0);
    buttonsSizer->Add(okButton, 0, wxALIGN_CENTER|wxALL, 5);

    mainSizer->Add(buttonsSizer, 0, 0, 5);

    if (set_sizer) {
        parent->SetSizer(mainSizer);
        if (call_fit) {
            mainSizer->SetSizeHints( parent );
        }
    }
    
    return mainSizer;
}
