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
 * @file OperationDialog.cc
 *
 * Implementation of OperationDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <list>
#include <boost/format.hpp>
#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/statline.h>

#include "OperationDialog.hh"
#include "FunctionUnit.hh"
#include "FUPort.hh"
#include "WxConversion.hh"
#include "HWOperation.hh"
#include "ExecutionPipeline.hh"
#include "PipelineElement.hh"
#include "InformationDialog.hh"
#include "WidgetTools.hh"
#include "WxConversion.hh"
#include "Conversion.hh"
#include "GUITextGenerator.hh"
#include "ProDeTextGenerator.hh"
#include "ContainerTools.hh"
#include "WidgetTools.hh"
#include "WarningDialog.hh"
#include "ProDeConstants.hh"
#include "MachineTester.hh"
#include "ConfirmDialog.hh"

using std::string;
using boost::format;
using namespace TTAMachine;

const int OperationDialog::GRID_COLUMN_WIDTH = 25;
const wxString OperationDialog::READ_MARK = _T("R");
const wxString OperationDialog::WRITE_MARK = _T("W");
const wxString OperationDialog::USE_MARK = _T("X");

// too long lines to keep doxygen quiet
BEGIN_EVENT_TABLE(OperationDialog, wxDialog)

    EVT_TEXT(ID_NAME, OperationDialog::onName)
    EVT_TEXT(ID_RESOURCE_NAME, OperationDialog::onName)

    EVT_BUTTON(ID_ADD_OPERAND, OperationDialog::onAddOperand)
    EVT_BUTTON(ID_DELETE_OPERAND, OperationDialog::onDeleteOperand)
    EVT_CHOICE(ID_PORT, OperationDialog::onBindOperand)

    EVT_BUTTON(wxID_OK, OperationDialog::onOK)

    EVT_BUTTON(ID_ADD_RESOURCE, OperationDialog::onAddResource)
    EVT_BUTTON(ID_DELETE_RESOURCE, OperationDialog::onDeleteResource)
    EVT_MENU(ID_DELETE_RESOURCE, OperationDialog::onDeleteResource)

    EVT_LIST_ITEM_SELECTED(ID_BIND_LIST, OperationDialog::onOperandSelection)
    EVT_LIST_ITEM_DESELECTED(ID_BIND_LIST, OperationDialog::onOperandSelection)

    EVT_GRID_CELL_LEFT_CLICK(OperationDialog::onGridLClick)
    EVT_GRID_CELL_RIGHT_CLICK(OperationDialog::onGridRClick)

    EVT_GRID_RANGE_SELECT(OperationDialog::onResourceSelection)
END_EVENT_TABLE()


/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param operation Operation to modify.
 */
OperationDialog::OperationDialog(
    wxWindow* parent,
    HWOperation* operation) :
    wxDialog(
        parent, -1, _T(""), wxDefaultPosition, wxDefaultSize, wxRESIZE_BORDER),
    operation_(operation),
    name_(_T("")),
    resourceName_(_T("")),
    latencyText_(NULL)  {

    createContents(this, true, true);
    initialize();
    setTexts();
}



/**
 * The Destructor.
 */
OperationDialog::~OperationDialog() {
}


/**
 * Initializes the dialog widgets and some class variables.
 */
void
OperationDialog::initialize() {

    // Name field validators.
    FindWindow(ID_NAME)->SetValidator(wxTextValidator(wxFILTER_ASCII, &name_));
    FindWindow(ID_RESOURCE_NAME)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &resourceName_));

    // Set widget pointers.
    bindList_ = dynamic_cast<wxListCtrl*>(FindWindow(ID_BIND_LIST));
    resourceGrid_ = dynamic_cast<wxGrid*>(FindWindow(ID_RESOURCE_GRID));
    usageGrid_ = dynamic_cast<wxGrid*>(FindWindow(ID_OPERAND_GRID));
    portChoice_ = dynamic_cast<wxChoice*>(FindWindow(ID_PORT));
    numberControl_ = dynamic_cast<wxSpinCtrl*>(FindWindow(ID_NUMBER));

    // Operand list.
    FindWindow(ID_DELETE_OPERAND)->Disable();
    FindWindow(ID_DELETE_RESOURCE)->Disable();

    // Resource grid.
    resourceGrid_->EnableEditing(false);
    resourceGrid_->SetDefaultCellAlignment(wxALIGN_CENTRE, wxALIGN_CENTRE);
    resourceGrid_->SetDefaultCellBackgroundColour(
        wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
    resourceGrid_->SetSelectionBackground(ProDeConstants::WHITE_COLOR);
    resourceGrid_->SetSelectionForeground(ProDeConstants::BLACK_COLOR);
    resourceGrid_->DisableDragColSize();
    resourceGrid_->DisableDragRowSize();

    // Operand usage grid.
    usageGrid_->EnableEditing(false);
    usageGrid_->DisableDragColSize();
    usageGrid_->DisableDragRowSize();
    usageGrid_->SetDefaultCellBackgroundColour(ProDeConstants::WHITE_COLOR);

    latencyText_ = dynamic_cast<wxStaticText*>(FindWindow(ID_LATENCY));
}


