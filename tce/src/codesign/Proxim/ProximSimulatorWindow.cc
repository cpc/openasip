/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file ProximSimulatorWindow.cc
 *
 * Implementation of ProximSimulatorWindow class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "ProximSimulatorWindow.hh"
#include "ProximMainFrame.hh"
#include "ProximToolbox.hh"


BEGIN_EVENT_TABLE(ProximSimulatorWindow, wxPanel)
    EVT_SIMULATOR_COMMAND(-1, ProximSimulatorWindow::onSimulatorBusy)
    EVT_SIMULATOR_COMMAND_DONE(-1, ProximSimulatorWindow::onSimulatorDone)
END_EVENT_TABLE()


/**
 * The Constructor.
 *
 * @param mainFrame ProximMainFrame parent of the window.
 */
ProximSimulatorWindow::ProximSimulatorWindow(
    ProximMainFrame* mainFrame, wxWindowID id, wxPoint pos, wxSize size,
        long style) :
    wxPanel(mainFrame, id, pos, size, style) {

    mainFrame->addSubWindow(this);
}


/**
 * The Destructor.
 */
ProximSimulatorWindow::~ProximSimulatorWindow() {
    ProximMainFrame* mainFrame = ProximToolbox::mainFrame();
    if (mainFrame != NULL) {
        mainFrame->removeSubWindow(this);
    }
}


/**
 * Disables the window when the simulator is busy.
 */
void
ProximSimulatorWindow::onSimulatorBusy(SimulatorEvent&) {
    Disable();
}

/**
 * Enables the window when the simulator is not busy.
 */
void
ProximSimulatorWindow::onSimulatorDone(SimulatorEvent&) {
    Enable();
}

/**
 * Called before the simulator thread destroys any of it's data structures
 * accessed by GUI.
 */
void
ProximSimulatorWindow::reset() {
    GetParent()->Close();
}
