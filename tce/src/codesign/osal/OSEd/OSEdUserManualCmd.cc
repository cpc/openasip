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
 * @file OSEdUserManualCmd.cc
 *
 * Definition of OSEdUserManualCmd class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <boost/format.hpp>

#include "OSEdUserManualCmd.hh"
#include "OSEdConstants.hh"
#include "Environment.hh"
#include "FileSystem.hh"
#include "HelpBrowser.hh"
#include "OSEdTextGenerator.hh"
#include "WxConversion.hh"

using std::string;
using boost::format;

/**
 * Constructor.
 */
OSEdUserManualCmd::OSEdUserManualCmd() :
    GUICommand(OSEdConstants::CMD_NAME_USER_MANUAL, NULL) {
}

/**
 * Destructor.
 */
OSEdUserManualCmd::~OSEdUserManualCmd() {
}

/**
 * Returns the id of the command.
 *
 * @return The id of the command.
 */
int
OSEdUserManualCmd::id() const {
    return OSEdConstants::CMD_USER_MANUAL;
}

/**
 * Creates new command.
 *
 * @return New command.
 */
GUICommand*
OSEdUserManualCmd::create() const {
    return new OSEdUserManualCmd();
}

/**
 * Executes the command.
 *
 * @return True if execution is successful.
 */
bool
OSEdUserManualCmd::Do() {

    string manualFile = userManual();
		
    OSEdTextGenerator& texts = OSEdTextGenerator::instance();
    format fmt = texts.text(OSEdTextGenerator::TXT_USER_MANUAL_TITLE);

    HelpBrowser* browser = new HelpBrowser(
        WxConversion::toWxString(fmt.str()), 
        WxConversion::toWxString(manualFile), wxDefaultPosition,
        wxSize(800, 600));

    browser->Show(true);
    return true;
}

/**
 * Returns true if command is enabled.
 *
 * Command is enabled when user manual exists.
 *
 * @return True if command is enabled.
 */
bool
OSEdUserManualCmd::isEnabled() {
    string manualFile = userManual();
    return FileSystem::fileExists(manualFile);
}

/**
 * Returns the icon path.
 *
 * @return Empty string (icons not used).
 */
string
OSEdUserManualCmd::icon() const {
    return "";
}

/**
 * Returns the user manual location.
 *
 * @return User manual location.
 */
std::string
OSEdUserManualCmd::userManual() const {
    return Environment::manDirPath("OSALGUI") +
        FileSystem::DIRECTORY_SEPARATOR + "OSALGUI" +
        FileSystem::DIRECTORY_SEPARATOR + OSEdConstants::USER_MANUAL_NAME;
}
