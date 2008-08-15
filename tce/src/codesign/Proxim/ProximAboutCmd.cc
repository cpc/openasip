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
 * @file ProximAboutCmd.cc
 *
 * Implementation of ProximAboutCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "ProximAboutCmd.hh"
#include "ProximConstants.hh"
#include "ProximAboutDialog.hh"


/**
 * The Constructor.
 */
ProximAboutCmd::ProximAboutCmd():
    GUICommand(ProximConstants::COMMAND_NAME_ABOUT, NULL) {

}

/**
 * The Destructor.
 */
ProximAboutCmd::~ProximAboutCmd() {
}


/**
 * Executes the command.
 */
bool
ProximAboutCmd::Do() {
    assert(parentWindow() != NULL);
    ProximAboutDialog dialog(parentWindow());
    dialog.ShowModal();
    return true;
}


/**
 * Returns full path to the command icon file.
 *
 * @return Full path to the command icon file.
 */
std::string
ProximAboutCmd::icon() const {
    return "about.png";
}


/**
 * Returns ID of this command.
 */
int
ProximAboutCmd::id() const {
    return ProximConstants::COMMAND_ABOUT;
}


/**
 * Creates and returns a new isntance of this command.
 *
 * @return Newly created instance of this command.
 */
ProximAboutCmd*
ProximAboutCmd::create() const {
    return new ProximAboutCmd();
}


/**
 * Returns true if the command is enabled, false otherwise.
 *
 * @return Always true.
 */
bool
ProximAboutCmd::isEnabled() {
    return true;
}
