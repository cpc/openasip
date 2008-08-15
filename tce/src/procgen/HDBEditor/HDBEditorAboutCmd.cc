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
 * @file HDBEditorAboutCmd.cc
 *
 * Implementation of HDBEditorAboutCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "HDBEditorAboutCmd.hh"
#include "HDBEditorConstants.hh"
#include "HDBEditorAboutDialog.hh"


/**
 * The Constructor.
 */
HDBEditorAboutCmd::HDBEditorAboutCmd():
    GUICommand(HDBEditorConstants::COMMAND_NAME_ABOUT, NULL) {

}

/**
 * The Destructor.
 */
HDBEditorAboutCmd::~HDBEditorAboutCmd() {
}


/**
 * Executes the command.
 */
bool
HDBEditorAboutCmd::Do() {
    assert(parentWindow() != NULL);
    HDBEditorAboutDialog dialog(parentWindow());
    dialog.ShowModal();
    return true;
}


/**
 * Returns full path to the command icon file.
 *
 * @return Full path to the command icon file.
 */
std::string
HDBEditorAboutCmd::icon() const {
    return "about.png";
}


/**
 * Returns ID of this command.
 */
int
HDBEditorAboutCmd::id() const {
    return HDBEditorConstants::COMMAND_ABOUT;
}


/**
 * Creates and returns a new isntance of this command.
 *
 * @return Newly created instance of this command.
 */
HDBEditorAboutCmd*
HDBEditorAboutCmd::create() const {
    return new HDBEditorAboutCmd();
}


/**
 * Returns true if the command is enabled, false otherwise.
 *
 * @return Always true.
 */
bool
HDBEditorAboutCmd::isEnabled() {
    return true;
}
