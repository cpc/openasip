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
 * @file ProximOpenMachineCmd.cc
 *
 * Implementation of ProximOpenMachineCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <wx/filedlg.h>
#include "ProximOpenMachineCmd.hh"
#include "WxConversion.hh"
#include "ProximConstants.hh"
#include "Proxim.hh"
#include "FileSystem.hh"
#include "ProximSimulationThread.hh"

/**
 * The Constructor.
 */
ProximOpenMachineCmd::ProximOpenMachineCmd():
    GUICommand(ProximConstants::COMMAND_NAME_OPEN_MACHINE, NULL) {

    }

/**
 * The Destructor.
 */
ProximOpenMachineCmd::~ProximOpenMachineCmd() {
}


/**
 * Executes the command.
 */
bool
ProximOpenMachineCmd::Do() {
    assert(parentWindow() != NULL);

    wxString wildcard = _T("Architecture Definition Files (*.adf)|*.adf");
    wildcard.Append(_T("|Processor Configuration Files (*.pcf)|*.pcf"));
    wildcard.Append(_T("|All files|*.*"));
    wxFileDialog dialog(
	parentWindow(), _T("Choose a file."), _T(""), _T(""),
	wildcard, wxOPEN);

    if (dialog.ShowModal() == wxID_CANCEL) {
	return false;
    }

    std::string command;
    std::string file = WxConversion::toString(dialog.GetPath());
    std::string extension = FileSystem::fileExtension(file);

    // Simulator interpreter command is chose according to file extension.
    // Processor configuration files (.pcf), have to be opened using
    // 'conf' command.
    if (extension == ".pcf") {
	command = ProximConstants::SCL_LOAD_CONF + " \"" + file + "\"";
    } else {
	command = ProximConstants::SCL_LOAD_MACHINE + " \"" + file + "\"";
    }

    wxGetApp().simulation()->lineReader().input(command);
    return true;
}


/**
 * Returns full path to the command icon file.
 *
 * @return Full path to the command icon file.
 */
std::string
ProximOpenMachineCmd::icon() const {
    return "open_machine.png";
}


/**
 * Returns ID of this command.
 */
int
ProximOpenMachineCmd::id() const {
    return ProximConstants::COMMAND_OPEN_MACHINE;
}


/**
 * Creates and returns a new isntance of this command.
 *
 * @return Newly created instance of this command.
 */
ProximOpenMachineCmd*
ProximOpenMachineCmd::create() const {
    return new ProximOpenMachineCmd();
}


/**
 * Returns true if the command is enabled, false otherwise.
 *
 * @return Always true.
 */
bool
ProximOpenMachineCmd::isEnabled() {
    return true;
}

/**
 * Returns shortened name of the command for toolbar button text.
 *
 * @return Short version of the command name.
 */
std::string
ProximOpenMachineCmd::shortName() const {
    return "Machine";
}