/**
 * Sets texts for widgets.
 */
void
OperationDialog::setTexts() {
    GUITextGenerator* generator = GUITextGenerator::instance();
    ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();

    // Dialog title
    format fmt = prodeTexts->text(
        ProDeTextGenerator::TXT_OPERATION_DIALOG_TITLE);
    SetTitle(WxConversion::toWxString(fmt.str()));

    // buttons
    WidgetTools::setLabel(generator, FindWindow(wxID_OK),
                          GUITextGenerator::TXT_BUTTON_OK);

    WidgetTools::setLabel(generator, FindWindow(wxID_CANCEL),
                          GUITextGenerator::TXT_BUTTON_CANCEL);

    WidgetTools::setLabel(generator, FindWindow(ID_HELP),
                          GUITextGenerator::TXT_BUTTON_HELP);

    WidgetTools::setLabel(generator, FindWindow(ID_ADD_OPERAND),
                          GUITextGenerator::TXT_BUTTON_ADD);

    WidgetTools::setLabel(generator, FindWindow(ID_DELETE_OPERAND),
                          GUITextGenerator::TXT_BUTTON_DELETE);

    WidgetTools::setLabel(generator, FindWindow(ID_ADD_RESOURCE),
                          GUITextGenerator::TXT_BUTTON_ADD);

    WidgetTools::setLabel(generator, FindWindow(ID_DELETE_RESOURCE),
                          GUITextGenerator::TXT_BUTTON_DELETE);


    // widget labels
    WidgetTools::setLabel(prodeTexts, FindWindow(ID_LABEL_NAME),
                          ProDeTextGenerator::TXT_LABEL_NAME);

    WidgetTools::setLabel(prodeTexts, FindWindow(ID_LABEL_PORT),
                          ProDeTextGenerator::TXT_LABEL_PORT);

    WidgetTools::setLabel(prodeTexts, FindWindow(ID_LABEL_OPERAND),
                          ProDeTextGenerator::TXT_LABEL_OPERAND);

    WidgetTools::setLabel(prodeTexts, FindWindow(ID_LABEL_RESOURCE_NAME),
                          ProDeTextGenerator::TXT_LABEL_NAME);

    // box sizer label
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_OPERATION_RESOURCES_BOX);
    WidgetTools::setWidgetLabel(resourceSizer_, fmt.str());

    fmt = prodeTexts->text(ProDeTextGenerator::TXT_OPERATION_OPERANDS_BOX);
    WidgetTools::setWidgetLabel(operandSizer_, fmt.str());

    fmt = prodeTexts->text(ProDeTextGenerator::TXT_OPERATION_USAGE_BOX);
    WidgetTools::setWidgetLabel(usageSizer_, fmt.str());

    // Create bind list columns.
    wxListCtrl* bindList =
        dynamic_cast<wxListCtrl*>(FindWindow(ID_BIND_LIST));
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_COLUMN_OPERAND);
    bindList->InsertColumn(0, WxConversion::toWxString(fmt.str()),
                           wxLIST_FORMAT_LEFT, 80);
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_COLUMN_PORT);
    bindList->InsertColumn(1, WxConversion::toWxString(fmt.str()),
                           wxLIST_FORMAT_LEFT, 100);
}


/**
 * Transfers data from the operation object to the dialog widgets.
 *
 * @return true, if the transfer was succesful, false otherwise
 */
bool
OperationDialog::TransferDataToWindow() {
    name_ = WxConversion::toWxString(operation_->name());
    updateOperandList();
    updateResourceGrid();
    updateUsageGrid();
    return wxDialog::TransferDataToWindow();
}


/**
 * Updates the operand bind list.
 */
void
OperationDialog::updateOperandList() {

    ExecutionPipeline* pipeline = operation_->pipeline();
    for (int cycle = 0; cycle <= pipeline->latency(); cycle++) {
        // Read operands.
        ExecutionPipeline::OperandSet operands =
            pipeline->readOperands(cycle);
        ExecutionPipeline::OperandSet::const_iterator iter =
            operands.begin();
        for (; iter != operands.end(); iter++) {
            operands_.insert(*iter);
        }
        // Written operands.
        operands = pipeline->writtenOperands(cycle);
        iter = operands.begin();
        for (; iter != operands.end(); iter++) {
            operands_.insert(*iter);
        }
    }

    // Search bound operands.
    for (int i = 0; i < operation_->parentUnit()->operationPortCount(); i++) {
        FUPort* port = operation_->parentUnit()->operationPort(i);
        if (operation_->isBound(*port)) {
            int operand = operation_->io(*port);
            operands_.insert(operand);
        }
    }

    // Add operand-port pairs to the operation list.
    std::set<int>::const_iterator iter = operands_.begin();
    bindList_->DeleteAllItems();
    for (; iter != operands_.end(); iter++) {
        wxString portName = _T("");
        if (operation_->port(*iter) != NULL) {
            portName =
                WxConversion::toWxString(operation_->port(*iter)->name());
        }
        bindList_->InsertItem(0, WxConversion::toWxString(*iter));
        bindList_->SetItem(0, 1, portName);
    }
    if (portChoice_->GetCount() > 1) {
        portChoice_->SetSelection(1);
    } else {
        portChoice_->SetSelection(0);
    }
    wxListEvent dummy;
    onOperandSelection(dummy);
    updateUsageGrid();
}


