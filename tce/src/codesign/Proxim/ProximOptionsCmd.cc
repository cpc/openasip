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
 * @file ProximOptionsCmd.cc
 * 
 * Implementation of ProximOptionsCmd class.
 * 
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "ProximOptionsCmd.hh"
#include "OptionsDialog.hh"
#include "ProximConstants.hh"
#include "GUIOptions.hh"
#include "Proxim.hh"
#include "ProximMainFrame.hh"
#include "ProximToolbox.hh"

using std::string;

/**
 * The Constructor.
 */
ProximOptionsCmd::ProximOptionsCmd():
    GUICommand(ProximConstants::COMMAND_NAME_EDIT_OPTIONS, NULL) {

}


/**
 * The Destructor.
 */
ProximOptionsCmd::~ProximOptionsCmd() {
}


/**
 * Executes the command.
 * 
 * @return True, if the command was succesfully executed, false otherwise.
 */
bool
ProximOptionsCmd::Do() {

    OptionsDialog dialog(
        parentWindow(), wxGetApp().options(), wxGetApp().commandRegistry());

    if (dialog.ShowModal() == wxID_OK) {
	if (wxGetApp().options().toolbarVisibility()) {
            ProximToolbox::mainFrame()->createToolbar();
	}
        ProximToolbox::mainFrame()->createMenubar();
    }
    return true;
}


/**
 * Returns id of this command.
 * 
 * @return ID for this command to be used in menus and toolbars.
 */
int
ProximOptionsCmd::id() const {
    return ProximConstants::COMMAND_EDIT_OPTIONS;
}
		  
		  
/**
 * Creates and returns a new instance of this command.
 * 
 * @return Newly created instance of this command.
 */
ProximOptionsCmd*
ProximOptionsCmd::create() const {
    return new ProximOptionsCmd();
}


/**
 * Returns path to the command's icon file.
 * 
 * @return Full path to the command's icon file.
 */
string
ProximOptionsCmd::icon() const {
    return "options.png";
}


/**
 * This command is always executable.
 * 
 * @return Always true.
 */
bool
ProximOptionsCmd::isEnabled() {
    return true;
}
