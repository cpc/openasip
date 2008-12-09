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
 * @file ProximMachineStateWindow.cc
 *
 * Definition of ProximMachineStateWindow class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <wx/wx.h>
#include <wx/statline.h>
#include "ProximMachineStateWindow.hh"
#include "ProximSimulationThread.hh"
#include "ProximMachineCanvasTool.hh"
#include "TracedSimulatorFrontend.hh"
#include "MachineCanvas.hh"
#include "Machine.hh"
#include "Proxim.hh"
#include "MachineEditPartFactory.hh"
#include "ProximEditPolicyFactory.hh"
#include "Instruction.hh"
#include "Move.hh"
#include "WxConversion.hh"
#include "Terminal.hh"
#include "EditPart.hh"
#include "Figure.hh"
#include "Port.hh"
#include "Bus.hh"
#include "ProximToolbox.hh"
#include "UtilizationStats.hh"
#include "ExecutableInstruction.hh"
#include "MachinePart.hh"
#include "ProximConstants.hh"
#include "ErrorDialog.hh"
#include "FileSystem.hh"
#include "Request.hh"
#include "ComponentCommand.hh"

BEGIN_EVENT_TABLE(ProximMachineStateWindow, ProximSimulatorWindow)
    EVT_SIMULATOR_STOP(0, ProximMachineStateWindow::onSimulationStop)
    EVT_SIMULATOR_PROGRAM_LOADED(0, ProximMachineStateWindow::onProgramLoaded)
    EVT_MENU(COMMAND_ZOOM_IN, ProximMachineStateWindow::onZoom)
    EVT_MENU(COMMAND_ZOOM_OUT, ProximMachineStateWindow::onZoom)
    EVT_MENU(COMMAND_TOGGLE_UNIT_INFO, ProximMachineStateWindow::onToggleUnitInfo)
    EVT_MENU(COMMAND_TOGGLE_MOVES, ProximMachineStateWindow::onToggleMoves)
    EVT_MENU(COMMAND_TOGGLE_UTILIZATIONS, ProximMachineStateWindow::onToggleUtilizations)
    EVT_MENU(COMMAND_EXPORT, ProximMachineStateWindow::onExport)
    EVT_UPDATE_UI_RANGE(COMMAND_TOGGLE_UNIT_INFO, COMMAND_TOGGLE_UTILIZATIONS, ProximMachineStateWindow::onUpdateUIEvent)
END_EVENT_TABLE()

using std::string;
using namespace TTAMachine;
using namespace TTAProgram;

const double ProximMachineStateWindow::ZOOM_STEP = 0.2;
const double ProximMachineStateWindow::MIN_ZOOM_FACTOR = 0.5;
const double ProximMachineStateWindow::MAX_ZOOM_FACTOR = 4;
const int ProximMachineStateWindow::MINIMUM_PANE_WIDTH = 150;
const int ProximMachineStateWindow::INITIAL_DETAILS_PANE_WIDTH = 200;

/**
 * Constructor.
 *
 * @param parent Parent window of the window.
 * @param id Window identifier.
 */
ProximMachineStateWindow::ProximMachineStateWindow(
    ProximMainFrame* parent, int id):
    ProximSimulatorWindow(parent, id, wxDefaultPosition, wxSize(800,600)),
    showMoves_(true), showUtilizations_(true),
    detailsCtrl_(NULL), utilizationCtrl_(NULL) {

    createContents();

    SetSizeHints(400, 300);

    simulator_ = wxGetApp().simulation()->frontend();

    if (simulator_->isSimulationInitialized() ||
	simulator_->isSimulationStopped() ||
	simulator_->isSimulationRunning() ||
	simulator_->hasSimulationEnded()) {

	TTAMachine::Machine* machine =
            const_cast<Machine*>(&ProximToolbox::machine());

	canvas_->setMachine(machine);
    
    if (showUtilizations_) {
        setUtilizationHighlights();
    }
    
    }
}


/**
 * Destructor.
 */
ProximMachineStateWindow::~ProximMachineStateWindow() {    
}


/**
 * Event handler which is called when a new program is loaded in the simulator.
 */
void
ProximMachineStateWindow::onProgramLoaded(const SimulatorEvent&) {
    TTAMachine::Machine* machine =
        const_cast<Machine*>(&simulator_->machine());
    canvas_->setMachine(machine);
}