/**
 * Updates the list of pipeline resources.
 */
void
OperationDialog::updateResourceGrid() {

    int selected = -1;

    if (resourceGrid_->IsSelection()) {
        selected = resourceGrid_->GetSelectedRows().Item(0);
    }

    if (resourceGrid_->GetNumberCols() > 0) {
        resourceGrid_->DeleteCols(0, resourceGrid_->GetNumberCols());
    }
    if (resourceGrid_->GetNumberRows() > 0) {
        resourceGrid_->DeleteRows(0, resourceGrid_->GetNumberRows());
    }

    ExecutionPipeline* pipeline = operation_->pipeline();
    FunctionUnit* parent = operation_->parentUnit();

    resourceGrid_->AppendCols(pipeline->latency() + 50);
    resourceGrid_->AppendRows(parent->pipelineElementCount());

    resourceGrid_->EnableEditing(false);

    // Set column labels (cycle numbers).
    for (int i = 0; i < pipeline->latency() + 50; i++) {
        resourceGrid_->SetColLabelValue(i, WxConversion::toWxString(i));
        resourceGrid_->SetColSize(i, GRID_COLUMN_WIDTH);
    }

    // Set row labels (resource names).
    std::list<string> resources;
    for (int i = 0; i < parent->pipelineElementCount(); i++) {
        string label = parent->pipelineElement(i)->name();
        resources.push_back(label);
    }
    resources.sort();
    int row = 0;
    std::list<string>::const_iterator iter = resources.begin();
    for (; iter != resources.end(); iter++) {
        resourceGrid_->SetRowLabelValue(row, WxConversion::toWxString(*iter));
        row++;
    }

    // add resource uses to the grid
    for (int cycle = 0; cycle < (pipeline->latency() + 1); cycle++) {
        row = 0;
        for (iter = resources.begin(); iter != resources.end(); iter++) {
            if (pipeline->isResourceUsed((*iter), cycle)) {
                resourceGrid_->SetCellValue(row, cycle, USE_MARK);
            }
            row++;
        }
    }

    // Add resources that are not used
    newResources_.sort();
    iter = newResources_.begin();
    for (; iter != newResources_.end(); iter++) {
        resourceGrid_->AppendRows();
        resourceGrid_->SetRowLabelValue(row, WxConversion::toWxString(*iter));
        row++;
    }

    if (selected >= 0) {
        resourceGrid_->SelectRow(selected);
    }

    resourceGrid_->FitInside();
    updateLatency();
}


/**
 * Updates the operand usage grid.
 *
 * If a resource is selected in the resource grid, the grid is enabled
 * for editing and it displays the operand usage only for the selected
 * resource. If a resource isn't selected, the grid will be disabled,
 * and it displays summary of the operand use for all resources.
 */
void
OperationDialog::updateUsageGrid() {

    ExecutionPipeline* pipeline = operation_->pipeline();

    // Clear grid.
    if (usageGrid_->GetNumberCols() > 0) {
        usageGrid_->DeleteCols(0, usageGrid_->GetNumberCols());
    }
    if (usageGrid_->GetNumberRows() > 0) {
        usageGrid_->DeleteRows(0, usageGrid_->GetNumberRows());
    }

    usageGrid_->AppendCols(pipeline->latency() + 50);
    usageGrid_->AppendRows(operands_.size());

    // Reset row labels to numbers (clear R/W flags).
    int i = 0;
    std::set<int>::const_iterator iter = operands_.begin();
    for (; iter != operands_.end(); iter++) {
        usageGrid_->SetRowLabelValue(i, WxConversion::toWxString(*iter));
        i++;
    }

    // Update operand uses to the grid
    for (int cycle = 0; cycle < pipeline->latency() + 50; cycle++) {

        usageGrid_->SetColSize(cycle, GRID_COLUMN_WIDTH);
        usageGrid_->SetColLabelValue(
            cycle, WxConversion::toWxString(cycle));

        wxColour na = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);

        // Reads
        ExecutionPipeline::OperandSet operands =
            pipeline->readOperands(cycle);
        ExecutionPipeline::OperandSet::const_iterator iter =
            operands.begin();

        for (; iter != operands.end(); iter++) {
            int row = operandRow(*iter);
            assert(row >= 0);
            setOperandType(*iter, true);
            // operand read
            usageGrid_->SetCellValue(row, cycle, READ_MARK);
        }

        // Writes
        operands = pipeline->writtenOperands(cycle);
        iter = operands.begin();
        for (; iter != operands.end(); iter++) {
            int row = operandRow(*iter);
            assert(row >= 0);
            setOperandType(*iter, false);
            // operand written
            usageGrid_->SetCellValue(row, cycle, WRITE_MARK);
        }
    }

    usageGrid_->FitInside();
    updateLatency();
}


