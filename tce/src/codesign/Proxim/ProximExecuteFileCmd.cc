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
 * @file ProximExecuteFileCmd.cc
 *
 * Implementation of ProximExecuteFileCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <fstream>
#include "ProximExecuteFileCmd.hh"
#include "ProximConstants.hh"
#include "ProximToolbox.hh"
#include "ProximLineReader.hh"
#include "WxConversion.hh"
#include "ErrorDialog.hh"
#include "TracedSimulatorFrontend.hh"

/**
 * The Constructor.
 */
ProximExecuteFileCmd::ProximExecuteFileCmd():
    GUICommand(ProximConstants::COMMAND_NAME_EXECUTE_FILE, NULL) {

    simulator_ = ProximToolbox::frontend();
}

/**
 * The Destructor.
 */
ProximExecuteFileCmd::~ProximExecuteFileCmd() {
}


/**
 * Executes the command.
 */
bool
ProximExecuteFileCmd::Do() {
    wxString message = _T("Choose a file.");
    wxString wildcard = _T("Command history logs (*.log)|*.log|All files|*.*");
    wxFileDialog dialog(
        parentWindow(), message, _T(""), _T(""), wildcard, wxOPEN);

    if (dialog.ShowModal() == wxID_CANCEL) {
        return false;
    }

    std::string filename = WxConversion::toString(dialog.GetPath());
    std::ifstream file(filename.c_str());

    if (file.bad()) {
        wxString message = _T("Error opening file '");
        message.Append(dialog.GetPath());
        message.Append(_T("'."));
        ErrorDialog dialog(parentWindow(), message);
        dialog.ShowModal();
        return false;
    }

    ProximLineReader& lineReader = ProximToolbox::lineReader();

    while (!file.eof()) {
        std::string command;
        getline(file, command);
        lineReader.input(command);
    }

    return true;
}


/**
 * Returns full path to the command icon file.
 *
 * @return Full path to the command icon file.
 */
std::string
ProximExecuteFileCmd::icon() const {
    return "exec.png";
}


/**
 * Returns ID of this command.
 */
int
ProximExecuteFileCmd::id() const {
    return ProximConstants::COMMAND_EXECUTE_FILE;
}


/**
 * Creates and returns a new isntance of this command.
 *
 * @return Newly created instance of this command.
 */
ProximExecuteFileCmd*
ProximExecuteFileCmd::create() const {
    return new ProximExecuteFileCmd();
}


/**
 * Returns true if the command is enabled, false otherwise.
 *
 * @return True if the simulation is initialized or stopped.
 */
bool
ProximExecuteFileCmd::isEnabled() {

    if (simulator_ != NULL) {
	return true;
    } else {
	return false;
    }
}
