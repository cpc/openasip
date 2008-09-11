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
 * @file ProximOpenProgramCmd.cc
 *
 * Implementation of ProximOpenProgramCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "ProximOpenProgramCmd.hh"
#include "WxConversion.hh"
#include "ProximConstants.hh"
#include "Proxim.hh"
#include "ProximSimulationThread.hh"

/**
 * The Constructor.
 */
ProximOpenProgramCmd::ProximOpenProgramCmd():
    GUICommand(ProximConstants::COMMAND_NAME_OPEN_PROGRAM, NULL) {
}


/**
 * The Destructor.
 */
ProximOpenProgramCmd::~ProximOpenProgramCmd() {
}


/**
 * Executes the command.
 */
bool
ProximOpenProgramCmd::Do() {
    assert(parentWindow() != NULL);

    wxString wildcard = _T("TPEF Program Files (*.tpf, *.tpef)|*.tpf;*.tpef");
    wildcard.Append(_T("|All files|*"));
    wxFileDialog dialog(
	parentWindow(), _T("Choose a file."), _T(""), _T(""),
	wildcard, wxOPEN);

    if (dialog.ShowModal() == wxID_CANCEL) {
	return false;
    }

    std::string command;
    std::string file = WxConversion::toString(dialog.GetPath());

    command = ProximConstants::SCL_LOAD_PROGRAM + " \"" + file + "\"";
    wxGetApp().simulation()->lineReader().input(command);

    return true;
}


/**
 * Returns full path to the command icon file.
 *
 * @return Full path to the command icon file.
 */
std::string
ProximOpenProgramCmd::icon() const {
    return "open_program.png";
}


/**
 * Returns ID of this command.
 */
int
ProximOpenProgramCmd::id() const {
    return ProximConstants::COMMAND_OPEN_PROGRAM;
}


/**
 * Creates and returns a new instance of this command.
 *
 * @return Newly created instance of this command.
 */
ProximOpenProgramCmd*
ProximOpenProgramCmd::create() const {
    return new ProximOpenProgramCmd();
}


/**
 * Returns true if the command is enabled, false otherwise.
 *
 * @return Always true.
 */
bool
ProximOpenProgramCmd::isEnabled() {
    return true;
}


/**
 * Returns shortened name of the command for toolbar button text.
 *
 * @return Short version of the command name.
 */
std::string
ProximOpenProgramCmd::shortName() const {
    return "Program";
}