/**
 * Resets the machine displayed on the canvas when the machine is unloaded
 * in the simulator.
 */
void
ProximMachineStateWindow::reset() {
    canvas_->setMachine(NULL);
}



/**
 * Event handler for simulation stop.
 *
 * Refreshes the register values.
 */
void
ProximMachineStateWindow::onSimulationStop(const SimulatorEvent&) {

    // Store pointer to the selected machine part.
    const MachinePart* selection = NULL;
    if (canvas_->selection() != NULL) {
        selection = canvas_->selection()->model();
    }

    canvas_->clearSelection();
    clearDetails();

    if (showMoves_) {
        addMoves();
    }
    if (showUtilizations_) {
        setUtilizationHighlights();
    }
    
    canvas_->Refresh();

    // Reselect & display details of the previously selected machine part.
    Request statusRequest(Request::DETAILS_REQUEST);
    if (selection != NULL) {
        EditPart * part = canvas_->findEditPart(selection);
        if (part != NULL && part->canHandle(&statusRequest)) {
            canvas_->select(part);
            ComponentCommand* command = part->performRequest(&statusRequest);
            command->Do();
            delete command;
        }
    }
}


/**
 * Creates the window contents.
 *
 * Code generated by wxWidgets.
 */
void
ProximMachineStateWindow::createContents() {

    sizer_ = new wxBoxSizer(wxVERTICAL);

    wxSplitterWindow* splitter = new wxSplitterWindow(
        this, ID_SPLITTER, wxDefaultPosition, wxSize(800,600),
        wxSP_BORDER|wxSP_3D|wxCLIP_CHILDREN);

    wxPanel* left = new wxPanel(splitter, -1);
    wxPanel* right = new wxPanel(splitter, -1);

    // Splitter window left hand pane sizer.
    wxFlexGridSizer* leftSizer = new wxFlexGridSizer(1, 0, 0);
    leftSizer->AddGrowableCol(0);
    leftSizer->AddGrowableRow(1);
    leftSizer->AddGrowableRow(4);

    wxStaticText* title = new wxStaticText(
        left, ID_TITLE, ProximConstants::MACH_WIN_DETAILS_TITLE,
        wxDefaultPosition, wxDefaultSize, 0);

    detailsCtrl_ = new wxTextCtrl(
        left, ID_DETAILS, wxT(""), wxDefaultPosition, wxSize(80,40),
        wxTE_MULTILINE|wxTE_READONLY|wxVSCROLL|wxHSCROLL);

    wxStaticLine* line = new wxStaticLine(
        left, ID_LINE, wxDefaultPosition, wxSize(20,-1), wxLI_HORIZONTAL);

    wxStaticText* utilizationTitle = new wxStaticText(
        left, ID_LABEL_UTILIZATION,
        ProximConstants::MACH_WIN_UTILIZATION_TITLE, wxDefaultPosition,
        wxDefaultSize, 0);

    utilizationCtrl_ = new wxTextCtrl(
        left, ID_UTILIZATION, wxT(""), wxDefaultPosition, wxSize(80,40),
        wxTE_MULTILINE|wxTE_READONLY|wxVSCROLL|wxHSCROLL);

    leftSizer->Add(title, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    leftSizer->Add(detailsCtrl_, 0, wxGROW|wxALL, 5);
    leftSizer->Add(line, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    leftSizer->Add(utilizationTitle, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    leftSizer->Add(utilizationCtrl_, 0, wxGROW|wxALL, 5);
    left->SetSizer(leftSizer);
    

    // Splitter window right hand pane sizer.
    wxFlexGridSizer* rightSizer = new wxFlexGridSizer(1, 0, 0);
    rightSizer->AddGrowableCol(0);
    rightSizer->AddGrowableRow(0);
    canvas_ = new MachineCanvas(right, new ProximEditPolicyFactory());
    statusbar_ = new wxStatusBar(right, -1);
    rightSizer->Add(canvas_, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);
    rightSizer->Add(statusbar_, 0, wxGROW|wxALL, 5);
    right->SetSizer(rightSizer);

    // Set sizers and split the window.
    splitter->SetMinimumPaneSize(MINIMUM_PANE_WIDTH);
    splitter->SplitVertically(left, right);
    splitter->SetSashPosition(INITIAL_DETAILS_PANE_WIDTH);

    sizer_->Add(splitter, 1, wxGROW);
    SetSizer(sizer_);

    // Create tool for the canvas.
    MachineCanvasTool* canvasTool = new ProximMachineCanvasTool(canvas_);
    canvas_->setTool(canvasTool);
}


/**
 * Sets the status line text.
 *
 * @param status Status text to set.
 */
void
ProximMachineStateWindow::setStatusText(std::string status) {
    wxString text = WxConversion::toWxString(status);
    statusbar_->SetStatusText(text);
}


/**
 * Handles zoom menu item events.
 *
 * @param event Menu event to handle.
 */
void
ProximMachineStateWindow::onZoom(wxCommandEvent& event) {

    // Zoom in.
    if (event.GetId() == COMMAND_ZOOM_IN) {
	double factor = canvas_->zoomFactor() + ZOOM_STEP;
	if (factor > MAX_ZOOM_FACTOR) {
	    factor = MAX_ZOOM_FACTOR;
	}
	canvas_->setZoomFactor(factor);
    }

    // Zoom out.
    if (event.GetId() == COMMAND_ZOOM_OUT) {
	double factor = canvas_->zoomFactor() - ZOOM_STEP;
	if (factor < MIN_ZOOM_FACTOR) {
	    factor = MIN_ZOOM_FACTOR;
	}
	canvas_->setZoomFactor(factor);
    }
}


/**
 * Sets machine part highlights based on part utilizations.
 */
void
ProximMachineStateWindow::setUtilizationHighlights() {

    canvas_->clearHighlights();

    Machine* machine = const_cast<Machine*>(&ProximToolbox::machine());

    const UtilizationStats& stats =
        ProximToolbox::frontend()->utilizationStatistics();

    ClockCycleCount cycles = ProximToolbox::frontend()->cycleCount();

    // Highlight buses.
    const Machine::BusNavigator& busNavigator = machine->busNavigator();
    for (int i = 0; i < busNavigator.count(); i++) {
        ClockCycleCount writes =
            stats.busWrites(busNavigator.item(i)->name());
        double utilization = static_cast<double>(writes) / cycles;
        unsigned value = unsigned(utilization * 255);
        canvas_->highlight(busNavigator.item(i), wxColour(value, 0, 0));
    }

    // Highlight sockets.
    const Machine::SocketNavigator& socketNavigator =
        machine->socketNavigator();

    for (int i = 0; i < socketNavigator.count(); i++) {
        ClockCycleCount writes =
            stats.socketWrites(socketNavigator.item(i)->name());
        double utilization = static_cast<double>(writes) / cycles;
        unsigned value = 255 - unsigned(utilization * 255);
        wxColour colour = wxColour(255, value, value);
        canvas_->highlight(socketNavigator.item(i), colour);
    }
        
    // Highlight function units.
    const Machine::FunctionUnitNavigator& fuNavigator =
        machine->functionUnitNavigator();

    for (int i = 0; i < fuNavigator.count(); i++) {
        ClockCycleCount writes =
            stats.triggerCount(fuNavigator.item(i)->name());
        double utilization = static_cast<double>(writes) / cycles;
        unsigned value = 255 - unsigned(utilization * 255);
        wxColour colour = wxColour(255, value, value);
        canvas_->highlight(fuNavigator.item(i), colour);
    }
        
}


/**
 * Adds active moves to be drawn on the canvas.
 */
void
ProximMachineStateWindow::addMoves() {

    InstructionAddress address = simulator_->lastExecutedInstruction();

    const TTAProgram::Instruction& instruction =
	simulator_->program().instructionAt(address);

    canvas_->clearMoves();
    for (int i = 0; i < instruction.moveCount(); i++) {

        // Check that the move wasn't squashed by a guard.
        const ExecutableInstruction& lastInstruction =
            simulator_->lastExecInstruction();

        if (lastInstruction.moveSquashed(i)) {
            continue;
        }

        // Move not squashed, add move to be drawn on the canvas.
        TTAProgram::Move& move = instruction.move(i);
        const Bus* bus = &move.bus();
        const Port* source = NULL;
        const Port* target = NULL;
        if (move.source().isFUPort() || move.source().isGPR()) {
            source = &move.source().port();
        }
        if (move.destination().isFUPort() || move.destination().isGPR()) {
            target = &move.destination().port();
        }
        canvas_->addMove(bus, source, target);
    }

}

/**
 * Toggles the unit info string display on and off.
 */
void
ProximMachineStateWindow::onToggleUnitInfo(wxCommandEvent&) {

    OptionValue& showInfo = canvas_->options().optionValue(
        MachineCanvasOptions::SHOW_UNIT_INFO_STRING);

    showInfo.setBoolValue(!showInfo.isFlagOn());
    canvas_->updateMachine();
    
}


/**
 * Toggles the move display on and off.
 */
void
ProximMachineStateWindow::onToggleMoves(wxCommandEvent&) {
    showMoves_ = !showMoves_;
    if (showMoves_) {
        addMoves();
    } else {
        canvas_->clearMoves();
    }
    canvas_->Refresh();
}

/**
 * Toggles the utilization display on and off.
 */
void
ProximMachineStateWindow::onToggleUtilizations(wxCommandEvent&) {
 
    showUtilizations_ = !showUtilizations_;
    
    SimulatorFrontend* frontend = ProximToolbox::frontend();
    if (showUtilizations_ && frontend != NULL) {
        setUtilizationHighlights();
    } else {
        canvas_->clearHighlights();
    }
    canvas_->Refresh();
}


/**
 * Event handler for meni item UI update events.
 *
 * @param event Update event to handle.
 */
void
ProximMachineStateWindow::onUpdateUIEvent(wxUpdateUIEvent& event) {

    if (event.GetId() == COMMAND_TOGGLE_MOVES) {
        event.Check(showMoves_);
    } else if (event.GetId() == COMMAND_TOGGLE_UTILIZATIONS) {
        event.Check(showUtilizations_);
    } else if (event.GetId() == COMMAND_TOGGLE_UNIT_INFO) {
        OptionValue& showInfo = canvas_->options().optionValue(
            MachineCanvasOptions::SHOW_UNIT_INFO_STRING);
        event.Check(showInfo.isFlagOn());
    } else {
        event.Skip();
    }
}


/**
 * Appends text to the component utilization widget.
 *
 * @param text Text to append.
 */
void
ProximMachineStateWindow::appendUtilizationData(std::string text) {
    wxString data = WxConversion::toWxString(text);
    utilizationCtrl_->AppendText(data);
}


/**
 * Appends text to the component details widget.
 *
 * @param text Text to append.
 */
void
ProximMachineStateWindow::appendDetails(std::string text) {
    wxString data = WxConversion::toWxString(text);
    detailsCtrl_->AppendText(data);
}

/**
 * Clears data in the component details pane.
 */
void
ProximMachineStateWindow::clearDetails() {
    detailsCtrl_->Clear();
    utilizationCtrl_->Clear();
}


/**
 * Displays a dialog for saving machine figure in a file.
 */
void
ProximMachineStateWindow::onExport(wxCommandEvent&) {

    wxString message = _T("Export processor figure.");
    wxString defaultDir = _T(".");
    wxString defaultFile= _T("");
    wxString fileTypes = _T("Encapsulated Postscript (.eps)|*.eps|");
    fileTypes.Append(_T("Portable Network Graphics (.png)|*.png"));

    wxFileDialog dialog(
        this, message, defaultDir, defaultFile, fileTypes,
        wxSAVE | wxOVERWRITE_PROMPT);

    if (dialog.ShowModal() == wxID_CANCEL) {
        return;
    }

    std::string filename = WxConversion::toString(dialog.GetPath());
    std::string extension = FileSystem::fileExtension(filename);
    std::string creator = "TTA Processor Simulator (Proxim)";
    std::string title = "Processor Simulation";

    if (extension == ".eps" || extension == ".epsi") {
        if (!canvas_->saveEPS(filename, title, creator)) {
            wxString message = _T("Error saving file '");
            message.Append(dialog.GetPath());
            message.Append(_T("'."));
            ErrorDialog errorDialog(this, message);
            errorDialog.ShowModal();
            return;
        }
    } else if (extension == ".png") {
        if (!canvas_->savePNG(filename)) {  
            wxString message = _T("Error saving file '");
            message.Append(dialog.GetPath());
            message.Append(_T("'."));
            ErrorDialog errorDialog(this, message);
            errorDialog.ShowModal();
            return;
        }
    } else {
        wxString message = _T("File type with extension '");
        message.Append(WxConversion::toWxString(extension));
        message.Append(_T("' is not supported."));
        ErrorDialog dialog(this, message);
        dialog.ShowModal();
    }
}