/**
 * Sets operand row label read/write flag.
 *
 * @param operand Operand to set the label to.
 * @param read True if operand is read, false if written.
 */
void
OperationDialog::setOperandType(int operand, bool read) {
    wxString label = WxConversion::toWxString(operand);
    label.Append(_T(" ("));
    if (read) {
        label.Append(READ_MARK);
    } else {
        label.Append(WRITE_MARK);
    }
    label.Append(_T(")"));
    int row = operandRow(operand);
    assert(row >= 0);
    usageGrid_->SetRowLabelValue(row , label);
}


/**
 * Warns if pipeline resources without usages exist
 */
void 
OperationDialog::warnOnResourcesWithoutUsages() {
    ExecutionPipeline* pipeline = operation_->pipeline();
    ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();
    for (std::list<string>::const_iterator iter = newResources_.begin();
        iter != newResources_.end(); iter++) {
            
        bool warn = true;
        for (int cycle = 0; cycle < pipeline->latency(); cycle++) {
            if (pipeline->isResourceUsed(*iter, cycle)) {
                warn = false;
                break;
            }
        }
        
        if (warn) {
            format fmt = prodeTexts->text(
                ProDeTextGenerator::MSG_WARN_RES_WITHOUT_USAGES);
            WarningDialog warning(this, WxConversion::toWxString(fmt.str() 
                + *iter));
            warning.ShowModal();
        }
    }
}


/**
 * Returns row nubmer of the operand in the resource usage grid.
 *
 * Returns -1 if the operand is not found.
 *
 * @param Operand to search.
 * @return Operand grid row number of the operand.
 */
int
OperationDialog::operandRow(int operand) {
    int row = 0;
    std::set<int>::const_iterator iter = operands_.begin();
    for (; iter != operands_.end(); iter++) {
        if (*iter == operand) {
            return row;
        }
        row++;
    }
    return -1;
}

/**
 * Handles the left mouse button event in both grids.
 *
 * The event is passed to the grid specific handler.
 *
 * @param event The mouse event to handle.
 */
void
OperationDialog::onGridLClick(wxGridEvent& event) {
    if (event.GetId() == ID_RESOURCE_GRID) {
        onResourceLClick(event);
    } else if (event.GetId() == ID_OPERAND_GRID) {
        onOperandLClick(event);
    }
}

/**
 * Handles the right mouse button event in both grids.
 *
 * @param event The mouse event to handle.
 */
void
OperationDialog::onGridRClick(wxGridEvent& event) {
    if (event.GetId() == ID_RESOURCE_GRID) {
        resourceGrid_->ClearSelection();
    }
}


/**
 * Handles left mouse button cliks in the resource grid.
 *
 * @param event The mouse event to handle.
 */
void
OperationDialog::onResourceLClick(wxGridEvent& event) {

    if (event.GetCol() < 0 || event.GetRow() < 0) {
        return;
    }

    ExecutionPipeline* pipeline = operation_->pipeline();
    int cycle = event.GetCol();
    string resource = selectedResource();

    // First click just selects the row.
    if (selectedResource() != WxConversion::toString(
            resourceGrid_->GetRowLabelValue(event.GetRow()))) {

        resourceGrid_->SelectRow(event.GetRow());
        return;
    }

    if (operation_->pipeline()->isResourceUsed(resource, cycle)) {
        //Try to remove resource use.
        try {
            pipeline->removeResourceUse(resource, cycle);
        } catch (StartTooLate& e) {
            ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();
            format fmt = prodeTexts->text(
                ProDeTextGenerator::MSG_ERROR_PIPELINE_START);
            InformationDialog dialog(
                this, WxConversion::toWxString(fmt.str()));
            dialog.ShowModal();
            return;
        }
        if (!operation_->parentUnit()->hasPipelineElement(resource)) {
            newResources_.push_back(resource);
        }
    } else {
        // Try to add resource use.
        try {
            pipeline->addResourceUse(resource, cycle, 1);
        } catch (StartTooLate& e) {
            ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();
            format fmt = prodeTexts->text(
                ProDeTextGenerator::MSG_ERROR_PIPELINE_START);
            InformationDialog dialog(
                this, WxConversion::toWxString(fmt.str()));
            dialog.ShowModal();
            return;
        }
        ContainerTools::removeValueIfExists(newResources_, resource);
    }
    updateResourceGrid();
    resourceGrid_->SelectRow(event.GetRow());
}

/**
 * Handles left mouse button clicks in the operand usage grid.
 *
 * @param event The mouse event to handle.
 */
