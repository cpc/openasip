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
 * @file ProximFindOperationCmd.cc
 *
 * Implementation of ProximFindOperationCmd class.
 *
 * @author Alex Hirvonen 2017 (alex.hirvonen-no.spam-gmail.com)
 * @note rating: red
 */

#include "ProximFindOperationCmd.hh"
#include "ProximConstants.hh"
#include "FindOperationWindow.hh"
#include "TracedSimulatorFrontend.hh"
#include "ProximToolbox.hh"
#include "ErrorDialog.hh"



ProximFindOperationCmd::ProximFindOperationCmd():
    GUICommand(ProximConstants::COMMAND_NAME_FIND_OPERATION, NULL) {
}


ProximFindOperationCmd::~ProximFindOperationCmd() {
}


/**
 * Executes the command.
 */
bool
ProximFindOperationCmd::Do() {

    SimulatorFrontend* simulation = ProximToolbox::frontend();

    if (simulation->isSimulationInitialized() ||
        simulation->isSimulationRunning() ||
        simulation->isSimulationStopped()) {

        FindOperationWindow* findOperationWindow = new FindOperationWindow(
            ProximToolbox::mainFrame(), -1);
        ProximToolbox::addFramedWindow(
            findOperationWindow, _T("Find operation in assembly code"), true);
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
ProximFindOperationCmd::icon() const {
    return "find.png";
}


/**
 * Returns ID of this command.
 */
int
ProximFindOperationCmd::id() const {
    return ProximConstants::COMMAND_FIND_OPERATION;
}


/**
 * Creates and returns a new instance of this command.
 *
 * @return Newly created instance of this command.
 */
ProximFindOperationCmd*
ProximFindOperationCmd::create() const {
    return new ProximFindOperationCmd();
}


/**
 * Returns true if the command is enabled, false otherwise.
 *
 * @return Always true.
 */
bool
ProximFindOperationCmd::isEnabled() {
    return true;
}
