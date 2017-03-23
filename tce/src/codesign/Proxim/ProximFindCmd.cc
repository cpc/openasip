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
 * @file ProximFindCmd.cc
 *
 * Implementation of ProximFindCmd class.
 *
 * @author Alex Hirvonen 2017 (alex.hirvonen-no.spam-gmail.com)
 * @note rating: red
 */

#include "ProximFindCmd.hh"
#include "ProximConstants.hh"
#include "FindWindow.hh"
#include "TracedSimulatorFrontend.hh"
#include "ProximToolbox.hh"
#include "ErrorDialog.hh"
#include "ProximMainFrame.hh"


ProximFindCmd::ProximFindCmd():
    GUICommand(ProximConstants::COMMAND_NAME_FIND, NULL) {
}


ProximFindCmd::~ProximFindCmd() {
}


/**
 * Executes the command.
 */
bool
ProximFindCmd::Do() {

    SimulatorFrontend* simulation = ProximToolbox::frontend();

    if (simulation->isSimulationInitialized() ||
        simulation->isSimulationRunning() ||
        simulation->isSimulationStopped()) {

        FindWindow* findWindow = dynamic_cast<FindWindow*>(
            ProximToolbox::mainFrame()->
            FindWindowById(ProximConstants::ID_FIND_WINDOW));

        if (findWindow == NULL) {
            findWindow = new FindWindow(ProximToolbox::mainFrame(),
                ProximConstants::ID_FIND_WINDOW);

            ProximToolbox::addFramedWindow(
                findWindow, _T("Find pattern in disassembly"), false,
                    wxSize(300, 150));
                findWindow->GetParent()->Center();
        }
    } else {
        ErrorDialog error(parentWindow(), _T("Simulation not initialized."));
        error.ShowModal();
    }

    return true;
}


/**
 * Returns full path to the command icon file.
 *
 * @return Full path to the command icon file.
 */
std::string
ProximFindCmd::icon() const {
    return "find.png";
}


/**
 * Returns ID of this command.
 */
int
ProximFindCmd::id() const {
    return ProximConstants::COMMAND_FIND;
}


/**
 * Creates and returns a new instance of this command.
 *
 * @return Newly created instance of this command.
 */
ProximFindCmd*
ProximFindCmd::create() const {
    return new ProximFindCmd();
}


/**
 * Returns true if the command is enabled, false otherwise.
 *
 * @return Always true.
 */
bool
ProximFindCmd::isEnabled() {
    return true;
}
