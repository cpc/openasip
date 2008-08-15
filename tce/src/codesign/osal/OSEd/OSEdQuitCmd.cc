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
 * @file OSEdQuitCmd.cc
 *
 * Definition of OSEdQuitCmd class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#include "OSEdQuitCmd.hh"
#include "OSEdConstants.hh"
#include "OSEdMainFrame.hh"
#include "OSEd.hh"

using std::string;

/**
 * Constructor.
 */
OSEdQuitCmd::OSEdQuitCmd() : GUICommand(OSEdConstants::CMD_NAME_QUIT, NULL) {
}

/**
 * Destructor.
 */
OSEdQuitCmd::~OSEdQuitCmd() {
}

/**
 * Returns the id of the command.
 */
int
OSEdQuitCmd::id() const {
    return OSEdConstants::CMD_QUIT;
}

/**
 * Creates a new command.
 *
 * @return Created command.
 */
GUICommand*
OSEdQuitCmd::create() const {
    return new OSEdQuitCmd();
}

/**
 * Executes the command.
 *
 * @return False.
 */
bool
OSEdQuitCmd::Do() {
    OSEdMainFrame* parent = wxGetApp().mainFrame();
    parent->Close(true);
    return false;
}

/**
 * Returns true if command is enabled.
 *
 * @return True.
 */
bool
OSEdQuitCmd::isEnabled() {
    return true;
}

/**
 * Returns the icon file.
 *
 * @return Empty string (no icons used).
 */
string
OSEdQuitCmd::icon() const {
    return "";
}
