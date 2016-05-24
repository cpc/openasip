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
 * @file OpsetDialog.cc
 *
 * Implementation of OpsetDialog class.
 *
 * @author Veli-Pekka J��skel�inen 2006 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <algorithm>
#include <string>

#include <wx/spinctrl.h>
#include <wx/statline.h>
#include <wx/textctrl.h>
#include "OpsetDialog.hh"
#include "HWOperation.hh"
#include "Operation.hh"
#include "FunctionUnit.hh"
#include "OperationPool.hh"
#include "OperationModule.hh"
#include "OperationIndex.hh"
#include "WxConversion.hh"
#include "ErrorDialog.hh"
#include "FUPort.hh"
#include "ExecutionPipeline.hh"
#include "Operand.hh"

using namespace TTAMachine;

BEGIN_EVENT_TABLE(OpsetDialog, wxDialog)
    EVT_LISTBOX(ID_LIST, OpsetDialog::onSelectOperation)
    EVT_BUTTON(wxID_OK, OpsetDialog::onOK)
    EVT_TEXT(ID_OP_FILTER, OpsetDialog::onOperationFilterChange)
END_EVENT_TABLE()

/**
 * The constructor.
 *
 * @param parent Parent window of the dialog.
 */
OpsetDialog::OpsetDialog(wxWindow* parent):
    wxDialog(parent, -1, _T("Choose operation & latency")),
    latency_(1), operation_("") {

    createContents(this, true, true);
    operationList_ = dynamic_cast<wxListBox*>(FindWindow(ID_LIST));
    FindWindow(wxID_OK)->Disable();
}

/**
 * The Destructor.
 */
OpsetDialog::~OpsetDialog() {
}

/**
 * Transfers data to the opset list.
 */
bool
OpsetDialog::TransferDataToWindow() {

    operationList_->Clear();

    // Cleanup operation pool cache so new operations
    // appear w/o restarting prode.
    OperationPool::cleanupCache();
    OperationPool pool;
    OperationIndex& index = pool.index();
    std::set<std::string> opset;
    for (int m = 0; m < index.moduleCount(); m++) {
        OperationModule& module = index.module(m);
        for (int i = 0; i < index.operationCount(module); i++) {
            std::string opName = index.operationName(i, module);
            if (opNameFilter_.empty()
                || opName.find(opNameFilter_) != std::string::npos) {
                opset.insert(opName);
            }
        }
    }

    for (const auto& opName : opset) {
        operationList_->Append(WxConversion::toWxString(opName));
    }

    return true;
}

/**
 * Reads user choices from the dialog widgets.
 */
bool
OpsetDialog::TransferDataFromWindow() {
    latency_ = dynamic_cast<wxSpinCtrl*>(FindWindow(ID_LATENCY))->GetValue();
    operation_ = WxConversion::toString(operationList_->GetStringSelection());
    return true;
}

/**
 * Event handler for the operation list selections.
 *
 * Enables and disables the OK button.
 */
void
OpsetDialog::onSelectOperation(wxCommandEvent&) {
    FindWindow(wxID_OK)->Enable(operationList_->GetSelection() != wxNOT_FOUND);
}


/**
 * Event handler for the OK button.
 */
void
OpsetDialog::onOK(wxCommandEvent&) {
    if (operationList_->GetSelection() == wxNOT_FOUND) {
        wxString message = _T("No operation selected.");
        ErrorDialog dialog(this, message);
        dialog.ShowModal();
        return;
    }
    TransferDataFromWindow();
    EndModal(wxID_OK);
}


/**
 * Event handler for opset filtering.
 */
void
OpsetDialog::onOperationFilterChange(wxCommandEvent& event) {
    std::string pattern(event.GetString().mb_str());
    std::string::iterator it;
    it = std::remove_if(pattern.begin(), pattern.end(), [](const char& c) {
        return c == ' ';
    });
    pattern.erase(it, pattern.end());
    for (auto& c : pattern) c = toupper(c);
    opNameFilter_ = pattern;
    OpsetDialog::TransferDataToWindow();
}


/**
 * Creates a new HWOperation based on user choices.
 *
 * @param fu Parent function unit of the new operation.
 * @return Pointer to the created operation.
 */
