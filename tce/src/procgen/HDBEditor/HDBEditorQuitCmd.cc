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
 * @file HDBEditorQuitCmd.cc
 *
 * Implementation of HDBEditorQuitCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "HDBEditorQuitCmd.hh"
#include "WxConversion.hh"
#include "HDBEditorConstants.hh"
#include "HDBEditor.hh"
#include "HDBEditorMainFrame.hh"

/**
 * The Constructor.
 */
HDBEditorQuitCmd::HDBEditorQuitCmd():
    GUICommand(HDBEditorConstants::COMMAND_NAME_QUIT, NULL) {
}


/**
 * The Destructor.
 */
HDBEditorQuitCmd::~HDBEditorQuitCmd() {
}


/**
 * Executes the command.
 */
bool
HDBEditorQuitCmd::Do() {

    wxGetApp().mainFrame().Close();

    return true;
}


/**
 * Returns full path to the command icon file.
 *
 * @return Full path to the command icon file.
 */
std::string
HDBEditorQuitCmd::icon() const {
    return "quit.png";
}


/**
 * Returns ID of this command.
 */
int
HDBEditorQuitCmd::id() const {
    return HDBEditorConstants::COMMAND_QUIT;
}


/**
 * Creates and returns a new isntance of this command.
 *
 * @return Newly created instance of this command.
 */
HDBEditorQuitCmd*
HDBEditorQuitCmd::create() const {
    return new HDBEditorQuitCmd();
}


/**
 * Returns true if the command is enabled, false otherwise.
 *
 * @return Always true.
 */
bool
HDBEditorQuitCmd::isEnabled() {
    return true;
}