void
OperationDialog::onOperandLClick(wxGridEvent& event) {

    int cycle = event.GetCol();

    std::set<int>::const_iterator iter = operands_.begin();
    for (int i = 0; i < event.GetRow(); i++) {
        assert(iter != operands_.end());
        iter++;
    }
    int operand = *iter;
    ExecutionPipeline* pipeline = operation_->pipeline();


    bool read = false;
    bool written = false;

    // Check if the operand is already being read.
    ExecutionPipeline::OperandSet readOperands =
        pipeline->readOperands(cycle);
    if (ContainerTools::containsValue(readOperands, operand)) {
        read = true;
    }
    // Check if the operand is already being written.
    ExecutionPipeline::OperandSet writtenOperands =
        pipeline->writtenOperands(cycle);
    if (ContainerTools::containsValue(writtenOperands, operand)) {
        written = true;
    }

    bool toggled = false;
    if (written == false && read == false) {
        try {
            // not used -> read.
            pipeline->addPortRead(operand, cycle, 1);
            toggled = true;
        } catch (StartTooLate& e) {
            ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();
            format fmt = prodeTexts->text(
                ProDeTextGenerator::MSG_ERROR_PIPELINE_START);
            InformationDialog dialog(
                this, WxConversion::toWxString(fmt.str()));
            dialog.ShowModal();
            return;
        } catch (Exception& e) {
            // do nothing
        }
    }
    if (toggled == false && read == false && written == false) {
        try {
            // not used -> written
            pipeline->addPortWrite(operand, cycle, 1);
            toggled = true;
        } catch (Exception& e) {
            // do nothing
        }
    }
    if (toggled == false && read == true && written == false) {
        try {
            // read -> written
            pipeline->removeOperandUse(operand, cycle);
            pipeline->addPortWrite(operand, cycle, 1);
            toggled = true;
        } catch (StartTooLate& e) {
            ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();
            format fmt = prodeTexts->text(
                ProDeTextGenerator::MSG_ERROR_PIPELINE_START);
            InformationDialog dialog(
                this, WxConversion::toWxString(fmt.str()));
            dialog.ShowModal();
            return;
        } catch (Exception& e) {
            // do nothing
        }
    }
    if (toggled == false && (read == true || written == true)) {
        try {
            // written/read -> not used
            pipeline->removeOperandUse(operand, cycle);
            toggled = true;
        } catch (StartTooLate& e) {
            ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();
            format fmt = prodeTexts->text(
                ProDeTextGenerator::MSG_ERROR_PIPELINE_START);
            InformationDialog dialog(
                this, WxConversion::toWxString(fmt.str()));
            dialog.ShowModal();
            return;
        } catch (Exception& e) {
            // do nothing
        }
    }
    updateResourceGrid();
    updateUsageGrid();
    usageGrid_->MakeCellVisible(event.GetRow(), event.GetCol());
}

/**
 * Checks whether name field is empty and disables OK button of the
 * dialog if it is.
 */
void
OperationDialog::onName(wxCommandEvent&) {

    if (!TransferDataFromWindow()) {
        assert(false);
    }

    // OK-button.
    wxString trimmedName = name_.Trim(false).Trim(true);
    if (trimmedName == _T("")) {
        FindWindow(wxID_OK)->Disable();
    } else {
        FindWindow(wxID_OK)->Enable();
    }

    // Add resource button.
    wxString trimmedResourceName = resourceName_.Trim(false).Trim(true);
    if (trimmedResourceName == _T("")) {
        FindWindow(ID_ADD_RESOURCE)->Disable();
    } else {
        FindWindow(ID_ADD_RESOURCE)->Enable();
    }
}


/**
 * Handles the Add bind button event.
 *
 * Adds a bind to the operation according to the number and port selection
 * widgets.
 */
void
OperationDialog::onAddOperand(wxCommandEvent&) {

    int number = numberControl_->GetValue();

    std::pair<std::set<int>::iterator, bool> result =
        operands_.insert(number);

    // Check that the number is not reserved for an operand.
    if (!result.second) {
        ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();
        format fmt = prodeTexts->text(
            ProDeTextGenerator::MSG_ERROR_OPERAND_NUM_RESERVED);
        fmt % number;
        InformationDialog dialog(this, WxConversion::toWxString(fmt.str()));
        dialog.ShowModal();
        return;
    }

    updateOperandList();
    numberControl_->SetValue(number + 1);

/*
    } else {
    }
*/
}


/**
 * Removes the selected bind from the operation.
 */
void
OperationDialog::onDeleteOperand(wxCommandEvent&) {

    ExecutionPipeline* pipeline = operation_->pipeline();

    string selected = WidgetTools::lcStringSelection(bindList_, 0);
    if (selected == "") {
        return;
    }

    int operand = Conversion::toInt(selected);

    // If the selected operand is read or written, a dialog is displayed
    // that allows user to choose if the operand reads and writes are
    // also cleared.
    if (ContainerTools::containsValue(pipeline->readOperands(), operand) ||
        ContainerTools::containsValue(pipeline->writtenOperands(), operand)) {

        ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();

        format msgFmt = prodeTexts->text(
                    ProDeTextGenerator::MSG_CONFIRM_OPERAND_DELETION);
        msgFmt % operand;
        ConfirmDialog dialog(this, WxConversion::toWxString(msgFmt.str()));

        int choice = dialog.ShowModal();
        if (choice == wxID_YES) {
            try {
                for (int c = pipeline->latency() - 1; c >= 0; c--) {
                    pipeline->removeOperandUse(operand, c);
                }
            } catch (StartTooLate& e) {
                format fmt = prodeTexts->text(
                    ProDeTextGenerator::MSG_ERROR_PIPELINE_START);
                InformationDialog dialog(
                    this, WxConversion::toWxString(fmt.str()));
                dialog.ShowModal();
                return;
            }
        } else if (choice != wxID_NO) {
            return;
        }
    }

    ContainerTools::removeValueIfExists(operands_, operand);

    FUPort* port = operation_->port(operand);
    if (port != NULL) {
        operation_->unbindPort(*port);
    }
    updateOperandList();
}

