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
 * @file ProximRunCmd.cc
 *
 * Implementation of ProximRunCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "ProximRunCmd.hh"
#include "ProximConstants.hh"
#include "Proxim.hh"
#include "ProximSimulationThread.hh"
#include "TracedSimulatorFrontend.hh"

/**
 * The Constructor.
 */
ProximRunCmd::ProximRunCmd():
    GUICommand(ProximConstants::COMMAND_NAME_RUN, NULL) {

    simulator_ = wxGetApp().simulation()->frontend();
}

/**
 * The Destructor.
 */
ProximRunCmd::~ProximRunCmd() {
}


/**
 * Executes the command.
 */
bool
ProximRunCmd::Do() {
    wxGetApp().simulation()->lineReader().input("run");
    return true;
}


/**
 * Returns full path to the command icon file.
 *
 * @return Full path to the command icon file.
 */
std::string
ProximRunCmd::icon() const {
    return "run.png";
}


/**
 * Returns ID of this command.
 */
int
ProximRunCmd::id() const {
    return ProximConstants::COMMAND_RUN;
}


/**
 * Creates and returns a new isntance of this command.
 *
 * @return Newly created instance of this command.
 */
ProximRunCmd*
ProximRunCmd::create() const {
    return new ProximRunCmd();
}


/**
 * Returns true if the command is enabled, false otherwise.
 *
 * @return True if the simulation is initialized and ready to run,
 *         false otherwise.
 */
bool
ProximRunCmd::isEnabled() {

    if (simulator_ != NULL && simulator_->isSimulationInitialized()) {
	return true;
    } else {
	return false;
    }
}
