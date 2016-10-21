/*
    Copyright (c) 2002-2016 Tampere University of Technology.

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
 * @file ProximDebuggerWindow.cc
 *
 * Definition of ProximDebuggerWindow class.
 *
 * @author Alex Hirvonen 2016 (alex.hirvonen-no.spam-gmail.com)
 * @note rating: red
 */


#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <vector>
#include <string>

#include "ProximDebuggerWindow.hh"
#include "ProximToolbox.hh"
#include "Proxim.hh"
#include "TracedSimulatorFrontend.hh"
#include "ProximSimulationThread.hh"
#include "Machine.hh"
#include "Program.hh"
#include "Instruction.hh"
#include "Move.hh"
#include "WxConversion.hh"
#include "ErrorDialog.hh"

using namespace TTAProgram;

BEGIN_EVENT_TABLE(ProximDebuggerWindow, ProximSimulatorWindow)
    EVT_SIMULATOR_STOP(0, ProximDebuggerWindow::onSimulationStop)
    EVT_SIMULATOR_PROGRAM_LOADED(0, ProximDebuggerWindow::onProgramLoaded)
    EVT_CHOICE(ID_SOURCEFILE_CHOICE, ProximDebuggerWindow::onSourceFileChoice)
END_EVENT_TABLE()


ProximDebuggerWindow::ProximDebuggerWindow(
    ProximMainFrame* parent, int id):
    ProximSimulatorWindow(parent, id, wxDefaultPosition, wxSize(800,600)),
    currentFile_(), currentLineNum_(0) {

    createContents(this, true, true);
    loadProgram(ProximToolbox::program());
}


ProximDebuggerWindow::~ProximDebuggerWindow() {
}


/**
 * Resets the window when a new program or machine is being loaded in the
 * simulator.
 */
void
ProximDebuggerWindow::reset() {
    sourceFileList_->Clear();
    sourceCodeText_->Clear();
    currentFile_.clear();
    currentLineNum_ = 0;
}


/**
 * Loads a program model to the window.
 *
 * @param program Program to load.
 */
void
ProximDebuggerWindow::loadProgram(const Program& program) {

    // get list of used source code files for drop-down list
    for (int i = 0; i < program.moveCount(); i++) {

        const TTAProgram::Move& m = program.moveAt(i);

        if (m.hasSourceFileName()) {
            wxString sourceFile = WxConversion::toWxString(m.sourceFileName());

            if (sourceFileList_->FindString(sourceFile) == -1) {
                sourceFileList_->Append(sourceFile);
            }
        }
    }

    if (sourceFileList_->GetCount() > 0) {
        loadSourceCode(sourceFileList_->GetString(0));
    } else {
        ErrorDialog dialog(
            this, _T("No debugging information available."));
        dialog.ShowModal();
    }
}


/**
 * Loads source code into the sourceCodeText_ widget
 *
 * @param sourceFile Absolute file location as wxString.
 */
void
ProximDebuggerWindow::loadSourceCode(wxString sourceFile) {

    if (sourceFile != currentFile_) {
        sourceCodeText_->LoadFile(sourceFile);
        currentFile_ = sourceFile;
        
        sourceFileList_->SetSelection(sourceFileList_->FindString(sourceFile));
    }
}


/**
 * Sets certain style attributes to the line in sourceCodeText_ widget
 *
 * @param lineNum Line number
 * @param style Style attributes (text color, background color, font, alignment)
 */
void
ProximDebuggerWindow::setLineAttributes(int lineNum, wxTextAttr style) {
    long lineStart = sourceCodeText_->XYToPosition(0, lineNum - 1);
    long lineEnd = lineStart + sourceCodeText_->GetLineLength(lineNum - 1);
    sourceCodeText_->SetStyle(lineStart, lineEnd, style);
}


/**
 * Highlights line of code in the sourceCodeText_ widget
 *
 * @param lineNum Source code line number.
 */
void
ProximDebuggerWindow::highlightLine(int lineNum) {

    if (lineNum != currentLineNum_) {
        // run this only if we have some line selection active
        if (currentLineNum_ != 0) {
            // unhighlight old line first
            setLineAttributes(currentLineNum_, wxTextAttr(*wxBLACK, *wxWHITE));
        }
        // highlight new line with blue
        currentLineNum_ = lineNum;
        setLineAttributes(currentLineNum_, wxTextAttr(*wxWHITE, *wxBLUE));
        // make highlined line visible
        showLine(lineNum);
    }
}


/**
 * Makes line visible in the sourceCodeText_ widget
 *
 * @param lineNum Source code line number.
 */
void
ProximDebuggerWindow::showLine(int lineNum) {
    sourceCodeText_->ShowPosition(sourceCodeText_->XYToPosition(0,lineNum - 1));
}


/**
 * Event handler which is called when a new program is loaded in the simulator.
 */
void
ProximDebuggerWindow::onProgramLoaded(const SimulatorEvent&) {
    loadProgram(ProximToolbox::program());
}


/**
 * Event handler for simulation stop.
 *
 * Refreshes the source code window information according to current
 * simulation state.
 */
void
ProximDebuggerWindow::onSimulationStop(const SimulatorEvent&) {

    if (sourceFileList_->GetCount() > 0) {
        Word pc = wxGetApp().simulation()->frontend()->programCounter();

        const TTAProgram::Instruction& instruction =
            ProximToolbox::program().instructionAt(pc);

        if (instruction.moveCount() > 0) {
            const TTAProgram::Move& m = instruction.move(0);

            if (m.hasSourceLineNumber()) {
                loadSourceCode(WxConversion::toWxString(m.sourceFileName()));
                highlightLine(m.sourceLineNumber());
            }
        }
    }
}


/**
 * Event handler for drop-down list selection.
 *
 * Loads selected source code from the drop-down list.
 */
void
ProximDebuggerWindow::onSourceFileChoice(wxCommandEvent&) {
    loadSourceCode(sourceFileList_->GetString(sourceFileList_->GetSelection()));
}


/**
 * Creates the window contents.
 *
 * Code generated by wxDesigner.
 */
wxSizer*
ProximDebuggerWindow::createContents(
    wxWindow *parent, bool call_fit, bool set_sizer) {

    wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer *upperSizer = new wxBoxSizer(wxHORIZONTAL);
    sourceFileList_ = new wxChoice(
        parent, ID_SOURCEFILE_CHOICE, wxDefaultPosition, wxDefaultSize);

    upperSizer->Add(sourceFileList_, 1, wxGROW|wxALL, 5);
    mainSizer->Add(upperSizer, 0, wxGROW|wxALL, 5);

    sourceCodeText_ = new wxTextCtrl(
        parent, ID_SOURCECODE, wxEmptyString, wxDefaultPosition,
        wxSize(640, 600), wxTE_MULTILINE | wxTE_READONLY | wxTE_DONTWRAP);

    mainSizer->Add(sourceCodeText_, 1, wxGROW|wxALL, 5);

    if (set_sizer)
    {
        parent->SetSizer( mainSizer );
        if (call_fit)
            mainSizer->SetSizeHints( parent );
    }
    
    return mainSizer;
}