/**
 * Event handler for the operand port binding choicer.
 */
void
OperationDialog::onBindOperand(wxCommandEvent&) {

    string selected = WidgetTools::lcStringSelection(bindList_, 0);
    int number = Conversion::toInt(selected);

    if (portChoice_->GetSelection() > 0) {
        string portName=
            WxConversion::toString(portChoice_->GetStringSelection());

        FUPort* port = operation_->parentUnit()->operationPort(portName);
        operation_->bindPort(number, *port);
    } else {
        FUPort* port = operation_->port(number);
        if (port != NULL) {
            operation_->unbindPort(*port);
        }
    }
    updateOperandList();
}


/**
 * Handles the bind list item selection events.
 *
 * Enables and disables the delete bind button according to the selection.
 */
void
OperationDialog::onOperandSelection(wxListEvent&) {

    portChoice_->Clear();
    portChoice_->Append(_T("none"));

    string selected = WidgetTools::lcStringSelection(bindList_, 0);

    if (selected == "") {
        FindWindow(ID_DELETE_OPERAND)->Disable();
        FindWindow(ID_PORT)->Disable();
        return;
    } else {
        int selection = 0;
        int operand = Conversion::toInt(selected);
        FindWindow(ID_DELETE_OPERAND)->Enable();
        FindWindow(ID_PORT)->Enable();

        // Search bound operands.
        for (int i = 0; i < operation_->parentUnit()->operationPortCount();
             i++) {

            FUPort* port = operation_->parentUnit()->operationPort(i);

            if (!operation_->isBound(*port)) {
                portChoice_->Append(WxConversion::toWxString(port->name()));
            } else if (operation_->io(*port) == operand) {
                selection = portChoice_->Append(
                    WxConversion::toWxString(port->name()));
            }
        }
        portChoice_->SetSelection(selection);
    }
}


/**
 * Handles the resource list item selection events.
 *
 * Enables and disables the edit and delete resource buttons
 * according to the selection.
 */
void
OperationDialog::onResourceSelection(wxGridRangeSelectEvent&) {
    if (resourceGrid_->IsSelection()) {
        FindWindow(ID_DELETE_RESOURCE)->Enable();
    } else {
        FindWindow(ID_DELETE_RESOURCE)->Disable();
    }
    updateUsageGrid();
}


/**
 * Returns name of the resource selected in the resource grid.
 *
 * If a resource is not selected, an empty string is returned.
 */
string
OperationDialog::selectedResource() {
    if (!resourceGrid_->IsSelection()) {
        return "";
    }
    // Sometimes GetSelectedRows() returns an empty array even if
    // IsSelection() returns true.
    if (resourceGrid_->GetSelectedRows().Count() != 1) {
        resourceGrid_->ClearSelection();
        return "";
    }
    int row = resourceGrid_->GetSelectedRows().Item(0);
    return WxConversion::toString(resourceGrid_->GetRowLabelValue(row));
}


/**
 * Handles the Add resource button event.
 *
 * Opens pipeline resource dialog for adding a new resource to the operation.
 */
void
OperationDialog::onAddResource(wxCommandEvent&) {

    string newName = WxConversion::toString(resourceName_);

    // Check the name validity.
    if (!MachineTester::isValidComponentName(newName)) {
        ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();
        format message =
            prodeTexts->text(ProDeTextGenerator::MSG_ERROR_ILLEGAL_NAME);
        InformationDialog warning(
            this, WxConversion::toWxString(message.str()));
        warning.ShowModal();
        return;
    }

    // Check that the resource name is not reserved.
    if (operation_->parentUnit()->hasPipelineElement(newName) ||
        ContainerTools::containsValue(newResources_, newName)) {

        ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();
        format message =
            prodeTexts->text(ProDeTextGenerator::MSG_ERROR_SAME_NAME);
        format component =
            prodeTexts->text(ProDeTextGenerator::COMP_A_RESOURCE);
        format functionUnit =
            prodeTexts->text(ProDeTextGenerator::COMP_FUNCTION_UNIT);
        message % newName % component.str() % functionUnit.str();
        component = prodeTexts->text(ProDeTextGenerator::COMP_RESOURCE);
        message % component.str();
        WarningDialog warning(this, WxConversion::toWxString(message.str()));
        warning.ShowModal();
        return;
    }
    newResources_.push_back(newName);
    updateResourceGrid();
    dynamic_cast<wxTextCtrl*>(FindWindow(ID_RESOURCE_NAME))->Clear();
}


/**
 * Handles the delete resource button event.
 *
 * Deletes the selected resource usage from the operation.
 */
