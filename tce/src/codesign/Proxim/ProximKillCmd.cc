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
 * @file ProximKillCmd.cc
 *
 * Implementation of ProximKillCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "ProximKillCmd.hh"
#include "ProximConstants.hh"
#include "Proxim.hh"
#include "ProximSimulationThread.hh"
#include "TracedSimulatorFrontend.hh"
#include "ConfirmDialog.hh"

/**
 * The Constructor.
 */
ProximKillCmd::ProximKillCmd():
    GUICommand(ProximConstants::COMMAND_NAME_KILL, NULL) {

    simulator_ = wxGetApp().simulation()->frontend();
}

/**
 * The Destructor.
 */
ProximKillCmd::~ProximKillCmd() {
}


/**
 * Executes the command.
 */
bool
ProximKillCmd::Do() {
    assert(parentWindow() != NULL);
    wxString message = _T("Are you sure you want to restart simulation?");
    wxString title = _T("Confirm kill");
    wxMessageDialog dialog(
	parentWindow(), message, title, wxYES_NO | wxCENTRE | wxICON_QUESTION);

    if (dialog.ShowModal() == wxID_YES) {
	wxGetApp().simulation()->killSimulation();
    }

    return true;
}


/**
 * Returns full path to the command icon file.
 *
 * @return Full path to the command icon file.
 */
std::string
ProximKillCmd::icon() const {
    return "kill.png";
}


/**
 * Returns ID of this command.
 */
int
ProximKillCmd::id() const {
    return ProximConstants::COMMAND_KILL;
}


/**
 * Creates and returns a new isntance of this command.
 *
 * @return Newly created instance of this command.
 */
ProximKillCmd*
ProximKillCmd::create() const {
    return new ProximKillCmd();
}


/**
 * Returns true if the command is enabled, false otherwise.
 *
 * @return True if the simulation is running.
 */
bool
ProximKillCmd::isEnabled() {
    if (simulator_ != NULL &&
        (simulator_->isSimulationRunning() ||
         simulator_->isSimulationStopped() ||
         simulator_->hasSimulationEnded())) {

	return true;
    } else {
	return false;
    }
}
