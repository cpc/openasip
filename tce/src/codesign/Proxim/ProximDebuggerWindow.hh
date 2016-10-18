/*
    Copyright (c) 2002-20016 Tampere University of Technology.

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
 * Declaration of ProximDebuggerWindow class.
 *
 * @author Alex Hirvonen 2016 (alex.hirvonen-no.spam-gmail.com)
 * @note rating: red
 */


#ifndef TTA_PROXIM_DEBUGGER_WINDOW_HH
#define TTA_PROXIM_DEBUGGER_WINDOW_HH


#include "ProximSimulatorWindow.hh"
#include "SimulatorEvent.hh"


namespace TTAProgram {
    class Program;
}

/**
 * Proxim subwindow with buttons to control the simulation process.
 *
 * ProximDebuggerWindow doesn't handle button events. The events are
 * passed to the parent window. Button enabled status is updated
 * using wxUpdateUI events.
 */
class ProximDebuggerWindow : public ProximSimulatorWindow {
public:
    ProximDebuggerWindow(ProximMainFrame* parent, int id);
    virtual ~ProximDebuggerWindow();
    virtual void reset();
    void loadProgram(const TTAProgram::Program& program);
    void loadSourceCode(wxString sourceFile);
    void setLineAttributes(int lineNum, wxTextAttr style);
    void highlightLine(int lineNum);
    void showLine(int lineNum);

private:
    void onProgramLoaded(const SimulatorEvent& event);
    void onSimulationStop(const SimulatorEvent& event);
    void onSourceFileChoice(wxCommandEvent& event);
    wxSizer* createContents(wxWindow *parent, bool call_fit, bool set_sizer);

    // Currently loaded source code file
    wxString currentFile_;
    // Currently highlighted source code line number
    int currentLineNum_;
    // Drop-down list of available source code files
    wxChoice* sourceFileList_;
    // Widget for displaying source code contents
    wxTextCtrl* sourceCodeText_;
    // Additional instruction info
    wxStaticText* instrInfo_;

    // Widget IDs.
    enum {
        ID_SOURCEFILE_CHOICE = 19000,
        ID_SOURCECODE,
        ID_TEXT_INSTR_INFO
    };

    DECLARE_EVENT_TABLE()
};

#endif
