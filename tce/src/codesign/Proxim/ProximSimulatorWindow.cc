/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
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
