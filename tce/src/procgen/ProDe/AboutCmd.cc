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
 * @file AboutCmd.cc
 *
 * Definition of AboutCmd class.
 *
 * @author Tommi Rantanen 2004 (tommi.rantanen-no.spam-tut.fi)
 */

#include "AboutCmd.hh"
#include "ProDeConstants.hh"
#include "AboutDialog.hh"

using std::string;

/**
 * The Constructor.
 */
AboutCmd::AboutCmd():
    EditorCommand(ProDeConstants::CMD_NAME_ABOUT) {

}


/**
 * The Destructor.
 */
AboutCmd::~AboutCmd() {}


/**
 * Executes the command.
 *
 * @return True, if the command was succesfully executed, false otherwise.
 */
bool
AboutCmd::Do() {
    AboutDialog about(parentWindow());
    about.ShowModal();
    return true;
}


/**
 * Returns id of this command.
 */
int
AboutCmd::id() const {
    return ProDeConstants::COMMAND_ABOUT;
}


/**
 * Creates and returns a new instance of this command.
 */
AboutCmd*
AboutCmd::create() const {
    return new AboutCmd();
}


/**
 * This command is always executable.
 *
 * @return Always true.
 */
bool
AboutCmd::isEnabled() {
    return true;
}


/**
 * Returns path to the command's icon file.
 */
string
AboutCmd::icon() const {
    return ProDeConstants::CMD_ICON_ABOUT;
}

