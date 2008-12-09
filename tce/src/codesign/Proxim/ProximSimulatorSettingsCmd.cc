/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
