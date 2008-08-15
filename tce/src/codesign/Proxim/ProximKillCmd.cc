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
 * @file ProximKillCmd.cc
 *
 * Implementation of ProximKillCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel@cs.tut.fi)
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