void
OperationDialog::onDeleteResource(wxCommandEvent&) {

    string selected = selectedResource();

    // Search resource from the unused resources.
    if (selected == "" ||
        ContainerTools::removeValueIfExists(newResources_, selected)) {

        updateResourceGrid();
        return;
    }

    // Remove resource from the operation pipeline.
    try {
        operation_->pipeline()->removeResourceUse(selected);
    } catch (StartTooLate& e) {
        ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();
        format fmt = prodeTexts->text(
            ProDeTextGenerator::MSG_ERROR_PIPELINE_START);
        InformationDialog dialog(
            this, WxConversion::toWxString(fmt.str()));
        dialog.ShowModal();
        return;
    }
    updateResourceGrid();
}


/**
 * Handles the OK-button event.
 *
 * Updates the operation and closes the dialog.
 */
void
OperationDialog::onOK(wxCommandEvent&) {

    TransferDataFromWindow();
    string newName =
        WxConversion::toString(name_.Trim(true).Trim(false));

    // Check the name validity.
    if (!MachineTester::isValidComponentName(newName)) {
        ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();
        format message =
            prodeTexts->text(ProDeTextGenerator::MSG_ERROR_ILLEGAL_NAME);
        InformationDialog warning(
            this, WxConversion::toWxString(message.str()));
        warning.ShowModal();
        return;
    }

    try {
        operation_->setName(newName);
    } catch (ComponentAlreadyExists& e) {

        // Display an error message indicating that the name is reserved for
        // another operation.
        ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();
        format message =
            prodeTexts->text(ProDeTextGenerator::MSG_ERROR_SAME_NAME);
        format component =
            prodeTexts->text(ProDeTextGenerator::COMP_AN_OPERATION);

        format bus = prodeTexts->text(ProDeTextGenerator::COMP_MACHINE);
        message % newName % component.str() % bus.str();
        component = prodeTexts->text(ProDeTextGenerator::COMP_OPERATION);
        message % component.str();

        InformationDialog dialog(
            this, WxConversion::toWxString(message.str()));

        dialog.ShowModal();
        return;
    }
    
    warnOnResourcesWithoutUsages();
    
    EndModal(wxID_OK);
}

/**
 * Updates the latency information static text widget.
 */
void
OperationDialog::updateLatency() {
    wxString latency = WxConversion::toWxString(operation_->latency());
    latencyText_->SetLabel(latency);
}

/**
 * Creates the dialog contents.
 *
 * This function was generated by wxDesigner.
 *
 * @return Main sizer of the created contents.
 * @param parent The dialog window.
 * @param call_fit If true, fits the contents inside the dialog.
 * @param set_sizer If true, sets the main sizer as dialog contents.
 */
