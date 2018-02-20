/*
    Copyright (c) 2002-2017 Tampere University of Technology.

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
 * @file FindWindow.cc
 * 
 * Definition of FindWindow class.
 * 
 * @author Alex Hirvonen 2017 (alex.hirvonen-no.spam-gmail.com)
 * @note rating: red
 */


#include <algorithm>
#include <wx/textctrl.h>
#include <wx/stattext.h>

#include "FindWindow.hh"
#include "ProximToolbox.hh"
#include "WxConversion.hh"
#include "ProximDisassemblyWindow.hh"
#include "Program.hh"
#include "Instruction.hh"
#include "Move.hh"


BEGIN_EVENT_TABLE(FindWindow, ProximSimulatorWindow)
    EVT_TEXT(ID_OP_INPUT, FindWindow::onInputText)
    EVT_SIMULATOR_PROGRAM_LOADED(0, FindWindow::onInputText)
    EVT_TEXT_ENTER(ID_OP_INPUT, FindWindow::onFindNext)
    EVT_BUTTON(ID_FIND_PREV, FindWindow::onFindPrev)
    EVT_BUTTON(ID_FIND_NEXT, FindWindow::onFindNext)
    EVT_CHECKBOX(ID_MATCH_CASE, FindWindow::onInputText)
END_EVENT_TABLE()


FindWindow::FindWindow(ProximMainFrame* parent, int id):
    ProximSimulatorWindow(parent, id) {

    createContents(this, true, true);
    opInput_->SetFocus();
    matchCase_->SetValue(false);
    findPrevBtn_->Disable();
    findNextBtn_->Disable();
}


FindWindow::~FindWindow() {
}


/**
 * Called when the simulator program, memory and machine models are reset.
 */
void
FindWindow::reset() {
    // Do nothing.
}

/**
 * Called when the input text changes or match case checkbox changes state.
 */
void
FindWindow::onInputText(wxCommandEvent&) {
    wxString inputwxString = opInput_->GetValue();

    if (inputwxString.Length() > 2) {
        std::string pattern = WxConversion::toString(inputwxString);

        bool found = find(pattern);
        if (found) {
            findPrevBtn_->Enable();
            findNextBtn_->Enable();

            int total = matchedLines.size();
            // update label
            infoLabel_->SetLabel(wxT("1 of ") + WxConversion::toWxString(total)+
                wxT(" matched lines"));
            // jump to first matched line
            ProximToolbox::disassemblyWindow()->showAddress(matchedLines[0]);

            return;

        } else {
            infoLabel_->SetLabel(wxT("Pattern not found"));
            return;
        }

    }
    infoLabel_->SetLabel(wxT(""));
    findPrevBtn_->Disable();
    findNextBtn_->Disable();
}


/*
 * Called when the [Previous] button is pressed.
 */
void
FindWindow::onFindPrev(wxCommandEvent&) {

    int matchedSize = matchedLines.size();
    if (matchedSize == 0) {
        return;
    }

    if (matchedIndex == 0) {
        matchedIndex = matchedSize - 1;
    } else {
        matchedIndex--;
    }
    infoLabel_->SetLabel(WxConversion::toWxString(matchedIndex+1) + wxT(" of ") +
        WxConversion::toWxString(matchedSize) + wxT(" matched lines"));
    ProximToolbox::disassemblyWindow()->showAddress(matchedLines[matchedIndex]);
}


/*
 * Called when the [Find Next] button is pressed.
 */
void
FindWindow::onFindNext(wxCommandEvent&) {

    int matchedSize = matchedLines.size();
    if (matchedSize == 0) {
        return;
    }

    if (matchedIndex == matchedSize - 1) {
        matchedIndex = 0;
    } else {
        matchedIndex++;
    }
    // update label
    infoLabel_->SetLabel(WxConversion::toWxString(matchedIndex+1) + wxT(" of ") +
        WxConversion::toWxString(matchedSize) + wxT(" matched lines"));

    ProximToolbox::disassemblyWindow()->showAddress(matchedLines[matchedIndex]);
}


/*
 * Searches through program's assembly instructions and collects information
 * on which lines the pattern text appears.
 *
 * @param pattern Text string to be searched.
 */
bool
FindWindow::find(std::string pattern) {

    const TTAProgram::Program& program = ProximToolbox::program();
    std::size_t found;
    matchedIndex = 0;
    matchedLines.clear();

    for (int i = 0; i < program.instructionCount(); i++) {
        const TTAProgram::Instruction& instruction = program.instructionAt(i);

        std::string instrString = "";

        for (int j = 0; j < instruction.moveCount(); j++) {
            const TTAProgram::Move& move = instruction.move(j);
            instrString += move.toString();
        }
        // remove spaces in instruction and search pattern
        instrString.erase(remove_if(instrString.begin(), instrString.end(),
            isspace), instrString.end());
        pattern.erase(remove_if(pattern.begin(), pattern.end(),
            isspace), pattern.end());
        // case insensitive search
        if (!matchCase_->IsChecked()) {
            std::transform(instrString.begin(), instrString.end(),
                instrString.begin(), ::tolower);
            std::transform(pattern.begin(), pattern.end(), pattern.begin(),
                ::tolower);
        }

        found = instrString.find(pattern);
        if (found != std::string::npos) {
            matchedLines.push_back(i);
        }
    }
    return matchedLines.size() > 0;
}


/**
 * Creates the dialog widgets.
 */
wxSizer*
FindWindow::createContents(
    wxWindow *parent, bool call_fit, bool set_sizer) {

    wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);

    opInput_ = new wxTextCtrl(parent, ID_OP_INPUT, wxT(""),
        wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    mainSizer->Add(opInput_, 0, wxALIGN_CENTER|wxALL|wxEXPAND, 5);

    matchCase_ = new wxCheckBox(parent, ID_MATCH_CASE, wxT("Case sensitive"),
        wxDefaultPosition, wxDefaultSize);
    mainSizer->Add(matchCase_, 0, wxALIGN_LEFT|wxALL|wxEXPAND, 5);

    infoLabel_ = new wxStaticText(parent, ID_INFO_LABEL, wxT(""),
        wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
    mainSizer->Add(infoLabel_, 0, wxALIGN_CENTER|wxALL|wxEXPAND, 5);

    findPrevBtn_ = new wxButton(parent, ID_FIND_PREV, wxT("Previous"),
        wxDefaultPosition, wxDefaultSize, 0);
    findNextBtn_ = new wxButton(parent, ID_FIND_NEXT, wxT("Find next"),
        wxDefaultPosition, wxDefaultSize, 0);

    buttonSizer->Add(findPrevBtn_, 0, wxALL, 5);
    buttonSizer->Add(findNextBtn_, 0, wxALL, 5);

    mainSizer->Add(buttonSizer, 0, wxALIGN_CENTER|wxALL, 5);

    if (set_sizer) {
        parent->SetSizer(mainSizer);
        if (call_fit) {
            mainSizer->SetSizeHints(parent);
        }
    }

    return mainSizer;
}