HWOperation*
OpsetDialog::createOperation(FunctionUnit& fu) {

    // Check that the function unit doesn't already have an operation with
    // the selected name.
    if (fu.hasOperation(operation_)) {
        wxString message = _T("Function unit already contains operation '");
        message.Append(WxConversion::toWxString(operation_));
        message.Append(_T("'."));
        ErrorDialog dialog(this, message);
        dialog.ShowModal();
        return NULL;
    }

    OperationPool pool;
    const Operation& op = pool.operation(operation_.c_str());
    
    HWOperation* operation = new HWOperation(operation_, fu);

    // Read operation operand information from the operation pool.
    std::set<int> inputs;
    std::set<int> outputs;
    for (int i = 1; i <= op.numberOfInputs() + op.numberOfOutputs(); i++) {
        const Operand& oper = op.operand(i);

        if (oper.isInput()) {
            inputs.insert(oper.index());
            operation->pipeline()->addPortRead(oper.index(), 0, 1);
        } else if (oper.isOutput()) {
            outputs.insert(oper.index());
            if (!inputs.empty()) {
                // 0 and 1 latency means that the output operand is written
                // on cycle 0.
                int latency = (latency_ > 0) ? latency_ - 1 : latency_;
                operation->pipeline()->addPortWrite(oper.index(), latency, 1);
            }
        }
    }

    // Try to bind operation operands to function unit ports.
    for (int i = 0; i < fu.operationPortCount(); i++) {
        const FUPort* port = fu.operationPort(i);

        // Input ports: triggering port is bound to the last input operand.
        if (port->inputSocket() != NULL && !inputs.empty()) {
            if (port->isTriggering()) {
                std::set<int>::iterator iter = --inputs.end();
                operation->bindPort(*iter, *port);
                inputs.erase(iter);
            } else {                
                std::set<int>::iterator iter = inputs.begin();
                operation->bindPort(*iter, *port);
                inputs.erase(iter);
            }
        } else if (port->outputSocket() != NULL && !outputs.empty()) {
            // Output ports.
            std::set<int>::iterator iter = outputs.begin();
            operation->bindPort(*iter, *port);
            outputs.erase(iter);
        } else if (!inputs.empty() && port->outputSocket() == NULL) {
            std::set<int>::iterator iter = inputs.begin();
            operation->bindPort(*iter, *port);
            inputs.erase(iter);
        } else if (!outputs.empty() && port->inputSocket() == NULL) {
            std::set<int>::iterator iter = outputs.begin();
            operation->bindPort(*iter, *port);
            outputs.erase(iter);
        }
    }

    // Display an error dialog and abort if the operands couldn't be bound
    // to ports.
    if (!inputs.empty() || !outputs.empty()) {
        wxString message;
        if (!inputs.empty()) {
            message.Append(
                _T("Not enough input ports for the operation "
                   "input operands.\n"));
        }
        if (!outputs.empty()) {
            message.Append(
                _T("Not enough output ports for the operation "
                   "output operands.\n"));
        }
        ErrorDialog dialog(this, message);
        dialog.ShowModal();
        delete operation;
        return NULL;
    }

    return operation;
}

/**
 * Creates the dialog widgets.
 *
 * Code generated by wxDesigner, do not modify.
 *
 * @param parent Parent window of the widgets.
 */
wxSizer*
OpsetDialog::createContents(wxWindow *parent, bool call_fit, bool set_sizer) {

    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxString *strs1 = (wxString*) NULL;
    wxListBox *item1 = new wxListBox( parent, ID_LIST, wxDefaultPosition, wxSize(100,200), 0, strs1, wxLB_SINGLE | wxLB_SORT );
    item0->Add( item1, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticText *opNameFilterLabel = new wxStaticText( parent,
        ID_OP_FILTER_LABEL, wxT("Filter:"), wxDefaultPosition, wxDefaultSize,
        0 );
    item0->Add(opNameFilterLabel, 0,
        wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxTextCtrl *opNameFilter = new wxTextCtrl(parent, ID_OP_FILTER, wxT(""),
        wxDefaultPosition, wxSize(100, -1), 0);
    item0->Add(opNameFilter, 1, wxEXPAND|wxALIGN_LEFT|wxLEFT|wxRIGHT, 5 );

    wxBoxSizer *item2 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticText *item3 = new wxStaticText( parent, ID_TEXT, wxT("Latency:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item3, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxSpinCtrl *item4 = new wxSpinCtrl( parent, ID_LATENCY, wxT("1"), wxDefaultPosition, wxSize(50,-1), 0, 1, 100, 1 );
    item2->Add( item4, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item2, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticLine *item5 = new wxStaticLine( parent, ID_LINE, wxDefaultPosition, wxSize(20,-1), wxLI_HORIZONTAL );
    item0->Add( item5, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item6 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item7 = new wxButton( parent, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item6->Add( item7, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item8 = new wxButton( parent, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item6->Add( item8, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item6, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;
}
