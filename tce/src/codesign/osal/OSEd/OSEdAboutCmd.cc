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
 * @file OSEdAboutCmd.cc
 *
 * Definition of OSEdAboutCmd class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#include <boost/format.hpp>

#include "OSEdAboutCmd.hh"
#include "OSEdConstants.hh"
#include "InformationDialog.hh"
#include "WxConversion.hh"
#include "OSEdTextGenerator.hh"
#include "OSEdAboutDialog.hh"

using boost::format;
using std::string;

/**
 * Constructor.
 */
OSEdAboutCmd::OSEdAboutCmd() : 
    GUICommand(OSEdConstants::CMD_NAME_ABOUT, NULL) {
}

/**
 * Destructor.
 */
OSEdAboutCmd::~OSEdAboutCmd() {
}

/**
 * Returns the id of the command.
 *
 * @return The id of the command.
 */
int
OSEdAboutCmd::id() const {
    return OSEdConstants::CMD_ABOUT;
}

/**
 * Creates a new command.
 *
 * @return The created command.
 */
GUICommand*
OSEdAboutCmd::create() const {
    return new OSEdAboutCmd();
}

/**
 * Executes the command.
 *
 * @return True.
 */
bool
OSEdAboutCmd::Do() {
    OSEdAboutDialog about(parentWindow());
    about.ShowModal();
    return true;
}

/**
 * Returns true, if command is enabled.
 *
 * @return Always true.
 */
bool
OSEdAboutCmd::isEnabled() {
    return true;
}

/**
 * Returns the icon path.
 *
 * @return Empty string (no icons used).
 */
string
OSEdAboutCmd::icon() const {
    return "";
}
