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
 * @file ProximSimulatorSettingsCmd.cc
 *
 * Implementation of ProximSimulatorSettingsCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "ProximSimulatorSettingsCmd.hh"
#include "ProximConstants.hh"
#include "ProximToolbox.hh"
#include "SimulatorSettingsDialog.hh"
#include "ProximLineReader.hh"
#include "TracedSimulatorFrontend.hh"

/**
 * The Constructor.
 */
ProximSimulatorSettingsCmd::ProximSimulatorSettingsCmd():
    GUICommand(ProximConstants::COMMAND_NAME_SIMULATOR_SETTINGS, NULL) {
}


/**
 * The Destructor.
 */
ProximSimulatorSettingsCmd::~ProximSimulatorSettingsCmd() {
}

/**
 * Executes the command.
 *
 * @retutn True, if the command was succesfully executed.
 */
bool
ProximSimulatorSettingsCmd::Do() {
    wxWindow* parent = parentWindow();
    assert(parent != NULL);
    SimulatorFrontend* frontend = ProximToolbox::frontend();
    ProximLineReader& lineReader = ProximToolbox::lineReader();
    SimulatorSettingsDialog dialog(parentWindow(), -1, *frontend, lineReader);
    dialog.ShowModal();
    return true;
}


/**
 * Returns ID of this command.
 */
int
ProximSimulatorSettingsCmd::id() const {
    return ProximConstants::COMMAND_SIMULATOR_SETTINGS;
}


/**
 * Returns path to the icon file for this command.
 */
std::string
ProximSimulatorSettingsCmd::icon() const {
    return "configure.png";
}


/**
 * Creates and returns a new isntance of this command.
 *
 * @return Newly created instance of this command.
 */
ProximSimulatorSettingsCmd*
ProximSimulatorSettingsCmd::create() const {
    return new ProximSimulatorSettingsCmd();
}


/**
 * Returns true if the command is enabled, false otherwise.
 *
 * @return Always true.
 */
bool
ProximSimulatorSettingsCmd::isEnabled() {
    return true;
}