wxSizer*
OperationDialog::createContents(
    wxWindow *parent, bool call_fit, bool set_sizer) {

    wxFlexGridSizer *item0 = new wxFlexGridSizer( 1, 0, 0 );
    item0->AddGrowableCol( 0 );
    item0->AddGrowableRow( 0 );

    wxFlexGridSizer *item1 = new wxFlexGridSizer( 2, 0, 0 );
    item1->AddGrowableCol( 1 );
    item1->AddGrowableRow( 0 );

    wxFlexGridSizer *item2 = new wxFlexGridSizer( 1, 0, 0 );
    item2->AddGrowableRow( 1 );

    wxBoxSizer *item3 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticText *item4 = new wxStaticText( parent, ID_LABEL_NAME, wxT("Name:"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( item4, 0, wxALIGN_CENTER|wxALL, 5 );

    wxTextCtrl *item5 = new wxTextCtrl( parent, ID_NAME, wxT(""), wxDefaultPosition, wxSize(160,-1), 0 );
    item3->Add( item5, 0, wxALIGN_CENTER, 5 );

    item2->Add( item3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticBox *item7 = new wxStaticBox( parent, -1, wxT("Operands:") );
    wxStaticBoxSizer *item6 = new wxStaticBoxSizer( item7, wxHORIZONTAL );
    operandSizer_ = item6;

    wxFlexGridSizer *item8 = new wxFlexGridSizer( 1, 0, 0 );
    item8->AddGrowableRow( 0 );

    wxListCtrl *item9 = new wxListCtrl( parent, ID_BIND_LIST, wxDefaultPosition, wxSize(200,300), wxLC_REPORT|wxSUNKEN_BORDER );
    item8->Add( item9, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item10 = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *item11 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticText *item12 = new wxStaticText( parent, ID_LABEL_PORT, wxT("Port:"), wxDefaultPosition, wxDefaultSize, 0 );
    item11->Add( item12, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxString *strs13 = (wxString*) NULL;
    wxChoice *item13 = new wxChoice( parent, ID_PORT, wxDefaultPosition, wxSize(100,-1), 0, strs13, 0 );
    item11->Add( item13, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxButton *item14 = new wxButton( parent, ID_DELETE_OPERAND, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    item11->Add( item14, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item10->Add( item11, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxStaticLine *item15 = new wxStaticLine( parent, ID_LINE, wxDefaultPosition, wxSize(20,-1), wxLI_HORIZONTAL );
    item10->Add( item15, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item16 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticText *item17 = new wxStaticText( parent, ID_LABEL_OPERAND, wxT("New operand:"), wxDefaultPosition, wxDefaultSize, 0 );
    item16->Add( item17, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxSpinCtrl *item18 = new wxSpinCtrl( parent, ID_NUMBER, wxT("1"), wxDefaultPosition, wxSize(60,-1), 0, 1, 10000, 1 );
    item16->Add( item18, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxButton *item19 = new wxButton( parent, ID_ADD_OPERAND, wxT("Add"), wxDefaultPosition, wxDefaultSize, 0 );
    item16->Add( item19, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item10->Add( item16, 0, wxALIGN_CENTER, 5 );

    item8->Add( item10, 0, wxALIGN_CENTER|wxALL, 5 );

    item6->Add( item8, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    item2->Add( item6, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    item1->Add( item2, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxFlexGridSizer *item20 = new wxFlexGridSizer( 1, 0, 0 );
    item20->AddGrowableCol( 0 );
    item20->AddGrowableRow( 0 );
    item20->AddGrowableRow( 1 );

    wxStaticBox *item22 = new wxStaticBox( parent, -1, wxT("Pipeline Resources:") );
    wxStaticBoxSizer *item21 = new wxStaticBoxSizer( item22, wxHORIZONTAL );
    resourceSizer_ = item21;

    wxFlexGridSizer *item23 = new wxFlexGridSizer( 1, 0, 0 );
    item23->AddGrowableCol( 0 );
    item23->AddGrowableRow( 0 );

    wxGrid *item24 = new wxGrid( parent, ID_RESOURCE_GRID, wxDefaultPosition, wxSize(400,200), wxWANTS_CHARS );
    item24->CreateGrid( 0, 0, wxGrid::wxGridSelectRows );
    item23->Add( item24, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item25 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticText *item26 = new wxStaticText( parent, ID_LABEL_RESOURCE_NAME, wxT("Name:"), wxDefaultPosition, wxDefaultSize, 0 );
    item25->Add( item26, 0, wxALIGN_CENTER|wxALL, 5 );

    wxTextCtrl *item27 = new wxTextCtrl( parent, ID_RESOURCE_NAME, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    item25->Add( item27, 1, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item28 = new wxButton( parent, ID_ADD_RESOURCE, wxT("Add"), wxDefaultPosition, wxDefaultSize, 0 );
    item28->Enable( false );
    item25->Add( item28, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item29 = new wxButton( parent, ID_DELETE_RESOURCE, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    item29->Enable( false );
    item25->Add( item29, 0, wxALIGN_CENTER|wxALL, 5 );

    //item25->Add( 30, 20, 0, wxALIGN_CENTER|wxALL, 5 );

    item23->Add( item25, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item21->Add( item23, 1, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    item20->Add( item21, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxBOTTOM, 5 );

    wxStaticBox *item33 = new wxStaticBox( parent, -1, wxT("Operand usage:") );
    wxStaticBoxSizer *item32 = new wxStaticBoxSizer( item33, wxHORIZONTAL );
    usageSizer_ = item32;

    wxGrid *item34 = new wxGrid( parent, ID_OPERAND_GRID, wxDefaultPosition, wxSize(400,200), wxWANTS_CHARS );
    item34->CreateGrid( 0, 0, wxGrid::wxGridSelectCells );
    item32->Add( item34, 1, wxGROW|wxALL, 5 );

    item20->Add( item32, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxBoxSizer *item35 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticText *item36 = new wxStaticText( parent, ID_LABEL_LATENCY, wxT("Operation latency:"), wxDefaultPosition, wxDefaultSize, 0 );
    item35->Add( item36, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item37 = new wxStaticText( parent, ID_LATENCY, wxT("                 "), wxDefaultPosition, wxDefaultSize, 0 );
    item35->Add( item37, 0, wxALIGN_CENTER|wxALL, 5 );

    item20->Add( item35, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item1->Add( item20, 0, wxGROW, 5 );

    item0->Add( item1, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item38 = new wxBoxSizer( wxVERTICAL );

    wxStaticLine *item39 = new wxStaticLine( parent, ID_LINE, wxDefaultPosition, wxSize(20,-1), wxLI_HORIZONTAL );
    item38->Add( item39, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxGridSizer *item40 = new wxGridSizer( 2, 0, 0 );

    wxButton *item41 = new wxButton( parent, ID_HELP, wxT("&Help"), wxDefaultPosition, wxDefaultSize, 0 );
    item40->Add( item41, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item42 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item43 = new wxButton( parent, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item42->Add( item43, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item44 = new wxButton( parent, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item42->Add( item44, 0, wxALIGN_CENTER|wxALL, 5 );

    item40->Add( item42, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

    item38->Add( item40, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    item0->Add( item38, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    if (set_sizer) {
        parent->SetSizer( item0 );
        if (call_fit) {
            item0->SetSizeHints( parent );
        }
    }
    
    return item0;
}
